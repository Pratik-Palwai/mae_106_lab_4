#ifndef ENCODER_HPP
#define ENCODER_HPP

#include <Arduino.h>

class MotorEncoder {
    const float CLICKS_PER_REVOLUTION = 464.1;

    int A_PIN = 0;
    int B_PIN = 0;
    volatile long previous_position = 0;
    volatile long raw_position = 0;
    volatile float raw_velocity = 0;
    volatile unsigned long previous_time = 0;

    void encoderAChange() {
        previous_position = raw_position;
        previous_time = millis();

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
        previous_position = raw_position;
        previous_time = millis();
        
        if (digitalRead(A_PIN) == digitalRead(B_PIN)) { 
            raw_position--; 
            updateVelocity(-1);
        }
        else { 
            raw_position++; 
            updateVelocity(1);
        }
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

    void updateVelocity(int direction) {
        if (previous_time == 0) {
            raw_velocity = 0;
            return;
        }
        
        unsigned long delta_t = millis() - previous_time;
        
        if (delta_t < 1) {
            raw_velocity = 0;
            return;
        }
        
        float delta_p = raw_position - previous_position;
        raw_velocity = (delta_p / CLICKS_PER_REVOLUTION) / (delta_t / 1000.0f);
        
        if (direction > 0) {
            raw_velocity = abs(raw_velocity);
        }
        else if (direction < 0) {
            raw_velocity = -abs(raw_velocity);
        }
    }

    long getPosition() {
        return raw_position;
    }

    float getVelocity() {
        return raw_velocity;
    }

    void resetPosition() {
        raw_position = 0;
        previous_position = 0;
    }
};

#endif