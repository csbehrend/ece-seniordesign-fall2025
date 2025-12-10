#include "automation.h"
#include "common.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "host/ble_gatt.h"
#include "motor_support.h"
#include "portmacro.h"
#include <stdint.h>

// c13f3ca3-55f5-4ffa-857a-1c3f40ccb300
SVC_DEFINE_UUID128(automation, 0x00, 0xb3, 0xcc, 0x40, 0x3f, 0x1c, 0x7a, 0x85,
                   0xfa, 0x4f, 0xf5, 0x55, 0xa3, 0x3c, 0x3f, 0xc1);

// 3861a947-7b94-495b-ae3d-c2d669d9f168
CHR_DEFINE_UUID128(auto_start, 0x68, 0xf1, 0xd9, 0x69, 0xd6, 0xc2, 0x3d, 0xae,
                   0x5b, 0x49, 0x94, 0x7b, 0x47, 0xa9, 0x61, 0x38);

// f9b94111-c76a-42a3-92a8-8609cfd2c28f
CHR_DEFINE_UUID128(auto_pause, 0x8f, 0xc2, 0xd2, 0xcf, 0x09, 0x86, 0xa8, 0x92,
                   0xa3, 0x42, 0x6a, 0xc7, 0x11, 0x41, 0xb9, 0xf9);
// ca11594c-0a4d-40dc-994c-b64e2b556e74
CHR_DEFINE_UUID128(auto_stop, 0x74, 0x6e, 0x55, 0x2b, 0x4e, 0xb6, 0x4c, 0x99,
                   0xdc, 0x40, 0x4d, 0x0a, 0x4c, 0x59, 0x11, 0xca);

// c045a031-c506-4756-8bdd-63d55ef3eced
CHR_DEFINE_UUID128(auto_state, 0xed, 0xec, 0xf3, 0x5e, 0xd5, 0x63, 0xdd, 0x8b,
                   0x56, 0x47, 0x06, 0xc5, 0x31, 0xa0, 0x45, 0xc0);

bool auto_state_ind_status = false;
int auto_state_chr_conn_handle = 0;
bool auto_state_chr_conn_handle_inited = false;

QueueHandle_t exercise_queue = NULL;

uint8_t last_glove_event = GLOVE_EVENT_UNKNOWN;

int auto_start_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                          struct ble_gatt_access_ctxt *ctxt, void *arg) {
  int rc = 0;

  if (ctxt->op != BLE_GATT_ACCESS_OP_WRITE_CHR) {
    ESP_LOGE(TAG, "object_type_chr_access: unexpected operation, opcode: %d",
             ctxt->op);
    return BLE_ATT_ERR_UNLIKELY;
  }
  verify_conn_handle(conn_handle, attr_handle);

  // Verify attribute handle
  if (attr_handle != auto_start_chr_handle) {
    ESP_LOGE(TAG, "auto_start_chr_access: invalid attr_handle %d", attr_handle);
    assert(0);
    return BLE_ATT_ERR_UNLIKELY;
  }

  uint16_t len = 0;
  uint8_t msg;
  rc = ble_hs_mbuf_to_flat(ctxt->om, &msg, sizeof(msg), &len);
  if (rc || len != sizeof(msg)) {
    ESP_LOGE(TAG, "auto_start_chr_access: bad mbuf read or bad payload size");
    return BLE_ATT_ERR_INSUFFICIENT_RES;
  }

  glove_user_event_t event = {.type = GLOVE_INPUT_START,
                              .exercise = {.id = 2, .reps = 2, .speed = 2}};

  if (xQueueSend(exercise_queue, &event, 0) == pdPASS) {
    ESP_LOGI(TAG, "auto_start_chr_handle: start exercise id %d", msg);
  }
  return rc;
}

int auto_pause_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                          struct ble_gatt_access_ctxt *ctxt, void *arg) {
  int rc = 0;

  if (ctxt->op != BLE_GATT_ACCESS_OP_WRITE_CHR) {
    ESP_LOGE(TAG, "auto_pause_chr_access: unexpected operation, opcode: %d",
             ctxt->op);
    return BLE_ATT_ERR_UNLIKELY;
  }
  verify_conn_handle(conn_handle, attr_handle);

  // Verify attribute handle
  if (attr_handle != auto_pause_chr_handle) {
    ESP_LOGE(TAG, "auto_pause_chr_access: invalid attr_handle %d", attr_handle);
    assert(0);
    return BLE_ATT_ERR_UNLIKELY;
  }

  uint16_t len = 0;
  uint8_t msg;
  rc = ble_hs_mbuf_to_flat(ctxt->om, &msg, sizeof(msg), &len);
  if (rc || len != 1) {
    ESP_LOGE(TAG, "auto_pause_chr_access: bad mbuf read or bad payload size");
    return BLE_ATT_ERR_INSUFFICIENT_RES;
  }

  glove_user_event_t event = {.type = GLOVE_INPUT_PAUSE};
  if (xQueueSend(exercise_queue, &event, 0) == pdPASS) {
    ESP_LOGI(TAG, "auto_pause_chr_access: pause current exercise");
  }
  return rc;
}

