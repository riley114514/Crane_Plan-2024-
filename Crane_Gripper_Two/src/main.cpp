#include <Arduino.h>

#include "steeping42.hpp"
#include "gripper.hpp"
#include "state.hpp"

State state_machine;
Steeping42 steeping42_motor_1;
Steeping42 steeping42_motor_2;
Gripper gripper_two;

void setup() {
  Serial.begin(115200);
  state_machine.State_Init();
  gripper_two.Gripper_Motor_Init(&steeping42_motor_1, &steeping42_motor_2, &Serial1, 25, 26);
}

void loop() {

}

