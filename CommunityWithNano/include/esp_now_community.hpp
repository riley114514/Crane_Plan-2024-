#ifndef __ESP_NOW_COMMUNITY_HPP
#define __ESP_NOW_COMMUNITY_HPP

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include "community.hpp"

extern Community NanoCommunity;


#define Header 0x55
#define Rear 0x6B


// uint8_t broadcastAddress_1[] = {0xC8, 0x2E, 0x18, 0xF7, 0x53, 0xE8};
uint8_t broadcastAddress_F[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // 广播模式
esp_now_peer_info_t peerInfo;


typedef enum
{
    move_stop = 0,
    start_scan,
    trans_location
}State;

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
        state = move_stop;
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
    void Send_Weight_Location() 
    {
        uint8_t send_data[10]= {Header, Header, 10, NanoCommunity.location_buffer[0], NanoCommunity.location_buffer[1], NanoCommunity.location_buffer[2], NanoCommunity.location_buffer[3], 
                            NanoCommunity.location_buffer[4], NanoCommunity.location_buffer[5], Rear};
        esp_err_t result = esp_now_send(broadcastAddress_F, (uint8_t *)&send_data, sizeof(send_data));
    }

    State state;

private:
    HardwareSerial *Esp_Now_Serial;
};

extern Esp_Now_Community esp_now_community;

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
    if(receive_data[0] == Header && receive_data[1] == Header)
    {
        if(receive_data[3] == 0x04 && receive_data[4] == 0x11)
        {
            esp_now_community.state = start_scan;
        }
    }

}

#endif