#ifndef SENSOR_DRIVER_H
#define SENSOR_DRIVER_H

#include <stdint.h>
#include <stdbool.h>
#include "driver/i2c_master.h"

// Settings
static const i2c_port_num_t i2c_port = 0;
static const gpio_num_t i2c_sda_pin = 21;           // GPIO pin for SDA
static const gpio_num_t i2c_scl_pin = 22;           // GPIO pin for SCL
static const uint32_t imu_scl_speed_hz = 100000;    // 100kHz

// Constants
static const uint8_t GY_ACCEL_XOUT_H = 0x3B;        //register to acceleration values
static const uint8_t GY_PWR_MGMT_1 = 0x6B;          //register to wake up 
static const uint16_t imu_addr = 0x68;              // IMU address

void imu_init(void);
void imu_read(int16_t values[3]);

# endif