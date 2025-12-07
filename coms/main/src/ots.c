#include "ots.h"
#include "coc.h"
#include "common.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "host/ble_att.h"
#include "host/ble_gatt.h"
#include "host/ble_hs_mbuf.h"
#include "host/ble_uuid.h"
#include "os/os_mbuf.h"
#include "ots_store.h"
#include "portmacro.h"
#include <stdbool.h>
#include <stdint.h>

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
CHR_DEFINE_UUID16(object_list_control_point, 0x2AC6);
// CHR_DEFINE_UUID16(object_list_filter, 0x2AC7);
// CHR_DEFINE_UUID16(object_changed, 0x2AC8);
bool object_action_control_point_ind_status = false;
bool object_list_control_point_ind_status = false;

// static const ots_feature_t ots_feature_chr_value = {
// .decoded = {.oacp.decoded = {.read = 1, .write = 1}}};
static const ots_feature_t ots_feature_chr_value = {
    .decoded = {.oacp.decoded = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}};

void verify_conn_handle(uint16_t conn_handle, uint16_t attr_handle) {
  if (conn_handle != BLE_HS_CONN_HANDLE_NONE) {
    ESP_LOGI(TAG, "characteristic read; conn_handle=%d attr_handle=%d",
             conn_handle, attr_handle);
  } else {
    ESP_LOGI(TAG, "characteristic read by nimble stack; attr_handle=%d",
             attr_handle);
  }
}

static bool oacp_request_is_valid_size(oacp_request_t *request,
                                       uint16_t length) {
  assert(request != NULL);
  if (length < sizeof(request->op)) {
    return false;
  }

  switch (request->op) {
  case OACP_OP_READ:
  case OACP_OP_CHECKSUM:
    if (length !=
        sizeof(request->op) + sizeof(request->parameter.checksum_read_param)) {
      return false;
    }
    break;
  case OACP_OP_WRITE:
    if (length !=
        sizeof(request->op) + sizeof(request->parameter.write_param)) {
      return false;
    }
    break;
  default:
    ESP_LOGE(TAG, "oacp_request_is_valid_size: invalid OACP opcode %d",
             request->op);
    return false;
  }

  return true;
}

// Return true if supported
static bool check_oacp_op_supported(oacp_opcode_t op) {
  switch (op) {
  case OACP_OP_CREATE:
    return ots_feature_chr_value.decoded.oacp.decoded.create;
  case OACP_OP_DELETE:
    return ots_feature_chr_value.decoded.oacp.decoded.delete;
  case OACP_OP_CHECKSUM:
    return ots_feature_chr_value.decoded.oacp.decoded.checksum;
  case OACP_OP_EXECUTE:
    return ots_feature_chr_value.decoded.oacp.decoded.execute;
  case OACP_OP_READ:
    return ots_feature_chr_value.decoded.oacp.decoded.read;
  case OACP_OP_WRITE:
    return ots_feature_chr_value.decoded.oacp.decoded.write;
  default:
    return false;
  }
}

int ots_feature_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                           struct ble_gatt_access_ctxt *ctxt, void *arg) {
  int rc = 0;

  if (ctxt->op != BLE_GATT_ACCESS_OP_READ_CHR) {
    ESP_LOGE(TAG, "ots_feature_chr_access: unexpected operation, opcode: %d",
             ctxt->op);
    return BLE_ATT_ERR_UNLIKELY;
  }

  verify_conn_handle(conn_handle, attr_handle);

  // Verify attribute handle
  if (attr_handle != ots_feature_chr_handle) {
    ESP_LOGE(TAG, "ots_feature_chr_access: bad attr_handle");
    return BLE_ATT_ERR_UNLIKELY;
  }

  rc = os_mbuf_append(ctxt->om, &ots_feature_chr_value,
                      sizeof(ots_feature_chr_value));
  return rc ? BLE_ATT_ERR_INSUFFICIENT_RES : 0;
}

