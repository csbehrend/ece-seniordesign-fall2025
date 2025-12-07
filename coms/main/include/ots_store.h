/*
 * ============================================================================
 * Filename   : ots_store.h
 * Author     : Connor Behrend
 * Description:
 * ============================================================================
 */

#ifndef OTS_STORE_H
#define OTS_STORE_H

#include "common.h"
#include "freertos/idf_additions.h"
#include "os/os_mbuf.h"
#include "ots.h"
#include <stddef.h>
#include <stdint.h>

typedef struct ots_object_s ots_object_t;

typedef int oacp_callback_fn(uint16_t conn_handle, uint16_t attr_handle,
                             oacp_request_t *request, void *arg);

typedef int oacp_read_cb(ots_object_t *obj, struct os_mbuf *buf,
                         uint32_t offset, uint32_t len);

typedef struct {
  oacp_callback_fn *create;
  oacp_callback_fn *delete;
  oacp_callback_fn *checksum;
  oacp_callback_fn *execute;
  oacp_read_cb *read;
  oacp_callback_fn *write;
} ots_object_cb_t;

struct ots_object_s {
  void *base;
  const char *name;
  const uint8_t name_len;
  const ble_uuid_t *type;
  ots_object_size_t size;
  ots_object_luid_t id;
  const ots_object_properties_t properties;
  const ots_object_cb_t cb_func;
};

extern ots_object_t *g_obj_array;
extern size_t g_obj_array_len;
extern size_t g_obj_current;

int lockOtsTable();
void unlockOtsTable();
void ots_store_init();
int oacp_request_worker(uint16_t conn_handle, ots_object_t *object,
                        oacp_request_t *request);

#define CURRENT_OBJ() (&g_obj_array[g_obj_current])

#endif // OTS_STORE_H
