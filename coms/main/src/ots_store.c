/*
 * ============================================================================
 * Filename   : ots_store.c
 * Author     : Connor Behrend
 * Description:
 * ============================================================================
 */

#include "ots_store.h"
#include "host/ble_uuid.h"

static volatile uint32_t m_testval = 0;

static int testval_read(uint16_t conn_handle, uint16_t attr_handle,
                        oacp_request_t *request, void *arg);

static const char testval_name[] = "TestVal";

static ots_object_t test[] = {
    {.name = testval_name,
     .name_len = sizeof(testval_name),
     .type =
         BLE_UUID128_DECLARE(0xc1, 0x8c, 0xe9, 0x55, 0x0c, 0xf6, 0xec, 0x8a,
                             0x2d, 0x44, 0xf3, 0x30, 0x8d, 0x70, 0x5d, 0xc1),
     .size.decoded = {.current_size = sizeof(m_testval),
                      .allocated_size = sizeof(m_testval)},
     .id.luid = {0xce, 0x70, 0xe2, 0xf8, 0x8d, 0xc1},
     .properties.decoded = {.read = 1},
     .cb_func = {.read = testval_read}}};

ots_object_t *g_obj_array = test;
uint8_t g_obj_array_len = sizeof(test) / sizeof(test[0]);
uint8_t g_obj_current = 0;

static int testval_read(uint16_t conn_handle, uint16_t attr_handle,
                        oacp_request_t *request, void *arg) {
  return 0;
}
