#ifndef IMU_IMU_CALIBRATION_H_
#define IMU_IMU_CALIBRATION_H_

#include "main.h"

typedef enum {
	AXIS_X = 0,
	AXIS_Y,
	AXIS_Z,
	AXIS_COUNT
} axis_t;

void imu_apply_calibration(int16_t accel[3], int16_t gyro[3]);
void imu_calibration(I2C_HandleTypeDef *hi2c);

#endif /* IMU_IMU_CALIBRATION_H_ */
