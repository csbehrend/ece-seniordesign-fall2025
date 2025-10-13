/*
 * ============================================================================
 * Filename   : adc.c
 * Author     : Connor Behrend
 * Description: Main app task for coms subsystem
 * ============================================================================
 */

#include "adc.h"
#include "ads7828.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include <stdint.h>
#include <stdio.h>

static const char *TAG = "adc";

#define I2C_MASTER_SCL_IO                                                      \
  CONFIG_I2C_MASTER_SCL /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO                                                      \
  CONFIG_I2C_MASTER_SDA          /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM I2C_NUM_0 /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ                                                     \
  CONFIG_I2C_MASTER_FREQUENCY       /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS 1000

#define I2C_MASTER_SDA_IO                                                      \
  CONFIG_I2C_MASTER_SDA /*!< GPIO number used for I2C master data  */

#define ADC_CMD_PD_SEL ADS7828_CMD_PD_IRON_ADON
#define ADC_PERIOD                                                             \
  2000 // 2 ms per sample alt btw 5 channels --> 100 Hz per channel

// Statics
static const uint8_t sensor_cmds[] = {ADS7828_CMD_CH_0_COM | ADC_CMD_PD_SEL,
                                      ADS7828_CMD_CH_1_COM | ADC_CMD_PD_SEL,
                                      ADS7828_CMD_CH_2_COM | ADC_CMD_PD_SEL,
                                      ADS7828_CMD_CH_3_COM | ADC_CMD_PD_SEL,
                                      ADS7828_CMD_CH_4_COM | ADC_CMD_PD_SEL};
static const uint8_t sensor_count =
    sizeof(sensor_cmds) / sizeof(sensor_cmds[0]);
static uint8_t current_idx = 0;
static i2c_master_bus_handle_t bus_handle;
static i2c_master_dev_handle_t dev_handle;
static esp_timer_handle_t periodic_timer;

static esp_err_t adc_cmd(i2c_master_dev_handle_t dev_handle, uint8_t cmd) {
  return i2c_master_transmit(dev_handle, &cmd, 1,
                             I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

static esp_err_t adc_read(i2c_master_dev_handle_t dev_handle, uint8_t *data,
                          size_t len) {
  return i2c_master_receive(dev_handle, data, len,
                            I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

/**
 * @brief i2c master initialization
 */
static void i2c_master_init(i2c_master_bus_handle_t *bus_handle,
                            i2c_master_dev_handle_t *dev_handle) {
  i2c_master_bus_config_t bus_config = {
      .i2c_port = I2C_MASTER_NUM,
      .sda_io_num = I2C_MASTER_SDA_IO,
      .scl_io_num = I2C_MASTER_SCL_IO,
      .clk_source = I2C_CLK_SRC_DEFAULT,
      .glitch_ignore_cnt = 7,
      .flags.enable_internal_pullup = true,
  };
  ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, bus_handle));

  i2c_device_config_t dev_config = {
      .dev_addr_length = I2C_ADDR_BIT_LEN_7,
      .device_address = ADS7828_ADDR_ADC0,
      .scl_speed_hz = I2C_MASTER_FREQ_HZ,
  };
  ESP_ERROR_CHECK(
      i2c_master_bus_add_device(*bus_handle, &dev_config, dev_handle));
}

static void vTaskReadAdc(void *pvParameters) {
  uint8_t data[2];
  // ESP_LOGI(TAG, "Sensor Idx = %d", current_idx);
  ESP_ERROR_CHECK(adc_cmd(dev_handle, sensor_cmds[current_idx]));
  ESP_ERROR_CHECK(adc_read(dev_handle, data, sizeof(data)));
  ESP_LOGI(TAG, "CH%d = %X%X", current_idx, data[0], data[1]);

  current_idx = (current_idx + 1) % sensor_count;
}

void init_adc() {
  i2c_master_init(&bus_handle, &dev_handle);
  const esp_timer_create_args_t periodic_timer_args = {
      .callback = &vTaskReadAdc, .name = "adc_timer"};
  ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
}

void adc_collection_start() {
  current_idx = 0;
  ESP_LOGI(TAG, "Starting adc data collection");
  ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, ADC_PERIOD));
}

void adc_collection_stop() {
  ESP_ERROR_CHECK(esp_timer_stop(periodic_timer));
  ESP_LOGI(TAG, "Stopped adc data collection");
}
