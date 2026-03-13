#ifndef SENSOR_DRIVERS_H
#define SENSOR_DRIVERS_H

#include "driver/gpio.h"
#include "driver/adc.h"

//pin configuration
//const uint8_t ntc_pin = ADC1_CHANNEL_7;
//const uint8_t photo_pin = ADC1_CHANNEL_6;

//refactor constants
#define VD_RESISTOR 5100U //5.1kOhm
#define V_IN (float)3.3   //3.3V 

//public functions
esp_err_t ntc_init(void);
uint16_t ntc_read(void);

esp_err_t photo_init(void);
uint16_t photo_read(void);

#endif