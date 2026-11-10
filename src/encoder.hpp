#ifndef ENCODER_HPP
#define ENCODER_HPP

#include <Arduino.h>

class Encoder {
    const float REVOLUTIONS_PER_CLICK = 1 / 464.1;

    int A_PIN = 0;
    int B_PIN = 0;
    volatile float position = 0;
    volatile float raw_velocity = 0;
    volatile long previous_time = 0;

    void encoderAChange() {
        previous_time = micros();

        if (digitalRead(A_PIN) == digitalRead(B_PIN)) {
            position -= REVOLUTIONS_PER_CLICK;
            updateVelocity(1);
        }

        if (digitalRead(A_PIN) != digitalRead(B_PIN)) {
            position += REVOLUTIONS_PER_CLICK;
            updateVelocity(-1);
        }
    }
    
    void encoderBChange() {
        previous_time = micros();
        
        if (digitalRead(A_PIN) == digitalRead(B_PIN)) {
            position += REVOLUTIONS_PER_CLICK;
            updateVelocity(1);
        }
        if (digitalRead(A_PIN) != digitalRead(B_PIN)) {
            position -= REVOLUTIONS_PER_CLICK;
            updateVelocity(-1);
        }
    }

    static void ARDUINO_ISR_ATTR isrA(void* arg) { static_cast<Encoder*>(arg)->encoderAChange(); }
    static void ARDUINO_ISR_ATTR isrB(void* arg) { static_cast<Encoder*>(arg)->encoderBChange(); }

public:
    Encoder(int _a_pin, int _b_pin) {
        A_PIN = _a_pin;
        B_PIN = _b_pin;
    }

    void begin() {
        pinMode(A_PIN, INPUT);
        pinMode(B_PIN, INPUT);

        attachInterruptArg(digitalPinToInterrupt(A_PIN), isrA, this, CHANGE);
        attachInterruptArg(digitalPinToInterrupt(B_PIN), isrB, this, CHANGE);
    }

    void updateVelocity(int direction) {
    }
};

#endif