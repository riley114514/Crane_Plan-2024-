#ifndef __STATUS_HPP
#define __STATUS_HPP

#include <Arduino.h>
#include "framework.hpp"
#include "esp_now_community.hpp"

void Status_Checking_Task(void *prsm);

extern Esp_Now_Community esp_now_community;
extern Framework framework;

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
        this->Status = framework.Framework_Status;
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
        xTaskCreatePinnedToCore(Status_Checking_Task, "Crane_Init", 4096, this, 5, NULL, 0);
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
void Status_Checking_Task(void *prsm)
{  


    Status *state_machine = (Status *)prsm;
    while (1)
    {
        // Serial2.println(state_machine->Status);
        //  state_machine->Status = framework.Framework_Status;
        // Serial2.println(state_machine->Status);

        Serial2.println(framework.Framework_Status);
        if (state_machine->Status==move_stop)
        {
                 framework.Framework_Move_Stop();
            if(digitalRead(26) == LOW)//左上角有一个总微动开关，摁下后启动线程
            {
                // while(digitalRead(34) == LOW)
                // delay(1);
                framework.Framework_Status = start_scan;
                state_machine->Status = framework.Framework_Status;
                Serial2.println(framework.Framework_Status);
            }
        }
        else if (state_machine->Status==start_scan)
        {
           

                esp_now_community.Gripper_Start_Scan();
                Serial2.println(2);
            
            framework.Framework_Status = move_stop;
                // Serial2.println(1);
        }
        else if (state_machine->Status==move_to_pick_location)
        {
             if(framework.Weight_Num == 3)
            {
                framework.Pick_Num = 1;
                esp_now_community.Gripper_One_Work(framework.Weight_Location[framework.pointer_weight - 1]);
                framework.pointer_weight -= 1;
                framework.Framework_Status = move_stop;
            }
            else
            {
                if(framework.Weight_Location[framework.pointer_weight] / 16 == framework.Weight_Location[framework.pointer_weight - 1] / 16)
                {
                    framework.Pick_Num = 2;
                    esp_now_community.Gripper_One_Work(framework.Weight_Location[framework.pointer_weight]);
                    esp_now_community.Gripper_Two_Work(framework.Weight_Location[framework.pointer_weight - 1]);
                    framework.Framework_Status = move_stop;
                    framework.pointer_weight -= 2;
                }
                else
                {
                    if(framework.Pick_Num % 2 == 0)
                        esp_now_community.Gripper_One_Work(framework.Weight_Location[framework.pointer_weight]);
                    else
                        esp_now_community.Gripper_Two_Work(framework.Weight_Location[framework.pointer_weight - 1]);
                    framework.Pick_Num++;
                    framework.pointer_weight -= 1;
                    if(framework.Pick_Num % 2 == 0)
                        framework.Framework_Status = move_stop;
                }
            }
        }else if (state_machine->Status==move_to_set_location)
        {
             framework.Frame_Set_Location(Set_Location[0][framework.Weight_Num]);
            if(framework.Weight_Num == 3)
            {
                framework.Set_Num = 1;
                esp_now_community.Gripper_One_Set(framework.Weight_Num);
            }
            else
            {
                framework.Set_Num = 2;
                esp_now_community.Gripper_Two_Set(framework.Weight_Num);
                esp_now_community.Gripper_One_Set(framework.Weight_Num - 1);
            }
            framework.Framework_Status = move_stop;
        }
        delay(100);
    }


}

#endif