#ifndef MOTOR_SUPPORT_H
#define MOTOR_SUPPORT_H

#include <stdint.h>
#include "driver/spi_master.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MOSI_PIN    8
#define CLK_PIN     9
#define LATCH_PIN   5
#define SPI_HOST    SPI2_HOST

#define PWM_PIN_1   15
#define PWM_PIN_2   19
#define PWM_PIN_3   20
#define PWM_PIN_4   21
#define PWM_PIN_5   23


extern spi_device_handle_t shift_register;

void spi_shift_register_init(void);
void spi_shift_register_send(uint16_t data);
void pwm_init();
void pwm_set_angle(uint8_t channel, float angle_deg);
void set_hand_position(uint16_t* shift_register_data);
void servo_slow_rotation(uint8_t channel, float start_angle, float end_angle, int duration);

#ifdef __cplusplus
}
#endif

#endif
