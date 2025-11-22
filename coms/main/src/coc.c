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

uint16_t psm = 0x1002;
static os_membuf_t sdu_coc_mem[OS_MEMPOOL_SIZE(COC_BUF_COUNT, MTU)];
static struct os_mempool sdu_coc_mbuf_mempool;
static struct os_mbuf_pool sdu_os_mbuf_pool;
static uint16_t peer_sdu_size;

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
    ESP_LOGI(TAG, "LE CoC disconnected, chan: %p\n", event->disconnect.chan);

    return 0;

  case BLE_L2CAP_EVENT_COC_DATA_RECEIVED:
    if (event->receive.sdu_rx != NULL) {
      MODLOG_DFLT(INFO, "Data received : ");
      for (int i = 0; i < event->receive.sdu_rx->om_len; i++) {
        ESP_LOGI(TAG, "%d ", event->receive.sdu_rx->om_data[i]);
      }
      os_mbuf_free(event->receive.sdu_rx);
    }
    fflush(stdout);
    bleprph_l2cap_coc_accept(event->receive.conn_handle, peer_sdu_size,
                             event->receive.chan);
    return 0;

  case BLE_L2CAP_EVENT_COC_ACCEPT:
    peer_sdu_size = event->accept.peer_sdu_size;
    bleprph_l2cap_coc_accept(event->accept.conn_handle,
                             event->accept.peer_sdu_size, event->accept.chan);
    return 0;

  default:
    return 0;
  }
}
void bleprph_l2cap_coc_mem_init(void) {
  int rc;
  rc = os_mempool_init(&sdu_coc_mbuf_mempool, COC_BUF_COUNT, MTU, sdu_coc_mem,
                       "coc_sdu_pool");
  assert(rc == 0);
  rc = os_mbuf_pool_init(&sdu_os_mbuf_pool, &sdu_coc_mbuf_mempool, MTU,
                         COC_BUF_COUNT);
  assert(rc == 0);
}
