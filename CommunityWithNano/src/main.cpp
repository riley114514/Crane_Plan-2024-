// #include <Arduino.h>
// #include "community.hpp"
// #include "esp_now_community.hpp"

// Esp_Now_Community esp_now_community;
// Community NanoCommunity;

// void setup() 
// {
//     esp_now_community.Esp_Now_Send_Init();
//     NanoCommunity.Communcation_Init(&Serial2,16,17);
// }

// void loop() 
// {
// }
#include <Arduino.h>
#include "community.hpp"

Community NanoCommunity;

void setup() 
{
NanoCommunity.Communcation_Init(&Serial2,16,17);
NanoCommunity.NANO_Send();
NanoCommunity.ScaleIdentification();
}

void loop() 
{

}

