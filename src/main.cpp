#include <Arduino.h>
#include <math.h>
#include "distance_sensor.h"
#include "servo_motor.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define BUTTON_PIN 4
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

DistanceSensor dist_sensor(5, 7);
servo_motor servo1(12);
servo_motor servo2(13);

int currentAngle = 0;
bool isScanning = false;
bool lastButtonState = LOW;
bool scanDone = false;
float lastTangent = NAN;
bool hasTangent = false;

void displayTangent(float tangent) {
    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Object detected!");

    display.setTextSize(1);
    display.setCursor(0, 20);
    display.print("Angle: ");
    display.print(currentAngle);
    display.println(" deg");

    display.setCursor(0, 35);
    display.print("Tangent:");

    display.setTextSize(2);
    display.setCursor(0, 48);
    display.print(tangent, 4);

    display.display();
}

void displayScanning() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Scanning...");
    display.setCursor(0, 20);
    display.print("Angle: ");
    display.print(currentAngle);
    display.println(" deg");
    display.display();
}

void displayIdle() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Press button");
    display.setCursor(0, 15);
    display.println("to start scan");

    if (hasTangent) {
        display.setCursor(0, 35);
        display.println("Last tangent:");
        display.setTextSize(2);
        display.setCursor(0, 48);
        display.print(lastTangent, 4);
    }

    display.display();
}

void setup()
{
    dist_sensor.begin();
    servo1.begin();
    servo2.begin();

    pinMode(5, OUTPUT);
    pinMode(7, INPUT);
    pinMode(BUTTON_PIN, INPUT);

    Serial.begin(115200);

    Wire.begin(3, 18); 
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("SSD1306 not found!");
        while (true);
    }
    display.clearDisplay();
    display.display();

    delay(2000);
    Serial.println("=== ESP32-S3 Servo Scanner with Button ===");
    Serial.println("Press button to Start / Stop");

    servo1.set_angle(0);
    servo2.set_angle(0);

    displayIdle();
}

void checkButton() {
    bool currentButtonState = digitalRead(BUTTON_PIN);

    if (currentButtonState == HIGH && lastButtonState == LOW) {
        delay(50);

        if (digitalRead(BUTTON_PIN) == HIGH) {
            if (!isScanning && scanDone) {
                scanDone = false;
                isScanning = true;
                currentAngle = 0;
                Serial.println(">>> SCANNING STARTED <<<");
                displayScanning();
            } else if (!isScanning && !scanDone) {
                isScanning = true;
                currentAngle = 0;
                Serial.println(">>> SCANNING STARTED <<<");
                displayScanning();
            } else if (isScanning) {
                isScanning = false;
                scanDone = false;
                Serial.println(">>> SCANNING STOPPED <<<");
                for (int i = currentAngle; i >= 0; i--) {
                    servo1.tick_right();
                    servo2.tick_left();
                    delay(200);
                }
                displayIdle();
            }
        }
    }
    lastButtonState = currentButtonState;
}

void loop()
{
    checkButton();

    if (isScanning && !scanDone) {

        for (int i = 0; i >= -80; i--) {
            checkButton();
            if (!isScanning) return;

            servo1.tick_left();
            servo2.tick_right();
            currentAngle = map(abs(i), 0, 80, 0, 60);

            displayScanning();

            delay(500);

            long totalDuration = 0;
            int measurements = 5;
            for (int m = 0; m < measurements; m++) {
                digitalWrite(5, LOW);
                delayMicroseconds(2);
                digitalWrite(5, HIGH);
                delayMicroseconds(10);
                digitalWrite(5, LOW);

                long duration = pulseIn(7, HIGH, 30000);
                totalDuration += duration;
                delayMicroseconds(100);
            }

            long avgDuration = totalDuration / measurements;
            uint16_t distance = avgDuration / 58;

            Serial.print("Angle: ");
            Serial.print(currentAngle);
            Serial.print(" degrees, Distance: ");
            Serial.print(distance);
            Serial.print(" cm");

            if (distance <= 10 && distance > 0) {
                float radians = currentAngle * (M_PI / 180.0);
                float tangent = tan(radians);

                Serial.print(" | Tangent: ");
                Serial.println(tangent, 4);

                lastTangent = tangent;
                hasTangent = true;
                displayTangent(tangent);

                break;
            }
            Serial.println();

            delay(100);
        }

        delay(500);
        for (int i = currentAngle; i >= 0; i--) {
            checkButton();
            if (!isScanning) return;

            servo1.tick_right();
            servo2.tick_left();
            delay(100);
        }

        isScanning = false;
        scanDone = true;
        Serial.println(">>> SCAN COMPLETED. Press button to scan again... <<<");

        displayIdle();
    }
}