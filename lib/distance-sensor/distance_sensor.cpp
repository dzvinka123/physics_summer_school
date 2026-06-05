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
    _risingTime = 0;   // ← скидаємо старі значення!
    _fallingTime = 0;  // ← скидаємо старі значення!
    
    digitalWrite(_trigPin, LOW);
    delayMicroseconds(4);  // трохи більше для стабільності
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
    if (_fallingTime <= _risingTime) return 0;  // ← захист від некоректних даних
    
    uint32_t duration = _fallingTime - _risingTime;
    uint16_t distance = (uint16_t)((duration * 343UL) / 20000UL);
    
    if (distance < 20)   distance = 20;
    if (distance > 4000) distance = 4000;
    _ready = false;
    return distance;
}