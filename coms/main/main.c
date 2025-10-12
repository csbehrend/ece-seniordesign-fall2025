/*
 * ============================================================================
 * Filename   : main.c
 * Author     : Connor Behrend
 * Description: Main app task for coms subsystem
 * ============================================================================
 */

#include "ads7828.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include <stdio.h>

static const char *TAG = "example";

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

static esp_err_t adc_cmd(i2c_master_dev_handle_t dev_handle, uint8_t cmd) {
  return i2c_master_transmit(dev_handle, &cmd, 1,
                             I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
  /*
  return i2c_master_transmit(dev_handle, &cmd, 1,
                             I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
                             */
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

void app_main(void) {
  uint8_t data[2];
  i2c_master_bus_handle_t bus_handle;
  i2c_master_dev_handle_t dev_handle;
  i2c_master_init(&bus_handle, &dev_handle);
  ESP_LOGI(TAG, "I2C initialized successfully");
  while (true) {
    ESP_ERROR_CHECK(
        adc_cmd(dev_handle, ADS7828_CMD_CH_0_COM | ADS7828_CMD_PD_IRON_ADON));
    ESP_ERROR_CHECK(adc_read(dev_handle, data, sizeof(data)));
    ESP_LOGI(TAG, "CH0 = %X%X", data[0], data[1]);
    /*
    ESP_ERROR_CHECK(
        adc_cmd(dev_handle, ADS7828_CMD_CH_3_COM | ADS7828_CMD_PD_IRON_ADON));
    ESP_ERROR_CHECK(adc_read(dev_handle, data, sizeof(data)));
    ESP_LOGI(TAG, "CH3 = %X%X", data[0], data[1]);
    */
  }
}
