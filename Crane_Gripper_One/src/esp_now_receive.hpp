#ifndef __ESP_NOW_RECEIVE_HPP
#define __ESP_NOW_RECEIVE_HPP

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len);

class Esp_Now_Receiver
{
public:
    void Esp_Now_Receive_Init()
    {
        this->Esp_Now_Serial = &Serial;
        // Initialize Serial Monitor
        this->Esp_Now_Serial->begin(115200);

        // Set device as a Wi-Fi Station
        WiFi.mode(WIFI_STA);

        // Init ESP-NOW
        if (esp_now_init() != ESP_OK)
        {
            Serial.println("Error initializing ESP-NOW");
            return;
        }

        // Once ESPNow is successfully Init, we will register for recv CB to
        // get recv packer info
        esp_now_register_recv_cb(OnDataRecv);
    }

    void Esp_Now_Receive_Init(HardwareSerial *sl)
    {
        this->Esp_Now_Serial = sl;
        // Initialize Serial Monitor
        this->Esp_Now_Serial->begin(115200);

        // Set device as a Wi-Fi Station
        WiFi.mode(WIFI_STA);

        // Init ESP-NOW
        if (esp_now_init() != ESP_OK)
        {
            Serial.println("Error initializing ESP-NOW");
            return;
        }

        // Once ESPNow is successfully Init, we will register for recv CB to
        // get recv packer info
        esp_now_register_recv_cb(OnDataRecv);
    }

    void Esp_Now_Receive_Init(HardwareSerial *sl, uint8_t rx, uint8_t tx)
    {
        this->Esp_Now_Serial = sl;
        this->Esp_Now_Serial->begin(115200, SERIAL_8N1, rx, tx);
        WiFi.mode(WIFI_STA);

        // Set device as a Wi-Fi Station
        WiFi.mode(WIFI_STA);

        // Init ESP-NOW
        if (esp_now_init() != ESP_OK)
        {
            Serial.println("Error initializing ESP-NOW");
            return;
        }
    }

private:
    HardwareSerial *Esp_Now_Serial;
};

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    /*
        编写执行功能函数
    */
    float myData[2];
    memcpy(&myData, incomingData, sizeof(myData));
    Serial.printf("Curren Speed is : %.2f\n", myData[0]);
    Serial.printf("Curren Location is : %.2f\n", myData[1]);

}

#endif