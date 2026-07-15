#ifndef ANGLE_CONTROLLER_H
#define ANGLE_CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>

void angle_controller_init(uint8_t prio);

void angle_controller_set_target(float angle_deg);
float angle_controller_get_target(void);
void angle_controller_enable(bool enable);

#endif