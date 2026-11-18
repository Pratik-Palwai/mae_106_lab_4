#ifndef MOTOR_CONTROL_HPP
#define MOTOR_CONTROL_HPP

#include <Arduino.h>
#include <PID_v1.h>

#include "drv8833.hpp"
#include "encoder.hpp"

const float CONSTANT_VELOCITY = false; // true: constant and false: sinusoidal
const float DESIRED_VELOCITY = 60.0; // magnitude of constant velocity or sinusoidal amplitude
const float VELOCITY_FREQUENCY = 1.0; // Hz

const float K_P = 1.0, K_I = 1.0, K_D = 0.0; // PID controller gains
double pid_input = 0.0, target_velocity = 0.0, pid_output = 0.0;
PID velocity_correction(&pid_input, &pid_output, &target_velocity, K_P, K_I, K_D, REVERSE);

DRV8833 motor_main(3, 4, 8);
MotorEncoder encoder_main(5, 6);

void motorController(void *param) {
    motor_main.begin();
    encoder_main.begin();

    velocity_correction.SetOutputLimits(-1.0, 1.0);
    velocity_correction.SetMode(AUTOMATIC);

    while(1) {
        if (CONSTANT_VELOCITY) { target_velocity = DESIRED_VELOCITY; }
        else {
            float omega = VELOCITY_FREQUENCY * millis() * 0.001;
            target_velocity = DESIRED_VELOCITY * sinf(omega);
        }

        pid_input = encoder_main.getVelocity();

        motor_main.write(target_velocity / DESIRED_VELOCITY);

        vTaskDelay(pdMS_TO_TICKS(2));
    }
}

#endif