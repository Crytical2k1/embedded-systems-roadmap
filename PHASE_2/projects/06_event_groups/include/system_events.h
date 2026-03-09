#ifndef SYSTEM_EVENTS_H
#define SYSTEM_EVENTS_H

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#define EVENT_PWR_BTN_PRESSED (1 << 0)
#define EVENT_IMU_INIT_DONE (1 << 1)
#define EVENT_LCD_INIT_DONE (1 << 2)
#define EVENT_SYSTEM_READY (1 << 3)

#define ALL_INITS_DONE (EVENT_IMU_INIT_DONE | EVENT_LCD_INIT_DONE)

extern EventGroupHandle_t system_event_group;

#endif