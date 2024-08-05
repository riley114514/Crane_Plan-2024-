#ifndef __STATUS_HPP
#define __STATUS_HPP

#include <Arduino.h>
#include "gripper.hpp"
#include "esp_now_community.hpp"

void Task_Status_Checking(void *prsm);

extern Esp_Now_Community esp_now_community;
extern ESP_Now_e eps_now_e;
extern Gripper gripper_one;

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
        xTaskCreatePinnedToCore(Task_Status_Checking, "Task_Status_Checking", 4096, this, 5, NULL, 0);
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
void Task_Status_Checking(void *prsm)
{
    while (1)
    {
        if (eps_now_e.gripper_one_next_state == gripper_one_next_start_scan)
        {
            gripper_one.Gripper_Set_Z_Location(240);
            gripper_one.Gripper_Set_Y_Location(200);
            eps_now_e.gripper_one_next_state == gripper_one_next_stop;
            esp_now_community.Send_Message();
        }
        else if (eps_now_e.gripper_one_next_state == gripper_one_next_pick_start)
        {
            gripper_one.Gripper_Set_Y_Location(Weight_Location_Y[eps_now_e.gripper_one_pick_num / 16 - 1][eps_now_e.gripper_one_pick_num % 16 - 1]);
            gripper_one.servo->Set_Servo_Angle(130);
            gripper_one.Gripper_Set_Z_Location(5);
            gripper_one.servo->Set_Servo_Angle(80);
            delay(500);
            gripper_one.Gripper_Set_Z_Location(240);
            if(eps_now_e.weight_pointer != 2)
            {
                gripper_one.Gripper_Set_Y_Location(245);
            }
            else
            {
                gripper_one.Gripper_Set_Y_Location(1000);
            }
            eps_now_e.gripper_one_next_state = gripper_one_next_pick_finish;
            if(eps_now_e.gripper_one_next_state == gripper_one_next_pick_finish && eps_now_e.gripper_two_next_state == gripper_two_next_pick_finish)
                eps_now_e.framework_next_state = framework_next_set_start;
            esp_now_community.Send_Message();
        }
        else if (eps_now_e.gripper_one_next_state == gripper_one_next_set_start)
        {
            if(eps_now_e.set_pointer == 0)
            {
                gripper_one.Gripper_Set_Z_Location(110);
                gripper_one.servo->Set_Servo_Angle(130);
                delay(500);
                gripper_one.Gripper_Set_Z_Location(240);
                gripper_one.Gripper_Set_Y_Location(200);
                eps_now_e.framework_next_state = framework_next_pick_start;
            }
            else
            {
                gripper_one.Gripper_Set_Z_Location(210);
                gripper_one.servo->Set_Servo_Angle(130);
                delay(500);
                gripper_one.Gripper_Set_Z_Location(240);
                gripper_one.Gripper_Set_Y_Location(200);
            }

            eps_now_e.gripper_one_next_state = gripper_one_next_set_finish;
            if(eps_now_e.gripper_one_next_state == gripper_one_next_set_finish && eps_now_e.gripper_two_next_state == gripper_two_next_set_finish)
            {
                if(eps_now_e.set_pointer == 1)
                {
                    eps_now_e.framework_next_state = framework_next_pick_start;
                }
            }
            esp_now_community.Send_Message();
        }
        delay(50);
    }
}

#endif