int object_name_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                           struct ble_gatt_access_ctxt *ctxt, void *arg) {
  int rc = 0;

  // Handle access events
  if (ctxt->op != BLE_GATT_ACCESS_OP_READ_CHR) {
    ESP_LOGE(TAG, "object_name_chr_access: unexpected operation, opcode: %d",
             ctxt->op);
    return BLE_ATT_ERR_UNLIKELY;
  }
  verify_conn_handle(conn_handle, attr_handle);

  // Verify attribute handle
  if (attr_handle != object_name_chr_handle) {
    ESP_LOGE(TAG, "object_name_chr_access: bad attr_handle");
    return BLE_ATT_ERR_UNLIKELY;
  }

  ots_object_t *obj = CURRENT_OBJ();
  if (lockOtsTable() != pdTRUE) {
    return BLE_ATT_ERR_UNLIKELY;
  }

  rc = os_mbuf_append(ctxt->om, obj->name, obj->name_len);
  unlockOtsTable();
  return rc ? BLE_ATT_ERR_INSUFFICIENT_RES : 0;
}

int object_type_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                           struct ble_gatt_access_ctxt *ctxt, void *arg) {
  int rc = 0;

  // Handle access events
  if (ctxt->op != BLE_GATT_ACCESS_OP_READ_CHR) {
    ESP_LOGE(TAG, "object_type_chr_access: unexpected operation, opcode: %d",
             ctxt->op);
    return BLE_ATT_ERR_UNLIKELY;
  }
  verify_conn_handle(conn_handle, attr_handle);

  // Verify attribute handle
  if (attr_handle != object_type_chr_handle) {
    ESP_LOGE(TAG, "object_type_chr_access: bad attr_handle");
    return BLE_ATT_ERR_UNLIKELY;
  }

  ots_object_t *obj = CURRENT_OBJ();
  if (lockOtsTable() != pdTRUE) {
    return BLE_ATT_ERR_UNLIKELY;
  }
  const ble_uuid_t *uuid = obj->type;
  assert(uuid);

  switch (uuid->type) {
  /*
  case BLE_UUID_TYPE_16:
    rc = os_mbuf_append(ctxt->om, &((ble_uuid16_t *)uuid)->value,
                        sizeof(((ble_uuid16_t *)uuid)->value));
    break;
  case BLE_UUID_TYPE_32:
    rc = os_mbuf_append(ctxt->om, &((ble_uuid32_t *)uuid)->value,
                        sizeof(((ble_uuid32_t *)uuid)->value));
    break;
    */
  case BLE_UUID_TYPE_128:
    rc = os_mbuf_append(ctxt->om, &((ble_uuid128_t *)uuid)->value,
                        sizeof(((ble_uuid128_t *)uuid)->value));
    break;
  default:
    ESP_LOGE(TAG, "object_type_chr_access: bad uuid type %d", obj->type->type);
    unlockOtsTable();
    return BLE_ATT_ERR_UNLIKELY;
  }

  unlockOtsTable();
  return rc ? BLE_ATT_ERR_INSUFFICIENT_RES : 0;
}

int object_size_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                           struct ble_gatt_access_ctxt *ctxt, void *arg) {
  int rc = 0;

  // Handle access events
  if (ctxt->op != BLE_GATT_ACCESS_OP_READ_CHR) {
    ESP_LOGE(TAG, "object_size_chr_access: unexpected operation, opcode: %d",
             ctxt->op);
    return BLE_ATT_ERR_UNLIKELY;
  }
  verify_conn_handle(conn_handle, attr_handle);

  // Verify attribute handle
  if (attr_handle != object_size_chr_handle) {
    ESP_LOGE(TAG, "object_size_chr_access: bad attr_handle");
    return BLE_ATT_ERR_UNLIKELY;
  }

  ots_object_t *obj = CURRENT_OBJ();
  if (lockOtsTable() != pdTRUE) {
    return BLE_ATT_ERR_UNLIKELY;
  }
  rc = os_mbuf_append(ctxt->om, &obj->size, sizeof(obj->size));
  unlockOtsTable();
  return rc ? BLE_ATT_ERR_INSUFFICIENT_RES : 0;
}

