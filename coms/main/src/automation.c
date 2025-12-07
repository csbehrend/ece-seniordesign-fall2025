#include "automation.h"
#include "common.h"
#include "host/ble_gatt.h"
#include <stdint.h>

// c13f3ca3-55f5-4ffa-857a-1c3f40ccb300
SVC_DEFINE_UUID128(automation, 0x00, 0xb3, 0xcc, 0x40, 0x3f, 0x1c, 0x7a, 0x85,
                   0xfa, 0x4f, 0xf5, 0x55, 0xa3, 0x3c, 0x3f, 0xc1);

// 3861a947-7b94-495b-ae3d-c2d669d9f168
CHR_DEFINE_UUID128(auto_start, 0x68, 0xf1, 0xd9, 0x69, 0xd6, 0xc2, 0x3d, 0xae,
                   0x5b, 0x49, 0x94, 0x7b, 0x47, 0xa9, 0x61, 0x38);
/*
// f9b94111-c76a-42a3-92a8-8609cfd2c28f
CHR_DEFINE_UUID128(auto_pause, 0x8f, 0xc2, 0xd2, 0xcf, 0x09, 0x86, 0xa8, 0x92,
                   0xa3, 0x42, 0x6a, 0xc7, 0x11, 0x41, 0xb9, 0xf9);
// ca11594c-0a4d-40dc-994c-b64e2b556e74
CHR_DEFINE_UUID128(auto_cancel, 0x74, 0x6e, 0x55, 0x2b, 0x4e, 0xb6, 0x4c, 0x99,
                   0xdc, 0x40, 0x4d, 0x0a, 0x4c, 0x59, 0x11, 0xca);
*/

int auto_start_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                          struct ble_gatt_access_ctxt *ctxt, void *arg) {
  int rc = 0;

  if (ctxt->op != BLE_GATT_ACCESS_OP_WRITE_CHR) {
    ESP_LOGE(TAG, "object_type_chr_access: unexpected operation, opcode: %d",
             ctxt->op);
    return BLE_ATT_ERR_UNLIKELY;
  }
  verify_conn_handle(conn_handle, attr_handle);

  // Verify attribute handle
  if (attr_handle != auto_start_chr_handle) {
    ESP_LOGE(TAG, "auto_start_chr_access: invalid attr_handle %d", attr_handle);
    assert(0);
    return BLE_ATT_ERR_UNLIKELY;
  }

  uint16_t len = 0;
  uint8_t msg;
  rc = ble_hs_mbuf_to_flat(ctxt->om, &msg, sizeof(msg), &len);
  if (rc || len != sizeof(msg)) {
    ESP_LOGE(TAG, "auto_start_chr_access: bad mbuf read or bad payload size");
    return BLE_ATT_ERR_INSUFFICIENT_RES;
  }

  ESP_LOGI(TAG, "auto_start_chr_handle: start exercise id %d", msg);

  return rc;
}
