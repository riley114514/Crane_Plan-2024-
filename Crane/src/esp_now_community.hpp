#ifndef __ESP_NOW_COMMUNITY_HPP
#define __ESP_NOW_COMMUNITY_HPP

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include "framework.hpp"

#define Header 0x55
#define Rear 0x6B

extern Framework framework;

// 状态机的状态表示声明
#define move_stop 0
#define move_to_pick_location 1
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

// uint8_t broadcastAddress_1[] = {0xC8, 0x2E, 0x18, 0xF7, 0x53, 0xE8};
uint8_t broadcastAddress_F[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // 广播模式
esp_now_peer_info_t peerInfo;


// 接收状态定义
enum package_type
{
    package_type_normal = 0,
    package_type_error = 1,
    package_type_request = 2,
    package_type_response = 3
};

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len);

class Esp_Now_Community
{

public:
    /**
     * @brief esp_now 初始化
     * 
     * @param 无
     * 
     * @return None
     */
    void Esp_Now_Send_Init()
    {
        this->Esp_Now_Serial = &Serial;
        this->Esp_Now_Serial->begin(115200);
        WiFi.mode(WIFI_STA);

        if (esp_now_init() != ESP_OK)
        {
            this->Esp_Now_Serial->println("Error initializing ESP-NOW");
            return;
        }

        esp_now_register_send_cb(onDataSent);
        esp_now_register_recv_cb(OnDataRecv);

        memcpy(peerInfo.peer_addr, broadcastAddress_F, 6);
        peerInfo.channel = 0;
        peerInfo.encrypt = false;

        if (esp_now_add_peer(&peerInfo) != ESP_OK)
        {
            this->Esp_Now_Serial->println("Failed to add peer");
            return;
        }
    }

    void Esp_Now_Send_Init(HardwareSerial *sl)
    {
        this->Esp_Now_Serial = sl;
        this->Esp_Now_Serial->begin(115200);
        WiFi.mode(WIFI_STA);

        if (esp_now_init() != ESP_OK)
        {
            this->Esp_Now_Serial->println("Error initializing ESP-NOW");
            return;
        }

        esp_now_register_send_cb(onDataSent);
        esp_now_register_recv_cb(OnDataRecv);

        memcpy(peerInfo.peer_addr, broadcastAddress_F, 6);
        peerInfo.channel = 0;
        peerInfo.encrypt = false;

        if (esp_now_add_peer(&peerInfo) != ESP_OK)
        {
            this->Esp_Now_Serial->println("Failed to add peer");
            return;
        }
    }

    void Esp_Now_Send_Init(HardwareSerial *sl, uint8_t rx, uint8_t tx)
    {
        this->Esp_Now_Serial = sl;
        this->Esp_Now_Serial->begin(115200, SERIAL_8N1, rx, tx);
        WiFi.mode(WIFI_STA);

        if (esp_now_init() != ESP_OK)
        {
            this->Esp_Now_Serial->println("Error initializing ESP-NOW");
            return;
        }

        esp_now_register_send_cb(onDataSent);
        esp_now_register_recv_cb(OnDataRecv);

        memcpy(peerInfo.peer_addr, broadcastAddress_F, 6);
        peerInfo.channel = 0;
        peerInfo.encrypt = false;

        if (esp_now_add_peer(&peerInfo) != ESP_OK)
        {
            this->Esp_Now_Serial->println("Failed to add peer");
            return;
        }
    }

    /**
     * @brief 发送函数，向两个机械爪发送指令
     * 
     * @param 无
     * 
     * @return None
     */
    // void Send_Message()
    // {
    //     String myData = "Send Message";
    //     esp_err_t result = esp_now_send(broadcastAddress_F, (uint8_t *)&myData, sizeof(myData));

    //     // if (result == ESP_OK)
    //     // {
    //     //     Serial.println("Send with success");
    //     // }
    //     // else
    //     // {
    //     //     Serial.println("Error sending the data");
    //     // }
    // }

