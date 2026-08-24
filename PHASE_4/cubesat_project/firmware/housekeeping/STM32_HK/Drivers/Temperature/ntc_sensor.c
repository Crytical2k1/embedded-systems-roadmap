#include "ntc_sensor.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "event_groups.h"
#include "queue.h"

#include "main.h"

static const char *TAG = "ntc_driver";


//forward declaration
static void ntc_manager_init(void *pvParameters);
static void ntc_read_timer(TimerHandle_t xTimer);

void NTC_init(void) {

}
