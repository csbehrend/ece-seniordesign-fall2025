#include "ots.h"
#include "common.h"
#include "host/ble_att.h"
#include "host/ble_gatt.h"
#include "host/ble_hs_mbuf.h"
#include "os/os_mbuf.h"
#include <stdbool.h>

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
  int rc = 0;
  oacp_request_t request = {0};
  oacp_response_t response = {0};

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

  // Update the response struct
  response.op = request.op;

  if (!check_oacp_op_supported(request.op)) {
  }

  switch (request.op) {
  case OACP_OP_READ:
    response.result = OACP_RESULT_SUCCESS;
    if (len !=
        sizeof(request.op) + sizeof(request.parameter.checksum_read_param)) {
      response.result = OACP_RESULT_INV_PARAM;
      ESP_LOGE(TAG,
               "object_action_control_point_chr_access: op_read bad len %d",
               len);
      goto indicate;
    }
    break;
    /*
    case OACP_OP_CREATE:
    case OACP_OP_DELETE = 0x02,
    case OACP_OP_CHECKSUM = 0x03,
    case OACP_OP_EXECUTE = 0x04,
    case OACP_OP_READ = 0x05,
    case OACP_OP_WRITE = 0x06,
    */
  default:
    response.result = OACP_RESULT_UNSUPP_OP;
    ESP_LOGE(TAG,
             "object_action_control_point_chr_access: invalid OACP opcode %d "
             "past support check ",
             request.op);
  }

indicate:
  struct os_mbuf *txom = ble_hs_mbuf_from_flat(&response, sizeof(response));
  if (!txom) {
    return BLE_ATT_ERR_INSUFFICIENT_RES;
  }
  ble_gatts_indicate_custom(conn_handle, object_action_control_point_chr_handle,
                            txom);
  return rc;
}
