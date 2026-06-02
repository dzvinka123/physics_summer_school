#include "servo_motor.h"

servo_motor::servo_motor(int pin) {
    _pin = pin;
    _currentMicros = 1000;
}

void servo_motor::begin() {
    _servo.attach(_pin, MIN_MICROS, MAX_MICROS);
}

void servo_motor::set_micros(int us) {
    if (us < MIN_MICROS) us = MIN_MICROS;
    if (us > MAX_MICROS) us = MAX_MICROS;
    
    _currentMicros = us;
    _servo.writeMicroseconds(_currentMicros);
}

void servo_motor::tick_right() {
    set_micros(_currentMicros + DEAD_BAND);
}

void servo_motor::tick_left() {
    set_micros(_currentMicros - DEAD_BAND);
}

void servo_motor::set_angle(int angle) {
    if (angle < -60) angle = -60;
    if (angle > 60) angle = 60;

    int us = map(angle, -60, 60, MIN_MICROS, MAX_MICROS);
    set_micros(us);
}
