#include <avr/io.h>
#include "serial.h"

uint16_t serial_compute_ubrr(uint16_t baudrate) {
    return (F_CPU / (16UL * baudrate)) - 1;
}

void serial_init(uint16_t baudrate){
    uint16_t ubrr = serial_compute_ubrr(baudrate);
    //set baud rate
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)(ubrr & 0xFF);
    //enable transmiter
    UCSR0B = (1 << TXEN0);
    //set format
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void serial_write_char(char c) {
    //wait until buffer is empty
    while (!(UCSR0A & (1 << UDRE0))){
        //busy wait
    }
    //send data
    UDR0 = c;
}

void serial_write_uint(uint16_t value){
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

void serial_write_string(const char *str) {
    while(*str) {
        serial_write_char(*str++);
    }
    serial_write_char('\n');
}