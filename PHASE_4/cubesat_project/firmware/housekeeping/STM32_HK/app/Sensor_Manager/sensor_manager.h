#ifndef SENSOR_MANAGER_SENSOR_MANAGER_H_
#define SENSOR_MANAGER_SENSOR_MANAGER_H_

#include "main.h"

typedef struct {
	I2C_HandleTypeDef *hi2c;
	ADC_HandleTypeDef *hadc;
} SensorManagerHandle_t;

void sensor_manager_init(void *pvParameters);

#endif /* SENSOR_MANAGER_SENSOR_MANAGER_H_ */
