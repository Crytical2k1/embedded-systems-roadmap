#ifndef IMU_DRIVER_H
#define IMU_DRIVER_H

#include <stdint.h>
#include <stdbool.h>
#include "driver/i2c_master.h"

//settings
static const i2c_port_num_t i2c_port = 0;
static const gpio_num_t i2c_sda_pin = 21;       //gpio pin for sda
static const gpio_num_t i2c_scl_pin = 22;       //gpio pin for scl
static const uint32_t i2c_scl_speed_hz = 100000;//100khz (std)

//registers
static const uint8_t GY_ACCEL_XOUT_H = 0x3B;    //register of accel data
static const uint8_t GY_PWR_MGMT_1 = 0x6B;      //register to wake up
static const uint8_t GY_ADDR = 0x68;            //imu address

void imu_init(void);
void imu_read(int16_t value[3]);


#endif