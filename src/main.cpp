#include <Arduino.h>

#include "drv8833.hpp"

DRV8833 motor_driver(3, 4, 9, 10);

void setup() {
    Serial.begin(115200);
    motor_driver.begin();
}

void loop() {
    motor_driver.write(-1.0);
    delay(1000);

    motor_driver.write(1.0);
    delay(1000);
}