#ifndef COC_H
#define COC_H

#include "common.h"
#include "ots.h"

#define COC_BUF_COUNT (20 * MYNEWT_VAL(BLE_L2CAP_COC_MAX_NUM))
#define MTU 512

int bleprph_l2cap_coc_event_cb(struct ble_l2cap_event *event, void *arg);
void bleprph_l2cap_coc_mem_init(void);

#endif // COC_H
