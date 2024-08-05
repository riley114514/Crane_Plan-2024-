#include "esp32-hal.h"
#ifndef __UART_H
#define __UART_H

#include <Arduino.h>

#define Max_Wait_Time 1000 // 通讯最大等待时间1秒
#define HeaderShoot 0x77
#define RearShoot 0x5B

typedef int Uart_Command_Status;

// 通讯状态枚举
typedef enum
{
    Uart_Send_False = 0,
    Uart_Send_Success,
    Uart_Receive_False,
    Uart_Receive_Success
} Uart_Status_e;

class Uart
{

public:
    /**
     * @brief 通讯初始化函数
     *
     * @param serial：串口
     *
     * @return None
     */
    void Uart_Init(HardwareSerial *serial)
    {
        this->Uart_Serial = serial;
        this->Uart_Serial->begin(115200);
    }

    /**
     * @brief 通讯初始化函数
     *
     * @param serial：串口
     * @param rx:串口复用rx引脚
     * @param tx:串口复用tx引脚
     *
     * @return None
     */
    void Uart_Init(HardwareSerial *serial, int rx, int tx)
    {
        this->Uart_Serial = serial;
        this->Uart_Serial->begin(115200, SERIAL_8N1, rx, tx);
    }

    /**
     * @brief 显示通讯消息
     *
     * @param data:存储回传消息的数组
     * @param length:显示数组的长度
     *
     * @return None
     */
    void Uart_Show_Data(uint8_t *data, int length)
    {
        for (int i = 0; i < length; i++)
        {
            Serial.print(data[i]);
            Serial.print(" ");
        }
        Serial.println("");
    }

    /**
     * @brief 通讯发送测试函数
     *
     * @param None
     *
     * @return None
     */
    void Uart_Test_Send()
    {
        uint8_t send_data[5] = {HeaderShoot, HeaderShoot, 0x05, 0x1F, RearShoot};
        this->Serial_Send(send_data, 5);
        // Serial被占用时可以换成其他串口或者ESP_Now
        Serial.println("send_success");
    }

    /**
     * @brief 通讯接收测试函数
     *
     * @param None
     *
     * @return None
     */
    void Uart_Test_Receive()
    {
        uint8_t receive_data[5];
        Uart_Status_e result = this->Serial_Receive(receive_data);
        while (result != Uart_Receive_Success)
        {
            delay(1);
            // Serial被占用时可以换成其他串口或者ESP_Now
            result = this->Serial_Receive(receive_data);
            Serial.println("waiting_for_receving");
        }
        Serial.println("receive_success");
        delay(10);
        this->Uart_Send_Check_Pack();
    }

    /**
     * @brief 发送校验数据包函数： Header，Header, Header, Rear
     *
     * @param None
     *
     * @return None
     */
    void Uart_Send_Check_Pack()
    {
        uint8_t send_data[4] = {HeaderShoot, HeaderShoot, HeaderShoot, RearShoot};
        this->Uart_Serial->write(send_data, 4);
    }
    // void
    // private:

    HardwareSerial *Uart_Serial;
    uint8_t Buffer[100];

    /**
     * @brief 发送通讯传输函数
     *
     * @param data:用于向tx发送的消息数组
     * @param length:向tx发送的消息数组的长度
     * @param need_delay:是否需要延时
     *
     * @return None
     */
    void Serial_Send(uint8_t *data, int length, bool need_delay = true)
    {
        for (int i = 0; i < length; i++)
        {
            this->Uart_Serial->write(data[i]);
        }

        if (need_delay)
            delay(5);

        uint8_t receive_data[4];
        bool flag = false;
        while (!flag)
        {
            for (int i = 0; i < Max_Wait_Time; i++)
            {
                if (this->Uart_Serial->available())
                {
                    receive_data[0] = this->Uart_Serial->read();
                    if (receive_data[0] == HeaderShoot)
                    {
                        receive_data[1] = this->Uart_Serial->read();

                        if (receive_data[1] == HeaderShoot)
                        {
                            receive_data[2] = this->Uart_Serial->read();
                            if (receive_data[2] == HeaderShoot)
                            {
                                receive_data[3] = this->Uart_Serial->read();
                                if (receive_data[3] == RearShoot)
                                {
                                    flag = true;
                                    break;
                                }
                            }
                        }
                    }
                }
                delay(1);
            }
            Serial.println("wait_for_receive_check");
            if (!flag)
            {
                for (int i = 0; i < length; i++)
                {
                    this->Uart_Serial->write(data[i]);
                    delay(1);
                }
                if (need_delay)
                    delay(1);
            }
        }
    }

    /**
     * @brief 接收通讯回传消息
     *
     * @param buffer:用于接收rx回传的消息数组
     * @param need_delay:是否需要延时
     *
     * @return Uart_Status_e:Uart_Receive_Success | Uart_Receive_False
     */
    Uart_Status_e Serial_Receive(uint8_t *buffer, bool need_delay = true)
    {
        if (need_delay)
            delay(10);
        for (int i = 0; i < Max_Wait_Time; i++)
        {
            if (this->Uart_Serial->available())
            {
                buffer[0] = this->Uart_Serial->read();
                
                if (buffer[0] == HeaderShoot)
                {
                    buffer[1] = this->Uart_Serial->read();
                    if (buffer[1] == HeaderShoot)
                    {
                        buffer[2] = this->Uart_Serial->read();
                        for (int j = 3; j < buffer[2]; j++)
                            buffer[j] = this->Uart_Serial->read();
                        if (buffer[buffer[2] - 1] == RearShoot)
                        {
                            uint8_t send_data[4] = {HeaderShoot, HeaderShoot, HeaderShoot, RearShoot};
                            this->Uart_Serial->write(send_data, 4);
                        }
                        return Uart_Receive_Success;
                        Serial.println("Receive_Success");
                    }
                }
            }
            delay(1);
        }
        return Uart_Receive_False;
    }
};

#endif
