#include "i2c_manager.h"

static SemaphoreHandle_t i2c1_mutex = NULL;

void I2C_manager_init(void) {
	  i2c1_mutex = xSemaphoreCreateMutex();

	  if (i2c1_mutex == NULL) {
		  Error_Handler();
	  }
}

void I2C_manager_lock(void) {
	if (i2c1_mutex != NULL) {
		xSemaphoreTake(i2c1_mutex, portMAX_DELAY);
	}
}
void I2C_manager_unlock(void) {
	if (i2c1_mutex != NULL) {
		xSemaphoreGive(i2c1_mutex);
	}
}
