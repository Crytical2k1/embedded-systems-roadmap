#include <avr/io.h>
#include "serial.h"

//baud rate calculation
static uint16_t serial_compute_ubrr(uint32_t baudrate) {
    return (F_CPU / (16UL * baudrate)) - 1;
}

void serial_init(uint32_t baudrate) {
    uint16_t ubrr = serial_compute_ubrr(baudrate);
    //set baud rate
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)(ubrr & 0xFF);
    //enable transmiter
    UCSR0B = (1 << TXEN0);
    //Frame format: 8 data bits, 1 stop bit, no parity
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void serial_write_char(char c) {
    //wait until transmit buffer is empty
    while (!(UCSR0A & (1 <<UDRE0))) {
        //busy wait
    }
    //put data into buffer and send data
    UDR0 = c;
}
void serial_write_uint16(uint16_t value) {
    //simple ASCII converter
    char buffer[6]; 
    uint8_t i = 0;

    if (value == 0) {
        serial_write_char('0');
        return;
    }
    while (value > 0) {
        buffer[i++] = '0' + (value % 10);
        value /= 10;
    }
    while (i > 0) {
        serial_write_char(buffer[--i]);
    }
}