#ifndef MOTOR_SUPPORT_H
#define MOTOR_SUPPORT_H

#include "automation.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PWM_PIN_1 18
#define PWM_PIN_2 19
#define PWM_PIN_3 20
#define PWM_PIN_4 21
#define PWM_PIN_5 22

// void pwm_init();
// void pwm_set_angle(uint8_t channel, float angle_deg, int finger);
// static void IRAM_ATTR gpio_isr_handler(void* arg);
// void emergency_stop_task(void *arg);
// void gpio_init();
// void i2c_master_init();
// void close_finger(int speed, int finger);
// void open_finger(int speed, int finger);
// void close_fist(int speed);

void pwm_set_angle_1(uint8_t channel, float angle_deg);
void servo_slow_rotation(uint8_t channel, float start_angle, float end_angle,
                         int duration);
int bend_finger(glove_exercise_t *exercise, int initial,
                glove_input_type_t *event_type);

#ifdef __cplusplus
}
#endif

#endif
