#ifndef SENSOR_DRIVERS_H
#define SENSOR_DRIVERS_H

#include "driver/gpio.h"
#include "driver/adc.h"

//public functions
esp_err_t ntc_init(void);
uint16_t ntc_read(void);

esp_err_t photo_init(void);
uint16_t photo_read(void);

#endif