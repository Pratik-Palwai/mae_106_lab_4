#ifndef DRV8833_HPP
#define DRV8833_HPP

#include <Arduino.h>

// class to define the motor driver and make requesting motor power easier
class DRV8833 {
    int in_1_pin = 0;
    int in_2_pin = 0;
    int sleep_pin = 0;

public:
    DRV8833(int _in_1_pin, int _in_2_pin, int _sleep_pin) {
        in_1_pin = _in_1_pin;
        in_2_pin = _in_2_pin;
        sleep_pin = _sleep_pin;
    }

    void begin() {
        pinMode(in_1_pin, OUTPUT);
        pinMode(in_2_pin, OUTPUT);
        pinMode(sleep_pin, OUTPUT);

        analogWriteFrequency(20000);
        analogWriteResolution(8);

        digitalWrite(sleep_pin, HIGH);  // wake up the driver once the pins are set
    }

    void write(float target) { // sends a motor power command, from -1.0 for max reverse to 1.0 for max forward
        if (target > 1.0) { target = 1.0; }
        if (target < -1.0) { target = -1.0; }

        unsigned int duty_cycle = static_cast<int>(fabsf(target) * 255.0 + 0.5);

        if (target > 0.0) { // forwards power
            analogWrite(in_1_pin, duty_cycle);
            analogWrite(in_2_pin, 0); 
        }
        else if (target < 0.0f) { // backwards power
            analogWrite(in_1_pin, 0);
            analogWrite(in_2_pin, duty_cycle);
        }
        else { // coast mode
            analogWrite(in_1_pin, 0);
            analogWrite(in_2_pin, 0);
        }
    }

    void brake() { // puts the driver into braking mode which will abruptly stop the motor
        analogWrite(in_1_pin, 255);
        analogWrite(in_2_pin, 255);
    }

};

#endif