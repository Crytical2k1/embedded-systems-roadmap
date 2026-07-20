#ifndef SENSOR_DRIVER_H
#define SENSOR_DRIVER_H

#include "driver/gpio.h"
#include "driver/adc.h"

typedef struct {
    uint16_t sensor_1;
    uint16_t sensor_2;
} photo_reading_t;

esp_err_t photo_init(void);
void photo_read(photo_reading_t *value);
void photo_tracking_init(uint8_t prio);

#endif