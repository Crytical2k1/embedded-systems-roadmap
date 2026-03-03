#ifndef SENSOR_TASK_H
#define SENSOR_TASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

void sensor_task_init(QueueHandle_t queue);

#endif