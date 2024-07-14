#include <Arduino.h>

#include "steeping42.hpp"
#include "framework.hpp"
#include "state.hpp"
#include "esp_now_community.hpp"

Esp_Now_Community esp_now_community;
Steeping42 steeping42_motor_1;
Steeping42 steeping42_motor_2;
Framework framework;
State state_machine;

void setup() {
  Serial.begin(115200);
  state_machine.State_Init();
  framework.Framework_Motor_Init(&steeping42_motor_1, &steeping42_motor_2, &Serial1, 8, 18);
}

void loop() {

}

