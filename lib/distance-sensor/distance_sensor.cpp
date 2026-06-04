#include "distance_sensor.h"

DistanceSensor::DistanceSensor(uint8_t trigPin, uint8_t echoPin)
    : _trigPin(trigPin), _echoPin(echoPin),
      _risingTime(0), _fallingTime(0),
      _ready(false), _measuring(false) {}

void DistanceSensor::begin() {
    pinMode(_trigPin, OUTPUT);
    digitalWrite(_trigPin, LOW);
    pinMode(_echoPin, INPUT);

    attachInterruptArg(
        digitalPinToInterrupt(_echoPin),
        _echoISR,
        this,
        CHANGE
    );
}

void DistanceSensor::send_signal() {
    _ready = false;
    _measuring = true;

    digitalWrite(_trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(_trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(_trigPin, LOW);
}

void IRAM_ATTR DistanceSensor::_echoISR(void *arg) {
    DistanceSensor *self = static_cast<DistanceSensor *>(arg);

    if (!self->_measuring) return;

    if (digitalRead(self->_echoPin) == HIGH) {
        self->_risingTime = micros();
    } else {
        self->_fallingTime = micros();
        self->_ready = true;
        self->_measuring = false;
    }
}

bool DistanceSensor::is_ready() {
    return _ready;
}

uint16_t DistanceSensor::get_distance() {
    if (!_ready) return 0;

    uint32_t duration = _fallingTime - _risingTime;

    // дистанція = (час_мкс * 0.0343) / 2
    // щоб уникнути float: (duration * 343) / 20000
    uint16_t distance = (uint16_t)((duration * 343UL) / 20000UL);

    // SR04 діапазон: 2мм–4000мм
    if (distance < 20)   distance = 20;
    if (distance > 4000) distance = 4000;

    _ready = false;
    return distance;
}