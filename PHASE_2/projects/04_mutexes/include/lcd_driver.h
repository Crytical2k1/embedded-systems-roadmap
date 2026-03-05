#ifndef LCD_DRIVER_H
#define LCD_DRIVER_H

#include <stdint.h>
#include "driver/gpio.h"

//pin configuration
#define LCD_RS GPIO_NUM_14 
#define LCD_E GPIO_NUM_27
#define LCD_D4 GPIO_NUM_26
#define LCD_D5 GPIO_NUM_25
#define LCD_D6 GPIO_NUM_33
#define LCD_D7 GPIO_NUM_32


void lcd_init(void);
void lcd_clear(void);
void lcd_set_cursor(uint8_t col, uint8_t row);
void lcd_write_char(char c);
void lcd_write_str(const char *str);

#endif