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
    void Ultrasonic_Init(int trig, int echo)
    {
        tri_pin = trig;
        echo_pin = echo;
        pinMode(tri_pin,OUTPUT);
        pinMode(echo_pin,INPUT);
    }
    //超声波测距函数，返回给本类的distance中。
    void Ultrasonic_Get()
    {
        digitalWrite(tri_pin,HIGH);
        delayMicroseconds(10);
        digitalWrite(tri_pin,LOW);
        this->distance = pulseIn(echo_pin,HIGH) /2 *0.00034;
    }
// private:
    float distance;
    int tri_pin;
    int echo_pin;
    
};



