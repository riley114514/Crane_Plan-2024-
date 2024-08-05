#ifndef __EMMC423V53_HPP
#define __EMMC423V53_HPP

#include <Arduino.h>

#define Max_Wait_Time 50
#define Once_Circle_Pulse 3200
#define Max_Speed 2000
#define Trip_mm 224.4 // 平移行程：66 mm
#define Height_mm 31.25 // 高度行程：27 mm

typedef enum
{
    Command_Success = 1,
    Command_Error,
    Command_Insufficient,
    Send_False
} EMMC423V53_Command_Status;

class EMMC423V53
{
public:
    /*
        初始化
    */
    EMMC423V53_Command_Status Steeping_Init(HardwareSerial *serial, uint8_t id)
    {
        if (id < 1 || id > 255 || serial == NULL)
            return Command_Error;
        this->Steeping_Serial = serial;
        this->ID = id;
        this->Now_Location = 0;
        this->Motor_Enable_Cmd();
        return Command_Success;
    }

    EMMC423V53_Command_Status Steeping_Init(HardwareSerial *serial, uint8_t id, int rx, int tx)
    {
        if (id < 1 || id > 255 || serial == NULL)
            return Command_Error;
        this->Steeping_Serial = serial;
        this->ID = id;
        this->Now_Location = 0;
        this->Steeping_Serial->begin(115200,SERIAL_8N1,rx,tx);
        this->Motor_Enable_Cmd();
        return Command_Success;
    }
    /*
        电机使能
    */
    EMMC423V53_Command_Status Motor_Enable_Cmd(void)
    {
        uint8_t Send_Data[6] = {this->ID, 0xF3, 0xAB, 0x01, 0x00, 0x6B};

        this->Serial_Send(Send_Data, 6);
        // this->Serial_Receive(this->Buffer, 4, 0xF3);

        return this->Check_Status();
    }

    /*
        电机失能
    */
    EMMC423V53_Command_Status Motor_Disable_Cmd(void)
    {
        uint8_t Send_Data[6] = {this->ID, 0xF3, 0xAB, 0x00, 0x00, 0x6B};

        this->Serial_Send(Send_Data, 6);
        this->Serial_Receive(this->Buffer, 4, 0xF3);

        return this->Check_Status();
    }

    /*
        速度控制模式
    */
    EMMC423V53_Command_Status Speed_Mode_Cmd(uint8_t direction, uint16_t speed, uint8_t acceleration, bool dual_machine = true)
    {
        uint8_t machine_flag = dual_machine ? 0x01 : 0x00;
        speed = speed > Max_Speed ? Max_Speed : speed;
        uint8_t speed1 = speed % 256;
        uint8_t speed2 = speed / 256;
        uint8_t Send_Data[8] = {this->ID, 0xF6, direction, speed2, speed1, acceleration, machine_flag, 0x6B};

        this->Serial_Send(Send_Data, 8);
        this->Serial_Receive(this->Buffer, 4, 0xF6);

        return this->Check_Status();
    }

    /*
        位置控制模式，默认相对模式
    */
    EMMC423V53_Command_Status Location_Mode_Cmd(uint8_t direction,uint32_t speed, uint32_t location, uint8_t acceleration = 0x00, float trip_mm = Trip_mm, bool loaction_mode = false, bool dual_machine = true)
    {
        uint32_t pulse = location * Once_Circle_Pulse / trip_mm;
        uint8_t loaction_flag = loaction_mode ? 0x01 : 0x00;
        uint8_t machine_flag = dual_machine ? 0x01 : 0x00;
        uint8_t speed1 = speed % 256;
        uint8_t speed2 = speed / 256;
        uint8_t pulse4 = pulse / 256 / 256 / 256;
        uint8_t pulse3 = pulse / 256  / 256 % 256;
        uint8_t pulse2 = pulse / 256 % 256;
        uint8_t pulse1 = pulse % 256;
        uint8_t Send_Data[13] = {this->ID, 0xFD, direction, speed2, speed1, acceleration, pulse4, pulse3, pulse2, pulse1, loaction_flag, machine_flag, 0x6B};

        this->Serial_Send(Send_Data, 13);
        // this->Serial_Receive(this->Buffer, 4, 0xFD);

        return this->Check_Status();
    }

    /*
        立即停止
    */
    EMMC423V53_Command_Status Stop_Instance(void)
    {
        uint8_t Send_Data[5] = {this->ID, 0xFE, 0x98, 0x00, 0x6B};

        this->Serial_Send(Send_Data, 5);
        // this->Serial_Receive(this->Buffer, 4, 0xFE);

        return this->Check_Status();
    }

    EMMC423V53_Command_Status Dual_Machine_Enable(void)
    {
        uint8_t Send_Data[4] = {0x00, 0xFF, 0x66, 0x6B};

        this->Serial_Send(Send_Data, 4);
        // this->Serial_Receive(this->Buffer, 4, 0xFF);

        return this->Check_Status();
    }

    /*
        将当前位置设置为零点
    */
    EMMC423V53_Command_Status Set_Once_Zero_Location(bool store = true)
    {
        uint8_t store_flag = store ? 0x01 : 0x00;
        uint8_t Send_Data[5] = {this->ID, 0x93, 0x88, store_flag, 0x6B};

        this->Serial_Send(Send_Data, 5);
        // this->Serial_Receive(this->Buffer, 4, 0x93);

        return this->Check_Status();
    }

