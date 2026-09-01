#include "telemetry_manager.h"

#include <string.h>

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

static SPI_HandleTypeDef *s_hspi = NULL;
static GPIO_TypeDef *s_cs_port = NULL;
static uint16_t s_cs_pin = 0;

static SemaphoreHandle_t s_snapshot_mutex;
static telemetry_packet_t s_latest_packet;
static bool s_have_valid_packet = false;

//forward declaration
void telemetry_manager_init(void *pvParameters);
static void telemetry_manager_task(void *pvParameters);

static uint16_t crc16_ccitt(const uint8_t *data, size_t len);
bool telemetry_request(telemetry_packet_t *packet);
bool telemetry_get_latest(telemetry_packet_t *packet);

//CRC-16
static uint16_t crc16_ccitt(const uint8_t *data, size_t len) {
	uint16_t crc = 0xFFFF;
	for (size_t i = 0; i < len; i++) {
		crc ^= (uint16_t)data[i] << 8;
		for (int b = 0; b < 8; b++) {
			crc = (crc & 0x8000) ? (uint16_t)((crc << 1) ^ 0x1021) : (uint16_t)(crc << 1);
		}
	}
	return crc;
}

bool telemetry_request(telemetry_packet_t *packet) {
	if (s_hspi == NULL) {
		return false; //telemetry_manager_init has not run yet
	}

	telemetry_packet_t rx = {0};
	uint8_t tx[TELEMETRY_PACKET_SIZE] = {0}; // dummy bytes, HK ignores its rx for now

	HAL_GPIO_WritePin(s_cs_port, s_cs_pin, GPIO_PIN_RESET); // cs low = start of frame

	vTaskDelay(pdMS_TO_TICKS(10));

	HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(s_hspi, tx, (uint8_t *)&rx, TELEMETRY_PACKET_SIZE, pdMS_TO_TICKS(100));

	HAL_GPIO_WritePin(s_cs_port, s_cs_pin, GPIO_PIN_SET); // cs high = end of frame

	if (status != HAL_OK) {
		return false;
	}

	if (rx.sync[0] != TELEMETRY_SYNC_0 || rx.sync[1] != TELEMETRY_SYNC_1) {
		return false; // some issues on wiring/timing or HK not armed
	}

	uint16_t expected_crc = crc16_ccitt((const uint8_t *)&rx, offsetof(telemetry_packet_t,crc16));
	if (expected_crc != rx.crc16) {
		return false; //corrupted in transit, skip this one
	}

	//valid packet
	if (xSemaphoreTake(s_snapshot_mutex, pdMS_TO_TICKS(20)) == pdTRUE) {
		s_latest_packet = rx;
		s_have_valid_packet = true;
		xSemaphoreGive(s_snapshot_mutex);
	}

	if (packet != NULL) {
		*packet = rx;
	}

	return true;
}

bool telemetry_get_latest(telemetry_packet_t *packet) {
	bool have = false;
	if (xSemaphoreTake(s_snapshot_mutex, pdMS_TO_TICKS(20)) == pdTRUE) {
		if (s_have_valid_packet) {
			*packet = s_latest_packet;
			have = true;
		}
		xSemaphoreGive(s_snapshot_mutex);
	}
	return have;
}

//main tasks
static void telemetry_manager_task(void *pvParameters) {
	(void)pvParameters;
	while (1) {
		telemetry_packet_t packet;
		if (telemetry_request(&packet)) {
			//if true, got a valid packet
			//add handling later
		}
		vTaskDelay(pdMS_TO_TICKS(TELEMETRY_UNPACK_PERIOD));
	}
}

void telemetry_manager_init(void *pvParameters) {
	telemetry_manager_handle_t *params = (telemetry_manager_handle_t *)pvParameters;
	s_hspi = params->hspi;
	s_cs_port = params->cs_port;
	s_cs_pin = params->cs_pin;

	HAL_GPIO_WritePin(s_cs_port, s_cs_pin, GPIO_PIN_SET); //idle high

	s_snapshot_mutex = xSemaphoreCreateMutex();
	if (s_snapshot_mutex == NULL) {
		Error_Handler();
	}
	xTaskCreate(telemetry_manager_task, "TLM_POLL", TELEMETRY_TASK_STACK_SIZE, NULL, TELEMETRY_TASK_PRIO, NULL);
}
