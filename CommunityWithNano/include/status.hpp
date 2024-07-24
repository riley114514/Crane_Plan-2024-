#ifndef __STATUS_HPP
#define __STATUS_HPP

#include <Arduino.h>
#include "community.hpp"
#include "esp_now_community.hpp"

extern Esp_Now_Community esp_now_community;
extern Community NanoCommunity;




void Task_Status_Check(void *prsm);

class Status
{
public:
    void Status_Init()
    {
        state = move_stop;
        this->State_Status_Check();
    }

    void State_Status_Check(void)
    {
        xTaskCreatePinnedToCore(Task_Status_Check, "Task_Status_Check", 4096, this, 5, NULL, 0);
    }

    State state;  

// private:

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
        state_machine->state = esp_now_community.state;
        switch (state_machine->state)
        {
        case start_scan:
        {
            NanoCommunity.NANO_Send();
            NanoCommunity.NANO_Recieve();
            NanoCommunity.ScaleIdentification();
            esp_now_community.state = trans_location;
            break;
        }

        case trans_location:
        {
            esp_now_community.Send_Weight_Location();
            esp_now_community.state = trans_location;
            break;
        }

        default:
            break;
        }
        delay(10);
    }
}

#endif