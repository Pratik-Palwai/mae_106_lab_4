#ifndef ENCODER_HPP
#define ENCODER_HPP

#include <Arduino.h>

class MotorEncoder {
    const float CLICKS_PER_REVOLUTION = 464.1;
    const unsigned long VELOCITY_TIMEOUT = 20000; // us without a tick before velocity reads as zero

    int A_PIN = 0;
    int B_PIN = 0;
    volatile long raw_position = 0;
    volatile unsigned long tick_period = 0; // us between the last two ticks, 0 if unknown
    volatile int tick_direction = 0;
    volatile unsigned long previous_time = 0;

    void encoderAChange() {
        if (digitalRead(A_PIN) == digitalRead(B_PIN)) { 
            raw_position++; 
            updateVelocity(1);
        }
        else { 
            raw_position--; 
            updateVelocity(-1);
        }
    }
    
    void encoderBChange() {
        if (digitalRead(A_PIN) == digitalRead(B_PIN)) { 
            raw_position--; 
            updateVelocity(-1);
        }
        else { 
            raw_position++; 
            updateVelocity(1);
        }
    }

    // Called from the ISRs: only record timing, velocity is computed in getVelocity()
    void updateVelocity(int direction) {
        unsigned long now = micros();
        unsigned long delta_t = now - previous_time;

        if (previous_time == 0 || delta_t > VELOCITY_TIMEOUT || direction != tick_direction) { tick_period = 0; }
        else { tick_period = delta_t; }

        tick_direction = direction;
        previous_time = now;
    }

    static void ARDUINO_ISR_ATTR isrA(void* arg) { static_cast<MotorEncoder*>(arg)->encoderAChange(); }
    static void ARDUINO_ISR_ATTR isrB(void* arg) { static_cast<MotorEncoder*>(arg)->encoderBChange(); }

public:
    MotorEncoder(int _a_pin, int _b_pin) {
        A_PIN = _a_pin;
        B_PIN = _b_pin;
    }

    void begin() {
        pinMode(A_PIN, INPUT);
        pinMode(B_PIN, INPUT);

        attachInterruptArg(digitalPinToInterrupt(A_PIN), isrA, this, CHANGE);
        attachInterruptArg(digitalPinToInterrupt(B_PIN), isrB, this, CHANGE);
    }

    long getPosition() { return raw_position; }

    // Revolutions per second, signed by direction
    float getVelocity() {
        unsigned long period = tick_period;
        unsigned long last = previous_time;
        int direction = tick_direction;

        if (period == 0 || micros() - last > VELOCITY_TIMEOUT) { return 0.0f; }

        return direction / (CLICKS_PER_REVOLUTION * (period / 1000000.0f));
    }

    void resetPosition() { raw_position = 0; }
};

#endif
