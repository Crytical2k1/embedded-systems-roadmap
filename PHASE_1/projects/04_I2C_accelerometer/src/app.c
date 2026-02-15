#include "app.h"
#include "twi.h"
#include "gy.h"
#include "uart.h"
#include "leds.h"

#include <avr/io.h>
#include <util/delay.h>
#include <math.h>

static uint8_t buffer[6];
static int16_t refactor_x = 0;
static int16_t refactor_y = 0;
static int16_t refactor_z = 0;
#define GRAV_FACTOR 16384
#define AXIS_X 0
#define AXIS_Y 1
#define AXIS_Z 2
#define ALPHA 0.3f
static float filter_value[2] = {0.0f, 0.0f};
static float roll;
static float pitch;

static void sensor_calibration(void) {
    int32_t buffer_ax = 0;
    int32_t buffer_ay = 0;
    int32_t buffer_az = 0;

    for (uint8_t i = 0; i < 20; i++) {
        gy_read_bytes(GY_ACCEL_XOUT_H, buffer, 6);

        buffer_ax += (buffer[0] << 8) | buffer[1];
        buffer_ay += (buffer[2] << 8) | buffer[3];
        buffer_az += (buffer[4] << 8) | buffer[5];

        _delay_ms(50);
    }

    refactor_x = buffer_ax / 20;
    refactor_y = buffer_ay / 20;
    refactor_z =(buffer_az / 20) - GRAV_FACTOR;
}

void app_init(void) {
    //enable interal pull-up resistors
    PORTC |= (1 << PC4) | (1 << PC5);
    //Initialize serial communication
    uart_init(9600);
    uart_write_string("=== MPU Sensor Test ===\n");
    //Initialize TWI (I2C)
    twi_init();
    uart_write_string("TWI Initialized\n");
    //delay to let sensor power up
    _delay_ms(100);
    //Initialize sensor
    gy_init();
    uart_write_string("Sensor Initialized\n");
    _delay_ms(100);
    //test sensor communication
    gy_who_am_i();
    //initialize leds
    led_init();

    //calibrate sensor
    sensor_calibration();
}

static int16_t refactor_calibrate(int16_t value, uint8_t axis) {
    switch (axis) {
        case 0:
            value -= refactor_x;
            break;
        case 1:
            value -= refactor_y;
            break;
        case 2:
            value -= refactor_z;
            break;
        default:
            break;
    }
    return value;
}

static float EMA_filter(float raw_value, uint8_t axis) {
    filter_value[axis] += (ALPHA * (raw_value - filter_value[axis])); //restore scale factor from ALPHA
    return filter_value[axis];
}

void get_angles(void) {
    gy_read_bytes(GY_ACCEL_XOUT_H, buffer, 6);

    float ax = (float)refactor_calibrate(((buffer[0] << 8) | buffer[1]), AXIS_X);
    float ay = (float)refactor_calibrate(((buffer[2] << 8) | buffer[3]), AXIS_Y);
    float az = (float)refactor_calibrate(((buffer[4] << 8) | buffer[5]), AXIS_Z);

    //Roll (rotation around x axis)
    float pitch_raw = atan2f(ay, az) * (180.0f / M_PI);
    pitch = EMA_filter(pitch_raw, AXIS_X);

    //Pitch (rotation around Y axis)
    float roll_raw = atan2f(-ax, sqrtf((ay*ay + az*az))) * (180.0f / M_PI);
    roll = EMA_filter(roll_raw, AXIS_Y);
}

void app_orientation(void) {

    get_angles();
    uint8_t led_bits = build_led_state(roll, pitch);
    shift595(led_bits);

    uart_write_float(roll);
    uart_write_char(',');
    uart_write_float(pitch);
    uart_write_char('\n');
    _delay_ms(100);
}