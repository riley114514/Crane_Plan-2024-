#ifndef __ESP_NOW_COMMUNITY_HPP
#define __ESP_NOW_COMMUNITY_HPP

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include "state.hpp"

#define Header 0x55
#define Rear 0x6B

extern State state_machine;

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
     * @brief 机械爪启动后复位函数,先让爪子上升，并向两侧复位，
     *        （然后滑杆开始复位，再让爪子下降）这是后面的两个函数
     * 
     * @param 无
     * 
     * @return None
     */
    void Gripper_Back_To_Loaction() 
    {
        uint8_t send_data[6]= {Header, Header, 6, 0x03, 0x01, Rear};
        esp_err_t result = esp_now_send(broadcastAddress_F, (uint8_t *)&send_data, sizeof(send_data));
    }

    /**
     * @brief 滑杆复位完成后，让爪子下降函数
     * 
     * @param 无
     * 
     * @return None
     */
    void Gripper_Down_To_Loaction() 
    {
        uint8_t send_data[6]= {Header, Header, 6, 0x03, 0x02, Rear};
        esp_err_t result = esp_now_send(broadcastAddress_F, (uint8_t *)&send_data, sizeof(send_data));
    }

    /**
     * @brief 一号机械爪开始工作函数，用于当前位置只抓一个砝码
     * 
     * @param 无
     * 
     * @return None
     */
    void Gripper_One_Work() 
    {
        uint8_t send_data[6]= {Header, Header, 6, 0x01, 0x03, Rear};
        esp_err_t result = esp_now_send(broadcastAddress_F, (uint8_t *)&send_data, sizeof(send_data));
    }


    /**
     * @brief 二号机械爪开始工作函数，用于当前位置只抓一个砝码
     * 
     * @param 无
     * 
     * @return None
     */
    void Gripper_Two_Work() 
    {
        uint8_t send_data[6]= {Header, Header, 6, 0x02, 0x04, Rear};
        esp_err_t result = esp_now_send(broadcastAddress_F, (uint8_t *)&send_data, sizeof(send_data));
    }


    /**
     * @brief 两个机械爪开始工作函数，用于当前位置要抓两个砝码
     * 
     * @param 无
     * 
     * @return None
     */
    void Gripper_Work_Together() 
    {
        uint8_t send_data[6]= {Header, Header, 6, 0x03, 0x05, Rear};
        esp_err_t result = esp_now_send(broadcastAddress_F, (uint8_t *)&send_data, sizeof(send_data));
    }

    /**
     * @brief 一号机械爪放到指定位置，用于只有一号机械爪夹取砝码
     * 
     * @param 无
     * 
     * @return None
     */
    void Gripper_One_Set() 
    {
        uint8_t send_data[6]= {Header, Header, 6, 0x01, 0x06, Rear};
        esp_err_t result = esp_now_send(broadcastAddress_F, (uint8_t *)&send_data, sizeof(send_data));
    }


    /**
     * @brief 二号机械爪放到指定位置，用于只有二号机械爪夹取砝码
     * 
     * @param 无
     * 
     * @return None
     */
    void Gripper_Two_Set() 
    {
        uint8_t send_data[6]= {Header, Header, 6, 0x02, 0x07, Rear};
        esp_err_t result = esp_now_send(broadcastAddress_F, (uint8_t *)&send_data, sizeof(send_data));
    }

    /**
     * @brief 两个机械爪放到指定位置，用于两个机械爪都夹取砝码
     * 
     * @param 无
     * 
     * @return None
     */
    void Gripper_Set_Together() 
    {
        uint8_t send_data[6]= {Header, Header, 6, 0x03, 0x08, Rear};
        esp_err_t result = esp_now_send(broadcastAddress_F, (uint8_t *)&send_data, sizeof(send_data));
    }

    /**
     * @brief 两个机械爪在准备开始扫描时，应该让它变成准备夹取状态，此时调整高度
     * 
     * @param 无
     * 
     * @return None
     */
    void Gripper_Start_To_Pick() 
    {
        uint8_t send_data[6]= {Header, Header, 6, 0x03, 0x09, Rear};
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
    uint8_t receive_data[6];
    memcpy(&receive_data, incomingData, sizeof(receive_data));
    if (receive_data[0] == Header && receive_data[1] == Header)
    {
        if (receive_data[3] == 0x04)
        {
            switch (receive_data[4])
            {
            case 0x11:
                state_machine.Pick_Finish_Num++;
                if(state_machine.Pick_Finish_Num == state_machine.Pick_Num)
                {
                    state_machine.Status = move_to_set_location;
                    state_machine.Pick_Finish_Num = 0;
                }
                break;

            case 0x12:
                state_machine.Set_Finish_Num++;
                if(state_machine.Set_Finish_Num == state_machine.Set_Num)
                {
                    state_machine.Status = start_scan;
                    state_machine.Weight_Num -= state_machine.Set_Finish_Num;
                    state_machine.Set_Finish_Num = 0;
                }
                break;

            default:
                break;
            }
        }
    }
}

#endif