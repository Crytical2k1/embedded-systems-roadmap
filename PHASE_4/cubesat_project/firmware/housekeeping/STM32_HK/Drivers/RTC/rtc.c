#include "rtc.h"
#include "i2c_manager.h"

#include <stdio.h>

static const char *TAG = "rtc_task";

//forward declaration
HAL_StatusTypeDef RTC_init(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef RTC_GetDateTime(I2C_HandleTypeDef *hi2c, RTC_DateTime_t *datetime);
HAL_StatusTypeDef RTC_SetDateTime(I2C_HandleTypeDef *hi2c, RTC_DateTime_t *datetime);
static uint8_t BCD_to_Decimal(uint8_t bcd);
static uint8_t Decimal_to_BCD(uint8_t decimal);

//Code Starts
HAL_StatusTypeDef RTC_init(I2C_HandleTypeDef *hi2c) {
	// Check if the RTC responds on the I2C bus
	//I2C_manager_lock();

	HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(
			hi2c,
			0xD0,
			3,
			HAL_MAX_DELAY
	);

	//I2C_manager_unlock();
	HAL_StatusTypeDef rtc_error = hi2c->ErrorCode;

	return status;
}

//Convert BCD to decimal
static uint8_t BCD_to_Decimal(uint8_t bcd) {
	return ((bcd >> 4) * 10) + (bcd & 0x0F);
}
//Convert decimal to BCD
static uint8_t Decimal_to_BCD(uint8_t decimal) {
	return ((decimal / 10) << 4) | (decimal % 10);
}

//Get date time function
HAL_StatusTypeDef RTC_GetDateTime(I2C_HandleTypeDef *hi2c, RTC_DateTime_t *datetime) {
	uint8_t data[7] = {0};

	HAL_StatusTypeDef status;

	I2C_manager_lock();

	status = HAL_I2C_Mem_Read(
			hi2c,
			RTC_ADDR,
			RTC_REG_SECONDS,
			I2C_MEMADD_SIZE_8BIT,
			data,
			7,
			HAL_MAX_DELAY
	);

	I2C_manager_unlock();

	if (status != HAL_OK) {
		return status;
	}
	// Convert BCD values from the RTC into decimal values
	datetime->seconds = BCD_to_Decimal(data[0] & 0x7F);
	datetime->minutes = BCD_to_Decimal(data[1] & 0x7F);
	datetime->hours = BCD_to_Decimal(data[2] & 0x3F);
	datetime->day = BCD_to_Decimal(data[3] & 0x07);
	datetime->date = BCD_to_Decimal(data[4] & 0x3F);
	datetime->month = BCD_to_Decimal(data[5] & 0x1F);
	datetime->year = BCD_to_Decimal(data[6]);

	return HAL_OK;
}

//Set date time
HAL_StatusTypeDef RTC_SetDateTime(I2C_HandleTypeDef *hi2c, RTC_DateTime_t *datetime) {
	uint8_t data[7] = {0};

	data[0] = Decimal_to_BCD(datetime->seconds) & 0x7F;
	data[1] = Decimal_to_BCD(datetime->minutes);
	data[2] = Decimal_to_BCD(datetime->hours);
	data[3] = Decimal_to_BCD(datetime->day);
	data[4] = Decimal_to_BCD(datetime->date);
	data[5] = Decimal_to_BCD(datetime->month);
	data[6] = Decimal_to_BCD(datetime->year);

	HAL_StatusTypeDef status;

	I2C_manager_lock();

	status = HAL_I2C_Mem_Write(
			hi2c,
			RTC_ADDR,
			RTC_REG_SECONDS,
			I2C_MEMADD_SIZE_8BIT,
			data,
			7,
			HAL_MAX_DELAY
	);

	I2C_manager_unlock();

	return status;
}
