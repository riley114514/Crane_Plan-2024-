#include <Arduino.h>

#include "steeping42.hpp"
#include "gripper.hpp"

Steeping42 steeping42_motor_1;
Steeping42 steeping42_motor_2;
Gripper gripper;

void setup() {
  Serial.begin(115200);
  gripper.Gripper_Motor_Init(&steeping42_motor_1, &steeping42_motor_2, &Serial1, 25, 26);
}

void loop() {

}

