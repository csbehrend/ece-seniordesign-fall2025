#ifndef COC_H
#define COC_H

#include "common.h"
#include "freertos/idf_additions.h"
#include "ots.h"
#include "ots_store.h"
#include <stdint.h>

#define COC_BUF_COUNT (20 * MYNEWT_VAL(BLE_L2CAP_COC_MAX_NUM))
#define MTU 512

// https://mynewt.apache.org/latest/os/core_os/mbuf/mbuf.html

#define COC_MBUF_PAYLOAD_SIZE (MTU)
#define COC_MBUF_MAGICAL_NUMBER (24)
#define COC_MBUF_MEMBLOCK_SIZE (COC_MBUF_PAYLOAD_SIZE + COC_MBUF_MAGICAL_NUMBER)
#define COC_MBUF_MEMPOOL_SIZE                                                  \
  OS_MEMPOOL_SIZE(COC_BUF_COUNT, COC_MBUF_MEMBLOCK_SIZE)

extern struct ble_l2cap_chan *coc_chan;

extern QueueHandle_t coc_queue;
extern SemaphoreHandle_t coc_processing;
extern SemaphoreHandle_t coc_channel_ready;
extern SemaphoreHandle_t coc_operation_initiated;
extern QueueHandle_t coc_forward_event;

typedef struct {
  int16_t conn_handle;
  ots_object_t *object;
  oacp_request_t *request;
} coc_operation_t;

int coc_request_operation(coc_operation_t *operation);
int await_coc_event(struct ble_l2cap_event *event);
struct os_mbuf *alloc_coc_mbuf();

struct ble_l2cap_chan *coc_acquire_channel();
void coc_release_channel();
void coc_force_close();

int bleprph_l2cap_coc_event_cb(struct ble_l2cap_event *event, void *arg);
void bleprph_l2cap_coc_mem_init(void);

#endif // COC_H
