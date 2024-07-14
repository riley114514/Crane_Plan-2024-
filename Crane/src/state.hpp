#ifndef __STATE_HPP
#define __STATE_HPP

#include <Arduino.h>
#include "framework.hpp"
#include "esp_now_community.hpp"

extern Esp_Now_Community esp_now_community;
extern Framework framework;
// 状态机的状态表示声明
enum State_Status
{
    move_stop = 0,
    move_forward,
    move_backward,
    gripper_one_pick_work,
    gripper_two_pick_work,
    gripper_together_pick_work,
    gripper_one_set_work,
    gripper_two_set_work,
    gripper_together_set_work,
    gripper_back_to_location,
    framework_back_to_location,
    start_scan,
    move_to_set_location
};

class State
{

public:
    /**
     * @brief 状态机初始化函数
     *
     * @param 无
     *
     * @return None
     */
    void State_Init()
    {
        this->Status = gripper_back_to_location;
        Weight_Num = 5;
        Pick_Num = 0;
        Set_Num = 0;
        Pick_Finish_Num = 0;
        Set_Finish_Num = 0;
    }

    /**
     * @brief 状态机状态查询函数
     *
     * @param 无
     *
     * @return None
     */
    void State_Status_Check(void)
    {
        xTaskCreatePinnedToCore(Task_Status_Check, "Task_Status_Check", 4096, this, 5, NULL, 1);
    }

    uint8_t Status;
    uint8_t Weight_Num;
    uint8_t Pick_Num;
    uint8_t Pick_Finish_Num;
    uint8_t Set_Num;
    uint8_t Set_Finish_Num;


private:
    /* data */
};

/**
 * @brief 状态机状态查询任务函数，根据当前状态执行不同功能
 *        注：需要通过nano来决定抓几个，然后传回状态机决定几个机械爪去夹取
 *        注：在每次夹取完成后，查询还剩几个砝码需要抓取，如果抓取完毕，则停止工作，否则继续跟nano通信开始扫描
 * 
 * @param prs, = prsm为将 State 类型 转变为 void 类型的指针，
 *       需要在函数里再将prst转变为 State 类型
 *
 * @return None
 */
void Task_Status_Check(void *prsm)
{
    State *state_machine = (State *)prsm;
    while (1)
    {
        switch (state_machine->Status)
        {

        case move_stop:
        {
            framework.Framework_Move_Stop();
            break;
        }

        case move_forward:
        {
            framework.Framework_Move_Speed(60);
            break;
        }

        case move_backward:
        {
            framework.Framework_Move_Speed(-60);
            break;
        }

        case gripper_one_pick_work:
        {
            esp_now_community.Gripper_One_Work();
            state_machine->Status = move_stop;
            state_machine->Pick_Num = 1;
            break;
        }

        case gripper_two_pick_work:
        {
            esp_now_community.Gripper_Two_Work();
            state_machine->Status = move_stop;
            state_machine->Pick_Num = 1;
            break;
        }

        case gripper_together_pick_work:
        {
            esp_now_community.Gripper_Work_Together();
            state_machine->Status = move_stop;
            state_machine->Pick_Num = 1;
            break;
        }

        case gripper_one_set_work:
        {
            esp_now_community.Gripper_One_Set();
            state_machine->Status = move_stop;
            break;
        }

        case gripper_two_set_work:
        {
            esp_now_community.Gripper_Two_Set();
            state_machine->Status = move_stop;
            break;
        }

        case gripper_together_set_work:
        {
            esp_now_community.Gripper_Set_Together();
            state_machine->Status = move_stop;
            break;
        }

        case gripper_back_to_location:
        {
            esp_now_community.Gripper_Back_To_Loaction();
            state_machine->Status = move_stop;
            break;
        }

        case framework_back_to_location:
        {
            framework.Frame_Back_To_Location();
            esp_now_community.Gripper_Down_To_Loaction();
            state_machine->Status = move_stop;
            break;
        }

        case start_scan:
        {
            // 这里插入与nano通信的开始扫描函数

            //
            esp_now_community.Gripper_Start_To_Pick();
            state_machine->Status = move_stop;
            break;
        }

        case move_to_set_location:
        {
            // framework.Frame_Set_Location();
            if(state_machine->Set_Num == 1)
                state_machine->Status = gripper_one_pick_work;
            else
                state_machine->Status = gripper_together_pick_work;
            break;
        }

        default:
            break;
        }
        delay(10);
    }
}

#endif