    /**
     * @brief 一号机械爪开始工作函数，用于当前位置只抓一个砝码
     * 
     * @param 无
     * 
     * @return None
     */
    void Gripper_One_Work(uint8_t num) 
    {
        uint8_t send_data[7]= {Header, Header, 7, 0x01, 0x05, num, Rear};
        esp_err_t result = esp_now_send(broadcastAddress_F, (uint8_t *)&send_data, sizeof(send_data));
    }


    /**
     * @brief 二号机械爪开始工作函数，用于当前位置只抓一个砝码
     * 
     * @param 无
     * 
     * @return None
     */
    void Gripper_Two_Work(uint8_t num) 
    {
        uint8_t send_data[7]= {Header, Header, 7, 0x02, 0x05, num, Rear};
        esp_err_t result = esp_now_send(broadcastAddress_F, (uint8_t *)&send_data, sizeof(send_data));
    }




    /**
     * @brief 一号机械爪放到指定位置，用于只有一号机械爪夹取砝码
     * 
     * @param 无
     * 
     * @return None
     */
    void Gripper_One_Set(uint8_t num) 
    {
        uint8_t send_data[7]= {Header, Header, 7, 0x01, 0x08, num, Rear};
        esp_err_t result = esp_now_send(broadcastAddress_F, (uint8_t *)&send_data, sizeof(send_data));
    }


    /**
     * @brief 二号机械爪放到指定位置，用于只有二号机械爪夹取砝码
     * 
     * @param 无
     * 
     * @return None
     */
    void Gripper_Two_Set(uint8_t num) 
    {
        uint8_t send_data[7]= {Header, Header, 7, 0x02, 0x08, num, Rear};
        esp_err_t result = esp_now_send(broadcastAddress_F, (uint8_t *)&send_data, sizeof(send_data));
    }

    /**
     * @brief 两个机械爪在准备开始扫描时，应该让它变成准备夹取状态，此时调整高度
     * 
     * @param 无
     * 
     * @return None
     */
    void Gripper_Start_Scan() 
    {
        uint8_t send_data[6]= {Header, Header, 6, 0x04, 0x11, Rear};
        esp_err_t result = esp_now_send(broadcastAddress_F, (uint8_t *)&send_data, sizeof(send_data));
    }

private:
    HardwareSerial *Esp_Now_Serial;
};

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    /*
        编写执行功能函数，用于用户提示
    */
    //    Serial.println(status == ESP_OK? "Success" : "Failed");
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    /*
        编写执行功能函数，用于接收函数
    */
    uint8_t receive_data[12];
    memcpy(&receive_data, incomingData, sizeof(receive_data));

    if (receive_data[0] == Header && receive_data[1] == Header)
    {
        if (receive_data[3] == 0x03)
        {
            switch (receive_data[4])
            {
            case 0x11:
            {
                framework.Pick_Finish_Num++;
                if(framework.Pick_Finish_Num == framework.Pick_Num)
                {
                    framework.Framework_Status = move_to_set_location;
                    framework.Pick_Finish_Num = 0;
                    framework.Pick_Num = 0;
                }
                break;
            }


            case 0x12:
            {
                framework.Set_Finish_Num++;
                if(framework.Set_Finish_Num == framework.Set_Num)
                {
                    framework.Weight_Num -= framework.Set_Finish_Num;
                    if(framework.Weight_Num > 0)
                        framework.Framework_Status = move_to_pick_location;
                    else
                        framework.Framework_Status = move_stop;
                    framework.Set_Finish_Num = 0;
                    framework.Set_Num = 0;
                }

                break;
            }

            case 0x13:
            {
                for(int i = 0; i< 6; i++)
                {
                    framework.Weight_Location[i] = receive_data[i+5];
                }
                framework.Framework_Status = move_to_pick_location;
                break;
            }

                
            default:
                break;
            }
        }
    }
}

#endif