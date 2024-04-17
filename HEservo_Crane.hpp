#ifndef __HESERVO_H
#define __HESERVO_H
#endif

#include <Arduino.h>

#define Max_Wait_Time 10
#define Header_Shoot 0x55

typedef int Servo_Command_Status;

enum 
{
    Command_Success = 1,
    Command_Error,
    Command_Insufficient,
    Send_False
}Command_Status;

class servo
{
public:

/*
    舵机初始化
*/
    void Servo_Init(HardwareSerial *serial, uint8_t id)
    {
        this->Servo_Serial = serial;
        this->ID = id;
    }

/*
    舵机在参数时间内匀速转动到指定角度
*/
    void Servo_Move_Time_Write(uint8_t cmd = 1, uint8_t length = 7, float angle, float time)
    {
        uint16_t angle_trans = (int)(angle / 240 * 1000);
        uint8_t angle1 = angle_trans % 256;
        uint8_t angle2 = angle_trans / 256;
        uint8_t time2 = (int)time;
        uint8_t time1 = time - (int)time;
        uint8_t Send_Data[length + 3] = {Header_Shoot, Header_Shoot, this->ID, length, cmd, angle2, angle1, time2, time1};
        Add_Check_Sum(Send_Data,length);

        this->Serial_Send(Send_Data,length);
    }

/*
    读取Servo_Move_Time_Write发送给舵机的角度和时间值，并接收返回的指令包
*/
    void Servo_Move_Time_Read(uint8_t cmd = 2, uint8_t length = 3)
    {
        uint8_t Send_Data[length + 3] = {Header_Shoot, Header_Shoot, this->ID, length, cmd};
        Add_Check_Sum(Send_Data,length);

        this->Serial_Send(Send_Data,length);
        this->Serial_Receive(length);
    }

/*
    需要在Servo_Move_Start指令送达后，才开始启动，并在参数时间内匀速移动到参数角度
*/
    void Servo_Move_Time_Wait_Write(uint8_t cmd = 7, uint8_t length = 7, float angle, uint16_t time_ms)
    {
        time_ms = time_ms > 30000? 30000: time_ms;
        uint16_t angle_trans = (int)(angle / 240 * 1000);
        uint8_t angle1 = angle_trans % 256;
        uint8_t angle2 = angle_trans / 256;
        uint8_t time1 = time_ms % 256;
        uint8_t time2 = time_ms / 256;
        uint8_t Send_Data[length + 3] = {Header_Shoot, Header_Shoot, this->ID, length, cmd, angle2, angle1, time2, time1};
        Add_Check_Sum(Send_Data,length);

        this->Serial_Send(Send_Data,length);
    }

/*
    读取Servo_Move_Time_Wait_Write发送给舵机的角度和时间值，并接收返回的指令包
*/
    void Servo_Move_Time_Wait_Read(uint8_t cmd = 8, uint8_t length = 3)
    {
        uint8_t Send_Data[length + 3] = {Header_Shoot, Header_Shoot, this->ID, length, cmd};
        Add_Check_Sum(Send_Data,length);

        this->Serial_Send(Send_Data,length);
        this->Serial_Receive(length);
    }

/*
    配合指令Servo_Move_Time_Wait_Write使用。启动舵机
*/
    void Servo_Move_Start(uint8_t cmd = 11, uint8_t length = 3)
    {
        uint8_t Send_Data[length + 3] = {Header_Shoot, Header_Shoot, this->ID, length, cmd};
        Add_Check_Sum(Send_Data,length);

        this->Serial_Send(Send_Data,length);
    }

/*
    舵机停止
*/
    void Servo_Move_Stop(uint8_t cmd = 12, uint8_t length = 3)
    {
        uint8_t Send_Data[length + 3] = {Header_Shoot, Header_Shoot, this->ID, length, cmd};
        Add_Check_Sum(Send_Data,length);

        this->Serial_Send(Send_Data,length);
    }

/*
    重新写入ID值
*/
    void Servo_ID_Write(uint8_t cmd = 13, uint8_t length = 4, uint8_t id)
    {
        id = id>253? 253: id;
        uint8_t Send_Data[length + 3] = {Header_Shoot, Header_Shoot, this->ID, length, cmd, id};
        Add_Check_Sum(Send_Data,length);

        this->Serial_Send(Send_Data,length);

    }

/*
    读取ID值
*/
    void Servo_ID_Read(uint8_t cmd = 14, uint8_t length = 3)
    {
        uint8_t Send_Data[length + 3] = {Header_Shoot, Header_Shoot, this->ID, length, cmd};
        Add_Check_Sum(Send_Data,length);

        this->Serial_Send(Send_Data,length);
    }

/*
    设置舵机内部的偏差值，需要配合Servo_Angle_Offset_Write指令使用
*/
    void Servo_Angle_Offset_Adjust(uint8_t cmd = 17, uint8_t length = 4, float angle)
    {
        angle = angle > 30? 30: angle;
        angle = angle < -30? -30: angle;
        uint8_t angle_trans = (uint8_t)((angle + 30) * 250);
        uint8_t Send_Data[length + 3] = {Header_Shoot, Header_Shoot, this->ID, length, cmd, angle_trans};
        Add_Check_Sum(Send_Data,length);

        this->Serial_Send(Send_Data,length);
    }

/*
    保存舵机内部的偏差值，需要先写入Servo_Angle_Offset_Adjust指令再执行该命令
*/
    void Servo_Angle_Offset_Write(uint8_t cmd = 18, uint8_t length = 3)
    {
        uint8_t Send_Data[length + 3] = {Header_Shoot, Header_Shoot, this->ID, length, cmd};
        Add_Check_Sum(Send_Data,length);

        this->Serial_Send(Send_Data,length);
    }

/*
    读取舵机设定的偏差值
*/
    void Servo_Angle_Offset_Read(uint8_t cmd = 19, uint8_t length = 3)
    {
        uint8_t Send_Data[length + 3] = {Header_Shoot, Header_Shoot, this->ID, length, cmd};
        Add_Check_Sum(Send_Data,length);

        this->Serial_Send(Send_Data,length);
        this->Serial_Receive(length);
    }

/*
    设置舵机的最大和最小角度
*/
    void Servo_Angle_Limit_Write(uint8_t cmd = 20, uint8_t length = 7, float min_angle, float max_angle)
    {
        uint16_t min_angle_trans = (int)(min_angle / 240 * 1000);
        uint8_t min_angle1 = min_angle_trans % 256;
        uint8_t min_angle2 = min_angle_trans / 256;
        uint16_t max_angle_trans = (int)(max_angle / 240 * 1000);
        uint8_t max_angle1 = max_angle_trans % 256;
        uint8_t max_angle2 = max_angle_trans / 256;

        uint8_t Send_Data[length + 3] = {Header_Shoot, Header_Shoot, this->ID, length, cmd, min_angle1, min_angle2, max_angle1, max_angle2};
        Add_Check_Sum(Send_Data,length);

        this->Serial_Send(Send_Data,length);
    }

/*
    读取舵机角度的限制值
*/
    void Servo_Angle_Limit_Read(uint8_t cmd = 21, uint8_t length = 3)
    {
        uint8_t Send_Data[length + 3] = {Header_Shoot, Header_Shoot, this->ID, length, cmd};
        Add_Check_Sum(Send_Data,length);

        this->Serial_Send(Send_Data,length);
        this->Serial_Receive(length);
    }

/*
    设置电压的最小和最大电压，如果超出范围，LED会闪烁报警（如果设置了LED报警），为了保护电机，将会处于卸载断电状态
*/
    void Servo_Vin_Limit_Write(uint8_t cmd = 24, uint8_t length = 4, uint16_t min_voltage, uint16_t max_voltage)
    {
        min_voltage = min_voltage > 12000? 12000: min_voltage;
        min_voltage = min_voltage < 4500? 4500: min_voltage;
        max_voltage = max_voltage > 12000? 12000: max_voltage;
        max_voltage = max_voltage < 4500? 4500: max_voltage;
        uint8_t min_voltage1 = min_voltage % 256;
        uint8_t min_voltage2 = min_voltage / 256;
        uint8_t max_voltage1 = max_voltage % 256;
        uint8_t max_voltage2 = max_voltage / 256;

        uint8_t Send_Data[length + 3] = {Header_Shoot, Header_Shoot, this->ID, length, cmd, min_voltage1, min_voltage2, max_voltage1, max_voltage2};
        Add_Check_Sum(Send_Data,length);

        this->Serial_Send(Send_Data,length);
    }

/*
    读取舵机输入电压的限制值
*/
    void Servo_Vin_Limit_Read(uint8_t cmd = 25, uint8_t length = 3)
    {
        uint8_t Send_Data[length + 3] = {Header_Shoot, Header_Shoot, this->ID, length, cmd};
        Add_Check_Sum(Send_Data,length);

        this->Serial_Send(Send_Data,length);
        this->Serial_Receive(length);
    }

/*
    舵机内部最高温度限制，范围50-100度，默认值85度
*/
    void Servo_Temp_Max_Limit_Write(uint8_t cmd = 26, uint8_t length = 4, uint8_t temp)
    {
        temp = temp > 100? 100: temp;
        temp = temp < 50? 50: temp;
        uint8_t Send_Data[length + 3] = {Header_Shoot, Header_Shoot, this->ID, length, cmd, temp};
        Add_Check_Sum(Send_Data,length);

        this->Serial_Send(Send_Data,length);
    }

/*
    读取舵机内部最高温度限制
*/
    void Servo_Temp_Max_Limit_Read(uint8_t cmd = 26, uint8_t length = 3)
    {
        uint8_t Send_Data[length + 3] = {Header_Shoot, Header_Shoot, this->ID, length, cmd};
        Add_Check_Sum(Send_Data,length);

        this->Serial_Send(Send_Data,length);
        this->Serial_Receive(length);
    }

/*
    读取舵机内部实时温度
*/
    void Servo_Temp_Read(uint8_t cmd = 26, uint8_t length = 3)
    {
        uint8_t Send_Data[length + 3] = {Header_Shoot, Header_Shoot, this->ID, length, cmd};
        Add_Check_Sum(Send_Data,length);

        this->Serial_Send(Send_Data,length);
        this->Serial_Receive(length);
    }

/*
    读取舵机当前的输入电压值
*/
    void Servo_Vin_Read(uint8_t cmd = 27, uint8_t length = 3)
    {
        uint8_t Send_Data[length + 3] = {Header_Shoot, Header_Shoot, this->ID, length, cmd};
        Add_Check_Sum(Send_Data,length);

        this->Serial_Send(Send_Data,length);
        this->Serial_Receive(length);
    }

/*
    读取舵机当前的实际角度位置值
*/
    void Servo_Pos_Read(uint8_t cmd = 28, uint8_t length = 3)
    {
        uint8_t Send_Data[length + 3] = {Header_Shoot, Header_Shoot, this->ID, length, cmd};
        Add_Check_Sum(Send_Data,length);

        this->Serial_Send(Send_Data,length);
        this->Serial_Receive(length);
    }

/*
    舵机或电机模式选择。并写入转动速度值,mode = 1为电机，0为舵机,默认为舵机模式
*/

