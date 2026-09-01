#ifndef TELEMETRY_MANAGER_TELEMETRY_MANAGER_H_
#define TELEMETRY_MANAGER_TELEMETRY_MANAGER_H_

#include <stdbool.h>
#include <stdint.h>

#include "main.h"

//Configuration
#define TELEMETRY_TASK_PRIO 4
#define TELEMETRY_TASK_STACK_SIZE 512
#define TELEMETRY_UNPACK_PERIOD 200

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
	GPIO_TypeDef *cs_port;
	uint16_t cs_pin;
} telemetry_manager_handle_t;

void telemetry_manager_init(void *pvParameters);
bool telemetry_request(telemetry_packet_t *packet);
bool telemetry_get_latest(telemetry_packet_t *packet);

#endif /* TELEMETRY_MANAGER_TELEMETRY_MANAGER_H_ */
