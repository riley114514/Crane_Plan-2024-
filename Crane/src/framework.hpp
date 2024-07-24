#ifndef __FRAMEWORK_HPP
#define __FRAMEWORK_HPP

#include <Arduino.h>
#include "steeping42.hpp"

// 状态机的状态表示声明
#define move_stop 0
#define gripper_one_pick_work 4
#define gripper_two_pick_work 5
#define gripper_together_pick_work 6
#define gripper_one_set_work 7
#define gripper_together_set_work 9
#define move_to_set_location 13
#define move_to_pick_location 14



void Task_Get_Location(void *prfrk);
float Weight_Location_X[18] = {2000, 0, 0,
                               2375, 2375, 2375,
                               2562.5, 2562.5, 0,
                               2937.5, 2937.5, 0,
                               3125, 3125, 3125,
                               500, 0, 0};
float Weight_Location_Y[18] = {1000, 0, 0,
                               1650, 1000, 350,
                               1325, 675, 0,
                               1325, 675, 0,
                               1650, 1000, 350,
                               1000, 0, 0};

float Set_Location[2][5] = {245,245,1750,3755,3755,
                            245,1755,1000,245,1755};
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
    void Framework_Motor_Init(Steeping42 *motor1, Steeping42 *motor2, HardwareSerial *slmotor)
    {
        slmotor->begin(115200);
        pinMode(27,INPUT_PULLUP);
        pinMode(14,INPUT_PULLUP);
        this->Steeping42_Motor_2 = motor2;
        this->Steeping42_Motor_2->Steeping_Init(slmotor, 2);
        this->Steeping42_Motor_1 = motor1;
        this->Steeping42_Motor_1->Steeping_Init(slmotor, 1);
        this->Frame_Back_To_Location();
        this->Steeping42_Motor_1->Clear_All();
        this->Steeping42_Motor_2->Clear_All();
        Current_Location = 200;
        Framework_Status = move_stop;
        Pick_Num = 2;
        Set_Num = 2;
        Pick_Finish_Num = 0;
        Set_Finish_Num = 0;
        Weight_Num = 5;
        pointer_weight = 6;
        this->Framework_Motor_Get_Location();
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
    void Framework_Motor_Init(Steeping42 *motor1, Steeping42 *motor2, HardwareSerial *slmotor, int rx, int tx)
    {
        slmotor->begin(115200,SERIAL_8N1,rx,tx);
        pinMode(27,INPUT_PULLUP);
        pinMode(14,INPUT_PULLUP);
        this->Steeping42_Motor_2 = motor2;
        this->Steeping42_Motor_2->Steeping_Init(slmotor, 2);
        this->Steeping42_Motor_1 = motor1;
        this->Steeping42_Motor_1->Steeping_Init(slmotor, 1);
        this->Frame_Back_To_Location();
        this->Steeping42_Motor_1->Clear_All();
        this->Steeping42_Motor_2->Clear_All();
        Current_Location = 200;
        Framework_Status = move_stop;
        Pick_Num = 2;
        Set_Num = 2;
        Pick_Finish_Num = 0;
        Set_Finish_Num = 0;
        Weight_Num = 5;
        pointer_weight = 6;
        this->Framework_Motor_Get_Location();
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
        this->Steeping42_Motor_1->Speed_Mode_Cmd(1, rpm, 0);
        this->Steeping42_Motor_2->Speed_Mode_Cmd(0, rpm, 0);
        this->Steeping42_Motor_1->Dual_Machine_Enable();
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
        this->Steeping42_Motor_1->Speed_Mode_Cmd(0, rpm, 0);
        this->Steeping42_Motor_2->Speed_Mode_Cmd(1, rpm, 0);
        this->Steeping42_Motor_1->Dual_Machine_Enable();
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
        this->Steeping42_Motor_1->Stop_Instance();
        this->Steeping42_Motor_2->Stop_Instance();
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
        while(digitalRead(14) != LOW)
        {
            delay(1);
        };
        this->Steeping42_Motor_2->Stop_Instance();
        this->Steeping42_Motor_1->Stop_Instance();

    }

    /**
     * @brief 控制框架到达指定位置: 位置模式， 主要用于放置砝码， 如果能够定位砝码位置，可用于定位夹取砝码位置
     * 
     * @param target_location : 目标位置
     * @param speed : 电机转速， 默认 600 rpm
     * 
     * @return None
     */
    void Frame_Set_Location(int target_location, int speed = 60)
    {
        float move_location = fabs(target_location - this->Current_Location);

        if(target_location >= this->Current_Location)
        {
            this->Steeping42_Motor_1->Location_Mode_Cmd(1, speed, move_location);
            this->Steeping42_Motor_2->Location_Mode_Cmd(0, speed, move_location);
        }
        else
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
        xTaskCreatePinnedToCore(Task_Get_Location, "Task_Get_Location_And_Speed", 4096, this, 5, NULL, 0);
    }

    Steeping42 *Steeping42_Motor_1;
    Steeping42 *Steeping42_Motor_2;
    float Current_Location;
    uint8_t Framework_Status;
    uint8_t Pick_Num;
    uint8_t Pick_Finish_Num;
    uint8_t Set_Num;
    uint8_t Set_Finish_Num;
    uint8_t Weight_Num;
    uint8_t Weight_Location[6];
    int pointer_weight;
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
        framework->Current_Location = 200.0 + framework->Steeping42_Motor_2->Now_Location / 65536 * Trip_mm;
        // Serial.printf("Now Laction is : %.2f m\n", chasis->Current_Location);
        // Serial.printf("Now Speed is : %.2f m\n", chasis->Current_Speed);
        delay(5);
    }
}

#endif