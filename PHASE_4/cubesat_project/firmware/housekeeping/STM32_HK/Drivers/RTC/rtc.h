#ifndef RTC_RTC_H_
#define RTC_RTC_H_

#include "main.h"
#include <stdint.h>

static const uint8_t RTC_ADDR = (0x68 << 1);

static const uint8_t RTC_REG_SECONDS = 0x00;
static const uint8_t RTC_REG_MINUTES = 0x01;
static const uint8_t RTC_REG_HOURS = 0x02;
static const uint8_t RTC_REG_DAY = 0x03;
static const uint8_t RTC_REG_DATE = 0x04;
static const uint8_t RTC_REG_MONTH = 0x05;
static const uint8_t RTC_REG_YEAR = 0x06;
static const uint8_t RTC_REG_CONTROL = 0x07;

typedef struct {
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;

	uint8_t day;
	uint8_t date;
	uint8_t month;
	uint8_t year;
} RTC_DateTime_t;

HAL_StatusTypeDef RTC_init(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef RTC_GetDateTime(I2C_HandleTypeDef *hi2c, RTC_DateTime_t *datetime);
HAL_StatusTypeDef RTC_SetDateTime(I2C_HandleTypeDef *hi2c, RTC_DateTime_t *datetime);
void RTC_task_create(I2C_HandleTypeDef *hi2c);

#endif /* RTC_RTC_H_ */
