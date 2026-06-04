// DistanceSensor.h
#pragma once
#include <Arduino.h>

class DistanceSensor {
public:
    DistanceSensor(uint8_t trigPin, uint8_t echoPin);

    void begin();
    void send_signal();
    uint16_t get_distance();
    bool is_ready();

private:
    uint8_t _trigPin;
    uint8_t _echoPin;

    volatile uint32_t _risingTime;
    volatile uint32_t _fallingTime;
    volatile bool _ready;
    volatile bool _measuring;
    
    static void IRAM_ATTR _echoISR(void *arg);
};