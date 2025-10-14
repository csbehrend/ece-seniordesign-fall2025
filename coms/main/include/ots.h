#ifndef OTS_H
#define OTS_H

/* Includes */
#include "common.h"
#include "esp_attr.h"
#include <stdint.h>

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

typedef union PACKED_ATTR {
  struct PACKED_ATTR {
    bool create : 1;
    bool delete : 1;
    bool checksum : 1;
    bool execute : 1;
    bool read : 1;
    bool write : 1;
    bool append : 1;
    bool truncate : 1;
    bool patch : 1;
    bool abort : 1;
  } decoded;
  uint32_t raw;
} ots_feature_val_oacp_t;

typedef union PACKED_ATTR {
  struct PACKED_ATTR {
    bool go_to : 1;
    bool order : 1;
    bool request_num_objs : 1;
    bool clear_marking : 1;
  } decoded;
  uint32_t raw;
} ots_feature_val_olcp_t;

typedef union PACKED_ATTR {
  struct PACKED_ATTR {
    ots_feature_val_oacp_t oacp;
    ots_feature_val_olcp_t olcp;
  } decoded;
  uint64_t raw;
} ots_feature_val_t;

// OTS Feature OACP
#define OTS_FEATURE_SUPPORTED_OACP_CREATE_Pos (0U)
#define OTS_FEATURE_SUPPORTED_OACP_CREATE                                      \
  (0x1UL << OTS_FEATURE_SUPPORTED_OACP_CREATE_Pos)

#define OTS_FEATURE_SUPPORTED_OACP_DELETE_Pos (1U)
#define OTS_FEATURE_SUPPORTED_OACP_DELETE                                      \
  (0x1UL << OTS_FEATURE_SUPPORTED_OACP_DELETE_Pos)

#define OTS_FEATURE_SUPPORTED_OACP_CHECKSUM_Pos (2U)
#define OTS_FEATURE_SUPPORTED_OACP_CHECKSUM                                    \
  (0x1UL << OTS_FEATURE_SUPPORTED_OACP_CHECKSUM_Pos)

#define OTS_FEATURE_SUPPORTED_OACP_EXECUTE_Pos (3U)
#define OTS_FEATURE_SUPPORTED_OACP_EXECUTE                                     \
  (0x1UL << OTS_FEATURE_SUPPORTED_OACP_EXECUTE_Pos)

#define OTS_FEATURE_SUPPORTED_OACP_READ_Pos (4U)
#define OTS_FEATURE_SUPPORTED_OACP_READ                                        \
  (0x1UL << OTS_FEATURE_SUPPORTED_OACP_READ_Pos)

#define OTS_FEATURE_SUPPORTED_OACP_WRITE_Pos (5U)
#define OTS_FEATURE_SUPPORTED_OACP_WRITE                                       \
  (0x1UL << OTS_FEATURE_SUPPORTED_OACP_WRITE_Pos)

#define OTS_FEATURE_SUPPORTED_OACP_APPEND_Pos (6U)
#define OTS_FEATURE_SUPPORTED_OACP_APPEND                                      \
  (0x1UL << OTS_FEATURE_SUPPORTED_OACP_APPEND_Pos)

#define OTS_FEATURE_SUPPORTED_OACP_TRUNCATE_Pos (7U)
#define OTS_FEATURE_SUPPORTED_OACP_TRUNCATE                                    \
  (0x1UL << OTS_FEATURE_SUPPORTED_OACP_TRUNCATE_Pos)

#define OTS_FEATURE_SUPPORTED_OACP_PATCH_Pos (8U)
#define OTS_FEATURE_SUPPORTED_OACP_PATCH                                       \
  (0x1UL << OTS_FEATURE_SUPPORTED_OACP_PATCH_Pos)

#define OTS_FEATURE_SUPPORTED_OACP_ABORT_Pos (9U)
#define OTS_FEATURE_SUPPORTED_OACP_ABORT                                       \
  (0x1UL << OTS_FEATURE_SUPPORTED_OACP_ABORT_Pos)

// OTS Feature OLCP
#define OTS_FEATURE_SUPPORTED_OLCP_GOTO_Pos (0U)
#define OTS_FEATURE_SUPPORTED_OLCP_GOTO                                        \
  (0x1UL << OTS_FEATURE_SUPPORTED_OLCP_GOTO_Pos)

#define OTS_FEATURE_SUPPORTED_OLCP_ORDER_Pos (1U)
#define OTS_FEATURE_SUPPORTED_OLCP_ORDER                                       \
  (0x1UL << OTS_FEATURE_SUPPORTED_OLCP_ORDER_Pos)

#define OTS_FEATURE_SUPPORTED_OLCP_REQUEST_NUM_OBJS_Pos (2U)
#define OTS_FEATURE_SUPPORTED_OLCP_REQUEST_NUM_OBJS                            \
  (0x1UL << OTS_FEATURE_SUPPORTED_OLCP_REQUEST_NUM_OBJS_Pos)

#define OTS_FEATURE_SUPPORTED_OLCP_CLEAR_MARKING_Pos (3U)
#define OTS_FEATURE_SUPPORTED_OLCP_CLEAR_MARKING                               \
  (0x1UL << OTS_FEATURE_SUPPORTED_OLCP_CLEAR_MARKING_Pos)

#endif // OTS_H
