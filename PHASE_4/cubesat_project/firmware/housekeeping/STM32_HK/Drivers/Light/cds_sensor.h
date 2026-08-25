#ifndef LIGHT_CDS_SENSOR_H_
#define LIGHT_CDS_SENSOR_H_

#include "main.h"

//refactor constants
#define VD_RESISTOR 10000.0f //5.1kOhm known resistance of voltage divider
#define V_IN 3.3f //3.3V  vref for voltage divider
#define V_max 2450.0f //2.45V value for adc raw to voltage
#define D_max 4095.0f // value for adc raw to voltage
//photoresistors constants
#define PHOTO_R10 30000.0f //resistance at 10Lux, 30kOhm (dont know)
#define PHOTO_GAMMA 0.7f //gamma value

HAL_StatusTypeDef photo_init(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef photo_read_light(ADC_HandleTypeDef *hadc, uint32_t channel, float *light);
float photo_convert_to_light(uint32_t adc_value);

#endif /* LIGHT_CDS_SENSOR_H_ */
