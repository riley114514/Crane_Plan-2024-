#ifndef __UART_H
#define __UART_H
#endif

#include <Arduino.h>

#define Max_Wait_Time 120000
#define HeaderShoot 0x77
#define RearShoot 0x5B


typedef int Uart_Command_Status;

enum 
{
    Uart_Command_Success = 1,
    Uart_Command_Error,
    Uart_Command_Insufficient,
    Uart_Send_False,
    Uart_Send_Success,
    Uart_Receive_False,
    Uart_Receive_Success
}Uart_Status;

class Uart
{

public:
    Uart_Command_Status Uart_Init(HardwareSerial *serial)
    {
        if(serial == NULL)
            return Uart_Command_Error;
        this->Uart_Serial = serial;
        this->Uart_Serial->begin(115200,SERIAL_8N1,12,13);
        return Uart_Command_Success;
    }
/*
    显示命令返回
*/
    void Show_Data(int length)
    {
        for(int i = 0; i < length; i++)
        {
            Serial.print(this->Buffer[i]);
            Serial.print(" ");
        }
        Serial.println("");
    }
// private:
    HardwareSerial *Uart_Serial;
    uint8_t Buffer[100];

/*
    向nano发送命令
*/
    void Serial_Send(uint8_t *data, int length, bool need_delay = true)
    {
        for (int i = 0; i < length; i++)
        {
            this->Uart_Serial->write(data[i]);
        }
        if(need_delay)
            delay(1);
    }

/*
    接收nano命令
*/
    Uart_Command_Status Serial_Receive(uint8_t *Buffer, int length, bool need_delay = true)
    {
        if(need_delay)
            delay(1);
        for (int i = 0; i < Max_Wait_Time; i++)
        {
            if (this->Uart_Serial->available())
            {
                this->Buffer[0] = this->Uart_Serial->read();
                this->Buffer[1] = this->Uart_Serial->read();
                if (this->Buffer[0] == HeaderShoot && this->Buffer[1] == HeaderShoot)
                {
                    this->Buffer[2] = this->Uart_Serial->read();
                    for(int j = 3; j < this->Buffer[2]; j++)
                        this->Buffer[j] = this->Uart_Serial->read();
                }
                return Uart_Receive_Success;
            }
            delay(1);
        }
        return Uart_Receive_False;
    }

};




