#ifndef IMU_IMU_CALIBRATION_H_
#define IMU_IMU_CALIBRATION_H_

#include "main.h"

typedef enum {
	AXIS_X = 0,
	AXIS_Y,
	AXIS_Z,
	AXIS_COUNT
} axis_t;

void imu_calibration_init(I2C_HandleTypeDef *hi2c);

#endif /* IMU_IMU_CALIBRATION_H_ */
