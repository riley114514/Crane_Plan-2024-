#include <Arduino.h>

#include "steeping42.hpp"
#include "framework.hpp"

Steeping42 steeping42_motor_1;
Steeping42 steeping42_motor_2;
Framework framework;

void setup() {
  Serial.begin(115200);
  framework.Framework_Motor_Init(&steeping42_motor_1, &steeping42_motor_2, &Serial1, 8, 18);
}

void loop() {

}

