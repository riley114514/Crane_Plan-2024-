#include <Arduino.h>

#include "motor.hpp"

Uart uart;
Motor motor;
State state;
Steeping42 motor_2;
Steeping42 motor_1;
Ultrasonic ult1;
Ultrasonic ult2;
Oled oled;
int flag; 
void setup()
{
  Serial.begin(115200);
  ult1.Ultrasonic_Init(25,26);
  ult2.Ultrasonic_Init(13,14);
  oled.OLED_Init();
  uart.Uart_Init(&Serial2);
  // motor.Motor_Init(&motor_2, &motor_1, &Serial2);
  state.State_Init(&motor,&uart,&oled,&ult1,&ult2);
  state.State_Choice();
}

void loop()
{

}




