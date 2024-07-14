#ifndef __GRIPPER_HPP
#define __GRIPPER_HPP

#include <Arduino.h>
#include "steeping42.hpp"

#define Length_mm 3960 // 框架可移动长度距离: 3960 mm
#define Width_mm 1960 // 框架可移动宽度距离: 1960 mm
#define Height_mm 280 // 框架可移动高度距离: 280 mm

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
    void Gripper_Motor_Init(Steeping42 *motor1, Steeping42 *motor2, HardwareSerial *slmotor, int rx, int tx)
    {
        slmotor->begin(115200,SERIAL_8N1,rx,tx);
        this->Steeping42_Motor_2 = motor2;
        this->Steeping42_Motor_2->Steeping_Init(slmotor, 2);
        this->Steeping42_Motor_1 = motor1;
        this->Steeping42_Motor_1->Steeping_Init(slmotor, 1);
        this->Gripper_Back_To_Location();
        this->Steeping42_Motor_1->Clear_All();
        this->Steeping42_Motor_2->Clear_All();
        Target_Y_Location = 0;
        Current_Y_Location = 0;
        Target_Z_Location = 0;
        Current_Z_Location = 0;
    }

    /**
     * @brief 控制机械爪Y方向上的电机运动 ： 采用速度模式 主要用于启动复位和寻找砝码时使用
     * 
     * @param rpm : 电机每分钟圈数 
     * 
     * @return None
     */
    void Gripper_Move_Y_Speed(uint16_t rpm)
    {
        if(rpm >= 0)
        {
            this->Steeping42_Motor_1->Speed_Mode_Cmd(0., rpm, 0);
        }
        else
        {
            this->Steeping42_Motor_1->Speed_Mode_Cmd(1, -rpm, 0);
        }
        this->Steeping42_Motor_1->Dual_Machine_Enable();
    }

    /**
     * @brief 控制机械爪Z方向上的电机运动 ： 采用速度模式 主要用于启动复位和寻找砝码时使用
     * 
     * @param rpm : 电机每分钟圈数 
     * 
     * @return None
     */
    void Gripper_Move_Z_Speed(uint16_t rpm)
    {
        if(rpm >= 0)
        {
            this->Steeping42_Motor_2->Speed_Mode_Cmd(0., rpm, 0);
        }
        else
        {
            this->Steeping42_Motor_2->Speed_Mode_Cmd(1, -rpm, 0);
        }
        this->Steeping42_Motor_2->Dual_Machine_Enable();
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
        this->Gripper_Move_Y_Speed(-60);
        while(this->Steeping42_Motor_1->Read_Instance_Current() < 600)
        {
            delay(5);
        };
        this->Steeping42_Motor_1->Motor_Disable_Cmd();

        this->Gripper_Move_Z_Speed(-60);
        while(this->Steeping42_Motor_2->Read_Instance_Current() < 600)
        {
            delay(5);
        };
        this->Steeping42_Motor_2->Motor_Disable_Cmd();

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
    void Gripper_Set_Y_Location(int target_y_location, int speed = 600)
    {
        float move_location = fabs(target_y_location - this->Current_Y_Location);

        if(target_y_location >= this->Current_Y_Location)
            this->Steeping42_Motor_1->Location_Mode_Cmd(0, speed, move_location);
        else
            this->Steeping42_Motor_1->Location_Mode_Cmd(1, speed, move_location);

        this->Steeping42_Motor_1->Dual_Machine_Enable();
        while (abs(target_y_location - this->Current_Y_Location) > 3)
            delay(1);
    }

    void Gripper_Set_Z_Location(int target_z_location, int speed = 600)
    {
        float move_location = fabs(target_z_location - this->Current_Y_Location);

        if(target_z_location >= this->Current_Y_Location)
            this->Steeping42_Motor_2->Location_Mode_Cmd(0, speed, move_location);
        else
            this->Steeping42_Motor_2->Location_Mode_Cmd(1, speed, move_location);

        this->Steeping42_Motor_2->Dual_Machine_Enable();
        while (abs(target_z_location - this->Current_Y_Location) > 3)
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
        this->Steeping42_Motor_1->Stop_Instance();
        this->Steeping42_Motor_2->Stop_Instance();
    }

    /**
     * @brief 机械爪开始执行夹取任务，注意，再夹取完成后，直接移动到要放置的y坐标处。
     * 
     * @param 无
     * 
     * @return None
     */
    void Gripper_Start_To_Pick()
    {

    }

    /**
     * @brief 机械爪开始执行放置任务
     * 
     * @param 无
     * 
     * @return None
     */
    void Gripper_Start_To_Set()
    {

    }

    /**
     * @brief 机械爪开始执行扫描前位置定位函数，移动到一定高度，防止机械爪与地面摩擦
     * 
     * @param 无
     * 
     * @return None
     */
    void Gripper_Start_To_Scan()
    {

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
        xTaskCreatePinnedToCore(Task_Get_Location, "Task_Get_Location_And_Speed", 4096, this, 5, NULL, 1);
    }

    Steeping42 *Steeping42_Motor_1;
    Steeping42 *Steeping42_Motor_2;
    float Target_Y_Location;
    float Current_Y_Location;
    float Target_Z_Location;
    float Current_Z_Location;

    
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
        gripper->Steeping42_Motor_1->Read_Instance_Location();
        gripper->Steeping42_Motor_2->Read_Instance_Location();
        if (gripper->Steeping42_Motor_2->Buffer[0] == gripper->Steeping42_Motor_2->ID && gripper->Steeping42_Motor_2->Buffer[1] == 0x36)
        {
            gripper->Steeping42_Motor_2->Now_Location = (gripper->Steeping42_Motor_2->Buffer[2] ? -1 : 1) * (gripper->Steeping42_Motor_2->Buffer[3] * pow(16, 6) +
                                                                                   gripper->Steeping42_Motor_2->Buffer[4] * pow(16, 4) + 
                                                                                   gripper->Steeping42_Motor_2->Buffer[5] * pow(16, 2) + 
                                                                                   gripper->Steeping42_Motor_2->Buffer[6]);
        }
        if (gripper->Steeping42_Motor_1->Buffer[0] == gripper->Steeping42_Motor_1->ID && gripper->Steeping42_Motor_1->Buffer[1] == 0x36)
        {
            gripper->Steeping42_Motor_1->Now_Location = (gripper->Steeping42_Motor_1->Buffer[2] ? -1 : 1) * (gripper->Steeping42_Motor_1->Buffer[3] * pow(16, 6) +
                                                                                   gripper->Steeping42_Motor_1->Buffer[4] * pow(16, 4) + 
                                                                                   gripper->Steeping42_Motor_1->Buffer[5] * pow(16, 2) + 
                                                                                   gripper->Steeping42_Motor_1->Buffer[6]);
        }
        // 暂定，需要看是左侧还是右侧电机去区别
        // gripper->Current_Y_Location = gripper->Steeping42_Motor_1->Now_Location / 65536 * Trip_mm;
        // gripper->Current_Z_Location = gripper->Steeping42_Motor_2->Now_Location / 65536 * Trip_mm;

        // Serial.printf("Now Laction is : %.2f m\n", chasis->Current_Location);
        // Serial.printf("Now Speed is : %.2f m\n", chasis->Current_Speed);
        delay(5);
    }
}

#endif