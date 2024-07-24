#include <Arduino.h>

#include "steeping42.hpp"
#include "gripper.hpp"
#include "esp_now_community.hpp"
#include "status.hpp"

#include "servo.hpp"

Esp_Now_Community esp_now_community;
Steeping42 steeping42_motor_1;
Steeping42 steeping42_motor_2;
Gripper gripper_one;
Status state_machine;
Servo servo_gripper;

void setup() {
  Serial.begin(115200);
  // delay(2000);
  esp_now_community.Esp_Now_Init();
  gripper_one.Gripper_Motor_Init(&steeping42_motor_1, &steeping42_motor_2, &Serial1, 25, 26);
  gripper_one.Gripper_Servo_Init(&servo_gripper, 0, 33);
  state_machine.State_Init();
  // servo_gripper.Servo_Init(0,33);
  // servo_gripper.Set_Servo_Angle(150);
  // gripper_one.Gripper_Set_Y_Location(1000);
  // gripper_one.Gripper_Set_Z_Location(5);
  // servo_gripper.Set_Servo_Angle(80);
  // gripper_one.Gripper_Set_Z_Location(245);
  // gripper_one.Gripper_Set_Y_Location(245);
  // // gripper_one.Gripper_Set_Z_Location(5);
  // servo_gripper.Set_Servo_Angle(150);
  // gripper_one.Gripper_Set_Z_Location(245);

  // steeping42_motor_2.Steeping_Init(&Serial1,1,25,26);
  // steeping42_motor_2.Speed_Mode_Cmd(1,60,0,false);
  // delay(1000);
  // steeping42_motor_2.Stop_Instance();

}

void loop() {

}

