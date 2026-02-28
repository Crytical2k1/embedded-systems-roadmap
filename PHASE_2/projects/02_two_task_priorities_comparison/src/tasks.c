#include <stdio.h>
#include "tasks.h"

static void high_priority_task(void *pvParameters);
static void medium_priority_task(void *pvParameters);
static void low_priority_task(void *pvParameters);

void create_tasks(void) {
    xTaskCreate(
        high_priority_task,
        "HighTask",
        2048,
        NULL,
        3,
        NULL
    );
    xTaskCreate(
        medium_priority_task,
        "MediumhTask",
        2048,
        NULL,
        2,
        NULL
    );
    xTaskCreate(
        low_priority_task,
        "LowTask",
        2048,
        NULL,
        1,
        NULL
    );
}

static void low_priority_task(void *pvParameters) {
    while(1) {
        printf("Low priority running\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void medium_priority_task(void *pvParameters) {
    while(1) {
        printf("Medium priority running\n");
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

static void high_priority_task(void *pvParameters) {
    while(1) {
        printf("High priority running\n");
        vTaskDelay(pdMS_TO_TICKS(250));
    }
}