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
#include "common.h"
#include "driver/gpio.h"
#include "led_strip.h"
#include "sdkconfig.h"

/* Public function declarations */
uint8_t get_led_state(void);
void led_on(void);
void led_off(void);
void led_init(void);

SVC_DECLARE_UUID16(auto_io);
CHR_DECLARE_UUID128(led);

#define LED_SERVICE_ENTRY()                                                    \
  GATT_SVC_ENTRY_BEGIN(auto_io)                                                \
  GATT_CHR_ENTRY(led, BLE_GATT_CHR_F_WRITE), GATT_SVC_ENTRY_END()

SVC_DECLARE_UUID128(nrf_service);
CHR_DECLARE_UUID128(nrf_led);
CHR_DECLARE_UUID128(nrf_button);

#define NRF_SERVICE_ENTRY()                                                    \
  GATT_SVC_ENTRY_BEGIN(nrf_service)                                            \
  GATT_CHR_ENTRY(nrf_led, BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_NO_RSP), \
      GATT_CHR_ENTRY(nrf_button, BLE_GATT_CHR_F_NOTIFY), GATT_SVC_ENTRY_END()

#endif // LED_H
