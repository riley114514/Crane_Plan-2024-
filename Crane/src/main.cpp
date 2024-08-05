#include <Arduino.h>

#include "EMMC42V53.hpp"
#include "framework.hpp"
#include "esp_now_community.hpp"
#include "status.hpp"

ESP_Now_e eps_now_e;
Esp_Now_Community esp_now_community;
EMMC423V53 motor_1;
EMMC423V53 motor_2;
Framework framework;
Status state_machine;

void setup(void) {
  Serial.begin(115200);
  esp_now_community.Esp_Now_Init();
  framework.Framework_Motor_Init(&motor_1, &motor_2, &Serial1, 25, 26);
  state_machine.State_Init();
}

void loop() {
  // Serial.println(1);
  // uart_with_nano.Uart_Send_Check_Pack();
  // delay(10);
}