#ifndef SYSTEM_EVENTS_H
#define SYSTEM_EVENTS_H

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#define EVENT_NTC_INIT (1ULL << 0)
#define EVENT_PHOTO_INIT (1ULL << 1)
#define EVENT_SYSTEM_READY (1ULL << 2)

#define ALL_INIT_DONE (EVENT_NTC_INIT | EVENT_PHOTO_INIT)

extern EventGroupHandle_t system_event_group;

#endif