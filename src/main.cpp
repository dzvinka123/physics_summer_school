// #include <Arduino.h>

// // put function declarations here:
// int myFunction(int, int);

// void setup() {
//   // put your setup code here, to run once:
//   int result = myFunction(2, 3);
// }

// void loop() {
//   // put your main code here, to run repeatedly:
// }

// // put function definitions here:
// int myFunction(int x, int y) {
//   return x + y;
// }

#include <Arduino.h>
#include "distance_sensor.h"
#include "tiltsensor.h"
#include "servo_motor.h"

DistanceSensor dist_sensor(5, 7);

#define SDA_PIN 47
#define SCL_PIN 20

servo_motor servo1(12);
servo_motor servo2(13);

uint32_t last_time  = 0;

void setup()
{
    dist_sensor.begin();

    servo1.begin();
    servo2.begin();

    Serial.begin(115200);

    delay(2000);
    Serial.println("=== ESP32-S3 + GY-521 ===");

    if (!tiltBegin(SDA_PIN, SCL_PIN, TILT_AXIS_Y))
    {
        Serial.println("Checking connection!");
        while (true)
            delay(100);
    }

    // servo1.set_angle(90);
    // servo2.set_angle(90);

    servo1.set_angle(0);
    servo2.set_angle(0);

    delay(20000);

    dist_sensor.send_signal();
}

void loop()
{

    if (millis() - last_time > 1000) {
        dist_sensor.send_signal();
        last_time = millis();
    }

    if (dist_sensor.is_ready()) {
        uint16_t distance = dist_sensor.get_distance();
        Serial.printf("Distance: %d \n", distance);
    }

    for (int i = -60; i <= 60; i++) {
        servo1.set_angle(i);
        servo2.set_angle(-i);
        delay(50);
    }

    for (int i = -60; i <= 60; i++) {
        servo1.set_angle(90 - i);
        servo2.set_angle(-(90 - i));
        delay(50);
    }
    
    // tiltUpdate();
    // TiltData data = tiltGetData();
    // if (!data.valid) return;

    // Serial.print("Angle: ");
    // Serial.print(data.angle, 2);
    // Serial.print(" °\t\tmu = ");
    // Serial.println(data.mu, 4);
}