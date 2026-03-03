#include "sensor_driver.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

//helpers
static esp_err_t esp_ret;
static i2c_master_bus_handle_t i2c_bus;
static i2c_master_dev_handle_t imu_dev;
static uint8_t reg;
static uint8_t data_buffer[6];

void imu_init(void) {

    // Set I2C bus configuration
    i2c_master_bus_config_t bus_config = {
        .i2c_port = i2c_port,
        .sda_io_num = i2c_sda_pin,
        .scl_io_num = i2c_scl_pin,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .flags.enable_internal_pullup = true,
    };

    // Initialize the I2C bus
    esp_ret = i2c_new_master_bus(&bus_config, &i2c_bus);
    if (esp_ret != ESP_OK) {
        printf("Error: Failed to initialize I2C bus \r\n");
        abort();
    }

    // Set I2C device configuration
    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = imu_addr,
        .scl_speed_hz = imu_scl_speed_hz,
    };

    // Initialize the IMU I2C device on the bus
    esp_ret = i2c_master_bus_add_device(i2c_bus, &dev_config, &imu_dev);
    if (esp_ret != ESP_OK) {
        printf("Error: Failed to initailize I2C device \r\n");
        abort();
    }

    //Wake up the IMU
    uint8_t wake_cmd[2] = {GY_PWR_MGMT_1, 0x00}; // set power management register to 00 to reset
    esp_ret = i2c_master_transmit(imu_dev, wake_cmd, 2, portMAX_DELAY);
    if (esp_ret != ESP_OK) {
        printf("Wake failed: %s\n", esp_err_to_name(esp_ret));
        abort();
    }
}

void imu_read(int16_t values[3]) {
    // Store register address in buffer
    reg = GY_ACCEL_XOUT_H;

    // Read acceleration (device handle, address of register buffer, size of buffer, data buffer, size of buffer, timeout)
    esp_ret = i2c_master_transmit_receive(imu_dev, &reg, 1, data_buffer, 6, portMAX_DELAY);
    if (esp_ret != ESP_OK) {
        printf("Error: Failed to read from imu\r\n");
    }
    //rewrite the values of the buffer into the variable with proper format
    for (uint8_t axis = 0; axis < 3; axis++ ) {
        values[axis] = (int16_t)((data_buffer[axis * 2] << 8) | data_buffer[axis *2 + 1]);
    }
}