int auto_stop_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                         struct ble_gatt_access_ctxt *ctxt, void *arg) {
  int rc = 0;

  if (ctxt->op != BLE_GATT_ACCESS_OP_WRITE_CHR) {
    ESP_LOGE(TAG, "auto_cancel_chr_access: unexpected operation, opcode: %d",
             ctxt->op);
    return BLE_ATT_ERR_UNLIKELY;
  }
  verify_conn_handle(conn_handle, attr_handle);

  // Verify attribute handle
  if (attr_handle != auto_stop_chr_handle) {
    ESP_LOGE(TAG, "auto_cancel_chr_access: invalid attr_handle %d",
             attr_handle);
    assert(0);
    return BLE_ATT_ERR_UNLIKELY;
  }

  uint16_t len = 0;
  uint8_t msg;
  rc = ble_hs_mbuf_to_flat(ctxt->om, &msg, sizeof(msg), &len);
  if (rc || len != 1) {
    ESP_LOGE(TAG, "auto_cancel_chr_access: bad mbuf read or bad payload size");
    return BLE_ATT_ERR_INSUFFICIENT_RES;
  }

  glove_user_event_t event = {.type = GLOVE_INPUT_STOP};
  if (xQueueSend(exercise_queue, &event, 0) == pdPASS) {
    ESP_LOGI(TAG, "auto_cancel_chr_access: stop exercise");
  }
  return rc;
}

void update_glove_event(glove_event_t event) {
  last_glove_event = event;
  if (auto_state_ind_status && auto_state_chr_conn_handle_inited) {
    ble_gatts_indicate(auto_state_chr_conn_handle, auto_state_chr_handle);
  }
  ESP_LOGI(TAG, "glove state updated to %d", last_glove_event);
}

int auto_state_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                          struct ble_gatt_access_ctxt *ctxt, void *arg) {
  int rc = 0;

  if (ctxt->op != BLE_GATT_ACCESS_OP_READ_CHR) {
    ESP_LOGE(TAG, "auto_state_chr_access: unexpected operation, opcode: %d",
             ctxt->op);
    return BLE_ATT_ERR_UNLIKELY;
  }
  verify_conn_handle(conn_handle, attr_handle);

  // Verify attribute handle
  if (attr_handle != auto_state_chr_handle) {
    ESP_LOGE(TAG, "auto_state_chr_access: invalid attr_handle %d", attr_handle);
    assert(0);
    return BLE_ATT_ERR_UNLIKELY;
  }

  rc = os_mbuf_append(ctxt->om, &last_glove_event, sizeof(last_glove_event));
  return rc ? BLE_ATT_ERR_INSUFFICIENT_RES : 0;
}

/*
static int start_exercise(glove_exercise_t *exercise, int initial) {
  int rc = 0;
  assert(initial < exercise->reps);
  if (initial > 0) {
    update_glove_event(GLOVE_EVENT_ACTIVITY_RESUMED);
  } else {
    update_glove_event(GLOVE_EVENT_STARTED_ACTIVITY);
  }

  glove_user_event_t event = {0};
  for (int i = initial; i < exercise->reps; ++i) {
    rc = xQueueReceive(exercise_queue, &event, pdMS_TO_TICKS(500));
    if (rc == pdPASS) {
      if (event.type == GLOVE_INPUT_PAUSE) {
        update_glove_event(GLOVE_EVENT_ACTIVITY_PAUSED);
        return i;
      } else if (event.type == GLOVE_INPUT_STOP) {
        update_glove_event(GLOVE_EVENT_ACTIVITY_CANCELED);
        return -1;
      }
    }
    update_glove_event(GLOVE_EVENT_REP_COMPLETED);
  }
  return exercise->reps;
}
*/

void glove_exercise_task(void *param) {
  int rc = 0;
  for (;;) {
    glove_user_event_t event = {0};
    do {
      rc = xQueueReceive(exercise_queue, &event, portMAX_DELAY);
      assert(rc == pdTRUE);
    } while (event.type != GLOVE_INPUT_START);

    update_glove_event(GLOVE_EVENT_STARTED_ACTIVITY);
    glove_input_type_t input;
    int i = bend_finger(&event.exercise, 0, &input);
    bool good = true;
    while (i < event.exercise.reps && i >= 0) {
      if (input == GLOVE_INPUT_PAUSE) {
        update_glove_event(GLOVE_EVENT_ACTIVITY_PAUSED);
      } else if (input == GLOVE_INPUT_STOP) {
        update_glove_event(GLOVE_EVENT_ACTIVITY_CANCELED);
        good = false;
        break;
      }
      glove_user_event_t newEvent;
      rc = xQueueReceive(exercise_queue, &newEvent, portMAX_DELAY);
      assert(rc == pdPASS);
      if (newEvent.type == GLOVE_INPUT_START) {
        i = bend_finger(&event.exercise, i, &input);
      } else if (newEvent.type == GLOVE_INPUT_STOP) {
        update_glove_event(GLOVE_EVENT_ACTIVITY_CANCELED);
        good = false;
        break;
      }
    }
    if (good) {
      update_glove_event(GLOVE_EVENT_ACTIVITY_COMPLETED);
    }
  }
}

void init_glove_automation() {
  exercise_queue = xQueueCreate(1, sizeof(glove_user_event_t));
  assert(exercise_queue != NULL);
  // xTaskCreate(glove_exercise_task, "Glove Exercise Sim", 4096, NULL, 5,
  // NULL);
}
