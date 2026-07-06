#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>

#define GPIO_A1IN GPIO_NUM_33
#define GPIO_A2IN GPIO_NUM_32
#define GPIO_STBY GPIO_NUM_14
#define GPIO_PWM GPIO_NUM_25

void motor_gpio_init(void);
void pwm_init(void);

void motor_enable(void);
void motor_forward(uint16_t speed);
void motor_backward(uint16_t speed);
void motor_stop(void);

void motor_set_speed(uint16_t speed);

void calculate_rpm(void);

#endif