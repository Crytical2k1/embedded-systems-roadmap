#ifndef IMU_IMU_H_
#define IMU_IMU_H_

#include "main.h"
#include "i2c.h"

//registers
static const uint8_t GY_PWR_MGMT_1 = 0x6B;
static const uint8_t GY_ADDR = (0x68 << 1);
static const uint8_t GY_ACCEL_XOUT_H = 0x3B;
static const uint8_t GY_GYRO_XOUT_H = 0x43;

HAL_StatusTypeDef imu_init(void);
HAL_StatusTypeDef imu_read(int16_t accel[3], int16_t gyro[3]);

#endif /* IMU_IMU_H_ */