    void Servo_Or_Motor_Mode_Write(uint8_t cmd = 29, uint8_t length = 7, uint8_t mode = 0,  short int speed)
    {
        uint16_t speed_trans = (uint16_t)speed;
        uint8_t speed1 = speed_trans % 256;
        uint8_t speed2 = speed_trans / 256;
        uint8_t Send_Data[length + 3] = {Header_Shoot, Header_Shoot, this->ID, length, cmd, mode, 0x00, speed1, speed2};
        Add_Check_Sum(Send_Data,length);

        this->Serial_Send(Send_Data,length);
    }

/*
    读取舵机模式的相关参数
*/
    void Servo_Or_Motor_Mode_Read(uint8_t cmd = 30, uint8_t length = 3)
    {
        uint8_t Send_Data[length + 3] = {Header_Shoot, Header_Shoot, this->ID, length, cmd};
        Add_Check_Sum(Send_Data,length);

        this->Serial_Send(Send_Data,length);
        this->Serial_Receive(length);
    }

/*
    舵机内部是否卸载掉电，0代表卸载掉电，无力矩输出，1代表装载电机，有力矩输出，默认值为0
*/
    void Servo_Load_Or_Unload_Write(uint8_t cmd = 31, uint8_t length = 4, uint8_t mode = 0)
    {
        uint8_t Send_Data[length + 3] = {Header_Shoot, Header_Shoot, this->ID, length, cmd, mode};
        Add_Check_Sum(Send_Data,length);

        this->Serial_Send(Send_Data,length);
    }

/*
    读取舵机内部电机的状态
*/
    void Servo_Load_Or_Unload_Read(uint8_t cmd = 32, uint8_t length = 3)
    {
        uint8_t Send_Data[length + 3] = {Header_Shoot, Header_Shoot, this->ID, length, cmd};
        Add_Check_Sum(Send_Data,length);

        this->Serial_Send(Send_Data,length);
        this->Serial_Receive(length);
    }

/*
    LED灯的亮灭状态，0代表LED常亮， 1代表LED常灭，支持掉电保存,默认常灭
*/
    void Servo_LED_Ctrl_Write(uint8_t cmd = 33, uint8_t length = 4, uint8_t mode = 1)
    {
        uint8_t Send_Data[length + 3] = {Header_Shoot, Header_Shoot, this->ID, length, cmd, mode};
        Add_Check_Sum(Send_Data,length);

        this->Serial_Send(Send_Data,length);
    }

/*
    读取LED灯的状态
*/
    void Servo_LED_Ctrl_Read(uint8_t cmd = 34, uint8_t length = 3)
    {
        uint8_t Send_Data[length + 3] = {Header_Shoot, Header_Shoot, this->ID, length, cmd};
        Add_Check_Sum(Send_Data,length);

        this->Serial_Send(Send_Data,length);
        this->Serial_Receive(length);

    }

/*
    舵机故障闪烁报警，默认为7,过温，过压，堵转都会发生闪烁报警，不管LED常亮还是常灭
*/
    void Servo_LED_Error_Write(uint8_t cmd = 35, uint8_t length = 4, uint8_t mode = 7)
    {
        uint8_t Send_Data[length + 3] = {Header_Shoot, Header_Shoot, this->ID, length, cmd, mode};
        Add_Check_Sum(Send_Data,length);

        this->Serial_Send(Send_Data,length);
    }

/*
    读取舵机故障报警值
*/
    void Servo_LED_Error_Read(uint8_t cmd = 35, uint8_t length = 3)
    {
        uint8_t Send_Data[length + 3] = {Header_Shoot, Header_Shoot, this->ID, length, cmd};
        Add_Check_Sum(Send_Data,length);

        this->Serial_Send(Send_Data,length);
        this->Serial_Receive(length);
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
    HardwareSerial *Servo_Serial;
    uint8_t ID;
    uint8_t Buffer[100];

/*
    向电机发送命令
*/
    void Serial_Send(uint8_t *data, int length, bool need_delay = true)
    {
        for (int i = 0; i < length; i++)
        {
            this->Servo_Serial->write(data[i]);
        }
        if(need_delay)
            delay(1);
    }

/*
    接收电机的命令回传
*/
    void Serial_Receive(int length, bool need_delay = true)
    {
        if(need_delay)
            delay(1);
        for (int i = 0; i < Max_Wait_Time; i++)
        {
            if (this->Servo_Serial->available())
            {
                this->Buffer[0] = this->Servo_Serial->read();
                this->Buffer[1] = this->Servo_Serial->read();

                if (this->Buffer[0] == Header_Shoot && this->Buffer[1] == Header_Shoot)
                {
                    for(int j = 2; j < length + 3; j++)
                        this->Buffer[j] = this->Servo_Serial->read();
                }
            }
            delay(1);
        }
    }

/*
    检验舵机接收命令状态
*/
    Servo_Command_Status Check_Status(void)
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

/*
    计算Check_Sum的值
*/
    void Add_Check_Sum(uint8_t *data, int length)
    {
        uint8_t Check_Sum = 0;
        for(int i = 0; i < length + 2; i++)
        {
            Check_Sum += data[i];
        }
        Check_Sum = ~Check_Sum;
        data[length + 2] = Check_Sum;
    }
};

