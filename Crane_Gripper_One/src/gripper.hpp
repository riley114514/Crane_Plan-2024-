#ifndef __GRIPPER_HPP
#define __GRIPPER_HPP

#include <Arduino.h>
#include "EMMC42V53.hpp"
#include "servo.hpp"

#define Motor_2_Button  12
#define Motor_1_Button  13


float Weight_Location_Y[6][3] = {1000, 0, 0,
                               1650, 1000, 350,
                               1325, 675, 0,
                               1325, 675, 0,
                               1650, 1000, 350,
                               1000, 0, 0};

float Set_Location_Y[5] = {245,1755,1000,245,1755};
                            
void Task_Get_Location(void *prfrk);

class Gripper
{
public:
    /**
     * @brief 整体机械爪初始化
     * 
     * @param motor1 : y方向上的电机
     * @param motor2 : z方向上的电机
     * @param slmotor : 电机通信串口
     * @param rx : 串口 rx_pin
     * @param tx : 串口 tx_pin
     * 
     * @return None
     */
    void Gripper_Motor_Init(EMMC423V53 *motor1, EMMC423V53 *motor2, HardwareSerial *slmotor, int rx, int tx)
    {
        slmotor->begin(115200,SERIAL_8N1,rx,tx);
        pinMode(Motor_2_Button,INPUT_PULLUP);
        pinMode(Motor_1_Button,INPUT_PULLUP);

        this->Motor_2 = motor2;
        this->Motor_2->Steeping_Init(slmotor, 2);
        this->Motor_1 = motor1;
        this->Motor_1->Steeping_Init(slmotor, 1);

        this->Gripper_Back_To_Location();
        this->Motor_1->Clear_All();
        this->Motor_2->Clear_All();
        Current_Y_Location = 110.0;
        Current_Z_Location = 250.0;
        this->Gripper_Motor_Get_Location();
        this->Gripper_Set_Z_Location(10,30);//机械爪初始化，到高度为5mm处
    }

    void Gripper_Servo_Init(Servo *sv, int servo_channel, int servo_pin)
    {
        this->servo = sv;
        this->servo->Servo_Init(servo_channel,servo_pin);
        this->servo->Set_Servo_Angle(130);
    }
    /**
     * @brief 控制机械爪Y方向上的电机运动 ： 采用速度模式 主要用于启动复位和寻找砝码时使用
     * 
     * @param rpm : 电机每分钟圈数 
     * 
     * @return None
     */
    void Gripper_Move_Left(uint16_t rpm)
    {

        this->Motor_1->Speed_Mode_Cmd(0, rpm, 0);

        this->Motor_1->Dual_Machine_Enable();
    }

    /**
     * @brief 控制机械爪Y方向上的电机运动 ： 采用速度模式 主要用于启动复位和寻找砝码时使用
     * 
     * @param rpm : 电机每分钟圈数 
     * 
     * @return None
     */
    void Gripper_Move_Right(uint16_t rpm)
    {

        this->Motor_1->Speed_Mode_Cmd(1, rpm, 0);

        this->Motor_1->Dual_Machine_Enable();
    }

    /**
     * @brief 控制机械爪Z方向上的电机运动 ： 采用速度模式 主要用于启动复位和寻找砝码时使用
     * 
     * @param rpm : 电机每分钟圈数 
     * 
     * @return None
     */
    void Gripper_Move_Up(uint16_t rpm)
    {

        this->Motor_2->Speed_Mode_Cmd(0., rpm, 0);
        this->Motor_2->Dual_Machine_Enable();
    }

    /**
     * @brief 控制机械爪Z方向上的电机运动 ： 采用速度模式 主要用于启动复位和寻找砝码时使用
     * 
     * @param rpm : 电机每分钟圈数 
     * 
     * @return None
     */
    void Gripper_Move_Down(uint16_t rpm)
    {

        this->Motor_2->Speed_Mode_Cmd(1, rpm, 0);
        this->Motor_2->Dual_Machine_Enable();
    }

    /**
     * @brief 用于机械爪开机复位：回到赛题指定位置： 先复位Z方向，再复位Y方向
     * 
     * @param 无
     * 
     * @return None
     */
    void Gripper_Back_To_Location()
    {

        this->Gripper_Move_Up(30);
        while(digitalRead(Motor_2_Button) != LOW)
        {
            delay(1);
        };
        this->Motor_2->Stop_Instance();

        this->Gripper_Move_Right(30);
        while( digitalRead(Motor_1_Button) != LOW)
        {
            delay(1);
        };
        this->Motor_1->Stop_Instance();

        // this->Steeping42_Motor_2->Motor_Disable_Cmd();
        // this->Steeping42_Motor_1->Motor_Disable_Cmd();
        // delay(1000);
        // this->Steeping42_Motor_2->Motor_Enable_Cmd();
        // this->Steeping42_Motor_1->Motor_Enable_Cmd();
    }