    /*
        回到设置的零点位置，建议zero_mode使用多圈无限位回零模式，即0x02
    */
    EMMC423V53_Command_Status Back_To_Zero_Location(uint8_t zero_mode = 0x02, bool dual_machine = false)
    {
        uint8_t machine_flag = dual_machine ? 0x01 : 0x00;
        uint8_t Send_Data[5] = {this->ID, 0x9A, zero_mode, machine_flag, 0x6B};

        this->Serial_Send(Send_Data, 5);
        // this->Serial_Receive(this->Buffer, 4, 0x9A);

        return this->Check_Status();
    }
    /*
        读取当前电流
    */
    int Read_Instance_Current(void)
    {
        uint8_t Send_Data[3] = {this->ID, 0x27, 0x6B};
        
        this->Serial_Send(Send_Data, 3);
        this->Serial_Receive(this->Buffer, 5, 0x27);
        return (this->Buffer[2] * 256 + this->Buffer[3]);
    }

    /*
        将位置，角度，脉冲清零
    */
    EMMC423V53_Command_Status Clear_All(void)
    {
        uint8_t Send_Data[4] = {this->ID, 0x0A, 0x6D, 0x6B};

        this->Serial_Send(Send_Data, 4);
        // this->Serial_Receive(this->Buffer, 4, 0x0A);

        return this->Check_Status();
    }

    /*
        读取电机的实时位置
    */
    EMMC423V53_Command_Status Read_Instance_Location(void)
    {
        uint8_t Send_Data[3] = {this->ID, 0x36, 0x6B};

        this->Serial_Send(Send_Data, 3);
        this->Serial_Receive(this->Buffer, 8, 0x36);

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
        修改电机的回零的参数
    */
    EMMC423V53_Command_Status Change_Zero_Parameter(uint16_t speed, uint8_t zero_mode = 0x02, uint8_t direction = 0x00, uint8_t store = 0x01)
    {
        uint8_t speed2 = speed / 16;
        uint8_t speed1 = speed % 16;
        uint8_t Send_Data[20] = {this->ID, 0x4C, 0xAE, 0x01, 0x02, 0x00, speed2, speed1, 0x00, 0x00, 0x27, 0x10, 0x01, 0x2C, 0x03, 0x20, 0x00, 0x3C, 0x00, 0x6B};

        this->Serial_Send(Send_Data, 20);
        // this->Serial_Receive(this->Buffer, 4, 0x4C);

        return this->Check_Status();
    }

    /*
        修改电机的ID
    */
    EMMC423V53_Command_Status Change_ID(uint8_t id, bool store = true)
    {
        uint8_t store_flag = store ? 0x01 : 0x00;
        uint8_t Send_Data[6] = {this->ID, 0xAE, 0x4B, store_flag, id, 0x6B};

        this->Serial_Send(Send_Data, 6);
        // this->Serial_Receive(this->Buffer, 4, 0xAE);

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
        读取电机的参数
    */
    EMMC423V53_Command_Status Read_Motor_Info(void)
    {
        uint8_t Send_Data[3] = {this->ID, 0x3A, 0x6B};
        this->Serial_Send(Send_Data, 3);
        this->Serial_Receive(this->Buffer, 4, 0x3A);

        return this->Check_Status();
    }

    EMMC423V53_Command_Status Reload_Current(void)
    {
        uint8_t Send_Data[4] = {this->ID, 0x0E, 0x52, 0x6B};

        this->Serial_Send(Send_Data, 4);
        // this->Serial_Receive(this->Buffer, 4, 0x0E);
        return this->Check_Status();
    }

    /*
        显示命令返回
    */
    void Show_Data(int length)
    {
        for (int i = 0; i < length; i++)
        {
            Serial.print(this->Buffer[i]);
            Serial.print(" ");
            this->Buffer[i] = 0;
        }
        Serial.println("");
    }

    // private:
    HardwareSerial *Steeping_Serial;
    uint8_t ID;
    uint8_t Buffer[100];
    float Now_Location;
    int Target_Length;

    /*
        向电机发送命令
    */
    void Serial_Send(uint8_t *data, int length, bool need_delay = true)
    {
        if (need_delay)
            delay(10);
        for (int i = 0; i < length; i++)
        {
            this->Steeping_Serial->write(data[i]);
        }
         if (need_delay)
            delay(10);
    }

    /*
        接收电机的命令回传
    */
    void Serial_Receive(uint8_t *Buffer, int length, uint8_t instruct, bool need_delay = true)
    {
        if (need_delay)
            delay(10);
        for (int i = 0; i < Max_Wait_Time; i++)
        {
            if (this->Steeping_Serial->available())
            {
                this->Buffer[0] = this->Steeping_Serial->read();
                if(this->Buffer[0] == this->ID)
                    this->Buffer[1] = this->Steeping_Serial->read();
                if(this->Buffer[1] == instruct)
                {               
                    for (int j = 2; j < length; j++)
                        this->Buffer[j] = this->Steeping_Serial->read();
                    // Serial.println("Success");
                    return ;
                }
                else
                {
                    this->Buffer[0] = 0;
                    this->Buffer[1] = 0;
                }

            }
            delay(1);
        }
        // Serial.println("Error");
    }

    /*
        检验电机接收命令状态
    */
    EMMC423V53_Command_Status Check_Status(void)
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


#endif