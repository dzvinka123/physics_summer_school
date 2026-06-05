#pragma once

#include <Wire.h>
#include <MPU6050_light.h>
#include <math.h>

#define TILT_AXIS_X 0
#define TILT_AXIS_Y 1

struct TiltData
{
    float angle;
    float mu;
    bool valid;
};

bool tiltBegin(uint8_t sda, uint8_t scl, uint8_t axis = TILT_AXIS_Y);

void tiltUpdate();

TiltData tiltGetData();