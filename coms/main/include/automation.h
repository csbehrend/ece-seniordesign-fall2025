#ifndef AUTOMATION_H
#define AUTOMATION_H

/* Includes */
#include "common.h"
#include "esp_attr.h"
#include "freertos/idf_additions.h"
#include "host/ble_gatt.h"

SVC_DECLARE_UUID128(automation);

CHR_DECLARE_UUID128(auto_start);
CHR_DECLARE_UUID128(auto_pause);
CHR_DECLARE_UUID128(auto_stop);
CHR_DECLARE_UUID128(auto_state);

#define AUTOMATION_SERVICE_ENTRY()                                             \
  GATT_SVC_ENTRY_BEGIN(automation)                                             \
  GATT_CHR_ENTRY(auto_start, BLE_GATT_CHR_F_WRITE),                            \
      GATT_CHR_ENTRY(auto_state,                                               \
                     BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_INDICATE),           \
      GATT_CHR_ENTRY(auto_pause, BLE_GATT_CHR_F_WRITE),                        \
      GATT_CHR_ENTRY(auto_stop, BLE_GATT_CHR_F_WRITE), GATT_SVC_ENTRY_END()

// GATT_CHR_ENTRY(auto_pause, BLE_GATT_CHR_F_WRITE),
// GATT_CHR_ENTRY(auto_cancel, BLE_GATT_CHR_F_WRITE),
extern bool auto_state_ind_status;
extern int auto_state_chr_conn_handle;
extern bool auto_state_chr_conn_handle_inited;

typedef struct PACKED_ATTR {
  uint8_t id;
  uint8_t reps;
  uint8_t speed;
} glove_exercise_t;

extern QueueHandle_t exercise_queue;

void init_glove_automation();

typedef enum {
  GLOVE_INPUT_START = 0x01,
  GLOVE_INPUT_PAUSE = 0x02,
  GLOVE_INPUT_STOP = 0x03,
} glove_input_type_t;

typedef struct {
  glove_input_type_t type;
  glove_exercise_t exercise;
} glove_user_event_t;

/*
typedef enum {
  GLOVE_MODE_IDLE = 0x01,
  GLOVE_MODE_ACTIVE = 0x02,
  GLOVE_MODE_PAUSED = 0x03,
  GLOVE_MODE_STOPPED = 0x04,
  GLOVE_EVENT_MODE_CHANGE = 0x01,
} glove_mode_t;
*/

typedef enum {
  GLOVE_EVENT_UNKNOWN = 0x00,
  GLOVE_EVENT_STARTED_ACTIVITY = 0x01,
  GLOVE_EVENT_REP_COMPLETED = 0x02,
  GLOVE_EVENT_ACTIVITY_COMPLETED = 0x03,
  GLOVE_EVENT_ACTIVITY_PAUSED = 0x04,
  GLOVE_EVENT_ACTIVITY_RESUMED = 0x05,
  GLOVE_EVENT_ACTIVITY_CANCELED = 0x06,
  GLOVE_EVENT_ERROR = 0x70,
} glove_event_t;

void glove_exercise_task(void *param);
void update_glove_event(glove_event_t event);

#endif // AUTOMATION_H
