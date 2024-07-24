#include <Arduino.h>
#include "community.hpp"


Community NanoCommunity;

void setup() 
{
NanoCommunity.Communcation_Init(&Serial2,16,17);
}

void loop() 
{
}

