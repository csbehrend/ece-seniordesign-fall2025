#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "driver/ledc.h"


//reference article used for spi implementation
//https://circuitlabs.net/spi-interface-implementation-with-esp-idf/

#define MOSI_PIN 8
#define CLK_PIN 9
#define LATCH_PIN 5
#define SPI_HOST SPI2_HOST

#define PWM_PIN_1 15
#define PWM_PIN_2 19
#define PWM_PIN_3 20
#define PWM_PIN_4 21
#define PWM_PIN_5 23

spi_device_handle_t shift_register; //defines the slave device

void spi_shift_register_init(void){
    esp_err_t ret;
    //configuring LATCH pin as GPIO output
    gpio_config_t io_config = {
        .pin_bit_mask = (1 << LATCH_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_config);
    gpio_set_level(LATCH_PIN, 0); //start with the latch open
    
    spi_bus_config_t buscfg = {
        .mosi_io_num = MOSI_PIN,
        .miso_io_num = -1,
        .sclk_io_num = CLK_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 2
    };

    //initialize the SPI bus
    ret = spi_bus_initialize(SPI_HOST, &buscfg, SPI_DMA_DISABLED);
    ESP_ERROR_CHECK(ret);

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 5000,
        .mode = 0,
        .spics_io_num = -1,
        .queue_size = 1
    };

    //add an SPI device to the bus
    ret = spi_bus_add_device(SPI_HOST, &devcfg, &shift_register);
    ESP_ERROR_CHECK(ret);
}

void spi_shift_register_send(uint16_t data){
    esp_err_t ret;

    //takes a 16 bit unsigned int, makes it 2 8 bit unsigned ints called tx_data
    uint8_t tx_data[2] = {(data >> 8) & 0xFF, data & 0xFF};

    //struct which defines the parameters of one spi transaction
    spi_transaction_t t = {
        .length = 16,
        .tx_buffer = tx_data,
        .rx_buffer = NULL
    };

    gpio_set_level(LATCH_PIN, 0); //chip select low
    ret = spi_device_transmit(shift_register, &t);
    ESP_ERROR_CHECK(ret);
    gpio_set_level(LATCH_PIN, 1); //chip select high

}

void pwm_init(){
    //timer for all 5 motors
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_13_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 50,
        .clk_cfg = LEDC_AUTO_CLK
    };

    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    //motor 1
    ledc_channel_config_t ledc_channel_0 = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = PWM_PIN_1,
        .duty = 0,
        .hpoint = 0
    };

    //motor 2
        ledc_channel_config_t ledc_channel_1 = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_1,
        .timer_sel = LEDC_TIMER_0,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = PWM_PIN_2,
        .duty = 0,
        .hpoint = 0
    };

    //motor 3
        ledc_channel_config_t ledc_channel_2 = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_2,
        .timer_sel = LEDC_TIMER_0,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = PWM_PIN_3,
        .duty = 0,
        .hpoint = 0
    };

    //motor 4
        ledc_channel_config_t ledc_channel_3 = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_3,
        .timer_sel = LEDC_TIMER_0,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = PWM_PIN_4,
        .duty = 0,
        .hpoint = 0
    };

    //motor 5
        ledc_channel_config_t ledc_channel_4 = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_4,
        .timer_sel = LEDC_TIMER_0,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = PWM_PIN_5,
        .duty = 0,
        .hpoint = 0
    };

    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_0));
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_1));
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_2));
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_3));
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_4));
}

//creates a pwm signal in order to control a servo depending on how far it should rotate
void pwm_set_angle(uint8_t channel, float angle_deg) {
    if (angle_deg < 0) angle_deg = 0;
    if (angle_deg > 180) angle_deg = 180;

    float min_pulse_ms = 0.5;
    float max_pulse_ms = 2.5;
    float pulse_width_ms = min_pulse_ms + (angle_deg / 180.0f) * (max_pulse_ms - min_pulse_ms);

    float period_ms = 1000.0f / 50.0f; 
    float duty_percent = pulse_width_ms / period_ms;

    uint32_t duty = duty_percent * ((1 << 13) - 1);

    ledc_channel_t ledc_channel = (ledc_channel_t)(LEDC_CHANNEL_0 + channel);

    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, ledc_channel, duty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, ledc_channel));
}

//duration = 2000 is a good starting speed
//1000 makes it a bit too choppy
void servo_slow_rotation(uint8_t channel, float start_angle, float end_angle, int duration){
    int steps = 100;
    float step_size = (end_angle - start_angle) / steps;
    int delay_per_step = duration / steps;
    float current_angle;
    for(int i = 0; i <= steps; i++){
        current_angle = start_angle + i * step_size;
        pwm_set_angle(channel, current_angle);
        vTaskDelay(pdMS_TO_TICKS(delay_per_step));
    }
}

//function to control solenoids with bits 15-6 of data integer
void set_hand_position(uint16_t* shift_register_data){
    spi_shift_register_send(*shift_register_data);
    vTaskDelay(pdMS_TO_TICKS(1000));
    *shift_register_data = *shift_register_data & 0x003F;
    spi_shift_register_send(*shift_register_data);
}