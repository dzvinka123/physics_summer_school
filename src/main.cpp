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

void setup() {
    dist_sensor.begin();

    Serial.begin(115200);
}

void loop() {
    dist_sensor.send_signal();

    while (!dist_sensor.is_ready())
        delay(10);

    uint16_t distance = dist_sensor.get_distance();
    Serial.printf("Distance: %d", distance);
}