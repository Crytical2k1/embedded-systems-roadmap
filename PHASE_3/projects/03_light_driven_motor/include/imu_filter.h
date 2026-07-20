#ifndef IMU_FILTER_H
#define IMU_FILTER_H

#include <stdint.h>

void imu_filter_init(void);
void imu_filter_update(const int16_t accel[3], const int16_t gyro[3], float dt_s);

float imu_filter_get_angle(void);
float imu_filter_get_rate(void);

float imu_filter_get_heading(void);
float imu_filter_get_yaw_rate(void);

void imu_filter_reset_heading(void);

#endif