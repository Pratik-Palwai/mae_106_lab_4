#include <Arduino.h>

#include "motor_control.hpp"
#include "serial_output.hpp"

void setup() {
    Serial.begin(115200);
    delay(200);

    xTaskCreate(motorController, "VELOCITY", 4096, NULL, 2, NULL);
    xTaskCreate(serialOutput, "SERIAL", 4096, NULL, 1, NULL);
}

void loop() { }