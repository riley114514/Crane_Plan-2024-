#include <Arduino.h>

#include "EMMC42V53.hpp"
#include "gripper.hpp"
#include "esp_now_community.hpp"
#include "status.hpp"
#include "servo.hpp"

ESP_Now_e eps_now_e;
Esp_Now_Community esp_now_community;
EMMC423V53 motor_1;
EMMC423V53 motor_2;
Gripper gripper_two;
Status state_machine;
Servo servo_gripper;

void setup() {
  Serial.begin(115200);
  // delay(2000);
  // Serial.begin(115200,SERIAL_8N1,34,35);
  esp_now_community.Esp_Now_Init();
  gripper_two.Gripper_Motor_Init(&motor_1, &motor_2, &Serial1, 25, 26);
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

