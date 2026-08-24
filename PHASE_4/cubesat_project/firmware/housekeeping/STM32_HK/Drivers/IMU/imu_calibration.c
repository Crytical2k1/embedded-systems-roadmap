#include "imu_calibration.h"
#include "imu.h"

#include "FreeRTOS.h"
#include "task.h"

#include <stdio.h>

static int16_t accel_offset[AXIS_COUNT] = {0};
static int16_t gyro_offset[AXIS_COUNT] = {0};

#define MPU_SENSITIVITY 16384.0f
#define CALIBRATION_ITERATIONS 1000

static const char *TAG = "imu_calibration";

//forward declaration
void imu_apply_calibration(int16_t accel[3], int16_t gyro[3]);
void imu_calibration(I2C_HandleTypeDef *hi2c);
static int16_t apply_calibration(int16_t value, const int16_t offset[AXIS_COUNT], uint8_t axis);


//Code starts
static int16_t apply_calibration(int16_t value, const int16_t offset[AXIS_COUNT], uint8_t axis) {
	return value - offset[axis];
}

void imu_apply_calibration(int16_t accel[3], int16_t gyro[3]) {
	for (uint8_t axis = 0; axis < AXIS_COUNT; axis++) {
		accel[axis] = apply_calibration(accel[axis], accel_offset, axis);
		gyro[axis] = apply_calibration(gyro[axis], gyro_offset, axis);
	}
}

void imu_calibration(I2C_HandleTypeDef *hi2c) {
	int32_t sum_accel[AXIS_COUNT] = {0};
	int32_t sum_gyro[AXIS_COUNT] = {0};
	//take measurements and add them up
	for (uint16_t i = 0; i < CALIBRATION_ITERATIONS; i++) {
		int16_t a[AXIS_COUNT];
		int16_t g[AXIS_COUNT];
		if (imu_read(hi2c, a,g) == HAL_OK) {
			for (uint8_t axis = 0; axis < AXIS_COUNT; axis++) {
				sum_accel[axis] += a[axis];
				sum_gyro[axis] += g[axis];
			}
		} else {
			//failed measurements, dont include in calibration
		}
		vTaskDelay(pdMS_TO_TICKS(5));
	}
	//divide the sums by the number of measurements to get the mean
	for (uint8_t axis = 0; axis < AXIS_COUNT; axis++) {
		accel_offset[axis] = sum_accel[axis] / CALIBRATION_ITERATIONS;
		gyro_offset[axis] = sum_gyro[axis] / CALIBRATION_ITERATIONS;
	}
	accel_offset[AXIS_Z] -= MPU_SENSITIVITY;
}


