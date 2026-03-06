#include "lcd_driver.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "lcd_driver";

static void lcd_enable_pulse(void) {
    gpio_set_level(LCD_E, 1);
    esp_rom_delay_us(1);
    gpio_set_level(LCD_E, 0);
    esp_rom_delay_us(50);
}

static void lcd_send_nibble(uint8_t nibble) {
    //set each pin to the corresponding values of the nibble
    gpio_set_level(LCD_D4, (nibble >> 0) & 0x01);
    gpio_set_level(LCD_D5, (nibble >> 1) & 0x01);
    gpio_set_level(LCD_D6, (nibble >> 2) & 0x01);
    gpio_set_level(LCD_D7, (nibble >> 3) & 0x01);
    //enable pulse
    lcd_enable_pulse();
}

static void lcd_send_byte(uint8_t data, bool is_data) {
    gpio_set_level(LCD_RS, is_data ? 1 : 0); //True send data, False send command
    // as we work in 4 bit mode, data is sent in 2 brakets.
    lcd_send_nibble(data >> 4);
    lcd_send_nibble(data & 0x0F);
}

void lcd_init(void) {
    //set every pin to output
    gpio_config_t lcd_config = {
        .pin_bit_mask = (1ULL << LCD_RS) | (1ULL << LCD_E) | (1ULL << LCD_D4) | (1ULL << LCD_D5) | (1ULL << LCD_D6) | (1ULL << LCD_D7),
        .mode = GPIO_MODE_OUTPUT,
    };
    esp_err_t esp_ret = gpio_config(&lcd_config);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "LCD failed to initialise gpio");
        abort();
    }
    //force lcd reset
    gpio_set_level(LCD_RS, 0); //force RS low before reset
    gpio_set_level(LCD_E, 0);
    vTaskDelay(pdMS_TO_TICKS(40));

    lcd_send_nibble(0x03); // set command
    vTaskDelay(pdMS_TO_TICKS(5));
    lcd_send_nibble(0x03); // repeat command (en case was ignored)
    esp_rom_delay_us(150);
    lcd_send_nibble(0x03); // repeat command ensures synchro)

    lcd_send_nibble(0x02); // 4-bit mode

    //initialize lcd
    lcd_send_byte(0x28, false); // 2 lines, 5x8 font
    lcd_send_byte(0x0C, false); // display on
    lcd_send_byte(0x06, false); // entry mode
    lcd_send_byte(0x01, false); // clear display
}

void lcd_clear(void) {
    lcd_send_byte(0x01, false);
}

void lcd_set_cursor(uint8_t col, uint8_t row) {
    uint8_t row_offsets[] = {0x00, 0x40};
    lcd_send_byte(0x80 | (col + row_offsets[row & 0x01]), false);
}

void lcd_write_char(char c) {
    lcd_send_byte((uint8_t)c, true);
}

void lcd_write_str(const char *str) {
    while (*str) {
        lcd_write_char(*str++);
    }
}