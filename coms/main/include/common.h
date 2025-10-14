/*
 * ============================================================================
 * Filename   : common.h
 * Author     : Connor Behrend
 * Description: Based on ESP-IDF example.
 * ============================================================================
 */

#ifndef COMMON_H
#define COMMON_H

/* Includes */
/* STD APIs */
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/* ESP APIs */
#include "esp_log.h"
#include "nvs_flash.h"
#include "sdkconfig.h"

/* FreeRTOS APIs */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/* NimBLE stack APIs */
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "host/util/util.h"
#include "nimble/ble.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"

/* Defines */
#define TAG "ReadyRightGlove_Server"
#define DEVICE_NAME "ReadyRightGlove"

// CHR Declare Macros
#define CHR_DECLARE_CALLBACK(name)                                             \
  int name##_chr_access(uint16_t conn_handle, uint16_t attr_handle,            \
                        struct ble_gatt_access_ctxt *ctxt, void *arg)

#define CHR_DECLARE_HANDLE(name) extern uint16_t name##_chr_handle;

#define _CHR_DECLARE_UUIDX(name, uuid_bits)                                    \
  extern const ble_uuid##uuid_bits##_t name##_chr_uuid

#define CHR_DECLARE_UUIDX(name, uuid_bits)                                     \
  _CHR_DECLARE_UUIDX(name, uuid_bits);                                         \
  CHR_DECLARE_CALLBACK(name);                                                  \
  CHR_DECLARE_HANDLE(name)

#define CHR_DECLARE_UUID16(name) CHR_DECLARE_UUIDX(name, 16)
#define CHR_DECLARE_UUID32(name) CHR_DECLARE_UUIDX(name, 32)
#define CHR_DECLARE_UUID128(name) CHR_DECLARE_UUIDX(name, 128)

// CHR Define Macros
#define CHR_DEFINE_HANDLE(name) extern uint16_t name##_chr_handle

#define _CHR_DEFINE_UUIDX(name, uuid_bits, uuidX...)                           \
  const ble_uuid##uuid_bits##_t name##_chr_uuid =                              \
      BLE_UUID##uuid_bits##_INIT(uuidX)

#define CHR_DEFINE_UUIDX(name, uuid_bits, uuidX...)                            \
  _CHR_DEFINE_UUIDX(name, uuid_bits, uuidX);                                   \
  CHR_DEFINE_HANDLE(name)

#define CHR_DEFINE_UUID16(name, uuidX...) CHR_DEFINE_UUIDX(name, 16, uuidX)
#define CHR_DEFINE_UUID32(name, uuidX...) CHR_DEFINE_UUIDX(name, 32, uuidX)
#define CHR_DEFINE_UUID128(name, uuidX...) CHR_DEFINE_UUIDX(name, 128, uuidX)

// SVC Declare Macros
#define SVC_DECLARE_UUIDX(name, uuid_bits)                                     \
  extern const ble_uuid##uuid_bits##_t name##_svc_uuid;

#define SVC_DECLARE_UUID16(name) SVC_DECLARE_UUIDX(name, 16)
#define SVC_DECLARE_UUID32(name) SVC_DECLARE_UUIDX(name, 32)
#define SVC_DECLARE_UUID128(name) SVC_DECLARE_UUIDX(name, 128)

// SVC Define Macros
#define SVC_DEFINE_UUIDX(name, uuid_bits, uuidX...)                            \
  const ble_uuid##uuid_bits##_t name##_svc_uuid =                              \
      BLE_UUID##uuid_bits##_INIT(uuidX)

#define SVC_DEFINE_UUID16(name, uuidX...) SVC_DEFINE_UUIDX(name, 16, uuidX)
#define SVC_DEFINE_UUID32(name, uuidX...) SVC_DEFINE_UUIDX(name, 32, uuidX)
#define SVC_DEFINE_UUID128(name, uuidX...) SVC_DEFINE_UUIDX(name, 128, uuidX)

#endif // COMMON_H