    /**
     * @brief 控制框架到达指定位置: 位置模式， 主要用于放置砝码， 如果能够定位砝码位置，可用于定位夹取砝码位置
     * 
     * @param target_location : 目标位置
     * @param speed : 电机转速， 默认 600 rpm
     * 
     * @return None
     */
    void Gripper_Set_Y_Location(int target_y_location, int speed = 30)
    {
        float move_location = fabs(target_y_location - this->Current_Y_Location);

        if(target_y_location >= this->Current_Y_Location)
            this->Motor_1->Location_Mode_Cmd(0, speed, move_location, Trip_mm);
        else
            this->Motor_1->Location_Mode_Cmd(1, speed, move_location, Trip_mm);

        this->Motor_1->Dual_Machine_Enable();
        while (abs(target_y_location - this->Current_Y_Location) > 3)
            delay(1);
    }

    void Gripper_Set_Z_Location(int target_z_location, int speed = 30)
    {
        float move_location = fabs(target_z_location - this->Current_Z_Location);

        if(target_z_location >= this->Current_Z_Location)
            this->Motor_2->Location_Mode_Cmd(0, speed, move_location, 0, Height_mm);
        else
            this->Motor_2->Location_Mode_Cmd(1, speed, move_location, 0, Height_mm);

        this->Motor_2->Dual_Machine_Enable();
        while (abs(target_z_location - this->Current_Z_Location) > 3)
            delay(1);
    }

    /**
     * @brief 机械爪处于静止状态函数
     * 
     * @param 无
     * 
     * @return None
     */
    void Gripper_Move_Stop()
    {
        this->Motor_1->Stop_Instance();
        this->Motor_2->Stop_Instance();
    }

    /**
     * @brief 机械爪开始执行夹取任务，注意，再夹取完成后，直接移动到要放置的y坐标处。
     * 
     * @param 无
     * 
     * @return None
     */
    void Gripper_Start_To_Pick(int target_locatoin)
    {
        this->servo->Set_Servo_Angle(180);
        this->Gripper_Set_Y_Location(target_locatoin);
        this->Gripper_Set_Z_Location(5);
        this->servo->Set_Servo_Angle(80);
        delay(300);
        this->Gripper_Set_Z_Location(240);
        this->Gripper_Set_Y_Location(245);
    }

    /**
     * @brief 机械爪开始执行放置任务
     * 
     * @param 无
     * 
     * @return None
     */
    void Gripper_Start_To_Set(int target_location)
    {
        this->Gripper_Set_Y_Location(target_location);
        if(target_location == 1000)
            this->Gripper_Set_Z_Location(110,30);
        else
            this->Gripper_Set_Z_Location(210,30);
        this->servo->Set_Servo_Angle(180);
        delay(300);
        this->Gripper_Set_Z_Location(240);
    }



    /**
     * @brief 框架循环检测当前位置
     * 
     * @param 无
     * 
     * @return None
     */
    void Gripper_Motor_Get_Location(void)
    {
        xTaskCreatePinnedToCore(Task_Get_Location, "Task_Get_Location_And_Speed", 4096, this, 5, NULL, 0);
    }

    EMMC423V53 *Motor_1;
    EMMC423V53 *Motor_2;
    float Current_Y_Location;
    float Current_Z_Location;
    Servo *servo;

private:
    /* data */
};

/**
 * @brief 获取位置信息任务函数
 * 
 * @param prfrk : Framework 类型 转换为 void 类型的指针，将框架指针传入
 * 
 * @return None
 */
void Task_Get_Location(void *prfrk)
{
    Gripper *gripper = (Gripper *)prfrk;
    while (1)
    {
        gripper->Motor_1->Read_Instance_Location();
        gripper->Motor_2->Read_Instance_Location();
        if (gripper->Motor_2->Buffer[0] == gripper->Motor_2->ID && gripper->Motor_2->Buffer[1] == 0x36)
        {
            gripper->Motor_2->Now_Location = (gripper->Motor_2->Buffer[2] ? -1 : 1) * (gripper->Motor_2->Buffer[3] * pow(16, 6) +
                                                                                   gripper->Motor_2->Buffer[4] * pow(16, 4) + 
                                                                                   gripper->Motor_2->Buffer[5] * pow(16, 2) + 
                                                                                   gripper->Motor_2->Buffer[6]);
        }
        if (gripper->Motor_1->Buffer[0] == gripper->Motor_1->ID && gripper->Motor_1->Buffer[1] == 0x36)
        {
            gripper->Motor_1->Now_Location = (gripper->Motor_1->Buffer[2] ? -1 : 1) * (gripper->Motor_1->Buffer[3] * pow(16, 6) +
                                                                                   gripper->Motor_1->Buffer[4] * pow(16, 4) + 
                                                                                   gripper->Motor_1->Buffer[5] * pow(16, 2) + 
                                                                                   gripper->Motor_1->Buffer[6]);
        }
        // 暂定，需要看是左侧还是右侧电机去区别
        gripper->Current_Y_Location = 110.0 + gripper->Motor_1->Now_Location / 65536 * Trip_mm;
        gripper->Current_Z_Location = 250.0 + gripper->Motor_2->Now_Location / 65536 * Height_mm;


        // Serial.println(gripper->Current_Z_Location);
        // Serial.println(gripper->Current_Y_Location);

        delay(5);
    }
}

#endif