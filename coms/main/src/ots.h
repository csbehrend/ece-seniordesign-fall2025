#ifndef OTS_H
#define OTS_H

/* Includes */
#include "common.h"

SVC_DECLARE_UUID16(ots);

CHR_DECLARE_UUID16(ots_feature);
CHR_DECLARE_UUID16(object_name);
CHR_DECLARE_UUID16(object_type);
CHR_DECLARE_UUID16(object_size);
// CHR_DECLARE_UUID16(object_first_created);
// CHR_DECLARE_UUID16(object_last_modified);
CHR_DECLARE_UUID16(object_id);
CHR_DECLARE_UUID16(object_properties);
CHR_DECLARE_UUID16(object_action_control_point);
// CHR_DECLARE_UUID16(object_list_control_point);
// CHR_DECLARE_UUID16(object_list_filter);
// CHR_DECLARE_UUID16(object_changed);

#define OTS_SERVICE_ENTRY()                                                    \
  GATT_SVC_ENTRY_BEGIN(ots) {                                                  \
    GATT_CHR_ENTRY(ots_feature, BLE_GATT_CHR_F_READ),                          \
        GATT_CHR_ENTRY(object_name, BLE_GATT_CHR_F_READ),                      \
        GATT_CHR_ENTRY(object_type, BLE_GATT_CHR_F_READ),                      \
        GATT_CHR_ENTRY(object_size, BLE_GATT_CHR_F_READ),                      \
        GATT_CHR_ENTRY(object_id, BLE_GATT_CHR_F_READ),                        \
        GATT_CHR_ENTRY(object_properties, BLE_GATT_CHR_F_READ),                \
        GATT_CHR_ENTRY(object_action_control_point, BLE_GATT_CHR_F_READ),      \
        GATT_SVC_ENTRY_END()

#endif // OTS_H
