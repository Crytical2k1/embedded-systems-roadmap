#include "imu.h"

HAL_StatusTypeDef imu_init(void) {
	uint8_t data = 0x00;
	//Wake up
	return HAL_I2C_Mem_Write(
			&hi2c1,
			GY_ADDR,
			GY_PWR_MGMT_1,
			I2C_MEMADD_SIZE_8BIT,
			&data,
			1,
			HAL_MAX_DELAY
	);
}

HAL_StatusTypeDef imu_read(int16_t accel[3], int16_t gyro[3]) {
	uint8_t data[14];
	//store register in buffer
	uint8_t reg = GY_ACCEL_XOUT_H;
	//read data from sensor
	HAL_StatusTypeDef status = HAL_I2C_Mem_Read(
			&hi2c1,
			GY_ADDR,
			reg,		//accel_xout_h
			I2C_MEMADD_SIZE_8BIT,
			data,
			14,
			HAL_MAX_DELAY
	);

	if (status != HAL_OK) {
		return status;
	}
	//acceleration
	for (uint8_t axis = 0; axis < 3; axis++) {
		accel[axis] = (int16_t)((data[axis*2] << 8) | data[axis*2 + 1]);
	}
	//gyroscope
	for (uint8_t axis = 0; axis < 3; axis++) {
		uint8_t index = 8 + 2 * axis; //it skips the values of accel and temp
		gyro[axis] = (int16_t)((data[index] << 8) | data[index + 1]);
	}

	return HAL_OK;
}
