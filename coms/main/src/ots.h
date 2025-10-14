#ifndef OTS_H
#define OTS_H

/* Includes */
#include "common.h"

SVC_DECLARE_UUID16(ots)
CHR_DECLARE_UUID16(ots_feature);
CHR_DECLARE_UUID16(object_name);
CHR_DECLARE_UUID16(object_type);
CHR_DECLARE_UUID16(object_size);
CHR_DECLARE_UUID16(object_first_created);
CHR_DECLARE_UUID16(object_last_modified);
CHR_DECLARE_UUID16(object_id);
CHR_DECLARE_UUID16(object_properties);
CHR_DECLARE_UUID16(object_action_control_point);
CHR_DECLARE_UUID16(object_list_control_point);
CHR_DECLARE_UUID16(object_list_filter);
CHR_DECLARE_UUID16(object_changed);

/*
int ots_feature_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                           struct ble_gatt_access_ctxt *ctxt, void *arg);
int object_name_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                           struct ble_gatt_access_ctxt *ctxt, void *arg);
int object_type_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                           struct ble_gatt_access_ctxt *ctxt, void *arg);
int object_size_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                           struct ble_gatt_access_ctxt *ctxt, void *arg);
int object_first_created_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                                    struct ble_gatt_access_ctxt *ctxt,
                                    void *arg);
int object_last_modified_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                                    struct ble_gatt_access_ctxt *ctxt,
                                    void *arg);
int object_id_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                         struct ble_gatt_access_ctxt *ctxt, void *arg);
int object_properties_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                                 struct ble_gatt_access_ctxt *ctxt, void *arg);
int object_action_control_point_chr_access(uint16_t conn_handle,
                                           uint16_t attr_handle,
                                           struct ble_gatt_access_ctxt *ctxt,
                                           void *arg);
int object_list_control_point_chr_access(uint16_t conn_handle,
                                         uint16_t attr_handle,
                                         struct ble_gatt_access_ctxt *ctxt,
                                         void *arg);
int object_list_filter_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                                  struct ble_gatt_access_ctxt *ctxt, void *arg);
int object_changed_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                              struct ble_gatt_access_ctxt *ctxt, void *arg);

extern uint16_t ots_feature_chr_handle;
extern uint16_t object_name_chr_handle;
extern uint16_t object_type_chr_handle;
extern uint16_t object_size_chr_handle;
extern uint16_t object_first_created_chr_handle;
extern uint16_t object_last_modified_chr_handle;
extern uint16_t object_id_chr_handle;
extern uint16_t object_properties_chr_handle;
extern uint16_t object_action_control_point_chr_handle;
extern uint16_t object_list_control_point_chr_handle;
extern uint16_t object_list_filter_chr_handle;
extern uint16_t object_changed_chr_handle;

static const ble_uuid16_t ots_svc_uuid = BLE_UUID16_INIT(0x1825);

static const ble_uuid16_t ots_feature_chr_uuid = BLE_UUID16_INIT(0x2ABD);
static const ble_uuid16_t object_name_chr_uuid = BLE_UUID16_INIT(0x2ABE);
static const ble_uuid16_t object_type_chr_uuid = BLE_UUID16_INIT(0x2ABF);
static const ble_uuid16_t object_size_chr_uuid = BLE_UUID16_INIT(0x2AC0);
static const ble_uuid16_t object_first_created_chr_uuid =
    BLE_UUID16_INIT(0x2AC1);
static const ble_uuid16_t object_last_modified_chr_uuid =
    BLE_UUID16_INIT(0x2AC2);
static const ble_uuid16_t object_id_chr_uuid = BLE_UUID16_INIT(0x2AC3);
static const ble_uuid16_t object_properties_chr_uuid = BLE_UUID16_INIT(0x2AC4);
static const ble_uuid16_t object_action_control_point_chr_uuid =
    BLE_UUID16_INIT(0x2AC5);
static const ble_uuid16_t object_list_control_point_chr_uuid =
    BLE_UUID16_INIT(0x2AC6);
static const ble_uuid16_t object_list_filter_chr_uuid = BLE_UUID16_INIT(0x2AC7);
static const ble_uuid16_t object_changed_chr_uuid = BLE_UUID16_INIT(0x2AC8);
*/

#endif // OTS_H
