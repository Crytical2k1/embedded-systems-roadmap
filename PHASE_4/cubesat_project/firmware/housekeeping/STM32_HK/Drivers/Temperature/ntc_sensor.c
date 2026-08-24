#include "ntc_sensor.h"

#include "main.h"
#include <math.h>

//static const char *TAG = "ntc_driver";


//forward declaration
HAL_StatusTypeDef NTC_init(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef NTC_read_temperature(ADC_HandleTypeDef *hadc, float *temperature);
float NTC_convert_to_temperature(uint32_t adc_value);

HAL_StatusTypeDef NTC_init(ADC_HandleTypeDef *hadc) {
	if (hadc == NULL) {
		return HAL_ERROR;
	}

	return HAL_OK;
}

float NTC_convert_to_temperature(uint32_t adc_value) {
	if (adc_value == 0) {
		adc_value = 1; // avoid dividing by zero
	}
	//Voltage divider
	float resistance_ntc = VD_RESISTOR * ((D_max / (float)adc_value) - 1.0f);

	//Beta equation
	float temp_k = 1.0f / ((1.0f / NTC_T0) + (logf(resistance_ntc / NTC_R25) / NTC_B));

	//return the value in celsius
	return temp_k - KELVIN_CELSIUS;
}

HAL_StatusTypeDef NTC_read_temperature(ADC_HandleTypeDef *hadc, float *temperature) {
	HAL_StatusTypeDef status;
	uint32_t adc_value;

	status = HAL_ADC_Start(hadc);
	if (status != HAL_OK) {
		return status;
	}

	status = HAL_ADC_PollForConversion(hadc, 100);
	if (status != HAL_OK) {
		HAL_ADC_Stop(hadc);
		return status;
	}

	adc_value = HAL_ADC_GetValue(hadc);
	HAL_ADC_Stop(hadc);

	*temperature = NTC_convert_to_temperature(adc_value);

	return HAL_OK;
}
