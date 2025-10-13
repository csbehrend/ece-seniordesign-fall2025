/*
 * ============================================================================
 * Filename   : led.h
 * Author     : Connor Behrend
 * Description: Based on ESP-IDF example.
 * ============================================================================
 */

#ifndef LED_H
#define LED_H

/* Includes */
/* ESP APIs */
#include "driver/gpio.h"
#include "led_strip.h"
#include "sdkconfig.h"

/* Public function declarations */
uint8_t get_led_state(void);
void led_on(void);
void led_off(void);
void led_init(void);

#endif // LED_H
