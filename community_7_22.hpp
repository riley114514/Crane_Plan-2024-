#ifndef __COMMUNITY_H
#define __COMMUNITY_H
#endif

#include <Arduino.h>

#define Max_Wait_Time 120000

typedef int community_Command_Status;

enum 
{
    community_Command_Success = 1,
    community_command_error,
    community_Command_Insufficient,
    community_command_part_error,
    community_Send_False,
}Command_Status_1;

class community
{
public:
community_Command_Status Communcation_Init(HardwareSerial *serial, uint8_t id)/*串口初始化*/
    {
    if(serial == NULL)
        return community_command_error;
        this->serial = serial;
        this->ID = id;
        Serial.begin(115200);
    return community_Command_Success;
    }

community_Command_Status NANO_Send()//向NANO发送开始检测的信号(此为默认检测方向)
    {
      buffer[0]=serial->read();
      while(buffer[0]!=0x77)
      {
        uint8_t send_data[1]={0x1D};//此为第三、四。0x1D开始检测，0x1e让nano向从左往右边扫描
        Serial.write(0x77);
        Serial.write(0x77);
        Serial.write(send_data,1);
        Serial.write(0x5B);
        delay(100);
        buffer[0]=serial->read();
        while(buffer[0]==0x77)
        {
          return buffer[0];
          break;
        }
      }
    }

community_Command_Status NANO_Identification_Status_1()//	0x2D 检测停止（此时不需要检测第五帧）（用于机械爪搬运砝码时，以及将砝码放置到对应地点前）
    {
      buffer[0]=serial->read();
      while(buffer[0]!=0x77)
      {
        uint8_t send_data[1]={0x2D};
        Serial.write(0x77);
        Serial.write(0x77);
        serial_Send(send_data,1);
        Serial.write(0x5B);
        delay(5);
        buffer[0]=serial->read();
        while(Serial.available()>0&&buffer[0]==0x77)
        {
          return buffer[0];
          break;
        }
      }
    }


community_Command_Status NANO_Recieve()//接收NANO的信号,停止向NANO发送数据，同时让NANO也停止发送数据
    {
      while(buffer[0]==0x77)
      {
        buffer[1]=serial->read();
        delay(1);

        while(buffer[1]==0x77)// 接收数据长度（数据长度+4）
        {
          for (int i = 2; i < buffer[2]; i++) 
          {
            buffer[i] = Serial.read();
          }
          buffer[3]=serial->read();//第三帧接受后接收第四帧
          while(buffer[3]==0x1B)//验证第四帧，是否为终止发送向NANO发送信号
          { 
            // int led_pin=12;
            // pinMode(led_pin,OUTPUT);
            // digitalWrite(led_pin,HIGH);
            // delay(3000);
            // digitalWrite(12,LOW);
            // break;

          } 
          
        }
      }
    }

/*
*@brief 砝码识别，通过识别到固定点位上的砝码，传入到buffer[4]->buffer[9]中

*/
community_Command_Status ScaleIdentification()
{
buffer[0]=serial->read();
while (buffer[0]==0x77)
{
  buffer[1]=serial->read();
  delay(1);
  while (buffer[1]==0x77)
  {
    buffer[2]=serial->read();
    delay(1);
    while(buffer[2]==0x8F)
    {
      for(int i=3;i<9;i++)
      {
        buffer[i]=serial->read();
        delay(1);
      }
    }
  }
}



}
private:
HardwareSerial *serial;
HardwareSerial *serial2;
uint8_t ID;//每个esp32芯片的ID号，便于区分
uint8_t buffer[100];
int Rx_pin;
int Tx_pin;
int char1;


/*
    向NANO发送命令
*/

void serial_Send(uint8_t *data, int length, bool need_delay = true)
  {
  for (int i = 0; i < length; i++)
  {
    this->serial->write(data[i]);
  }
  if(need_delay)
   delay(10);
  }

/*
    从NANO接收命令
*/

community_Command_Status serial_Receive(uint8_t *buffer, int length, bool need_delay = true)
    {
        if(need_delay)
            delay(10);
        for (int i = 0; i < Max_Wait_Time; i++)
        {
            if (this->serial->available())
            {
                this->buffer[0] = this->serial->read();
                this->buffer[1] = this->serial->read();
                if (this->buffer[0] == this->ID)
                {
                    for(int j = 1; j < length; j++)
                        this->buffer[j] = this->serial->read();
                }
                return community_Command_Success;
            }
            delay(1);
        }
        return community_command_error;
    }
};



/*
通讯协议说明：

nano: 传输
帧头 0x77 0x77 
第三帧 ：
        0x1B：向esp32说明已经开始检测，停止发送数据包（此时不需要检测第五帧）

​        0x8F：这个帧之后开始接收各个砝码的位置

帧尾 0x5B

检测阶段：（同一条线上）

esp32: 传输
帧头 0x77 0x77

第三帧：
	0x1D 开始检测
	0x2D 检测停止（此时不需要检测第五帧）（用于机械爪搬运砝码时，以及将砝码放置到对应地点前）

帧尾 0x5B；

与NANO的通讯频率为10ms/次，

### 运动逻辑

1.先由nano进行扫描（77，77，1D，5B），确定5个砝码的位置，nano向esp32发送（2D），两个机械爪分别置于最右端和最左端（ID=1,ID=2）,在同一X轴上，当识别到一个砝码时，只让1号机运动；当识别到两个砝码时，两个机械爪分别抓取离自己最近的砝码；当扫到3个砝码时，两边的砝码由1，2抓取，最中心的砝码由1号爪抓取；
2.确定距横杆最近的砝码，并对其抓取，并保持原来的运动方向，运动到置物台（此时速度可以加快）

*/