#include "telemetry_manager.h"

#include <string.h>

//static const char *TAG = "telemetry_manager";

static SemaphoreHandle_t 	s_snapshot_mutex;
static telemetry_payload_t 	s_snapshot;
static uint8_t 				s_snapshot_status = 0;

static telemetry_packet_t s_tx_packet;
static telemetry_packet_t s_rx_scratch;

static uint8_t s_seq_counter = 0;
static SPI_HandleTypeDef *s_hspi = NULL;

//forward declaration
void telemetry_manager_init(void *pvParameters);
static void telemetry_manager_task(void *pvParameters); //refresh the packet

void telemetry_update_imu(const int16_t accel[3], const int16_t gyro[3]);
void telemetry_update_rtc(const RTC_DateTime_t *dt);
void telemetry_update_ntc(float temperature_c);
void telemetry_update_photo(float light1, float light2);

static uint16_t crc16_ccitt(const uint8_t *data, size_t len); //crc16 checksum
static void refresh_tx_packet(void); //copies the latest snapshot into the packet buffer


//sensor tasks
void telemetry_update_imu(const int16_t accel[3], const int16_t gyro[3]) {
	if (xSemaphoreTake(s_snapshot_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
		s_snapshot.accel[0] = accel[0];
		s_snapshot.accel[1] = accel[1];
		s_snapshot.accel[2] = accel[2];
		s_snapshot.gyro[0] = gyro[0];
		s_snapshot.gyro[1] = gyro[1];
		s_snapshot.gyro[2] = gyro[2];

		s_snapshot_status |= TLM_VALID_IMU;
		xSemaphoreGive(s_snapshot_mutex);
	}
}

void telemetry_update_rtc(const RTC_DateTime_t *dt) {
	if (xSemaphoreTake(s_snapshot_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
		s_snapshot.rtc_seconds = dt->seconds;
		s_snapshot.rtc_minutes = dt->minutes;
		s_snapshot.rtc_hours = dt->hours;
		s_snapshot.rtc_day = dt->day;
		s_snapshot.rtc_date = dt->date;
		s_snapshot.rtc_month = dt->month;
		s_snapshot.rtc_year = dt->year;

		s_snapshot_status |= TLM_VALID_RTC;
		xSemaphoreGive(s_snapshot_mutex);
	}
}

void telemetry_update_ntc(float temperature_c) {
	if (xSemaphoreTake(s_snapshot_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
		s_snapshot.temperature_c = temperature_c;

		s_snapshot_status |= TLM_VALID_NTC;
		xSemaphoreGive(s_snapshot_mutex);
	}
}

void telemetry_update_photo(float light1, float light2) {
	if (xSemaphoreTake(s_snapshot_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
		s_snapshot.light1 = light1;
		s_snapshot.light2 = light2;

		s_snapshot_status |= TLM_VALID_PHOTO;
		xSemaphoreGive(s_snapshot_mutex);
	}
}

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

//refresh packets
static void refresh_tx_packet(void) {
	s_tx_packet.sync[0] = TELEMETRY_SYNC_0;
	s_tx_packet.sync[1] = TELEMETRY_SYNC_1;
	s_tx_packet.seq 	= s_seq_counter++;

	if (xSemaphoreTake(s_snapshot_mutex, pdMS_TO_TICKS(20)) == pdTRUE) {
		s_tx_packet.payload = s_snapshot;
		s_tx_packet.status = s_snapshot_status;

		xSemaphoreGive(s_snapshot_mutex);
	}
	s_tx_packet.crc16 = crc16_ccitt((const uint8_t *)&s_tx_packet, offsetof(telemetry_packet_t, crc16));
}

//
static void arm_spi_for_next_transfer(void) {
	HAL_SPI_TransmitReceive_DMA(
			s_hspi,
			(uint8_t *)&s_tx_packet,
			(uint8_t *)&s_rx_scratch,
			TELEMETRY_PACKET_SIZE
	);
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {
	if (hspi == s_hspi) {
		arm_spi_for_next_transfer();
	}
}
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi) {
	if (hspi == s_hspi) {
		HAL_SPI_Abort(hspi);
		arm_spi_for_next_transfer();
	}
}

//main tasks
static void telemetry_manager_task(void *pvParameters) {
	while(1) {
		refresh_tx_packet();
		vTaskDelay(pdMS_TO_TICKS(TELEMETRY_PACK_PERIOD_MS));
	}
}

void telemetry_manager_init(void *pvParameters) {
	telemetry_manager_handle_t *params = (telemetry_manager_handle_t *)pvParameters;
	s_hspi = params->hspi;

	memset(&s_snapshot, 0, sizeof(s_snapshot));
	memset(&s_tx_packet, 0, sizeof(s_tx_packet));

	s_snapshot_mutex = xSemaphoreCreateMutex();
	if (s_snapshot_mutex == NULL) {
		Error_Handler();
	}

	refresh_tx_packet(); //build a valid first packet before arming the spi

	xTaskCreate(telemetry_manager_task, "TLM", TELEMETRY_TASK_STACK_SIZE, NULL, TELEMETRY_TASK_PRIO, NULL);

	arm_spi_for_next_transfer();
}
