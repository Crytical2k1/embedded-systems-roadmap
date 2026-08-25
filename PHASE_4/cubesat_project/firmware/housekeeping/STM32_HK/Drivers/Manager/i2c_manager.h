#ifndef MANAGER_I2C_MANAGER_H_
#define MANAGER_I2C_MANAGER_H_

#include "main.h"
#include "FreeRTOS.h"
#include "semphr.h"

void I2C_manager_init(void);	//Initializes the i2c manager
void I2C_manager_lock(void);	//Lock the i2c bus
void I2C_manager_unlock(void);	//Unlock the i2c bus


#endif /* MANAGER_I2C_MANAGER_H_ */
