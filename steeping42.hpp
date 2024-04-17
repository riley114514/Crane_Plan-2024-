#ifndef __STEEPING42_H
#define __STEEPING42_H
#endif

#include <Arduino.h>

#define Max_Wait_Time 10
#define Once_Circle_Pulse 3200
#define Max_Speed 2500
#define Radius 2.0
#define PI 3.1415926535

const double Once_Circle_Distance = 2* PI * Radius;

typedef int Steeping42_Command_Status;

enum 
{
    Command_Success = 1,
    Command_Error,
    Command_Insufficient,
    Send_False
}Command_Status;

class steeping42
{
public:

/*
    初始化
*/
    Steeping42_Command_Status Steeping_Init(HardwareSerial *serial, uint8_t id)
    {
        this->Steeping_Serial = serial;
        this->ID = id;

        return this->Motor_Enable_Cmd();
    }

/*
    电机使能
*/
    Steeping42_Command_Status Motor_Enable_Cmd(void)
    {
        uint8_t Send_Data[6] = {this->ID, 0xF3, 0xAB, 0x01, 0x00, 0x6B};

        this->Serial_Send(Send_Data, 6);
        this->Serial_Receive(this->Buffer, 4);

        return this->Check_Status(); 
    }


/*
    电机失能
*/
    Steeping42_Command_Status Motor_Disable_Cmd(void)
    {
        uint8_t Send_Data[6] = {this->ID, 0xF3, 0xAB, 0x00, 0x00, 0x6B};

        this->Serial_Send(Send_Data, 6);
        this->Serial_Receive(this->Buffer, 4);

        return this->Check_Status(); 
    }

/*
    速度控制模式
*/
    Steeping42_Command_Status Speed_Mode_Cmd(uint8_t direction, uint16_t speed, uint8_t acceleration, bool dual_machine = false)
    {
        uint8_t machine_flag = dual_machine? 0x01: 0x00;
        speed = speed>Max_Speed? Max_Speed: speed;
        uint8_t speed1 = speed % 256;
        uint8_t speed2 = speed / 256;
        uint8_t Send_Data[8] = {this->ID, 0xF6, direction, speed2, speed1, acceleration,machine_flag, 0x6B};

        this->Serial_Send(Send_Data, 8);
        this->Serial_Receive(this->Buffer, 4);

        return this->Check_Status(); 

    }

/*
    位置控制模式
*/
    Steeping42_Command_Status Location_Mode_Cmd(uint8_t direction, uint16_t speed, uint8_t acceleration, uint32_t location, bool loaction_mode = true, bool dual_machine = false)
    {
        uint32_t pulse = (int) (location / Once_Circle_Distance) * Once_Circle_Pulse;
        uint8_t loaction_flag = loaction_mode? 0x01: 0x00;
        uint8_t machine_flag = dual_machine? 0x01: 0x00;
        speed = speed>Max_Speed? Max_Speed: speed;
        uint8_t speed1 = speed % 256;
        uint8_t speed2 = speed / 256;
        uint8_t pulse3 = pulse /256 /256;
        uint8_t pulse2 = pulse /256 %256;
        uint8_t pulse1 = pulse %256;
        uint8_t Send_Data[13] = {this->ID, 0xFD, direction, speed2, speed1, acceleration,pulse3, pulse2, pulse1, loaction_flag, machine_flag, 0x6B};
        
        this->Serial_Send(Send_Data, 13);
        this->Serial_Receive(this->Buffer, 4);

        return this->Check_Status(); 
    }
    
/*
    立即停止
*/
    Steeping42_Command_Status Stop_Instance(void)
    {
        uint8_t Send_Data[5] = {this->ID, 0xFE, 0x98, 0x00, 0x6B};

        this->Serial_Send(Send_Data, 5);
        this->Serial_Receive(this->Buffer, 4);

        return this->Check_Status(); 
    }

