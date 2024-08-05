#ifndef __STATUS_HPP
#define __STATUS_HPP

#include <Arduino.h>
#include "esp_now_community.hpp"
#include "uart.hpp"

void Task_Status_Checking(void *prsm);

extern Uart uart_with_nano;
extern Esp_Now_Community esp_now_community;
extern ESP_Now_e eps_now_e;

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
        if (eps_now_e.framework_next_state == framework_next_start_scan)
        {
            uint8_t send_data[5] = {HeaderShoot,HeaderShoot,0x05,0x1F,RearShoot};
            uart_with_nano.Serial_Send(send_data,sizeof(send_data) / sizeof(send_data[0]));
            uart_with_nano.Serial_Receive(uart_with_nano.Buffer);
            for(int i = 0; i < 6; i++)
            {
                eps_now_e.weight_num[i] = uart_with_nano.Buffer[i+4];
                Serial.write(eps_now_e.weight_num[i]);
            }
            eps_now_e.framework_next_state = framework_next_pick_start;
            esp_now_community.Send_Message();
        }

        delay(50);
    }
}

#endif