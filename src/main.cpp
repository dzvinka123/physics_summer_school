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

DistanceSensor dist_sensor(5, 18);

void setup()
{
    dist_sensor.begin();

    Serial.begin(115200);

    delay(2000);
    Serial.println("=== ESP32-S3 + GY-521 ===");

    if (!tiltBegin(SDA_PIN, SCL_PIN, TILT_AXIS_Y))
    {
        Serial.println("Checking connection!");
        while (true)
            delay(100);
    }
}

void loop()
{
    dist_sensor.send_signal();

    while (!dist_sensor.is_ready())
        delay(10);

    uint16_t distance = dist_sensor.get_distance();
    Serial.printf("Distance: %d", distance);

    /// tilt sensor
    tiltUpdate();

    TiltData data = tiltGetData();
    if (!data.valid)
        return;

    Serial.print("Angle: ");
    Serial.print(data.angle, 2);
    Serial.print(" °\t\tmu = ");
    Serial.println(data.mu, 4);
}