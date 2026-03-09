#ifndef IMU_TASK_H
#define IMU_TASK_H

#include <stdio.h>

void imu_task_init(void *pvParameters);
void imu_task(void *pvParameters);

#endif