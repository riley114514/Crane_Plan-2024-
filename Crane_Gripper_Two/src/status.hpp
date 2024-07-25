#ifndef __STATUS_HPP
#define __STATUS_HPP

#include <Arduino.h>
#include "gripper.hpp"
#include "esp_now_community.hpp"

extern Gripper gripper_two;
extern Esp_Now_Community esp_now_community;
void Task_Status_Check(void *prsm);


// 状态机的状态表示声明
#define move_stop 0
#define start_to_pick 1
#define start_to_set 2

class Status
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
        this->Status = move_stop;
        this->State_Status_Check();
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
        xTaskCreatePinnedToCore(Task_Status_Check, "Task_Status_Check", 4096, this, 5, NULL, 0);
    }
    uint8_t Status;

private:
    /* data */
};

/**
 * @brief 状态机状态查询任务函数，根据当前状态执行不同功能
 *
 * @param prs, = prsm为将 State 类型 转变为 void 类型的指针，
 *       需要在函数里再将prst转变为 State 类型
 *
 * @return None
 */
void Task_Status_Check(void *prsm)
{
    Status *state_machine = (Status *)prsm;
    while (1)
    {   
        state_machine->Status = gripper_two.Gripper_Status;
        switch (state_machine->Status)
        {
        case move_stop:
        {
            gripper_two.Gripper_Move_Stop();
            break;
        }

        case start_to_pick:
        {
            gripper_two.Gripper_Start_To_Pick(gripper_two.Pick_Location);
            esp_now_community.Framework_Move_To_Set_Location();
            gripper_two.Gripper_Status = move_stop;
            break;
        }

        case start_to_set:
        {
            gripper_two.Gripper_Start_To_Set(gripper_two.Set_Location);
            gripper_two.Gripper_Status = move_stop;
            break;
        }

        case start_to_scan:
        {
            gripper_two.Gripper_Set_Y_Location(240);
            gripper_two.Gripper_Status = move_stop;
            break;
        }
        default:
            break;
        }
        delay(10);
    }
}

#endif