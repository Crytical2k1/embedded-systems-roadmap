#include "imu_calibration.h"
#include "imu.h"
#include "FreeRTOS.h"
#include "task.h"

#include <stdio.h>

static int16_t accel_offset[AXIS_COUNT] = {0};
static int16_t gyro_offset[AXIS_COUNT] = {0};

#define MPU_SENSITIVITY 16384.0f
#define CALIBRATION_ITERATIONS 1000
#define IMU_LOOP_PERIOD_MS 10

static const char *TAG = "imu_calibration";
//forward declaration
void imu_calibration_init(I2C_HandleTypeDef *hi2c);
static void imu_calibration_task(void *pvParameters);
static void sensor_calibration(I2C_HandleTypeDef *hi2c);
static int16_t apply_calibration(int16_t value, const int16_t offset[AXIS_COUNT], uint8_t axis);


//Code starts
static int16_t apply_calibration(int16_t value, const int16_t offset[AXIS_COUNT], uint8_t axis) {
	return value - offset[axis];
}

void imu_calibration_init(I2C_HandleTypeDef *hi2c) {
	BaseType_t task_created = xTaskCreate(imu_calibration_task, "ImuCalibration", 256, hi2c, 3, NULL);
	if (task_created != pdPASS) {
		printf("%s, Failed to create IMU task \r\n", TAG);
		Error_Handler();
	}
}
static void imu_calibration_task(void *pvParameters) {
	I2C_HandleTypeDef *hi2c = (I2C_HandleTypeDef *)pvParameters;
	//calibrate first the sensor
	sensor_calibration(hi2c);

	int16_t accel_buffer[AXIS_COUNT];
	int16_t gyro_buffer[AXIS_COUNT];
	int16_t accel[AXIS_COUNT];
	int16_t gyro[AXIS_COUNT];

	while(1) {
		imu_read(hi2c, accel_buffer, gyro_buffer);

		for (uint8_t axis = 0; axis < AXIS_COUNT; axis++) {
			accel[axis] = apply_calibration(accel_buffer[axis], accel_offset, axis);
			gyro[axis] = apply_calibration(gyro_buffer[axis], gyro_offset, axis);
		}
		accel;
		gyro;
		printf("Values acceleration: X=%d Y=%d Z=%d", accel[0], accel[1], accel[2]);
		printf("Values gyroscope: X=%d Y=%d Z=%d", gyro[0], gyro[1], gyro[2]);

		vTaskDelay(pdMS_TO_TICKS(IMU_LOOP_PERIOD_MS));

	}
}

static void sensor_calibration(I2C_HandleTypeDef *hi2c) {
	int32_t sum_accel[AXIS_COUNT] = {0};
	int32_t sum_gyro[AXIS_COUNT] = {0};
	//take measurements and add them up
	for (uint16_t i = 0; i < CALIBRATION_ITERATIONS; i++) {
		int16_t a[AXIS_COUNT];
		int16_t g[AXIS_COUNT];
		imu_read(hi2c, a,g);

		for (uint8_t axis = 0; axis < AXIS_COUNT; axis++) {
			sum_accel[axis] += a[axis];
			sum_gyro[axis] += g[axis];
		}
		vTaskDelay(pdMS_TO_TICKS(5));	}
	//divide the sums by the number of measurements to get the mean
	for (uint8_t axis = 0; axis < AXIS_COUNT; axis++) {
		accel_offset[axis] = sum_accel[axis] / CALIBRATION_ITERATIONS;
		gyro_offset[axis] = sum_gyro[axis] / CALIBRATION_ITERATIONS;
	}
	accel_offset[AXIS_Z] -= MPU_SENSITIVITY;
}


