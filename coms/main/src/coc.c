#include "common.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "host/ble_l2cap.h"
#include "nvs_flash.h"
/* BLE */
#include "coc.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "ots_store.h"
#include "portmacro.h"
#include "services/gap/ble_svc_gap.h"
#include <stdint.h>
#include <string.h>

static os_membuf_t sdu_coc_mem[COC_MBUF_MEMPOOL_SIZE];
static struct os_mempool sdu_coc_mbuf_mempool;
static struct os_mbuf_pool sdu_os_mbuf_pool;
uint16_t peer_sdu_size = 0;
struct ble_l2cap_chan *coc_chan = NULL;
static bool holding_channel = true;
static uint16_t coc_event_timeout = pdMS_TO_TICKS(2000);

QueueHandle_t coc_queue = NULL;
SemaphoreHandle_t coc_processing = NULL;
SemaphoreHandle_t coc_channel_ready = NULL;
SemaphoreHandle_t coc_operation_initiated = NULL;
QueueHandle_t coc_forward_event = NULL;

static void blecent_l2cap_coc_send_data(struct ble_l2cap_chan *chan) {
  struct os_mbuf *sdu_rx_data;
  int rc = 0;
  int len = 512 * 2;
  uint8_t value[len];

  for (int i = 0; i < len; i++) {
    value[i] = len - i - 1;
  }

  do {
    sdu_rx_data = os_mbuf_get_pkthdr(&sdu_os_mbuf_pool, 0);
    if (sdu_rx_data == NULL) {
      vTaskDelay(10 / portTICK_PERIOD_MS);
      sdu_rx_data = os_mbuf_get_pkthdr(&sdu_os_mbuf_pool, 0);
    }
  } while (sdu_rx_data == NULL);

  os_mbuf_append(sdu_rx_data, value, len);

  // print_mbuf_data(sdu_rx_data);

  rc = ble_l2cap_send(chan, sdu_rx_data);

  while (rc == BLE_HS_ESTALLED) {
    vTaskDelay(100 / portTICK_PERIOD_MS);
    rc = ble_l2cap_send(chan, sdu_rx_data);
  }
  if (rc == 0) {
    MODLOG_DFLT(INFO, "Data sent successfully");
  } else {
    MODLOG_DFLT(INFO, "Data sending failed, rc = %d", rc);
    os_mbuf_free_chain(sdu_rx_data);
  }
}

int bleprph_l2cap_coc_accept(uint16_t conn_handle, uint16_t peer_mtu,
                             struct ble_l2cap_chan *chan) {
  struct os_mbuf *sdu_rx;

  ESP_LOGI(TAG, "LE CoC accepting, chan: 0x%08lx, peer_mtu %d\n",
           (uint32_t)chan, peer_mtu);

  sdu_rx = os_mbuf_get_pkthdr(&sdu_os_mbuf_pool, 0);
  if (!sdu_rx) {
    return BLE_HS_ENOMEM;
  }

  return ble_l2cap_recv_ready(chan, sdu_rx);
}

static int default_coc_event_cb(struct ble_l2cap_event *event) {
  struct ble_l2cap_chan_info chan_info;
  ESP_LOGI(TAG, "LE COC default callback: %d\n");

  int rc = 0;

  assert(event != NULL);
  switch (event->type) {
  case BLE_L2CAP_EVENT_COC_CONNECTED:
    if (event->connect.status) {
      ESP_LOGI(TAG, "LE COC error: %d\n", event->connect.status);
      return 0;
    }

    if (ble_l2cap_get_chan_info(event->connect.chan, &chan_info)) {
      assert(0);
    }

    coc_chan = event->connect.chan;
    ESP_LOGI(TAG,
             "LE COC connected, conn: %d, chan: %p, psm: 0x%02x,"
             " scid: 0x%04x, "
             "dcid: 0x%04x, our_mps: %d, our_mtu: %d,"
             " peer_mps: %d, peer_mtu: %d\n",
             event->connect.conn_handle, event->connect.chan, chan_info.psm,
             chan_info.scid, chan_info.dcid, chan_info.our_l2cap_mtu,
             chan_info.our_coc_mtu, chan_info.peer_l2cap_mtu,
             chan_info.peer_coc_mtu);

    holding_channel = false;
    xSemaphoreGive(coc_channel_ready);
    return 0;

  case BLE_L2CAP_EVENT_COC_DISCONNECTED:
    rc = xSemaphoreTake(coc_channel_ready, portMAX_DELAY);
    assert(rc == pdTRUE);
    holding_channel = true;
    coc_chan = NULL;
    ESP_LOGI(TAG, "LE CoC disconnected, chan: %p\n", event->disconnect.chan);
    return 0;

  case BLE_L2CAP_EVENT_COC_ACCEPT:
    peer_sdu_size = event->accept.peer_sdu_size;

    bleprph_l2cap_coc_accept(event->accept.conn_handle,
                             event->accept.peer_sdu_size, event->accept.chan);
    MODLOG_DFLT(INFO, "Peer SDU Size = %d bytes", peer_sdu_size);
    return 0;
  default:
    assert(0);
    return 0;
  }
}

