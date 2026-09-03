#ifndef OLED_DISPLAY_OLED_DISPLAY_H_
#define OLED_DISPLAY_OLED_DISPLAY_H_

#include "main.h"

#define OLED_TASK_PRIO 3
#define OLED_TASK_STACK_SIZE 1028
#define OLED_REFRESH_PERIOD_MS 300

typedef struct {
	I2C_HandleTypeDef *hi2c;
} oled_display_handle_t;

void oled_display_init(void *pvParameters);

#endif /* OLED_DISPLAY_OLED_DISPLAY_H_ */
