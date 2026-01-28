#include <stdint.h>
#include "timer.h"
#include "adc.h"
#include "serial.h"
#include "app.h"

// Configuration
#define SAMPLE_PERIOD_MIN 0
#define SAMPLE_PERIOD_MAX 2000
#define CMD_BUFFER_SIZE 16
#define ADC_CHANNEL 1

// State
static uint32_t sampling_period_ms = 5; //default
static uint32_t last_sampled_time = 0;

//UART command handling
static char cmd_buffer[CMD_BUFFER_SIZE];
static uint8_t cmd_index = 0;



static void app_set_sampling_period(uint32_t new_period) {
    uint32_t now = timer_get_ms();

    sampling_period_ms = new_period;
    last_sampled_time = now;

    serial_write_string("OK S=");
    serial_write_uint16(new_period);
    serial_write_string("ms\n");
}

static void handle_serial_command(char *cmd) {
    //Set format
    if (cmd[0] != 'S') {
        serial_write_string("ERR\n");
        return;
    }
    if (cmd[1] == '\0') {
        serial_write_string("ERR\n");
        return;
    }

    uint32_t new_period = 0;

    for (uint8_t i = 1; cmd[i] != '\0'; i++) {
        if (cmd[i] < '0' || cmd[i] > '9') {
            serial_write_string("ERR\n");
            return;
        }
        new_period = new_period * 10 + (cmd[i] - '0');
    }

    if (new_period < SAMPLE_PERIOD_MIN || new_period > SAMPLE_PERIOD_MAX) {
        serial_write_string("RANGE\n");
        return;
    }

    app_set_sampling_period(new_period);
}

void handle_serial_rx(void) {
    if (!serial_rx_available()) {
        return;
    }
    char c = serial_read_char();

    if (c == '\r') {
        return;
    }

    // End of command
    if (c == '\n') {
        cmd_buffer[cmd_index] = '\0'; //null-terminate
        cmd_index = 0;
        handle_serial_command(cmd_buffer);
        return;
    }

    //Buffer overflow protection
    if (cmd_index >= CMD_BUFFER_SIZE -1) {
        cmd_index = 0; //discard command
        serial_write_string("ERR\n");
        return;
    }

    cmd_buffer[cmd_index++] = c;
}

void app_run_sampling(void) {
    uint32_t now = timer_get_ms();

    if ((now - last_sampled_time) >= sampling_period_ms) {
        uint16_t sample = adc_sample(ADC_CHANNEL);

        //csv format
        serial_write_uint16(now);
        serial_write_char(',');
        serial_write_uint16(sample);
        serial_write_char('\n'); 

        
        last_sampled_time = now;
    }
}

