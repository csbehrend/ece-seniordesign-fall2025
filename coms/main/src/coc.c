#include "common.h"
#include "esp_log.h"
#include "nvs_flash.h"
/* BLE */
#include "coc.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "services/gap/ble_svc_gap.h"

static os_membuf_t sdu_coc_mem[COC_MBUF_MEMPOOL_SIZE];
static struct os_mempool sdu_coc_mbuf_mempool;
static struct os_mbuf_pool sdu_os_mbuf_pool;
static uint16_t peer_sdu_size;
static struct ble_l2cap_chan *coc_chan = NULL;

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

static int bleprph_l2cap_coc_accept(uint16_t conn_handle, uint16_t peer_mtu,
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
  struct ble_l2cap_chan_info chan_info;
  ESP_LOGI(TAG, "LE COC callback: %d\n");

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
    return 0;

  case BLE_L2CAP_EVENT_COC_DISCONNECTED:
    coc_chan = NULL;
    ESP_LOGI(TAG, "LE CoC disconnected, chan: %p\n", event->disconnect.chan);
    return 0;

  case BLE_L2CAP_EVENT_COC_DATA_RECEIVED:
    if (event->receive.sdu_rx != NULL) {
      MODLOG_DFLT(INFO,
                  "Data received (%d bytes): ", event->receive.sdu_rx->om_len);
      os_mbuf_free(event->receive.sdu_rx);
    }
    fflush(stdout);
    bleprph_l2cap_coc_accept(event->receive.conn_handle, peer_sdu_size,
                             event->receive.chan);

    blecent_l2cap_coc_send_data(coc_chan);
    return 0;

  case BLE_L2CAP_EVENT_COC_ACCEPT:
    peer_sdu_size = event->accept.peer_sdu_size;
    bleprph_l2cap_coc_accept(event->accept.conn_handle,
                             event->accept.peer_sdu_size, event->accept.chan);
    MODLOG_DFLT(INFO, "Peer SDU Size = %d bytes", peer_sdu_size);
    return 0;

  default:
    return 0;
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
}
