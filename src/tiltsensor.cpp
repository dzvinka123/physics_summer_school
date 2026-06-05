#include "tiltsensor.h"

#define INTERVAL_MS 200

static MPU6050 _mpu(Wire);
static uint8_t _axis = TILT_AXIS_X;
static TiltData _data = {0.0f, 0.0f, false};
static unsigned long _lastUpdate = 0;

bool tiltBegin(uint8_t sda, uint8_t scl, uint8_t axis)
{
    _axis = axis;
    Wire.begin(sda, scl);

    if (_mpu.begin() != 0)
    {
        Serial.println("[TiltSensor] Error: sensor not found!");
        return false;
    }

    Serial.println("[TiltSensor] Calibrating... keep the platform horizontal!");
    delay(1000);
    _mpu.calcOffsets();
    Serial.println("[TiltSensor] Done!");

    return true;
}

void tiltUpdate()
{
    _mpu.update();

    if ((millis() - _lastUpdate) < INTERVAL_MS)
        return;
    _lastUpdate = millis();

    float angle = (_axis == TILT_AXIS_X) ? _mpu.getAngleX() : _mpu.getAngleY();

    _data.angle = angle;
    _data.mu = tan(fabsf(angle) * DEG_TO_RAD);
    _data.valid = true;
}

TiltData tiltGetData()
{
    return _data;
}