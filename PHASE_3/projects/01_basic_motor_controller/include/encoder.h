#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>

#define GPIO_HALL1 GPIO_NUM_26
#define GPIO_HALL2 GPIO_NUM_27

void encoder_init(void);

int32_t encoder_get_count(void);

void encoder_reset(void);

float encoder_get_rpm(void);

#endif