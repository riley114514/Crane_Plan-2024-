#include <Arduino.h>

#include "steeping42.hpp"
#include "gripper.hpp"
#include "esp_now_community.hpp"
#include "status.hpp"

Esp_Now_Community esp_now_community;
Steeping42 steeping42_motor_1;
Steeping42 steeping42_motor_2;
Gripper gripper_one;
Status state_machine;

void setup() {
  Serial.begin(115200);
  state_machine.State_Init();
  esp_now_community.Esp_Now_Send_Init();
  gripper_one.Gripper_Motor_Init(&steeping42_motor_1, &steeping42_motor_2, &Serial1, 25, 26);
}

void loop() {

}

