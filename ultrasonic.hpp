#ifndef __ULTRASONIC_H
#define __ULTRASONIC_H
#endif

#include <Arduino.h>
/*
    超声波驱动，需要用户自己定义每个超声波的Tri引脚和Echo引脚
    需要先创建类，然后进行初始化，最后调用Get函数获得距离。
*/
#define Trigger_Pin     15
#define Echo_Pin        2

class Ultrasonic
{
public:
    //超声波初始化，需要输入超声波的tri引脚和echo引脚
    void Ultrasonic_Init(void)
    {
        pinMode(Ultrasonic_Trig,OUTPUT);
        pinMode(Ultrasonic_Echo,INPUT);
    }
    //超声波测距函数，返回给本类的distance中。
    void Ultrasonic_Get()
    {
        digitalWrite(Ultrasonic_Trig,HIGH);
        delayMicroseconds(10);
        digitalWrite(Ultrasonic_Trig,LOW);
        this->distance = pulseIn(Ultrasonic_Echo,HIGH) /2 *0.00034;
    }
private:
    float distance;
};



