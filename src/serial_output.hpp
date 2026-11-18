#ifndef SERIAL_OUTPUT_HPP
#define SERIAL_OUTPUT_HPP

#include <Arduino.h>

#include "motor_control.hpp"

void serialOutput(void *param) {
    while (1) {
        Serial.print(">time:" + String(micros() / 1000000.0));
        Serial.print(",raw_position:" + String(encoder_main.getPosition()));
        Serial.print(",raw_velocity:" + String(encoder_main.getVelocity()));
        Serial.print(",pid_input:" + String(pid_input));
        Serial.print(",target_velocity:" + String(target_velocity) + "\r\n");

        vTaskDelay(pdMS_TO_TICKS(25));
    }
}

#endif