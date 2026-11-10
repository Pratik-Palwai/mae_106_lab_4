#ifndef MOTOR_CONTROL_HPP
#define MOTOR_CONTROL_HPP

#include <Arduino.h>
#include <PID_v1.h>

const float CONSTANT_VELOCITY = true; // true: constant and false: sinusoidal
const float DESIRED_VELOCITY = 60.0; // magnitude of constant velocity or sinusoidal amplitude
const float VELOCITY_FREQUENCY = 1.0; // Hz

const float K_P = 1.0, K_I = 1.0, K_D = 0.0; // PID controller gains
double pid_input = 0.0, target_velocity = 0.0, pid_output = 0.0;
PID velocity_correction(&pid_input, &pid_output, &target_velocity, K_P, K_I, K_D, REVERSE);

void motorController(void *param) {
    velocity_correction.SetOutputLimits(-1.0, 1.0);
    velocity_correction.SetMode(AUTOMATIC);

    while(1) {
        if (CONSTANT_VELOCITY) { target_velocity = DESIRED_VELOCITY; }
        else {
            float omega = VELOCITY_FREQUENCY * millis() * 0.000001;
            target_velocity = DESIRED_VELOCITY * sinf(omega);
        }

    }
}

#endif