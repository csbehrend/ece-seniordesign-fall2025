#ifndef AUTOMATION_H
#define AUTOMATION_H

/* Includes */
#include "common.h"

SVC_DECLARE_UUID128(automation);

CHR_DECLARE_UUID128(auto_start);
// CHR_DECLARE_UUID128(auto_pause);
// CHR_DECLARE_UUID128(auto_cancel);

#define AUTOMATION_SERVICE_ENTRY()                                             \
  GATT_SVC_ENTRY_BEGIN(automation)                                             \
  GATT_CHR_ENTRY(auto_start, BLE_GATT_CHR_F_WRITE),                            \
      GATT_CHR_ENTRY(object_type, BLE_GATT_CHR_F_READ), GATT_SVC_ENTRY_END()

// GATT_CHR_ENTRY(auto_pause, BLE_GATT_CHR_F_WRITE),
// GATT_CHR_ENTRY(auto_cancel, BLE_GATT_CHR_F_WRITE),

#endif // AUTOMATION_H
