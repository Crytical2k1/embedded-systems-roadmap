#include "oled_display.h"

#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "screen.h"
#include "telemetry_manager.h"

void oled_display_task(void *pvParameters) {
	(void)pvParameters;
	char line[22]; // 128px wide / 6px per char ~ 21 max char per line

	while (1) {

		telemetry_packet_t packet;
		bool have_data = telemetry_get_latest(&packet);

		screen_clear();

		if (!have_data) {
			//screen_draw_string(0, 0, "WAITING FOR HK...");
		} else {

			snprintf(line, sizeof(line), "SEQ:%u", packet.seq);
			screen_draw_string(0, 0, line);

			snprintf(line, sizeof(line), "TEMP:%d.%01dC",
					(int)packet.payload.temperature_c,
					(int)(packet.payload.temperature_c * 10) % 10 < 0
					? -((int)(packet.payload.temperature_c * 10) % 10)
					: (int)(packet.payload.temperature_c * 10) % 10);
			screen_draw_string(0, 10, line);

			snprintf(line, sizeof(line), "L1:%d L2:%d",
					(int)packet.payload.light1, (int)packet.payload.light2);
			screen_draw_string(0, 20, line);

			snprintf(line, sizeof(line), "AX:%d AY:%d AZ:%d",
					packet.payload.accel[0], packet.payload.accel[1], packet.payload.accel[2]);
			screen_draw_string(0, 30, line);

			snprintf(line, sizeof(line), "GX:%d GY:%d GZ:%d",
					packet.payload.gyro[0], packet.payload.gyro[1], packet.payload.gyro[2]);
			screen_draw_string(0, 40, line);

			snprintf(line, sizeof(line), "RTC %02u:%02u:%02u",
					packet.payload.rtc_hours, packet.payload.rtc_minutes, packet.payload.rtc_seconds);
			screen_draw_string(0, 50, line);
		}

		screen_update();

		vTaskDelay(pdMS_TO_TICKS(OLED_REFRESH_PERIOD_MS));
	}
}

void oled_display_init(void *pvParameters) {
	oled_display_handle_t *params = (oled_display_handle_t *)pvParameters;

	if (!screen_init(params->hi2c)) {
		Error_Handler();
	}

	xTaskCreate(oled_display_task, "OLED", OLED_TASK_STACK_SIZE, NULL, OLED_TASK_PRIO, NULL);

}
