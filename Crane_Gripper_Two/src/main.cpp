#include <Arduino.h>

#include "steeping42.hpp"
#include "gripper.hpp"
#include "esp_now_community.hpp"
#include "status.hpp"
#include "servo.hpp"

Servo servo_gripper;
Esp_Now_Community esp_now_community;
Steeping42 steeping42_motor_1;
Steeping42 steeping42_motor_2;
Gripper gripper_two;
Status state_machine;

void setup() {
  Serial.begin(115200);
  // delay(2000);
  esp_now_community.Esp_Now_Init();
  gripper_two.Gripper_Motor_Init(&steeping42_motor_1, &steeping42_motor_2, &Serial1, 25, 26);
  gripper_two.Gripper_Servo_Init(&servo_gripper, 0, 33);
  state_machine.State_Init();
  // servo_gripper.Servo_Init(0,33);
  // servo_gripper.Set_Servo_Angle(180);
  // gripper_two.Gripper_Set_Y_Location(1325);
  // gripper_two.Gripper_Set_Z_Location(5);
  // servo_gripper.Set_Servo_Angle(80);
  // delay(300);
  // gripper_two.Gripper_Set_Z_Location(240);
  // gripper_two.Gripper_Set_Y_Location(1775);
  // gripper_two.Gripper_Set_Z_Location(10,30);
  // servo_gripper.Set_Servo_Angle(180);
  // gripper_two.Gripper_Set_Z_Location(240);
}

void loop() {

}

