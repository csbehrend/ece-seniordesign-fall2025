/*
 * ============================================================================
 * Filename   : main.c
 * Author     : Connor Behrend
 * Description: Main app task for coms subsystem
 * ============================================================================
 */

#include "adc.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include <stdint.h>
#include <stdio.h>

static const char *TAG = "main";

void app_main(void) {
  ESP_LOGI(TAG, "Coms initialized successfully");

  init_adc();
  adc_collection_start();
  vTaskDelay(pdMS_TO_TICKS(4000));
  adc_collection_stop();
}
