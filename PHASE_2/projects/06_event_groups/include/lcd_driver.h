#ifndef LCD_DRIVER_H
#define LCD_DRIVER_H

#include <stdint.h>
#include <driver/gpio.h>

//config
static const gpio_num_t LCD_E = GPIO_NUM_27;
static const gpio_num_t LCD_RS = GPIO_NUM_14;
static const gpio_num_t LCD_D4 = GPIO_NUM_26;
static const gpio_num_t LCD_D5 = GPIO_NUM_25;
static const gpio_num_t LCD_D6 = GPIO_NUM_33;
static const gpio_num_t LCD_D7 = GPIO_NUM_32;

esp_err_t lcd_init(void);
void lcd_write_str(const char *str);
void lcd_set_cursor(uint8_t col, uint8_t row);
void lcd_clear(void);

#endif