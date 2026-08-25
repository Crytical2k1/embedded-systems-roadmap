#include "cds_sensor.h"

#include "main.h"
#include <math.h>
#include "adc_manager.h"

//static const char *TAG = "cds_driver";

//forward declaration
HAL_StatusTypeDef photo_init(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef photo_read_light(ADC_HandleTypeDef *hadc, uint32_t channel, float *light);
float photo_convert_to_lux(uint32_t adc_value);

HAL_StatusTypeDef photo_init(ADC_HandleTypeDef *hadc) {
	if (hadc == NULL) {
		return HAL_ERROR;
	}

	return HAL_OK;
}

float photo_convert_to_lux(uint32_t adc_value) {
	if (adc_value == 0) {
		adc_value = 1; // avoid dividing by zero
	}
	//Voltage divider
	float resistance_cds = VD_RESISTOR * ((D_max / (float)adc_value) - 1.0f);

	if (resistance_cds <= 0.0f) {
		resistance_cds = 0.01f; // avoid dividing by zero
	}
	//calculate lux
	float lux = 10.0f * powf(resistance_cds/PHOTO_R10, -1.0f/PHOTO_GAMMA);

	return lux;
}

HAL_StatusTypeDef photo_read_light(ADC_HandleTypeDef *hadc, uint32_t channel, float *light) {
	HAL_StatusTypeDef status;
	uint32_t adc_value;

	status = ADC_manager_read_channel(hadc, channel, &adc_value);
	if (status != HAL_OK) {
		return status;
	}

	*light = photo_convert_to_lux(adc_value);

	return HAL_OK;
}
