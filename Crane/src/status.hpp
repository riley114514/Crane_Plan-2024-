#ifndef __STATUS_HPP
#define __STATUS_HPP

#include <Arduino.h>
#include "framework.hpp"
#include "esp_now_community.hpp"

void Task_Status_Check(void *prsm);

extern Esp_Now_Community esp_now_community;
extern Framework framework;

// 状态机的状态表示声明
#define move_stop 0
#define move_forward 1
#define move_backward 2
#define gripper_one_pick_work 4
#define gripper_two_pick_work 5
#define gripper_together_pick_work 6
#define gripper_one_set_work 7
#define gripper_two_set_work 8
#define gripper_together_set_work 9
#define gripper_back_to_location 10
#define framework_back_to_location 11
#define start_scan 12
#define move_to_set_location 13




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
        this->Status = gripper_back_to_location;
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
    Status *state_machine = (Status *)prsm;
    while (1)
    {
        state_machine->Status = framework.Framework_Status;
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
            framework.Framework_Status = move_stop;
            framework.Pick_Num = 1;
            break;
        }

        case gripper_two_pick_work:
        {
            esp_now_community.Gripper_Two_Work();
            framework.Framework_Status = move_stop;
            framework.Pick_Num = 1;
            break;
        }

        case gripper_together_pick_work:
        {
            esp_now_community.Gripper_Work_Together();
            framework.Framework_Status = move_stop;
            framework.Pick_Num = 1;
            break;
        }

        case gripper_one_set_work:
        {
            esp_now_community.Gripper_One_Set();
            framework.Framework_Status = move_stop;
            break;
        }

        case gripper_two_set_work:
        {
            esp_now_community.Gripper_Two_Set();
            framework.Framework_Status = move_stop;
            break;
        }

        case gripper_together_set_work:
        {
            esp_now_community.Gripper_Set_Together();
            framework.Framework_Status = move_stop;
            break;
        }

        case gripper_back_to_location:
        {
            esp_now_community.Gripper_Back_To_Loaction();
            framework.Framework_Status = move_stop;
            break;
        }

        case framework_back_to_location:
        {
            framework.Frame_Back_To_Location();
            esp_now_community.Gripper_Down_To_Loaction();
            framework.Framework_Status = move_stop;
            break;
        }

        case start_scan:
        {
            // 这里插入与nano通信的开始扫描函数

            //
            esp_now_community.Gripper_Start_To_Pick();
            framework.Framework_Status = move_stop;
            break;
        }

        case move_to_set_location:
        {
            // framework.Frame_Set_Location();
            if(framework.Set_Num == 1)
                framework.Framework_Status = gripper_one_pick_work;
            else
                framework.Framework_Status = gripper_together_pick_work;
            break;
        }

        default:
            break;
        }
        delay(10);
    }
}

#endif