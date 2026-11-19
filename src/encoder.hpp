#ifndef ENCODER_HPP
#define ENCODER_HPP

#include <Arduino.h>

class MotorEncoder {
    const float CLICKS_PER_REVOLUTION = 464.1;
    const unsigned long VELOCITY_WINDOW = 10000; // us between velocity samples

    int A_PIN = 0;
    int B_PIN = 0;
    volatile long raw_position = 0;
    volatile float raw_velocity = 0.0f;
    volatile uint8_t previous_state = 0; // last (A << 1) | B reading
    long previous_position = 0;
    unsigned long previous_time = 0;

    void encoderChange() {
        static const int8_t TRANSITIONS[16] = {
             0, +1, -1,  0,
            -1,  0,  0, +1,
            +1,  0,  0, -1,
             0, -1, +1,  0
        };

        uint8_t state = (digitalRead(A_PIN) << 1) | digitalRead(B_PIN);
        raw_position += TRANSITIONS[(previous_state << 2) | state];
        previous_state = state;
    }

    static void ARDUINO_ISR_ATTR isr(void* arg) { static_cast<MotorEncoder*>(arg)->encoderChange(); }

public:
    MotorEncoder(int _a_pin, int _b_pin) {
        A_PIN = _a_pin;
        B_PIN = _b_pin;
    }

    void begin() {
        pinMode(A_PIN, INPUT_PULLUP);
        pinMode(B_PIN, INPUT_PULLUP);

        previous_state = (digitalRead(A_PIN) << 1) | digitalRead(B_PIN);

        attachInterruptArg(digitalPinToInterrupt(A_PIN), isr, this, CHANGE);
        attachInterruptArg(digitalPinToInterrupt(B_PIN), isr, this, CHANGE);

        previous_position = raw_position;
        previous_time = micros();
    }

    // recomputes velocity once per window when it is called
    void update() {
        unsigned long now = micros();
        unsigned long delta_t = now - previous_time;
        if (delta_t < VELOCITY_WINDOW) { return; }

        long position = raw_position;
        raw_velocity = ((position - previous_position) / CLICKS_PER_REVOLUTION) / (delta_t / 1000000.0f);

        previous_position = position;
        previous_time = now;
    }

    long getPosition() { return raw_position; }

    float getVelocity() { return raw_velocity; }

    void resetPosition() {
        raw_position = 0;
        previous_position = 0;
    }
};

#endif
