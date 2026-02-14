#include "uart.h"
#include <avr/io.h>

static uint16_t serial_compute_ubrr(uint32_t baudrate) {
    return (F_CPU / (16UL * baudrate)) - 1;
}

void uart_init(uint16_t baudrate){
    uint16_t ubrr = serial_compute_ubrr(baudrate);
    //set baud rate
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)(ubrr & 0xFF);
    //enable transmiter
    UCSR0B = (1 << TXEN0);
    //frame format: 8 data bits, 1 stop bit, no parity
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}
void uart_write_char(char c){
    //wait until transmit buffer is empty
    while (!(UCSR0A & (1 << UDRE0)));
    //put data into buffer and send
    UDR0 = c;
}
void uart_write_int(int16_t value){
    char buffer[7];
    uint8_t i = 0;

    if (value == 0) {
        uart_write_char('0');
        return;
    }
    if (value < 0) {
        uart_write_char('-');
        value = -value;
    }
    while (value > 0) {
        buffer[i++] = '0' + (value % 10);
        value /= 10;
    }
    while (i > 0) {
        uart_write_char(buffer[--i]);
    }
}

void uart_write_hex(uint8_t value) {
    char hex_chars[] = "0123456789ABCDEF";
    uart_write_string("0x");
    
    // Print high nibble
    uart_write_char(hex_chars[(value >> 4) & 0x0F]);
    
    // Print low nibble
    uart_write_char(hex_chars[value & 0x0F]);
}

void uart_write_float(float value) {
    if (value < 0) {
        uart_write_char('-');
        value = -value;
    }
    int16_t int_part = (uint16_t)value;
    uart_write_int(int_part);
    uart_write_char('.');
    float fraction = value - int_part;
    for (uint8_t i = 0; i < 2; i++) { // 2 -> number of decimals
        fraction *= 10;
    }
    uint16_t frac_part = (uint16_t)(fraction + 0.5f); //rounding
    uart_write_int(frac_part);
}

void uart_write_string(const char *str) {
    while(*str) {
        uart_write_char(*str++);
    }
}