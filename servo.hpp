#ifndef __SERVO_H
#define __SERVO_H
#endif

#include <Arduino.h>
/*
舵机驱动：
    定义引脚和对应的ledc通道、频率和分辨率
    频率和分辨率不建议修改，只修改舵机引脚和通道
    包含初始化函数、角度设定函数、PWM值计算函数
    调用只需设定初始化和设定角度
*/

#define L1_PIN      12
#define L2_PIN      13
#define L3_PIN      14
#define R1_PIN      25
#define R2_PIN      26
#define R3_PIN      27
#define Freq        50
#define Resolution  10
#define L1_CHANNEL  0
#define L2_CHANNEL  1
#define L3_CHANNEL  2
#define R1_CHANNEL  3
#define R2_CHANNEL  4
#define R3_CHANNEL  5

/*
    本舵机是270度。所以最大角度为270。最小角度为0度
*/
const float Max_Width = 2.5/20 * pow(2,Resolution);
const float Min_Width = 0.5/20 * pow(2,Resolution);  
const int Max_Angle = 270;
const int Min_Angle = 0;

class Servo{
    public:
        /*
            舵机初始化定义，
            只需天剑对应的ledcSetup()和LedcAttachPin().
        */
        void Servo_Init(void)
        {
            ledcSetup(L1_CHANNEL,Freq,Resolution);
            ledcAttachPin(L1_PIN,L1_CHANNEL);

            ledcSetup(L2_CHANNEL,Freq,Resolution);
            ledcAttachPin(L2_PIN,L2_CHANNEL);

            ledcSetup(L3_CHANNEL,Freq,Resolution);
            ledcAttachPin(L3_PIN,L3_CHANNEL);

            ledcSetup(R1_CHANNEL,Freq,Resolution);
            ledcAttachPin(R1_PIN,R1_CHANNEL);

            ledcSetup(R2_PIN,Freq,Resolution);
            ledcAttachPin(R2_PIN,R2_CHANNEL);

            ledcSetup(R3_CHANNEL,Freq,Resolution);
            ledcAttachPin(R3_PIN,R3_CHANNEL);           
        }
        /*
            舵机角度设定函数，直接传入对应舵机连接的通道和目标角度即可
        */
        void Set_Servo_Angle(int Channel,int angle)
        {
            ledcWrite(Channel,Calculate_Servo_PWM(angle));
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

};