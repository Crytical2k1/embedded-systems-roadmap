#ifndef PID_H
#define PID_H

typedef struct {
    float kp;
    float ki;
    float kd;

    float integral;
    float previous_error;

    float output_min;
    float output_max;
} pid_controller;

void pid_init(pid_controller *pid,
                float kp,
                float ki,
                float kd,
                float min,
                float max);

float pid_compute(pid_controller *pid,
                    float setpoint,
                    float measurement,
                    float dt);

void pid_reset(pid_controller *pid);

#endif