#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
//#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "driver/ledc.h"
//#include "driver/i2c.h"
//#include "max30102.h"

#define PWM_PIN_1 18
#define PWM_PIN_2 19
#define PWM_PIN_3 20
#define PWM_PIN_4 21
#define PWM_PIN_5 22

//#define ESP_INTR_FLAG_DEFAULT 0
//volatile bool is_stop_pressed = false;

//number of rotations necessary for complete close/complete open
//#define ONE_FULL_RANGE 3 //THIS ONE IS GOOD (probably)
//#define TWO_FULL_RANGE 3.5 //THIS ONE IS GOOD 
//#define THREE_FULL_RANGE 3
//#define FOUR_FULL_RANGE 3.5
//#define FIVE_FULL_RANGE 2

//#define MAX_SPEED 65
#define EMERGENCY_STOP 6

//volatile int finger_positions[5] = {0, 0, 0, 0, 0};


SemaphoreHandle_t emergency_stop = NULL;

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


/*
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
*/
//creates a pwm signal in order to control a servo depending on how far it should rotate
/*
float stop_pulses[5] = {1.47, 1.5, 1.5, 1.5, 1.5};

void pwm_set_angle(uint8_t channel, float angle_deg, int finger) {
    if (angle_deg > 100) angle_deg = 100;
    if (angle_deg < -100) angle_deg = -100;

    // FS90R pulse range ~1.0–2.0 ms, 1.5 ms = stop
    float min_pulse_ms = 1.0; //check this
    float max_pulse_ms = 2.0; //check this too
    float stop_pulse_ms = 1.5; //inconsistent ahh value
    float pulse_width_ms;
    if (angle_deg >= 0) {
        pulse_width_ms = stop_pulse_ms + (angle_deg / 100.0f) * (max_pulse_ms - stop_pulse_ms);
    } else {
        pulse_width_ms = stop_pulse_ms + (angle_deg / 100.0f) * (stop_pulse_ms - min_pulse_ms);
    }
    float period_ms = 1000.0f / 50.0f;  // 50 Hz
    float duty_percent = pulse_width_ms / period_ms;
    uint32_t duty = duty_percent * ((1 << 13) - 1);

    ledc_channel_t ledc_channel = (ledc_channel_t)(LEDC_CHANNEL_0 + channel);
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, ledc_channel, duty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, ledc_channel));
}

float ranges[5] = {ONE_FULL_RANGE, TWO_FULL_RANGE, THREE_FULL_RANGE, FOUR_FULL_RANGE, FIVE_FULL_RANGE};
//changes the amount of time a motor must be on dependent on speed
*/
//for the fucked up servo
void pwm_set_angle_1(uint8_t channel, float angle_deg) {
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

void servo_slow_rotation(uint8_t channel, float start_angle, float end_angle, int duration){
    int steps = 100;
    float step_size = (end_angle - start_angle) / steps;
    int delay_per_step = duration / steps;
    float current_angle;
    for(int i = 0; i <= steps; i++){
        current_angle = start_angle + i * step_size;
        pwm_set_angle_1(channel, current_angle);
        vTaskDelay(pdMS_TO_TICKS(delay_per_step));
    }
}

void bend_finger(int finger, int count, int speed){
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_13_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 50,
        .clk_cfg = LEDC_AUTO_CLK
    };

    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
    
    gpio_set_level(18, 0);
    gpio_set_level(19, 0);
    gpio_set_level(20, 0);
    gpio_set_level(21, 0);
    gpio_set_level(22, 0);

    if(finger==1){
        ledc_channel_config(&ledc_channel_0);
    }
    else if(finger==2){
        ledc_channel_config(&ledc_channel_1);
    }
    else if(finger==3){
        ledc_channel_config(&ledc_channel_2);
    }
    else if(finger==4){
        ledc_channel_config(&ledc_channel_3);
    }   
    else if(finger==5){
        ledc_channel_config(&ledc_channel_4);
    } 

    int is_stop_pressed;
    for(int i=0; i<count; i++){
        is_stop_pressed = gpio_get_level(6); //state of emergency stop button
        //0=off, 1=on
        if(!is_stop_pressed){
            while(!is_stop_pressed){
                //wait till is_stop_pressed=1
                is_stop_pressed = gpio_get_level(6);
            }
        }
        servo_slow_rotation((finger-1), 0, 180, speed);
        vTaskDelay(pdMS_TO_TICKS(1000)); //1 second pause after close
        servo_slow_rotation((finger-1), 180, 0, speed);
        is_stop_pressed = gpio_get_level(6); //get emergency stop button state
        if(!is_stop_pressed){
            while(!is_stop_pressed){
                is_stop_pressed = gpio_get_level(6);
                //wait till its 1 again
            }
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
/*
void close_finger(int speed, int finger){
float rev_per_sec = speed / 60.0f;
float distance = (ranges[finger] / rev_per_sec) * 1000.0f;

    for(int i=0; i<100; i++){ //this was such a stupid bug
        pwm_set_angle(3, (-1 * speed), finger);
        vTaskDelay(pdMS_TO_TICKS(distance/100));
        finger_positions[finger]++; 
    }
    //adjust utilizing flex sensor data
    //if i HAD one
    pwm_set_angle(3, 0, finger);
}

void open_finger(int speed, int finger){
    float rev_per_sec = speed / 60.0f;
    float distance = (ranges[finger] / rev_per_sec) * 1000.0f;

    pwm_set_angle(3, speed, finger);
    for(int i=0; i<100; i++){
        vTaskDelay(pdMS_TO_TICKS(distance/100));
        finger_positions[finger]--; 
    }
    pwm_set_angle(3, 0, finger);
    //flex sensor adjustment
}

void close_fist(int speed){
    float distance = 2 / (speed / 60) * 1000;
    pwm_set_angle(2, speed, 0);
    pwm_set_angle(3, speed, 1);
    vTaskDelay(pdMS_TO_TICKS(distance));
    pwm_set_angle(2, 0, 0);
    pwm_set_angle(1, 0, 1);
}
//FUNCTIONS FOR EMERGENCY STOP
//I think this is wrong
//wouldnt be shocked



static uint32_t last_isr_time = 0;
static void IRAM_ATTR gpio_isr_handler(void* arg){
    uint32_t now = xTaskGetTickCountFromISR();
    if (now - last_isr_time < pdMS_TO_TICKS(30)) return; // 30ms debounce
    last_isr_time = now;

    is_stop_pressed = (gpio_get_level(EMERGENCY_STOP) == 0);
    xSemaphoreGiveFromISR(emergency_stop, NULL);
}


//for the emergency stop button
void gpio_init(){
    emergency_stop = xSemaphoreCreateBinary();
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_ANYEDGE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << EMERGENCY_STOP),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_ENABLE
    };
    gpio_config(&io_conf);
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(EMERGENCY_STOP, gpio_isr_handler, NULL);

    gpio_config_t out_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << 4),
    };
    gpio_config(&out_conf);

    gpio_set_level(4, 0); // off initially
}

void emergency_stop_task(void *arg){
    while(1){
        if(xSemaphoreTake(emergency_stop, portMAX_DELAY) == pdTRUE){
            if(is_stop_pressed){
                servo_slow_rotation(2, finger_positions[2], 0, 2000);
            }
            
        }
    }
}
    */
