#ifndef __FRAMEWORK_HPP
#define __FRAMEWORK_HPP

#include <Arduino.h>
#include "EMMC42V53.hpp"

#define Motor_1_Button  12
#define Motor_2_Button  14
#define Start_Button    27


float Weight_Location_X[6][3] = {2000, 0, 0,
                               2375, 2375, 2375,
                               2562.5, 2562.5, 0,
                               2937.5, 2937.5, 0,
                               3125, 3125, 3125,
                               3500, 0, 0};

float Set_Location_X[3] = {245,1750,3755};

void Task_Get_Location(void *prfrk);

class Framework
{
public:

    /**
     * @brief 整体框架初始化
     * 
     * @param motor1 : 电机1
     * @param motor2 : 电机2
     * @param slmotor : 电机通信串口
     * @param rx : 串口 rx_pin
     * @param tx : 串口 tx_pin
     * 
     * @return None
     */
    void Framework_Motor_Init(EMMC423V53 *motor1, EMMC423V53 *motor2, HardwareSerial *slmotor)
    {
        slmotor->begin(115200);
        pinMode(Motor_1_Button,INPUT_PULLUP);
        pinMode(Motor_2_Button,INPUT_PULLUP);
        pinMode(Start_Button,INPUT_PULLUP);
        this->Motor_2 = motor2;
        this->Motor_2->Steeping_Init(slmotor, 2);
        this->Motor_1 = motor1;
        this->Motor_1->Steeping_Init(slmotor, 1);
        this->Frame_Back_To_Location();
        this->Motor_1->Clear_All();
        this->Motor_2->Clear_All();
        Current_Location = 200;
        this->Framework_Motor_Get_Location();
        this->Frame_Set_Location(1550);//去往起始点
    }

    /**
     * @brief 整体框架初始化
     * 
     * @param motor1 : 电机1
     * @param motor2 : 电机2
     * @param slmotor : 电机通信串口
     * @param rx : 串口 rx_pin
     * @param tx : 串口 tx_pin
     * 
     * @return None
     */
    void Framework_Motor_Init(EMMC423V53 *motor1, EMMC423V53 *motor2, HardwareSerial *slmotor, int rx, int tx)
    {
        this->Serial = slmotor;
        slmotor->begin(115200,SERIAL_8N1,rx,tx);
        pinMode(Motor_1_Button,INPUT_PULLUP);
        pinMode(Motor_2_Button,INPUT_PULLUP);
        pinMode(Start_Button,INPUT_PULLUP);
        this->Motor_2 = motor2;
        this->Motor_2->Steeping_Init(slmotor, 2);
        this->Motor_1 = motor1;
        this->Motor_1->Steeping_Init(slmotor, 1);
        this->Frame_Back_To_Location();
        this->Motor_1->Clear_All();
        this->Motor_2->Clear_All();
        Current_Location = 200;
        this->Framework_Motor_Get_Location();
        this->Frame_Set_Location(1550);
    }

    /**
     * @brief 控制框架运动 ： 采用速度模式 主要用于启动复位和寻找砝码时使用
     * 
     * @param rpm : 电机每分钟圈数 
     * 
     * @return None
     */
    void Framework_Move_Forward(uint16_t rpm)
    {
        this->Motor_1->Speed_Mode_Cmd(1, rpm, 0);
        this->Motor_2->Speed_Mode_Cmd(0, rpm, 0);
        this->Motor_1->Dual_Machine_Enable();
    }

    /**
     * @brief 控制框架运动 ： 采用速度模式 主要用于启动复位和寻找砝码时使用
     * 
     * @param rpm : 电机每分钟圈数 
     * 
     * @return None
     */
    void Framework_Move_Backward(uint16_t rpm)
    {
        this->Motor_1->Speed_Mode_Cmd(0, rpm, 0);
        this->Motor_2->Speed_Mode_Cmd(1, rpm, 0);
        this->Motor_1->Dual_Machine_Enable();
    }

    /**
     * @brief 控制框架保持不动
     * 
     * @param 无
     * 
     * @return None
     */
    void Framework_Move_Stop()
    {
        this->Motor_1->Stop_Instance();
        this->Motor_2->Stop_Instance();
    }

    /**
     * @brief 用于开机复位：回到赛题指定位置
     * 
     * @param 无
     * 
     * @return None
     */
    void Frame_Back_To_Location()
    {
        this->Framework_Move_Backward(30);
        while(digitalRead(Motor_2_Button) != LOW)
        {
            delay(1);
        };
        this->Motor_1->Stop_Instance();
        this->Motor_2->Stop_Instance();
    }

    /**
     * @brief 控制框架到达指定位置: 位置模式， 主要用于放置砝码， 如果能够定位砝码位置，可用于定位夹取砝码位置
     * 
     * @param target_location : 目标位置
     * @param speed : 电机转速， 默认 600 rpm
     * 
     * @return None
     */
    void Frame_Set_Location(int target_location, int speed = 30)
    {
        float move_location = fabs(target_location - this->Current_Location);

        if(target_location >= this->Current_Location)
        {
            this->Motor_1->Location_Mode_Cmd(1, speed, move_location);
            this->Motor_2->Location_Mode_Cmd(0, speed, move_location);
        }
        else
        {
            this->Motor_1->Location_Mode_Cmd(0, speed, move_location);
            this->Motor_2->Location_Mode_Cmd(1, speed, move_location);
        }
        this->Motor_2->Dual_Machine_Enable();
        while (abs(target_location - this->Current_Location) > 3)
            delay(1);
    }


    /**
     * @brief 框架循环检测当前位置
     * 
     * @param 无
     * 
     * @return None
     */
    void Framework_Motor_Get_Location(void)
    {
        xTaskCreatePinnedToCore(Task_Get_Location, "Task_Get_Location_And_Speed", 4096, this, 5, NULL, 0);
    }

    EMMC423V53 *Motor_1;
    EMMC423V53 *Motor_2;
    float Current_Location;
    HardwareSerial *Serial;

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
    Framework *framework = (Framework *)prfrk;
    while (1)
    {
        framework->Motor_1->Read_Instance_Location();
        framework->Motor_2->Read_Instance_Location();
        if (framework->Motor_2->Buffer[0] == framework->Motor_2->ID && framework->Motor_2->Buffer[1] == 0x36)
        {
            framework->Motor_2->Now_Location = (framework->Motor_2->Buffer[2] ? -1 : 1) * (framework->Motor_2->Buffer[3] * pow(16, 6) +
                                                                                   framework->Motor_2->Buffer[4] * pow(16, 4) + 
                                                                                   framework->Motor_2->Buffer[5] * pow(16, 2) + 
                                                                                   framework->Motor_2->Buffer[6]);
        }
        if (framework->Motor_1->Buffer[0] == framework->Motor_1->ID && framework->Motor_1->Buffer[1] == 0x36)
        {
            framework->Motor_1->Now_Location = (framework->Motor_1->Buffer[2] ? -1 : 1) * (framework->Motor_1->Buffer[3] * pow(16, 6) +
                                                                                   framework->Motor_1->Buffer[4] * pow(16, 4) + 
                                                                                   framework->Motor_1->Buffer[5] * pow(16, 2) + 
                                                                                   framework->Motor_1->Buffer[6]);
        }
        framework->Current_Location = 200.0 + framework->Motor_2->Now_Location / 65536 * Trip_mm;
        // Serial.printf("Now Laction is : %.2f m\n", chasis->Current_Location);
        // Serial.printf("Now Speed is : %.2f m\n", chasis->Current_Speed);
        delay(5);
    }
}

#endif