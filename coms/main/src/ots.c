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

int ots_feature_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                           struct ble_gatt_access_ctxt *ctxt, void *arg) {
  return 0;
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
  return 0;
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
}
