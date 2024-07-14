#ifndef __ESP_NOW_SEND_HPP
#define __ESP_NOW_SEND_HPP

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

// uint8_t broadcastAddress_1[] = {0xC8, 0x2E, 0x18, 0xF7, 0x53, 0xE8};
uint8_t broadcastAddress_F[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; //广播模式
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);

class Esp_Now_Sender
{

public:
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

        esp_now_peer_info_t peerInfo;
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

        esp_now_peer_info_t peerInfo;
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
        this->Esp_Now_Serial->begin(115200,SERIAL_8N1,rx,tx);
        WiFi.mode(WIFI_STA);

        if (esp_now_init() != ESP_OK)
        {
            this->Esp_Now_Serial->println("Error initializing ESP-NOW");
            return;
        }

        esp_now_register_send_cb(onDataSent);

        esp_now_peer_info_t peerInfo;
        memcpy(peerInfo.peer_addr, broadcastAddress_F, 6);
        peerInfo.channel = 0;
        peerInfo.encrypt = false;

        if (esp_now_add_peer(&peerInfo) != ESP_OK)
        {
            this->Esp_Now_Serial->println("Failed to add peer");
            return;
        }
    }

    void Send_Message()
    {
        String myData = "Send Message";
        esp_err_t result = esp_now_send(broadcastAddress_F, (uint8_t *)&myData, sizeof(myData));

        // if (result == ESP_OK)
        // {
        //     Serial.println("Send with success");
        // }
        // else
        // {
        //     Serial.println("Error sending the data");
        // }
    }

private:
    HardwareSerial *Esp_Now_Serial;

};

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    /*
        编写执行功能函数
    */
//    Serial.println(status == ESP_OK? "Success" : "Failed");

}

#endif