#include "../Manager/adc_manager.h"

static SemaphoreHandle_t adc1_mutex = NULL;

void ADC_manager_init(void) {
	  adc1_mutex = xSemaphoreCreateMutex();

	  if (adc1_mutex == NULL) {
		  Error_Handler();
	  }
}

void ADC_manager_lock(void) {
	if (adc1_mutex != NULL) {
		xSemaphoreTake(adc1_mutex, portMAX_DELAY);
	}
}
void ADC_manager_unlock(void) {
	if (adc1_mutex != NULL) {
		xSemaphoreGive(adc1_mutex);
	}
}

HAL_StatusTypeDef ADC_manager_read_channel(ADC_HandleTypeDef *hadc, uint32_t channel, uint32_t *value) {
	HAL_StatusTypeDef status;
	ADC_ChannelConfTypeDef sConfig = {0};

	ADC_manager_lock();

	sConfig.Channel = channel;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;

	status = HAL_ADC_ConfigChannel(hadc, &sConfig);
	if (status != HAL_OK) {
		ADC_manager_unlock();
		return status;
	}

	status = HAL_ADC_Start(hadc);
	if (status != HAL_OK) {
		ADC_manager_unlock();
		return status;
	}

	status = HAL_ADC_PollForConversion(hadc, 100);
	if (status != HAL_OK) {
		HAL_ADC_Stop(hadc);
		ADC_manager_unlock();
		return status;
	}

	*value = HAL_ADC_GetValue(hadc);
	HAL_ADC_Stop(hadc);


	ADC_manager_unlock();

	return HAL_OK;
}
