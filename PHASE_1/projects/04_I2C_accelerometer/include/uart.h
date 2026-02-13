#ifndef UART_H
#define UART_H

#include <stdint.h>

void uart_init(uint16_t baudrate);
void uart_write_char(char c);
void uart_write_int(int16_t value);
void uart_write_hex(uint8_t value);
void uart_write_string(const char *str);


#endif