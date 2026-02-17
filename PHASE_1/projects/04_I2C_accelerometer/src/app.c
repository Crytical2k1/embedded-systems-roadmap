#include "app.h"
#include "twi.h"
#include "gy.h"
#include "uart.h"
#include "leds.h"

#include <avr/io.h>
#include <util/delay.h>
#include <math.h>

static uint8_t buffer[6];
typedef enum {
    AXIS_X = 0,
    AXIS_Y,
    AXIS_Z,
    AXIS_COUNT
} axis_t;
static int16_t accel_offset[AXIS_COUNT] = {0};
static int16_t gyro_offset[AXIS_COUNT] = {0};
#define CALIBRATION_ITERATIONS 200
#define GRAV_FACTOR 16384
#define GYRO_SENSE_250 131.0f
#define DT 0.1f // 100ms
#define ALPHA 0.3f
#define FILTER_COUNT 2
static float ema_value[FILTER_COUNT] = {0};
typedef struct {
    float roll;
    float pitch;
    float yaw;
} app_orientation_t;
static app_orientation_t orientation;



// apply calibration
static int16_t apply_calibration(int16_t value, const int16_t offset[AXIS_COUNT], uint8_t axis) {
    return value - offset[axis];
}

//access the acceleration data
static void read_accel(int16_t accel[AXIS_COUNT]) {
    //read data from the sensor
    gy_read_bytes(GY_ACCEL_XOUT_H, buffer, 6);
    //store data and adjust with the calibration factor
    for (uint8_t axis = 0; axis < AXIS_COUNT; axis++) {
        int16_t raw = ((int16_t)(buffer[axis * 2] << 8)) | buffer[axis * 2 + 1];
        accel[axis] = apply_calibration(raw, accel_offset, axis);
    }
}

//access the gyroscope data
static void read_gyro(int16_t gyro[AXIS_COUNT]) {
    //read data from sensor
    gy_read_bytes(GY_GYRO_XOUT_H, buffer, 6);
    //store data
    for (uint8_t axis = 0; axis < AXIS_COUNT; axis++) {
        int16_t raw = ((int16_t)(buffer[axis * 2] << 8)) | buffer[axis * 2 + 1];
        gyro[axis] = apply_calibration(raw, gyro_offset, axis);
    }
}

// sensor calibration
static void sensor_calibration(void) {
    int32_t sum_accel[AXIS_COUNT] = {0};
    int32_t sum_gyro[AXIS_COUNT] = {0};
    //takes measurements and add them up
    for (uint8_t i = 0; i < CALIBRATION_ITERATIONS; i++) {
        int16_t a[AXIS_COUNT];
        int16_t g[AXIS_COUNT];
        read_accel(a);
        read_gyro(g);

        for (uint8_t axis = 0; axis < AXIS_COUNT; axis++) {
            sum_accel[axis] += a[axis];
            sum_gyro[axis] += g[axis];
        }

        _delay_ms(5);
    }
    //divide the sums by the number of measurements to get the mean
    for (uint8_t axis = 0; axis < AXIS_COUNT; axis++) {
        accel_offset[axis] = sum_accel[axis] / CALIBRATION_ITERATIONS;
        gyro_offset[axis] = sum_gyro[axis] / CALIBRATION_ITERATIONS;
    }
    //adjust the z-axis acceleration value due to gravity
    accel_offset[AXIS_Z] -= GRAV_FACTOR;
    uart_write_string("Sensor Calibrated");
    uart_write_char('\n');
}

// Reduce data noise with EMA filter
static float ema_filter(float input, uint8_t axis) {
    //restore scale factor from ALPHA
    ema_value[axis] += ALPHA * (input - ema_value[axis]); 
    return ema_value[axis];
}

// translate the acceleration data into angle coordinates
static void get_angles(void) {
    int16_t accel[AXIS_COUNT];
    read_accel(accel);

    float ax = (float)accel[AXIS_X];
    float ay = (float)accel[AXIS_Y];
    float az = (float)accel[AXIS_Z];

    //Calculate roll and pitch angles in degrees
    float pitch_raw = atan2f(ay, az) * (180.0f / M_PI);
    float roll_raw = atan2f(-ax, sqrtf((ay*ay + az*az))) * (180.0f / M_PI);

    //Filter the roll and pitch angles to minimize noise
    orientation.roll = ema_filter(roll_raw, AXIS_Y);
    orientation.pitch = ema_filter(pitch_raw, AXIS_X);
}
static void get_rotation(void) {
    int16_t gyro[AXIS_COUNT];
    read_gyro(gyro);
    // convert from LBS/s to deg/s
    float gz = (float)gyro[AXIS_Z] / GYRO_SENSE_250;
    // integrate over time (raw data is angular velocity)
    orientation.yaw += gz * DT;
}

// initialize app
void app_init(void) {
    //enable interal pull-up resistors
    PORTC |= (1 << PC4) | (1 << PC5);
    //Initialize serial communication
    uart_init(9600);
    //Initialize TWI (I2C)
    twi_init();
    //Initialize sensor
    gy_init();
    _delay_ms(100);
    //test sensor communication
    gy_who_am_i();
    //initialize leds
    led_init();

    //calibrate sensor
    sensor_calibration();
}

// main loop base function
void app_orientation(void) {

    get_angles();
    get_rotation();
    uint8_t led_bits = build_led_state(orientation.roll, orientation.pitch);
    shift595(led_bits);

    uart_write_float(orientation.roll);
    uart_write_char(',');
    uart_write_float(orientation.pitch);
    uart_write_char(',');
    uart_write_float(orientation.yaw);
    uart_write_char('\n');
    _delay_ms(100);
}