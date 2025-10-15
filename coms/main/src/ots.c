#include "ots.h"
#include "common.h"

SVC_DEFINE_UUID16(ots, 0x1825);

CHR_DEFINE_UUID16(ots_feature, 0x2ABD);
CHR_DEFINE_UUID16(object_name, 0x2ABE);
CHR_DEFINE_UUID16(object_type, 0x2ABF);
CHR_DEFINE_UUID16(object_size, 0x2AC0);
// CHR_DEFINE_UUID16(object_first_created, 0x2AC1);
// CHR_DEFINE_UUID16(object_last_modified, 0x2AC2);
CHR_DEFINE_UUID16(object_id, 0x2AC3);
CHR_DEFINE_UUID16(object_properties, 0x2AC4);
CHR_DEFINE_UUID16(object_action_control_point, 0x2AC5);
// CHR_DEFINE_UUID16(object_list_control_point, 0x2AC6);
// CHR_DEFINE_UUID16(object_list_filter, 0x2AC7);
// CHR_DEFINE_UUID16(object_changed, 0x2AC8);

static const ots_feature_t ots_feature_chr_value = {
    .decoded = {.oacp.decoded = {.read = 1}}};

static object_size_t object_size_chr_value = {
    .decoded = {.allocated_size = 0xDEAD, .current_size = 0xBEEF}};

static inline void verify_conn_handle(uint16_t conn_handle,
                                      uint16_t attr_handle) {
  if (conn_handle != BLE_HS_CONN_HANDLE_NONE) {
    ESP_LOGI(TAG, "characteristic read; conn_handle=%d attr_handle=%d",
             conn_handle, attr_handle);
  } else {
    ESP_LOGI(TAG, "characteristic read by nimble stack; attr_handle=%d",
             attr_handle);
  }
}

int ots_feature_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                           struct ble_gatt_access_ctxt *ctxt, void *arg) {
  int rc = 0;

  // Handle access events
  switch (ctxt->op) {
  case BLE_GATT_ACCESS_OP_READ_CHR:
    verify_conn_handle(conn_handle, attr_handle);

    // Verify attribute handle
    if (attr_handle == ots_feature_chr_handle) {
      rc = os_mbuf_append(ctxt->om, &ots_feature_chr_value,
                          sizeof(ots_feature_chr_value));
      return rc ? BLE_ATT_ERR_INSUFFICIENT_RES : 0;
    }
    goto error;
  default:
    goto error;
  }

error:
  ESP_LOGE(TAG, "ots_feature_chr_access: unexpected operation, opcode: %d",
           ctxt->op);
  return BLE_ATT_ERR_UNLIKELY;
}

int object_name_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                           struct ble_gatt_access_ctxt *ctxt, void *arg) {
  return 0;
}

int object_type_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                           struct ble_gatt_access_ctxt *ctxt, void *arg) {
  return 0;
}

int object_size_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                           struct ble_gatt_access_ctxt *ctxt, void *arg) {
  int rc = 0;

  // Handle access events
  switch (ctxt->op) {
  case BLE_GATT_ACCESS_OP_READ_CHR:
    verify_conn_handle(conn_handle, attr_handle);

    // Verify attribute handle
    if (attr_handle == object_size_chr_handle) {
      rc = os_mbuf_append(ctxt->om, &object_size_chr_value,
                          sizeof(object_size_chr_value));
      return rc ? BLE_ATT_ERR_INSUFFICIENT_RES : 0;
    }
    goto error;
  default:
    goto error;
  }

error:
  ESP_LOGE(TAG, "object_size_chr_access: unexpected operation, opcode: %d",
           ctxt->op);
  return BLE_ATT_ERR_UNLIKELY;
}

int object_id_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                         struct ble_gatt_access_ctxt *ctxt, void *arg) {
  return 0;
}

int object_properties_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                                 struct ble_gatt_access_ctxt *ctxt, void *arg) {
  return 0;
}

int object_action_control_point_chr_access(uint16_t conn_handle,
                                           uint16_t attr_handle,
                                           struct ble_gatt_access_ctxt *ctxt,
                                           void *arg) {
  return 0;
  /*
  int rc = 0;

  // Handle access events
  switch (ctxt->op) {
  case BLE_GATT_ACCESS_OP_READ_CHR:
    verify_conn_handle(conn_handle, attr_handle);

    // Verify attribute handle
    if (attr_handle != object_size_chr_handle) {
      goto error;
    }
    rc = os_mbuf_append(ctxt->om, &object_size_chr_value,
                        sizeof(object_size_chr_value));
    return rc ? BLE_ATT_ERR_INSUFFICIENT_RES : 0;
  default:
    goto error;
  }

error:
  ESP_LOGE(TAG, "object_size_chr_access: unexpected operation, opcode: %d",
           ctxt->op);
  return BLE_ATT_ERR_UNLIKELY;
  */
}
