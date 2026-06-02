#ifndef SERVO_MOTOR_H
#define SERVO_MOTOR_H

#include <Arduino.h>
#include <ESP32Servo.h>

class servo_motor {
private:
    Servo _servo;
    int _pin;
    int _currentMicros;

    const int MIN_MICROS = 1000;
    const int MAX_MICROS = 2000;
    const int DEAD_BAND  = 6;

public:
    servo_motor(int pin);
    void begin();
    
    void set_micros(int us); 
    void tick_right();
    void tick_left();

    void set_angle(int angle);
};

#endif