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

// #define Test_Pin 12
void setup() {
  Serial.begin(115200);
  framework.Framework_Motor_Init(&steeping42_motor_1, &steeping42_motor_2, &Serial);
  state_machine.State_Init();
  // framework.Frame_Set_Location(1550);
  // pinMode(Test_Pin,INPUT_PULLUP);

}

void loop() {
//   if(digitalRead(Test_Pin) ==LOW)
//     Serial.println(1);
// delay(100);
}