int object_id_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                         struct ble_gatt_access_ctxt *ctxt, void *arg) {
  int rc = 0;

  // Handle access events
  if (ctxt->op != BLE_GATT_ACCESS_OP_READ_CHR) {
    ESP_LOGE(TAG, "object_id_chr_access: unexpected operation, opcode: %d",
             ctxt->op);
    return BLE_ATT_ERR_UNLIKELY;
  }
  verify_conn_handle(conn_handle, attr_handle);

  // Verify attribute handle
  if (attr_handle != object_id_chr_handle) {
    ESP_LOGE(TAG, "object_id_chr_access: bad attr_handle");
    return BLE_ATT_ERR_UNLIKELY;
  }

  ots_object_t *obj = CURRENT_OBJ();
  if (lockOtsTable() != pdTRUE) {
    return BLE_ATT_ERR_UNLIKELY;
  }
  rc = os_mbuf_append(ctxt->om, &obj->id.luid, sizeof(obj->id.luid));
  unlockOtsTable();
  return rc ? BLE_ATT_ERR_INSUFFICIENT_RES : 0;
}

int object_properties_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                                 struct ble_gatt_access_ctxt *ctxt, void *arg) {
  int rc = 0;

  // Handle access events
  if (ctxt->op != BLE_GATT_ACCESS_OP_READ_CHR) {
    ESP_LOGE(TAG,
             "object_properties_chr_access: unexpected operation, opcode: %d",
             ctxt->op);
    return BLE_ATT_ERR_UNLIKELY;
  }
  verify_conn_handle(conn_handle, attr_handle);

  // Verify attribute handle
  if (attr_handle != object_properties_chr_handle) {
    ESP_LOGE(TAG, "object_properties_chr_access: bad attr_handle");
    return BLE_ATT_ERR_UNLIKELY;
  }

  ots_object_t *obj = CURRENT_OBJ();
  if (lockOtsTable() != pdTRUE) {
    return BLE_ATT_ERR_UNLIKELY;
  }
  rc = os_mbuf_append(ctxt->om, &obj->properties, sizeof(obj->properties));
  unlockOtsTable();
  return rc ? BLE_ATT_ERR_INSUFFICIENT_RES : 0;
}

int object_action_control_point_chr_access(uint16_t conn_handle,
                                           uint16_t attr_handle,
                                           struct ble_gatt_access_ctxt *ctxt,
                                           void *arg) {
  int rc = 0;
  oacp_request_t request = {0};
  oacp_response_t response = {0};
  struct os_mbuf *txom = NULL;

  // Will always be a write
  if (ctxt->op != BLE_GATT_ACCESS_OP_WRITE_CHR) {
    ESP_LOGE(TAG,
             "object_action_control_point_chr_access: unexpected operation, "
             "opcode: %d",
             ctxt->op);
    assert(0);
    return BLE_ATT_ERR_UNLIKELY;
  }

  // Handle access events
  verify_conn_handle(conn_handle, attr_handle);

  // Verify attribute handle
  if (attr_handle != object_action_control_point_chr_handle) {
    ESP_LOGE(TAG,
             "object_action_control_point_chr_access: invalid attr_handle %d",
             attr_handle);
    assert(0);
    return BLE_ATT_ERR_UNLIKELY;
  }

  // Read request
  uint16_t len = 0;
  rc = ble_hs_mbuf_to_flat(ctxt->om, &request, sizeof(request), &len);
  if (rc) {
    response.result = OACP_RESULT_INSUFFICIENT_RESOURCES;
    ESP_LOGE(TAG, "object_action_control_point_chr_access: bad mbuf read");
    goto indicate;
  }

  assert(len > 0);

  // Update the response struct
  response.op = request.op;

  if (!check_oacp_op_supported(request.op)) {
    response.result = OACP_RESULT_UNSUPP_OP;
    ESP_LOGE(TAG,
             "object_action_control_point_chr_access: unsupported opcode %d",
             request.op);
    goto indicate;
  }
  if (!oacp_request_is_valid_size(&request, len)) {
    response.result = OACP_RESULT_INV_PARAM;
    ESP_LOGE(
        TAG,
        "object_action_control_point_chr_access: invalid parameter size %d",
        len);
    goto indicate;
  }

indicate:
  txom = ble_hs_mbuf_from_flat(&response, sizeof(response));
  if (!txom) {
    return BLE_ATT_ERR_INSUFFICIENT_RES;
  }
  ble_gatts_indicate_custom(conn_handle, object_action_control_point_chr_handle,
                            txom);
  return rc;
}

