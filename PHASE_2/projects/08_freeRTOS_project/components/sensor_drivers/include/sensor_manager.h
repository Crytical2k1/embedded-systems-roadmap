#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <stdint.h>

typedef struct{
    int16_t data[3];
    uint8_t sensor_id;
} raw_data_t;

void sensor_manager_init(void *pvParameters);

#endif