    Steeping42_Command_Status Dual_Machine_Enable(void)
    {
        uint8_t Send_Data[4] = {this->ID, 0xFF, 0x66, 0x6B};

        this->Serial_Send(Send_Data, 4);
        this->Serial_Receive(this->Buffer, 4);

        return this->Check_Status(); 
    }

/*
    将当前位置设置为零点
*/
    Steeping42_Command_Status Set_Once_Zero_Location(bool store = true)
    {
        uint8_t store_flag = store? 0x01: 0x00;
        uint8_t Send_Data[5] = {this->ID, 0x93, 0x88, store_flag, 0x6B};

        this->Serial_Send(Send_Data, 5);
        this->Serial_Receive(this->Buffer, 4);

        return this->Check_Status(); 
    }

/*
    回到设置的零点位置，建议zero_mode使用多圈无限位回零模式，即0x02
*/
    Steeping42_Command_Status Back_To_Zero_Location(bool dual_machine = false, uint8_t zero_mode)
    {   
        uint8_t machine_flag = dual_machine? 0x01: 0x00;
        uint8_t Send_Data[5] = {this->ID, 0x9A, zero_mode, machine_flag, 0x6B};

        this->Serial_Send(Send_Data, 5);
        this->Serial_Receive(this->Buffer, 4);

        return this->Check_Status(); 
    }

/*
    将位置，角度，脉冲清零
*/
    Steeping42_Command_Status Clear_All(void)
    {
        uint8_t Send_Data[4] = {this->ID, 0x0A, 0x6D, 0x6B};

        this->Serial_Send(Send_Data, 5);
        this->Serial_Receive(this->Buffer, 4);

        return this->Check_Status(); 
    }

/*
    修改电机的实时位置
*/
    Steeping42_Command_Status Read_Instance_Location(void)
    {
        uint8_t Send_Data[3] = {this->ID, 0x36, 0x6B};

        this->Serial_Send(Send_Data, 3);
        this->Serial_Receive(this->Buffer, 8);

        switch (this->Buffer[1])
        {
        case 0x36:
            return Command_Success;
        case 0x00:
            return Command_Error;
        default:
            return Send_False;
        }
    }

/*
    修改电机的ID
*/
    Steeping42_Command_Status Change_ID(uint8_t id, bool store = true)
    {
        uint8_t store_flag = store? 0x01: 0x00;
        uint8_t Send_Data[6] = {this->ID, 0xAE, 0x4B, store_flag, id, 0x6B};

        this->Serial_Send(Send_Data, 6);
        this->Serial_Receive(this->Buffer, 4);
        
        switch (this->Buffer[2])
        {
        case 0x02:
            this->ID = id;
            return Command_Success;
        case 0xEE:
            return Command_Error;
        default:
            return Send_False;
        }


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

private:
    HardwareSerial *Steeping_Serial;
    uint8_t ID;
    uint8_t Buffer[100];

/*
    向电机发送命令
*/
    void Serial_Send(uint8_t *data, int length, bool need_delay = true)
    {
        for (int i = 0; i < length; i++)
        {
            this->Steeping_Serial->write(data[i]);
        }
        if(need_delay)
            delay(1);
    }

/*
    接收电机的命令回传
*/
    void Serial_Receive(uint8_t *Buffer, int length, bool need_delay = true)
    {
        if(need_delay)
            delay(1);
        for (int i = 0; i < Max_Wait_Time; i++)
        {
            if (this->Steeping_Serial->available())
            {
                this->Buffer[0] = this->Steeping_Serial->read();
                if (this->Buffer[0] == this->ID)
                {
                    for(int j = 1; j < length; j++)
                        this->Buffer[j] = this->Steeping_Serial->read();
                }
            }
            delay(1);
        }
    }

/*
    检验电机接收命令状态
*/
    Steeping42_Command_Status Check_Status(void)
    {
        switch (this->Buffer[2])
        {
        case 0x02:
            return Command_Success;
        case 0xE2:
            return Command_Insufficient;
        case 0xEE:
            return Command_Error;
        default:
            return Send_False;
        }
    }
};