int object_list_control_point_chr_access(uint16_t conn_handle,
                                         uint16_t attr_handle,
                                         struct ble_gatt_access_ctxt *ctxt,
                                         void *arg) {
  int rc = 0;
  olcp_request_t request = {0};
  olcp_response_t response = {0};
  struct os_mbuf *txom = NULL;

  // Will always be a write
  if (ctxt->op != BLE_GATT_ACCESS_OP_WRITE_CHR) {
    ESP_LOGE(TAG,
             "object_list_control_point_chr_access: unexpected operation, "
             "opcode: %d",
             ctxt->op);
    assert(0);
    return BLE_ATT_ERR_UNLIKELY;
  }

  // Handle access events
  verify_conn_handle(conn_handle, attr_handle);

  // Verify attribute handle
  if (attr_handle != object_list_control_point_chr_handle) {
    ESP_LOGE(TAG,
             "object_list_control_point_chr_access: invalid attr_handle %d",
             attr_handle);
    assert(0);
    return BLE_ATT_ERR_UNLIKELY;
  }

  // Read request
  uint16_t len = 0;
  rc = ble_hs_mbuf_to_flat(ctxt->om, &request, sizeof(request), &len);
  if (rc || len != sizeof(request)) {
    ESP_LOGE(TAG, "object_list_control_point_chr_access: bad mbuf read or bad "
                  "request size");
    return BLE_ATT_ERR_INSUFFICIENT_RES;
  }

  // Update the response struct
  response.op = request.op;

  // TODO: check global enable for opcode

  // TODO: add length and object property check (currently ignored)
  response.result = OACP_RESULT_SUCCESS;
  switch (request.op) {
  case OLCP_OP_FIRST:
    ESP_LOGI(TAG, "object_list_control_point_chr_access: FIRST");
    g_obj_current = 0;
    goto indicate;
  case OLCP_OP_LAST:
    ESP_LOGI(TAG, "object_list_control_point_chr_access: LAST");
    g_obj_current = g_obj_array_len - 1;
    goto indicate;
  case OLCP_OP_PREVIOUS:
    ESP_LOGI(TAG, "object_list_control_point_chr_access: PREVIOUS");
    if (!g_obj_current) {
      response.result = OLCP_RESULT_OUT_OF_BOUNDS;
      goto indicate;
    }
    g_obj_current--;
    goto indicate;
  case OLCP_OP_NEXT:
    ESP_LOGI(TAG, "object_list_control_point_chr_access: NEXT");
    if (g_obj_current >= g_obj_array_len - 1) {
      response.result = OLCP_RESULT_OUT_OF_BOUNDS;
      goto indicate;
    }
    g_obj_current++;
    goto indicate;
  default:
    response.result = OACP_RESULT_UNSUPP_OP;
    ESP_LOGE(TAG,
             "object_list_control_point_chr_access: invalid OLCP opcode %d "
             "past support check ",
             request.op);
    goto indicate;
  }

indicate:
  if (!object_list_control_point_ind_status) {
    ESP_LOGI(TAG, "object_list_control_point_chr_access: not subscribed to "
                  "indications");
    return rc;
  }

  txom = ble_hs_mbuf_from_flat(&response, sizeof(response));
  if (!txom) {
    return BLE_ATT_ERR_INSUFFICIENT_RES;
  }
  ble_gatts_indicate_custom(conn_handle, object_list_control_point_chr_handle,
                            txom);
  return rc;
}
