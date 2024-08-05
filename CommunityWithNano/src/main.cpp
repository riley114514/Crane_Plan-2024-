#include <Arduino.h>
#include "esp_now_community.hpp"
#include "status.hpp"
#include "uart.hpp"

Esp_Now_Community esp_now_community;
Status State_Machine;
Uart uart_with_nano;
ESP_Now_e eps_now_e;

void setup() 
{
    Serial.begin(115200);
    uart_with_nano.Uart_Init(&Serial2);
    esp_now_community.Esp_Now_Init();
    State_Machine.State_Init();
 }

void loop() 
{
    // NanoCommunity.NANO_Send();
    // delay(1000);
}


//测试代码
// #include <Arduino.h>
// #include "community.hpp"

// Community NanoCommunity;

// void setup() 
// {
// Serial.begin(115200);
// NanoCommunity.Communcation_Init(&Serial2,16,17);
// NanoCommunity.NANO_Send();

// }

// void loop() 
// {
// }

