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
#include "ots.h"

typedef int oacp_callback_fn(uint16_t conn_handle, uint16_t attr_handle,
                             oacp_request_t *request, void *arg);

typedef struct {
  oacp_callback_fn *create;
  oacp_callback_fn *delete;
  oacp_callback_fn *checksum;
  oacp_callback_fn *execute;
  oacp_callback_fn *read;
  oacp_callback_fn *write;
} ots_object_cb_t;

typedef struct {
  const char *name;
  const uint8_t name_len;
  const ble_uuid_t *type;
  ots_object_size_t size;
  ots_object_luid_t id;
  const ots_object_properties_t properties;
  const ots_object_cb_t cb_func;
} ots_object_t;

extern ots_object_t *g_obj_array;
extern uint8_t g_obj_array_len;
extern uint8_t g_obj_current;

#define CURRENT_OBJ() (&g_obj_array[g_obj_current])

#endif // OTS_STORE_H
