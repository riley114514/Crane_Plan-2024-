#ifndef __FRAMEWORK_HPP
#define __FRAMEWORK_HPP

#include <Arduino.h>
#include "steeping42.hpp"

#define Length_mm 3960 // 框架可移动长度距离: 3960 mm
#define Width_mm 1960 // 框架可移动宽度距离: 1960 mm

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
    void Framework_Motor_Init(Steeping42 *motor1, Steeping42 *motor2, HardwareSerial *slmotor, int rx, int tx)
    {
        slmotor->begin(115200,SERIAL_8N1,rx,tx);
        this->Steeping42_Motor_2 = motor2;
        this->Steeping42_Motor_2->Steeping_Init(slmotor, 2);
        this->Steeping42_Motor_1 = motor1;
        this->Steeping42_Motor_1->Steeping_Init(slmotor, 1);
        this->Frame_Back_To_Location();
        this->Steeping42_Motor_1->Clear_All();
        this->Steeping42_Motor_2->Clear_All();
        Target_Location = 0;
        Current_Location = 0;
    }

    /**
     * @brief 控制框架运动 ： 采用速度模式 主要用于启动复位和寻找砝码时使用
     * 
     * @param rpm : 电机每分钟圈数 
     * 
     * @return None
     */
    void Framework_Move_Speed(uint16_t rpm)
    {
        if(rpm >= 0)
        {
            this->Steeping42_Motor_1->Speed_Mode_Cmd(0., rpm, 0);
            this->Steeping42_Motor_2->Speed_Mode_Cmd(1, rpm, 0);
        }
        else
        {
            this->Steeping42_Motor_1->Speed_Mode_Cmd(1, -rpm, 0);
            this->Steeping42_Motor_2->Speed_Mode_Cmd(0, -rpm, 0);
        }
        this->Steeping42_Motor_1->Dual_Machine_Enable();
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
        this->Framework_Move_Speed(-60);
        while(this->Steeping42_Motor_1->Read_Instance_Current() < 600)
        {
            delay(5);
        };
        this->Steeping42_Motor_2->Motor_Disable_Cmd();
        this->Steeping42_Motor_1->Motor_Disable_Cmd();
        delay(1000);
        this->Steeping42_Motor_2->Motor_Enable_Cmd();
        this->Steeping42_Motor_1->Motor_Enable_Cmd();
    }

    /**
     * @brief 控制框架到达指定位置: 位置模式， 主要用于放置砝码， 如果能够定位砝码位置，可用于定位夹取砝码位置
     * 
     * @param target_location : 目标位置
     * @param speed : 电机转速， 默认 600 rpm
     * 
     * @return None
     */
    void Frame_Set_Location(int target_location, int speed = 600)
    {
        float move_location = fabs(target_location - this->Current_Location);

        if(target_location >= this->Current_Location)
        {
            this->Steeping42_Motor_1->Location_Mode_Cmd(0, speed, move_location);
            this->Steeping42_Motor_2->Location_Mode_Cmd(1, speed, move_location);
        }
        this->Steeping42_Motor_1->Dual_Machine_Enable();
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
        xTaskCreatePinnedToCore(Task_Get_Location, "Task_Get_Location_And_Speed", 4096, this, 5, NULL, 1);
    }

    Steeping42 *Steeping42_Motor_1;
    Steeping42 *Steeping42_Motor_2;
    float Target_Location;
    float Current_Location;
        
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
        framework->Steeping42_Motor_1->Read_Instance_Location();
        framework->Steeping42_Motor_2->Read_Instance_Location();
        if (framework->Steeping42_Motor_2->Buffer[0] == framework->Steeping42_Motor_2->ID && framework->Steeping42_Motor_2->Buffer[1] == 0x36)
        {
            framework->Steeping42_Motor_2->Now_Location = (framework->Steeping42_Motor_2->Buffer[2] ? -1 : 1) * (framework->Steeping42_Motor_2->Buffer[3] * pow(16, 6) +
                                                                                   framework->Steeping42_Motor_2->Buffer[4] * pow(16, 4) + 
                                                                                   framework->Steeping42_Motor_2->Buffer[5] * pow(16, 2) + 
                                                                                   framework->Steeping42_Motor_2->Buffer[6]);
        }
        if (framework->Steeping42_Motor_1->Buffer[0] == framework->Steeping42_Motor_1->ID && framework->Steeping42_Motor_1->Buffer[1] == 0x36)
        {
            framework->Steeping42_Motor_1->Now_Location = (framework->Steeping42_Motor_1->Buffer[2] ? -1 : 1) * (framework->Steeping42_Motor_1->Buffer[3] * pow(16, 6) +
                                                                                   framework->Steeping42_Motor_1->Buffer[4] * pow(16, 4) + 
                                                                                   framework->Steeping42_Motor_1->Buffer[5] * pow(16, 2) + 
                                                                                   framework->Steeping42_Motor_1->Buffer[6]);
        }
        framework->Current_Location = (framework->Steeping42_Motor_2->Now_Location - framework->Steeping42_Motor_1->Now_Location) / 65536 * Trip_mm / 2;
        // Serial.printf("Now Laction is : %.2f m\n", chasis->Current_Location);
        // Serial.printf("Now Speed is : %.2f m\n", chasis->Current_Speed);
        delay(5);
    }
}

#endif