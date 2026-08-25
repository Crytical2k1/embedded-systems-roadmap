#ifndef MANAGER_ADC_MANAGER_H_
#define MANAGER_ADC_MANAGER_H_

#include "main.h"
#include "FreeRTOS.h"
#include "semphr.h"

void ADC_manager_init(void);	//Initializes the ADC manager
void ADC_manager_lock(void);	//Lock the ADC bus
void ADC_manager_unlock(void);	//Unlock the ADC bus

HAL_StatusTypeDef ADC_manager_read_channel(ADC_HandleTypeDef *hadc, uint32_t channel, uint32_t *value);

#endif /* MANAGER_ADC_MANAGER_H_ */
