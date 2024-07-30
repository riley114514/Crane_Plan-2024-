#include <Arduino.h>

#include "steeping42.hpp"
#include "framework.hpp"
#include "status.hpp"
#include "esp_now_community.hpp"

Esp_Now_Community esp_now_community;
Steeping42 steeping42_motor_1;
Steeping42 steeping42_motor_2;
Framework framework;
Status state_machine;

void setup() {
  // Serial.begin(115200,SERIAL_8N1,16,17);
  esp_now_community.Esp_Now_Send_Init();
  framework.Framework_Motor_Init(&steeping42_motor_1, &steeping42_motor_2, &Serial1, 25, 26);
  state_machine.State_Init();
// Steeping42.
  // steeping42_motor_1.Speed_Mode_Cmd(0,600,0,false);
  // framework.Frame_Set_Location(2937.5);
  // pinMode(Test_Pin,INPUT_PULLUP);

}

void loop() {
//   if(digitalRead(Test_Pin) ==LOW)
  // Serial.println(framework.Current_Location);
  // delay(100);
    // Serial2.println(1);
// delay(100);
}

