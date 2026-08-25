#ifndef TEMPERATURE_NTC_SENSOR_H_
#define TEMPERATURE_NTC_SENSOR_H_

#include "main.h"

//refactor constants
#define VD_RESISTOR 10000.0f //5.1kOhm known resistance of voltage divider
#define V_IN 3.3f //3.3V  vref for voltage divider
#define V_max 2450.0f //2.45V value for adc raw to voltage
#define D_max 4095.0f // value for adc raw to voltage

#define NTC_T0 298.15f // reference temperature 25ºC
#define NTC_B 3950.0f // beta value
#define NTC_R25 10000.0f //resistance at 25ºC, 10kOhm
#define KELVIN_CELSIUS 273.15f

HAL_StatusTypeDef NTC_init(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef NTC_read_temperature(ADC_HandleTypeDef *hadc, uint32_t channel, float *temperature);
float NTC_convert_to_temperature(uint32_t adc_value);

#endif /* TEMPERATURE_NTC_SENSOR_H_ */
