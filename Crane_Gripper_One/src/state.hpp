#ifndef __STATE_HPP
#define __STATE_HPP

#include <Arduino.h>
#include "gripper.hpp"
#include "esp_now_community.hpp"

extern Esp_Now_Community esp_now_community;
extern Gripper gripper_one;

// 状态机的状态表示声明
enum State_Status
{
    move_stop = 0,
    start_to_pick,
    start_to_set,
    start_to_scan,
    back_to_init,
    set_to_init,
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
        this->Status = move_stop;
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
    State *state_machine = (State *)prsm;
    while (1)
    {
        switch (state_machine->Status)
        {
        case move_stop:
        {
            gripper_one.Gripper_Move_Stop();
            break;
        }

        case back_to_init:
        {
            gripper_one.Gripper_Move_Stop();
            break;
        }

        case set_to_init:
        {
            gripper_one.Gripper_Move_Stop();
            break;
        }

        case start_to_pick:
        {
            gripper_one.Gripper_Start_To_Pick();
            esp_now_community.Framework_Move_To_Set_Location();
            state_machine->Status = move_stop;
            break;
        }

        case start_to_set:
        {
            gripper_one.Gripper_Start_To_Set();
            esp_now_community.Framework_Start_To_Scan();
            state_machine->Status = start_to_scan;
            break;
        }

        case start_to_scan:
        {
            gripper_one.Gripper_Start_To_Scan();
            state_machine->Status = move_stop;
            break;
        }

        default:
            break;
        }
        delay(10);
    }
}

#endif