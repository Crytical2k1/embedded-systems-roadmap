#ifndef TELEMETRY_MANAGER_TELEMETRY_MANAGER_H_
#define TELEMETRY_MANAGER_TELEMETRY_MANAGER_H_

#include <stdint.h>

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#include "main.h"
#include "rtc.h"

//Configurtion
#define TELEMETRY_TASK_PRIO 4
#define TELEMETRY_TASK_STACK_SIZE 512
#define TELEMETRY_PACK_PERIOD_MS 50 	//must be faster than teh sensor period

#define TELEMETRY_SYNC_0 0xAA
#define TELEMETRY_SYNC_1 0x55

//Validity mask, set once a sensor has produced at least 1 sample
#define TLM_VALID_IMU 	(1 << 0)
#define TLM_VALID_RTC 	(1 << 1)
#define TLM_VALID_NTC 	(1 << 2)
#define TLM_VALID_PHOTO (1 << 3)

#pragma pack(push, 1)

typedef struct {
	int16_t accel[3];
	int16_t gyro[3];

	uint8_t rtc_seconds;
	uint8_t rtc_minutes;
	uint8_t rtc_hours;
	uint8_t rtc_day;
	uint8_t rtc_date;
	uint8_t rtc_month;
	uint8_t rtc_year;

	float temperature_c;
	float light1;
	float light2;
} telemetry_payload_t;

typedef struct {
	uint8_t			sync[2];	//TELEMETRY_SYNC_0/1, lets OBC re-sync mid-stream
	uint8_t			seq;		//increments every repack, lets OBC detect stale/duplicate reads
	uint8_t 		status;		//TLM_VALID_* mask
	telemetry_payload_t 	payload;
	uint16_t		crc16;
} telemetry_packet_t;

#pragma pack(pop)

#define TELEMETRY_PACKET_SIZE sizeof(telemetry_packet_t)

typedef struct {
	SPI_HandleTypeDef *hspi;
} telemetry_manager_handle_t;

void telemetry_manager_init(void *pvParameters);

void telemetry_update_imu(const int16_t accel[3], const int16_t gyro[3]);
void telemetry_update_rtc(const RTC_DateTime_t *dt);
void telemetry_update_ntc(float temperature_c);
void telemetry_update_photo(float light1, float light2);

#endif /* TELEMETRY_MANAGER_TELEMETRY_MANAGER_H_ */
