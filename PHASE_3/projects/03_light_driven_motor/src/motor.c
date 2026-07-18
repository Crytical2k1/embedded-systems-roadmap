#include "motor.h"

#include "driver/gpio.h"
#include "driver/ledc.h"

void motor_gpio_init(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = 
        (1ULL << GPIO_A1IN) |
        (1ULL << GPIO_A2IN) |
        (1ULL << GPIO_STBY),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
}

void pwm_init(void) {
    ledc_timer_config_t timer = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 20000,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer);

    ledc_channel_config_t channel = {
        .gpio_num = GPIO_PWM,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&channel);
}

void motor_enable(void) {
    motor_gpio_init();
    pwm_init();

    gpio_set_level(GPIO_STBY, 1);
    motor_stop();
}

void motor_forward(uint16_t speed) {
    gpio_set_level(GPIO_A1IN, 1);
    gpio_set_level(GPIO_A2IN, 0);

    motor_set_speed(speed);
}
void motor_backward(uint16_t speed) {
    gpio_set_level(GPIO_A1IN, 0);
    gpio_set_level(GPIO_A2IN, 1);

    motor_set_speed(speed);
}
void motor_stop(void) {
    motor_set_speed(0);

    gpio_set_level(GPIO_A1IN, 0);
    gpio_set_level(GPIO_A2IN, 0);
}

void motor_set_speed(uint16_t speed) {
    ledc_set_duty(
        LEDC_HIGH_SPEED_MODE,
        LEDC_CHANNEL_0,
        speed
    );
    ledc_update_duty(
        LEDC_HIGH_SPEED_MODE,
        LEDC_CHANNEL_0
    );
}