/**
 * The nimble host executes this callback when a L2CAP  event occurs.  The
 * application associates a L2CAP event callback with each connection that is
 * established.  blecent_l2cap_coc uses the same callback for all connections.
 *
 * @param event                 The event being signalled.
 * @param arg                   Application-specified argument; unused by
 *                                  blecent_l2cap_coc.
 *
 * @return                      0 if the application successfully handled the
 *                                  event; nonzero on failure.  The semantics
 *                                  of the return code is specific to the
 *                                  particular L2CAP event being signalled.
 */
int bleprph_l2cap_coc_event_cb(struct ble_l2cap_event *event, void *arg) {
  ESP_LOGI(TAG, "LE COC callback dispatcher: %d\n");

  int rc = 0;

  bool claimedEvent = (xSemaphoreTake(coc_operation_initiated, 0) == pdTRUE);

  if (!claimedEvent && !holding_channel) {
    rc = xQueueSend(coc_forward_event, event, 0);
    assert(rc == pdPASS);
    if (event->type == BLE_L2CAP_EVENT_COC_DISCONNECTED) {
      // Will stall until ready
      return default_coc_event_cb(event);
    }
    return 0;
  }

  if (claimedEvent) {
    xSemaphoreGive(coc_operation_initiated);
  }
  return default_coc_event_cb(event);
}

int await_coc_event(struct ble_l2cap_event *event) {
  // Set struct to zero
  // memset(event, 0, sizeof(*event));
  return xQueueReceive(coc_forward_event, event, coc_event_timeout);
}

struct ble_l2cap_chan *coc_acquire_channel() {
  if (xSemaphoreTake(coc_operation_initiated, pdMS_TO_TICKS(500)) != pdTRUE) {
    return NULL;
  }
  if (xSemaphoreTake(coc_channel_ready, pdMS_TO_TICKS(500)) != pdTRUE) {
    xSemaphoreGive(coc_operation_initiated);
    return NULL;
  }
  assert(coc_chan != NULL);
  return coc_chan;
}

void coc_release_channel() {
  xSemaphoreGive(coc_operation_initiated);
  xSemaphoreGive(coc_channel_ready);
}
void coc_force_close() {
  assert(coc_chan != NULL);
  ble_l2cap_disconnect(coc_chan);
  coc_chan = NULL;
  coc_release_channel();
}

struct os_mbuf *alloc_coc_mbuf() {
  struct os_mbuf *sdu_rx_data;
  sdu_rx_data = os_mbuf_get_pkthdr(&sdu_os_mbuf_pool, 0);

  while (sdu_rx_data == NULL) {
    vTaskDelay(10 / portTICK_PERIOD_MS);
    sdu_rx_data = os_mbuf_get_pkthdr(&sdu_os_mbuf_pool, 0);
  }

  return sdu_rx_data;
}

void ble_coc_worker_task(void *pvParameters) {
  assert(coc_processing != NULL);

  for (;;) {
    int rc = 0;
    coc_operation_t operation;

    // Allow OACP to request operation
    xSemaphoreGive(coc_processing);
    rc = xQueueReceive(coc_queue, &operation, portMAX_DELAY);
    assert(rc == pdTRUE);
    ESP_LOGI(TAG, "ble_coc_worker_task: received coc operation from OACP");

    oacp_request_worker(operation.conn_handle, operation.object,
                        &operation.request);
    unlockOtsTable();
  }
}

void bleprph_l2cap_coc_mem_init(void) {
  int rc;
  rc = os_mempool_init(&sdu_coc_mbuf_mempool, COC_BUF_COUNT,
                       COC_MBUF_MEMBLOCK_SIZE, sdu_coc_mem, "coc_sdu_pool");
  assert(rc == 0);
  rc = os_mbuf_pool_init(&sdu_os_mbuf_pool, &sdu_coc_mbuf_mempool,
                         COC_MBUF_MEMBLOCK_SIZE, COC_BUF_COUNT);
  assert(rc == 0);

  coc_queue = xQueueCreate(3, sizeof(coc_operation_t));
  assert(coc_queue != NULL);

  coc_forward_event = xQueueCreate(3, sizeof(struct ble_l2cap_event));
  assert(coc_forward_event != NULL);

  // Gives when ready
  coc_processing = xSemaphoreCreateBinary();
  assert(coc_processing != NULL);

  coc_channel_ready = xSemaphoreCreateBinary();
  assert(coc_channel_ready != NULL);

  // Must start free
  coc_operation_initiated = xSemaphoreCreateBinary();
  assert(coc_operation_initiated != NULL);
  xSemaphoreGive(coc_operation_initiated);

  vQueueAddToRegistry(coc_processing, "COC_PROCESSING");
  vQueueAddToRegistry(coc_channel_ready, "COC_CHANNEL_READY");
  vQueueAddToRegistry(coc_operation_initiated, "COC_OP_INITIATED");

  xTaskCreate(ble_coc_worker_task, "BLE COC WORKER", 4096, NULL, 6, NULL);
}
