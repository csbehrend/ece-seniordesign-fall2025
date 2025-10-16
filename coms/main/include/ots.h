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
        GATT_CHR_ENTRY(object_action_control_point,                            \
                       BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_INDICATE),        \
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
} ots_feature_oacp_t;

typedef union PACKED_ATTR {
  struct PACKED_ATTR {
    bool go_to : 1;
    bool order : 1;
    bool request_num_objs : 1;
    bool clear_marking : 1;
  } decoded;
  uint32_t raw;
} ots_feature_olcp_t;

typedef union PACKED_ATTR {
  struct PACKED_ATTR {
    ots_feature_oacp_t oacp;
    ots_feature_olcp_t olcp;
  } decoded;
  uint64_t raw;
} ots_feature_t;

typedef union PACKED_ATTR {
  struct PACKED_ATTR {
    uint32_t current_size;
    uint32_t allocated_size;
  } decoded;
  uint64_t raw;
} ots_object_size_t;

typedef union PACKED_ATTR {
  struct PACKED_ATTR {
    bool delete : 1;
    bool execute : 1;
    bool read : 1;
    bool write : 1;
    bool append : 1;
    bool truncate : 1;
    bool patch : 1;
    bool mark : 1;
  } decoded;
  uint32_t raw;
} ots_object_properties_t;

typedef enum PACKED_ATTR {
  OACP_OP_CREATE = 0x01,
  OACP_OP_DELETE = 0x02,
  OACP_OP_CHECKSUM = 0x03,
  OACP_OP_EXECUTE = 0x04,
  OACP_OP_READ = 0x05,
  OACP_OP_WRITE = 0x06,
  OACP_OP_RESPONSE = 0x60
} oacp_opcode_t;

typedef enum PACKED_ATTR {
  OACP_RESULT_SUCCESS = 0x01,
  OACP_RESULT_UNSUPP_OP = 0x02,
  OACP_RESULT_INV_PARAM = 0x03,
  OACP_RESULT_INSUFFICIENT_RESOURCES = 0x04,
  OACP_RESULT_INV_OBJECT = 0x05,
  OACP_RESULT_CHANNEL_UNAVAILBLE = 0x06,
  OACP_RESULT_UNSUPP_TYPE = 0x07,
  OACP_RESULT_PROCEDURE_NOT_PERMITTED = 0x08,
  OACP_RESULT_OBJ_LOCKED = 0x09,
  OACP_RESULT_OP_FAILED = 0x0A
} oacp_result_code_t;

typedef struct PACKED_ATTR {
  uint8_t pad0 : 1;
  bool truncate : 1;
  uint8_t pad2_7 : 6;
} ots_write_mode_t;

typedef struct PACKED_ATTR {
  oacp_opcode_t op;
  union PACKED_ATTR {
    struct PACKED_ATTR {
      uint32_t size;
      union PACKED_ATTR {
        uint16_t uuid16;
        uint32_t uuid32;
        uint8_t uuid128[16];
      } type;
    } create_param;
    struct PACKED_ATTR {
      uint32_t offset;
      uint32_t length;
    } checksum_read_param;
    struct PACKED_ATTR {
      uint32_t offset;
      uint32_t length;
      ots_write_mode_t mode;
    } write_param;
    uint8_t raw[20];
  } parameter;
} oacp_request_t;

typedef struct PACKED_ATTR {
  oacp_opcode_t op;
  oacp_result_code_t result;
} oacp_response_t;

typedef struct PACKED_ATTR {
  oacp_response_t response;
  uint32_t checksum;
} oacp_response_checksum_t;

#define OTS_OBJECT_LUID_BITS (48)
#define OTS_OBJECT_LUID_BYTES (OTS_OBJECT_LUID_BITS / 8)
#define OTS_OBJECT_LUID_MIN 0x0100

typedef union PACKED_ATTR {
  uint8_t luid[OTS_OBJECT_LUID_BYTES];
  uint64_t raw;
} ots_object_luid_t;

#endif // OTS_H
