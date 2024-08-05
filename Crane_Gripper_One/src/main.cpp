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
Gripper gripper_one;
Status state_machine;
Servo servo_gripper;

void setup() {
  Serial.begin(115200);
  esp_now_community.Esp_Now_Init();//STATE初始化必须在motor_init后面
  gripper_one.Gripper_Motor_Init(&motor_1, &motor_2, &Serial1, 25, 26);
  gripper_one.Gripper_Servo_Init(&servo_gripper, 0, 33);
  state_machine.State_Init();


}

void loop() {

}

