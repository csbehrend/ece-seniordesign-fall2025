#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include <stdio.h>
#include <string.h>
// #include "driver/spi_master.h"
#include "automation.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_log.h"
// #include "driver/i2c.h"
// #include "max30102.h"

#define PWM_PIN_1 18
#define PWM_PIN_2 19
#define PWM_PIN_3 20
#define PWM_PIN_4 21
#define PWM_PIN_5 22

// #define ESP_INTR_FLAG_DEFAULT 0
// volatile bool is_stop_pressed = false;

// number of rotations necessary for complete close/complete open
// #define ONE_FULL_RANGE 3 //THIS ONE IS GOOD (probably)
// #define TWO_FULL_RANGE 3.5 //THIS ONE IS GOOD
// #define THREE_FULL_RANGE 3
// #define FOUR_FULL_RANGE 3.5
// #define FIVE_FULL_RANGE 2

// #define MAX_SPEED 65
#define EMERGENCY_STOP 6

// volatile int finger_positions[5] = {0, 0, 0, 0, 0};

SemaphoreHandle_t emergency_stop = NULL;

ledc_channel_config_t ledc_channel_0 = {.speed_mode = LEDC_LOW_SPEED_MODE,
                                        .channel = LEDC_CHANNEL_0,
                                        .timer_sel = LEDC_TIMER_0,
                                        .intr_type = LEDC_INTR_DISABLE,
                                        .gpio_num = PWM_PIN_1,
                                        .duty = 0,
                                        .hpoint = 0};

// motor 2
ledc_channel_config_t ledc_channel_1 = {.speed_mode = LEDC_LOW_SPEED_MODE,
                                        .channel = LEDC_CHANNEL_1,
                                        .timer_sel = LEDC_TIMER_0,
                                        .intr_type = LEDC_INTR_DISABLE,
                                        .gpio_num = PWM_PIN_2,
                                        .duty = 0,
                                        .hpoint = 0};

// motor 3
ledc_channel_config_t ledc_channel_2 = {.speed_mode = LEDC_LOW_SPEED_MODE,
                                        .channel = LEDC_CHANNEL_2,
                                        .timer_sel = LEDC_TIMER_0,
                                        .intr_type = LEDC_INTR_DISABLE,
                                        .gpio_num = PWM_PIN_3,
                                        .duty = 0,
                                        .hpoint = 0};

// motor 4
ledc_channel_config_t ledc_channel_3 = {.speed_mode = LEDC_LOW_SPEED_MODE,
                                        .channel = LEDC_CHANNEL_3,
                                        .timer_sel = LEDC_TIMER_0,
                                        .intr_type = LEDC_INTR_DISABLE,
                                        .gpio_num = PWM_PIN_4,
                                        .duty = 0,
                                        .hpoint = 0};

// motor 5
ledc_channel_config_t ledc_channel_4 = {.speed_mode = LEDC_LOW_SPEED_MODE,
                                        .channel = LEDC_CHANNEL_4,
                                        .timer_sel = LEDC_TIMER_0,
                                        .intr_type = LEDC_INTR_DISABLE,
                                        .gpio_num = PWM_PIN_5,
                                        .duty = 0,
                                        .hpoint = 0};

// for the fucked up servo
void pwm_set_angle_1(uint8_t channel, float angle_deg) {
  if (angle_deg < 0)
    angle_deg = 0;
  if (angle_deg > 90)
    angle_deg = 90;

  float min_pulse_ms = 1.0;
  float max_pulse_ms = 2.0;
  float pulse_width_ms =
      min_pulse_ms + (angle_deg / 90) * (max_pulse_ms - min_pulse_ms);

  float period_ms = 20.0;
  float duty_percent = pulse_width_ms / period_ms;

  uint32_t duty = duty_percent * ((1 << 13) - 1);

  ledc_channel_t ledc_channel = (ledc_channel_t)(LEDC_CHANNEL_0 + channel);

  ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, ledc_channel, duty));
  ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, ledc_channel));
}

void servo_slow_rotation(uint8_t channel, float start_angle, float end_angle,
                         int duration) {
  int steps = 100;
  float step_size = (end_angle - start_angle) / steps;
  int delay_per_step = duration / steps;
  float current_angle;
  for (int i = 0; i <= steps; i++) {
    current_angle = start_angle + i * step_size;
    pwm_set_angle_1(channel, current_angle);
    vTaskDelay(pdMS_TO_TICKS(delay_per_step));
  }
}

int bend_finger(glove_exercise_t *exercise, int initial,
                glove_input_type_t *event_type) {
  assert(exercise != NULL);
  assert(event_type != NULL);
  int finger = exercise->id;
  int count = exercise->reps;
  // TODO: FIX THIS
  assert(exercise->speed > 0 && exercise->speed < 11);
  int speed = (11 - exercise->speed) * 300 + 500;

  assert(finger >= 1 && finger <= 5);
  assert(count > 0);

  int rc = count;

  ledc_timer_config_t ledc_timer = {.speed_mode = LEDC_LOW_SPEED_MODE,
                                    .duty_resolution = LEDC_TIMER_13_BIT,
                                    .timer_num = LEDC_TIMER_0,
                                    .freq_hz = 50,
                                    .clk_cfg = LEDC_AUTO_CLK};

  ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

  gpio_set_level(18, 0);
  gpio_set_level(19, 0);
  gpio_set_level(20, 0);
  gpio_set_level(21, 0);
  gpio_set_level(22, 0);

  if (finger == 1) {
    ledc_channel_config(&ledc_channel_0);
  } else if (finger == 2) {
    ledc_channel_config(&ledc_channel_1);
  } else if (finger == 3) {
    ledc_channel_config(&ledc_channel_2);
  } else if (finger == 4) {
    ledc_channel_config(&ledc_channel_3);
  } else if (finger == 5) {
    ledc_channel_config(&ledc_channel_4);
  }

  int is_stop_pressed;
  for (int i = initial; i < count; i++) {
    is_stop_pressed = gpio_get_level(6); // state of emergency stop button
    // 0=off, 1=on
    if (!is_stop_pressed) {
      while (!is_stop_pressed) {
        // wait till is_stop_pressed=1
        is_stop_pressed = gpio_get_level(6);
      }
    }

    // check glove events

    servo_slow_rotation((finger - 1), 0, 90, speed);
    vTaskDelay(pdMS_TO_TICKS(1000)); // 1 second pause after close
    servo_slow_rotation((finger - 1), 90, 0, speed);
    is_stop_pressed = gpio_get_level(6); // get emergency stop button state
    if (!is_stop_pressed) {
      while (!is_stop_pressed) {
        is_stop_pressed = gpio_get_level(6);
        // wait till its 1 again
      }
    }

    update_glove_event(GLOVE_EVENT_REP_COMPLETED);

    glove_user_event_t event;
    int err = xQueueReceive(exercise_queue, &event, pdMS_TO_TICKS(1000));
    if (err == pdTRUE) {
      switch (event.type) {
      case GLOVE_INPUT_PAUSE:
      case GLOVE_INPUT_STOP:
        *event_type = event.type;
        rc = i;
        goto outside;
        break;
      default:
        break;
      }
    }
  }

outside:
  /*
  gpio_set_level(18, 0);
  gpio_set_level(19, 0);
  gpio_set_level(20, 0);
  gpio_set_level(21, 0);
  gpio_set_level(22, 0);
  */

  return rc;
}
