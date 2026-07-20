#include "imu_filter.h"
#include "math.h"
#include "freertos/FreeRTOS.h"

#define ACCEL_SCALE 16384.0F // LSB per g
#define GYRO_SCALE 131.0f // LSB per deg/s

#define COMP_FILTER_ALPHA 0.98f

#define TILT_ACCEL_A 1 //accel[Y]
#define TILT_ACCEL_B 2 //accel[Z]
#define TILT_GYRO_AXIS 0 //gyro[X]
#define YAW_GYRO_AXIS 2 //gyro[Z]

static float angle_deg = 0.0f;
static float rate_dps = 0.0f;
static float heading_deg = 0.0f;
static float yaw_rate_dps = 0.0f;
//using portMUX_TYPE as a critical point where interrupts are disabled and does not allow other CPU cores to access the data.
static portMUX_TYPE angle_mux = portMUX_INITIALIZER_UNLOCKED;

void imu_filter_init(void) {
    angle_deg = 0.0f;
    rate_dps = 0.0f;
    heading_deg = 0.0f;
    yaw_rate_dps = 0.0f;
}

void imu_filter_update(const int16_t accel[3], const int16_t gyro[3], float dt_s) {
    //calculates the actual degrees that the device should turn, compared to imu_task where we had the raw data.
    float accel_angle = atan2f((float)accel[TILT_ACCEL_A], (float)accel[TILT_ACCEL_B]) * (180.0F / (float)M_PI);
    float gyro_rate = (float)gyro[TILT_GYRO_AXIS] / GYRO_SCALE;

    float new_angle = COMP_FILTER_ALPHA * (angle_deg + gyro_rate * dt_s) + (1.0f - COMP_FILTER_ALPHA) * accel_angle;

    float new_yaw_rate = (float)gyro[YAW_GYRO_AXIS] / GYRO_SCALE;
    float new_heading = heading_deg + new_yaw_rate * dt_s;

    taskENTER_CRITICAL(&angle_mux);
    angle_deg = new_angle;
    rate_dps = gyro_rate;
    heading_deg = new_heading;
    yaw_rate_dps = new_yaw_rate;
    taskEXIT_CRITICAL(&angle_mux);
}

float imu_filter_get_angle(void) {
    float a;
    taskENTER_CRITICAL(&angle_mux);
    a = angle_deg;
    taskEXIT_CRITICAL(&angle_mux);
    return a;
}
float imu_filter_get_rate(void) {
    float r;
    taskENTER_CRITICAL(&angle_mux);
    r = rate_dps;
    taskEXIT_CRITICAL(&angle_mux);
    return r;
}
float imu_filter_get_heading(void) {
    float h;
    taskENTER_CRITICAL(&angle_mux);
    h = heading_deg;
    taskEXIT_CRITICAL(&angle_mux);
    return h;
}
float imu_filter_get_yaw_rate(void) {
    float yr;
    taskENTER_CRITICAL(&angle_mux);
    yr = yaw_rate_dps;
    taskEXIT_CRITICAL(&angle_mux);
    return yr;
}
void imu_filter_reset_heading(void) {
    taskENTER_CRITICAL(&angle_mux);
    heading_deg = 0.0f;
    taskEXIT_CRITICAL(&angle_mux);
}