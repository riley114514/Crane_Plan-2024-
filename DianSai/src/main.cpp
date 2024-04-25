#include <Arduino.h>

#include "motor.hpp"

Uart uart;
Motor motor;
State state;
Steeping42 motor_y;
Steeping42 motor_z;
Ultrasonic ult1;
Ultrasonic ult2;
Oled oled;

void setup() {
  Serial.begin(115200);
  ult1.Ultrasonic_Init(11,12);
  ult1.Ultrasonic_Init(13,14);
  oled.OLED_Init();
  uart.Uart_Init(&Serial2);
  motor.Motor_Init(&motor_y, &motor_z, &Serial);
  state.State_Init(&motor,&uart,&oled,&ult1,&ult2);
  motor.Motor_Get_Location();
  state.State_Choice();
}

void loop() {
  

}

