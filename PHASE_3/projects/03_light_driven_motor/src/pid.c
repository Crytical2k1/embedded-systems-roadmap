#include "pid.h"

void pid_init(pid_controller *pid,
                float kp,
                float ki,
                float kd,
                float min,
                float max) 
{
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;

    pid->integral = 0.0f;
    pid->previous_error = 0.0f;

    pid->output_min = min;
    pid->output_max = max;
}
void pid_reset(pid_controller *pid) {
    pid-> integral = 0.0f;
    pid-> previous_error = 0.0f;
}

float pid_compute(pid_controller *pid,
                    float setpoint,
                    float measurement,
                    float dt)
{
    float error = setpoint - measurement;

    pid->integral += error * dt;
    if (pid->integral > 100.0f) {
        pid->integral = 100.0f;
    }
    if (pid->integral < -100.0f) {
        pid->integral = -100.0f;
    }

    float derivative = (error - pid->previous_error) / dt;

    float output = 
        pid->kp * error +
        pid->ki * pid->integral +
        pid->kd * derivative;

    pid->previous_error = error;

    if (output < pid->output_min) {
        output = pid->output_min;
    }
    if (output > pid->output_max) {
        output = pid->output_max;
    }

    return output;
}