#include "sensor_manager.h"

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include <stdio.h>

#include "imu.h"
#include "imu_calibration.h"
#include "rtc.h"
#include "ntc_sensor.h"
#include "cds_sensor.h"

static const char *TAG = "sensor_manager";

//Configuration
#define IMU_TASK_PRIO 5
#define RTC_TASK_PRIO 5
#define NTC_TASK_PRIO 5
#define PHOTO_TASK_PRIO 5

#define IMU_TASK_STACK_SIZE 512
#define RTC_TASK_STACK_SIZE 512
#define NTC_TASK_STACK_SIZE 512
#define PHOTO_TASK_STACK_SIZE 512

#define IMU_LOOP_PERIOD_MS 100
#define RTC_LOOP_PERIOD_MS 1000
#define NTC_LOOP_PERIOD_MS 500
#define PHOTO_LOOP_PERIOD_MS 100

#define NTC_ADC_CHANNEL ADC_CHANNEL_2 	//PA1
#define CDS1_ADC_CHANNEL ADC_CHANNEL_1	//PA2
#define CDS2_ADC_CHANNEL ADC_CHANNEL_3	//PA3

//Event bits
#define EVENT_IMU_READY (1 << 0)
#define EVENT_RTC_READY (1 << 1)
#define EVENT_NTC_READY (1 << 2)
#define EVENT_PHOTO_READY (1 << 3)

#define ALL_SENSORS_READY (EVENT_IMU_READY | EVENT_RTC_READY | EVENT_NTC_READY | EVENT_PHOTO_READY)

//Event group
static EventGroupHandle_t sensor_events;

//Forward declaration
static void imu_task(void *pvParameters);
static void rtc_task(void *pvParameters);
static void ntc_task(void *pvParameters);
static void photo_task(void *pvParameters);

//Sensor Manager
void sensor_manager_init(void *pvParameters) {
	SensorManagerHandle_t *params = (SensorManagerHandle_t *)pvParameters;

	I2C_HandleTypeDef *hi2c = params->hi2c;
	ADC_HandleTypeDef *hadc = params->hadc;

	//Create event group
	sensor_events = xEventGroupCreate();

	if (sensor_events == NULL) {
		Error_Handler();
	}

	//IMU
	if (imu_init(hi2c) == HAL_OK) {
		//printf("%s IMU initialize\r\n", TAG);
		imu_calibration(hi2c);

		xEventGroupSetBits(sensor_events, EVENT_IMU_READY);
	} else {
		//printf("%s IMU initialization failed\r\n", TAG);
	}
	//RTC
	if (RTC_init(hi2c) == HAL_OK) {
		//printf("%s RTC initialize\r\n", TAG);
		xEventGroupSetBits(sensor_events, EVENT_RTC_READY);
	} else {
		//printf("%s RTC initialization failed\r\n", TAG);
	}
	//NTC
	if (NTC_init(hadc) == HAL_OK) {
		//printf("%s NTC initialize\r\n", TAG);
		xEventGroupSetBits(sensor_events, EVENT_NTC_READY);
	} else {
		//printf("%s NTC initialization failed\r\n", TAG);
	}
	//PHOTO
	if (photo_init(hadc) == HAL_OK) {
		//printf("%s Photoresistor initialize\r\n", TAG);
		xEventGroupSetBits(sensor_events, EVENT_PHOTO_READY);
	} else {
		//printf("%s Photoresistor initialization failed\r\n", TAG);
	}

	//Wait for sensors
	EventBits_t bits = xEventGroupWaitBits(sensor_events, ALL_SENSORS_READY, pdFALSE, pdTRUE, pdMS_TO_TICKS(1000));

	if ((bits & ALL_SENSORS_READY) != ALL_SENSORS_READY) {
		//printf("Not all sensors initialize\r\n");
		// We can make the system continue even if some didnt initialize or break the code
	}

	// Start sensor tasks
	xTaskCreate(imu_task, "IMU", IMU_TASK_STACK_SIZE, hi2c, IMU_TASK_PRIO, NULL);
	xTaskCreate(rtc_task, "RTC", RTC_TASK_STACK_SIZE, hi2c, RTC_TASK_PRIO, NULL);
	xTaskCreate(ntc_task, "NTC", NTC_TASK_STACK_SIZE, hadc, NTC_TASK_PRIO, NULL);
	xTaskCreate(photo_task, "PHOTO", PHOTO_TASK_STACK_SIZE, hadc, PHOTO_TASK_PRIO, NULL);
	// Sensor manager finished
	vTaskDelete(NULL);
}

//Sensor Tasks
static void imu_task(void *pvParameters) {
	I2C_HandleTypeDef *hi2c = (I2C_HandleTypeDef *)pvParameters;

	int16_t accel[3];
	int16_t gyro[3];

	while(1) {
		//Read IMU
		if (imu_read(hi2c, accel, gyro) == HAL_OK) {
			imu_apply_calibration(accel, gyro);

		} else {
			//printf("IMU read failed\r\n");
		}

		//Send data to sensor queue
		// sensor_queue_send();

		vTaskDelay(pdMS_TO_TICKS(IMU_LOOP_PERIOD_MS));
	}
}

static void rtc_task(void *pvParameters) {
	I2C_HandleTypeDef *hi2c = (I2C_HandleTypeDef *)pvParameters;
	RTC_DateTime_t rtc =
	  {
	      .seconds = 0,
	      .minutes = 40,
	      .hours   = 14,
	      .day     = 3,
	      .date    = 19,
	      .month   = 8,
	      .year    = 26
	  };

	RTC_SetDateTime(hi2c, &rtc);

	while(1) {

		if (RTC_GetDateTime(hi2c, &rtc) == HAL_OK) {
			// Store/update system time
			rtc;

		} else {
			//printf("RTC read failed\r\n");
		}

		vTaskDelay(pdMS_TO_TICKS(RTC_LOOP_PERIOD_MS));
	}
}

static void ntc_task(void *pvParameters) {
	ADC_HandleTypeDef *hadc = (ADC_HandleTypeDef *)pvParameters;
	float temperature;

	while(1) {
		//read ntc
		if (NTC_read_temperature(hadc, NTC_ADC_CHANNEL, &temperature) == HAL_OK) {
			//send data to sensor queue
			temperature;

		} else {
			//printf("NTC read failed\r\n");
		}

		vTaskDelay(pdMS_TO_TICKS(NTC_LOOP_PERIOD_MS));
	}
}

static void photo_task(void *pvParameters) {
	ADC_HandleTypeDef *hadc = (ADC_HandleTypeDef *)pvParameters;
	float light1, light2;

	while(1) {
		//read cds
		if (photo_read_light(hadc, CDS1_ADC_CHANNEL, &light1) == HAL_OK &&
			photo_read_light(hadc, CDS2_ADC_CHANNEL, &light2) == HAL_OK) {
			//send data to sensor queue
			light1;
			light2;

		} else {
			//printf("CDS read failed\r\n");
		}

		vTaskDelay(pdMS_TO_TICKS(PHOTO_LOOP_PERIOD_MS));
	}
}
