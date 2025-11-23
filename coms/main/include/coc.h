#ifndef COC_H
#define COC_H

#include "common.h"
#include "ots.h"

#define COC_BUF_COUNT (20 * MYNEWT_VAL(BLE_L2CAP_COC_MAX_NUM))
#define MTU 512

// https://mynewt.apache.org/latest/os/core_os/mbuf/mbuf.html

#define COC_MBUF_PAYLOAD_SIZE (MTU)
#define COC_MBUF_MAGICAL_NUMBER (24)
#define COC_MBUF_MEMBLOCK_SIZE (COC_MBUF_PAYLOAD_SIZE + COC_MBUF_MAGICAL_NUMBER)
#define COC_MBUF_MEMPOOL_SIZE                                                  \
  OS_MEMPOOL_SIZE(COC_BUF_COUNT, COC_MBUF_MEMBLOCK_SIZE)

int bleprph_l2cap_coc_event_cb(struct ble_l2cap_event *event, void *arg);
void bleprph_l2cap_coc_mem_init(void);

#endif // COC_H
