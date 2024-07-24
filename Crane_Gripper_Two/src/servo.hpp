#ifndef __SERVO_HPP
#define __SERVO_HPP

#include <Arduino.h>


#define Freq        50 //定时器频率：50 MHz
#define Resolution  10 //单位周期内的分辨率： 计数： 2^10


/*
    本舵机是270度。所以最大角度为270。最小角度为0度
*/
const float Max_Width = 2.5/20 * pow(2,Resolution);
const float Min_Width = 0.5/20 * pow(2,Resolution);  
const int Max_Angle = 270;
const int Min_Angle = 0;

/*
舵机驱动：
    定义引脚和对应的ledc通道、频率和分辨率
    频率和分辨率不建议修改，只修改舵机引脚和通道
    包含初始化函数、角度设定函数、PWM值计算函数
    调用只需设定初始化和设定角度
*/

class Servo{
    public:
        /*
            舵机初始化定义，
            只需天剑对应的ledcSetup()和LedcAttachPin().
        */
        void Servo_Init(int servo_channel, int servo_pin)
        {
            this->pin = servo_pin;
            this->channel = servo_channel;
            ledcSetup(this->channel,Freq,Resolution);
            ledcAttachPin(this->pin,this->channel);
       
        }
        /*
            舵机角度设定函数，直接传入对应舵机连接的通道和目标角度即可
        */
        void Set_Servo_Angle(int angle)
        {
            ledcWrite(this->channel,Calculate_Servo_PWM(angle));
        }

private:
        /*
            舵机目标角度对应的PWM值,不允许用户调用。
        */
        int Calculate_Servo_PWM(int angle)
        {
            int PWM;
            if(angle < Min_Angle)
            angle = Min_Angle;
            if(angle > Max_Angle)
            angle = Max_Angle;
            return (int)((Max_Width-Min_Width)/Max_Angle * angle + Min_Width);
        }

        int channel;
        int pin;

};

#endif
