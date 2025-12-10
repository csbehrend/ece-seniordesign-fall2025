/*
 * ============================================================================
 * Filename   : led.c
 * Author     : Connor Behrend
 * Description: Based on ESP-IDF example.
 * ============================================================================
 */

#include "led.h"
#include "common.h"
#include "host/ble_gatt.h"

SVC_DEFINE_UUID16(auto_io, 0x1815);
CHR_DEFINE_UUID128(led, 0x23, 0xd1, 0xbc, 0xea, 0x5f, 0x78, 0x23, 0x15, 0xde,
                   0xef, 0x12, 0x12, 0x25, 0x15, 0x00, 0x00);

SVC_DEFINE_UUID128(nrf_service, 0x23, 0xd1, 0xbc, 0xea, 0x5f, 0x78, 0x23, 0x15,
                   0xde, 0xef, 0x12, 0x12, 0x23, 0x15, 0x00, 0x00);
CHR_DEFINE_UUID128(nrf_led, 0x23, 0xd1, 0xbc, 0xea, 0x5f, 0x78, 0x23, 0x15,
                   0xde, 0xef, 0x12, 0x12, 0x25, 0x15, 0x00, 0x00);
CHR_DEFINE_UUID128(nrf_button, 0x23, 0xd1, 0xbc, 0xea, 0x5f, 0x78, 0x23, 0x15,
                   0xde, 0xef, 0x12, 0x12, 0x24, 0x15, 0x00, 0x00);

/* Private variables */
static uint8_t led_state;

static led_strip_handle_t led_strip;

/* Public functions */
uint8_t get_led_state(void) { return led_state; }

void led_on(void) {
  /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
  led_strip_set_pixel(led_strip, 0, 16, 16, 16);

  /* Refresh the strip to send data */
  led_strip_refresh(led_strip);

  /* Update LED state */
  led_state = true;
}

void led_off(void) {
  /* Set all LED off to clear all pixels */
  led_strip_clear(led_strip);

  /* Update LED state */
  led_state = false;
}

void led_init(void) {
  ESP_LOGI(TAG, "example configured to blink addressable led!");
  /* LED strip initialization with the GPIO and pixels number*/
  led_strip_config_t strip_config = {
      .strip_gpio_num = CONFIG_BLINK_GPIO,
      .max_leds = 1, // at least one LED on board
  };
  led_strip_rmt_config_t rmt_config = {
      .resolution_hz = 10 * 1000 * 1000, // 10MHz
      .flags.with_dma = false,
  };
  ESP_ERROR_CHECK(
      led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
}

int led_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                   struct ble_gatt_access_ctxt *ctxt, void *arg) {
  /* Local variables */
  int rc = 0;

  /* Handle access events */
  /* Note: LED characteristic is write only */
  switch (ctxt->op) {

  /* Write characteristic event */
  case BLE_GATT_ACCESS_OP_WRITE_CHR:
    /* Verify connection handle */
    if (conn_handle != BLE_HS_CONN_HANDLE_NONE) {
      ESP_LOGI(TAG, "characteristic write; conn_handle=%d attr_handle=%d",
               conn_handle, attr_handle);
    } else {
      ESP_LOGI(TAG, "characteristic write by nimble stack; attr_handle=%d",
               attr_handle);
    }

    /* Verify attribute handle */
    if (attr_handle == led_chr_handle) {
      /* Verify access buffer length */
      if (ctxt->om->om_len == 1) {
        /* Turn the LED on or off according to the operation bit */
        if (ctxt->om->om_data[0]) {
          led_on();
          ESP_LOGI(TAG, "led turned on!");
        } else {
          led_off();
          ESP_LOGI(TAG, "led turned off!");
        }
      } else {
        goto error;
      }
      return rc;
    }
    goto error;

  /* Unknown event */
  default:
    goto error;
  }

error:
  ESP_LOGE(TAG, "unexpected access operation to led characteristic, opcode: %d",
           ctxt->op);
  return BLE_ATT_ERR_UNLIKELY;
}

int nrf_led_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                       struct ble_gatt_access_ctxt *ctxt, void *arg) {
  int rc = 0;

  // Handle access events
  switch (ctxt->op) {
  case BLE_GATT_ACCESS_OP_WRITE_CHR:
    /* Verify connection handle */
    if (conn_handle != BLE_HS_CONN_HANDLE_NONE) {
      ESP_LOGI(TAG, "characteristic write; conn_handle=%d attr_handle=%d",
               conn_handle, attr_handle);
    } else {
      ESP_LOGI(TAG, "characteristic write by nimble stack; attr_handle=%d",
               attr_handle);
    }

    // Verify attribute handle
    if (attr_handle == nrf_led_chr_handle) {
      uint16_t len = 0;
      uint8_t op = 0;
      rc = ble_hs_mbuf_to_flat(ctxt->om, &op, sizeof(op), &len);
      if (rc || len != sizeof(op)) {
        return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
      }
      if (op) {
        led_on();
        ESP_LOGI(TAG, "NORDIC turned on!");
      } else {
        led_off();
        ESP_LOGI(TAG, "NORDIC turned off!");
      }
      return rc;
    }
    goto error;
  default:
    goto error;
  }
error:
  ESP_LOGE(TAG,
           "unexpected access operation to NORDIC characteristic, opcode: %d",
           ctxt->op);
  return BLE_ATT_ERR_UNLIKELY;
}

int nrf_button_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                          struct ble_gatt_access_ctxt *ctxt, void *arg) {
  return 0;
}
