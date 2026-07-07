#ifndef SPEED_CONTROLLER_H
#define SPEED_CONTROLLER_H

void speed_controller_init(void);
void speed_controller_set_target(float rpm);
float speed_controller_get_target(void);

#endif