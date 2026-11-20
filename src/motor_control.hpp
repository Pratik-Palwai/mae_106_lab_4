#ifndef MOTOR_CONTROL_HPP
#define MOTOR_CONTROL_HPP

#include <Arduino.h>

#include "drv8833.hpp"
#include "encoder.hpp"

const float CONSTANT_VELOCITY = false; // true: constant and false: sinusoidal
const float DESIRED_VELOCITY = 60.0; // magnitude of constant velocity or sinusoidal amplitude
const float VELOCITY_FREQUENCY = 1.0; // Hz

const float K_P = 1.0; // gains
double target_velocity = 0.0, pid_output = 0.0;

DRV8833 motor_main(3, 4, 8);
MotorEncoder encoder_main(5, 6);

void motorController(void *param) {
    motor_main.begin();
    encoder_main.begin();

    while(1) {
        if (CONSTANT_VELOCITY) { target_velocity = DESIRED_VELOCITY; }
        else {
            float omega = VELOCITY_FREQUENCY * millis() * 0.001;
            target_velocity = DESIRED_VELOCITY * sinf(omega);
        }

        encoder_main.update();
        pid_output = K_P * (target_velocity - encoder_main.getVelocity());

        motor_main.write(pid_output);

        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

#endif
