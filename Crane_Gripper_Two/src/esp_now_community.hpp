#ifndef __ESP_NOW_COMMUNITY_HPP
#define __ESP_NOW_COMMUNITY_HPP

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

typedef enum
{
  framework_next_stop = 0,
  framework_next_start_scan,
  framework_next_pick_start,
  framework_next_pick_finish,
  framework_next_set_start,
  framework_next_set_finish,
}Framework_Next_Status;

typedef enum
{
  gripper_one_next_stop = 0,
  gripper_one_next_start_scan,
  gripper_one_next_pick_start,
  gripper_one_next_pick_finish,
  gripper_one_next_set_start,
  gripper_one_next_set_finish,
}Gripper_One_Next_Status;

typedef enum
{
  gripper_two_next_stop = 0,
  gripper_two_next_start_scan,
  gripper_two_next_pick_start,
  gripper_two_next_pick_finish,
  gripper_two_next_set_start,
  gripper_two_next_set_finish,
}Gripper_Two_Next_Status;

//用于测试的数据
typedef struct __ESP_Now_e{
  Framework_Next_Status framework_next_state;
  Gripper_One_Next_Status gripper_one_next_state;
  Gripper_Two_Next_Status gripper_two_next_state;
  uint8_t weight_num[6];
  uint8_t gripper_one_pick_num;
  uint8_t gripper_one_set_num;
  uint8_t gripper_two_pick_num;
  uint8_t gripper_two_set_num;
  uint8_t weight_pointer = 5;
  uint8_t set_pointer = 2;
} ESP_Now_e;

extern ESP_Now_e eps_now_e;

// uint8_t broadcastAddress_1[] = {0xC8, 0x2E, 0x18, 0xF7, 0x53, 0xE8};
uint8_t broadcastAddress_F[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // 广播模式
esp_now_peer_info_t peerInfo;

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
        WiFi.mode(WIFI_STA);

        if (esp_now_init() != ESP_OK)
        {
            // Serial.println("Error initializing ESP-NOW");
            return;
        }

        esp_now_register_send_cb(onDataSent);
        esp_now_register_recv_cb(OnDataRecv);

        memcpy(peerInfo.peer_addr, broadcastAddress_F, 6);
        peerInfo.channel = 0;
        peerInfo.encrypt = false;

        if (esp_now_add_peer(&peerInfo) != ESP_OK)
        {
            // Serial.println("Failed to add peer");
            return;
        }
        eps_now_e.framework_next_state = framework_next_stop;
        eps_now_e.gripper_one_next_state = gripper_one_next_stop;
        eps_now_e.gripper_two_next_state = gripper_two_next_stop;
    }
    /**
     * @brief 发送函数，向两个机械爪发送指令
     * 
     * @param 无
     * 
     * @return None
     */
    void Send_Message()
    {
        esp_err_t result = esp_now_send(broadcastAddress_F, (uint8_t *)&eps_now_e, sizeof(eps_now_e));
        // if (result == ESP_OK)
        // {
        //     Serial.println("Send with success");
        // }
        // else
        // {
        //     Serial.println("Error sending the data");
        // }
    }



// private:

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
    memcpy(&eps_now_e, incomingData, sizeof(eps_now_e));
}

#endif