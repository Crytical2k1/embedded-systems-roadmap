#ifndef CONTROLLER_TASK_H
#define CONTROLLER_TASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

void controller_task_init(QueueHandle_t queue);

#endif