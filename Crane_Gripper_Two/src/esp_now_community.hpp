#ifndef __ESP_NOW_COMMUNITY_HPP
#define __ESP_NOW_COMMUNITY_HPP

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include "gripper.hpp"

#define Header 0x55
#define Rear 0x6B

extern Gripper gripper_two;

//状态机状态定义
#define move_stop 0
#define start_to_pick 1
#define start_to_set 2
#define start_to_scan 3

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
    void Esp_Now_Init()
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

    void Esp_Now_Init(HardwareSerial *sl)
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
     * @brief 框架移动到放置点的x坐标处
     *
     * @param 无
     *
     * @return None
     */
    void Framework_Move_To_Set_Location()
    {
        uint8_t send_data[6] = {Header, Header, 6, 0x03, 0x11, Rear};
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
   Serial.println(1);
    uint8_t receive_data[12];
    memcpy(&receive_data, incomingData, sizeof(receive_data));
    

    if (receive_data[0] == Header && receive_data[1] == Header)
    {
        if (receive_data[3] == 0x01)
        {
            switch (receive_data[4])
            {
            case 0x05:
            {
                gripper_two.Gripper_Status = start_to_pick;
                gripper_two.Pick_Location = Weight_Location_Y[((receive_data[5] / 16 - 1) * 3
                                            + receive_data[5] % 16 - 1)];
                break;
            }
            case 0x08:
            {
                gripper_two.Gripper_Status = start_to_set;
                gripper_two.Set_Location = Set_Location[1][receive_data[5] - 1];
                break;
            }

            default:
                break;
            }
        }
        else if(receive_data[3] == 0x04)
        {
            if(receive_data[4] == 0x11)
             {
                gripper_two.Gripper_Status = start_to_scan;
    
             }
        }
    }
}

#endif