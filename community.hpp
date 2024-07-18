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
        // uint8_t length;
        uint8_t send_data[2]={0x1D,0x1e};//此为第三、四。0x1D开始检测，0x1e让nano向从左往右边扫描
        // length=sizeof(send_data)/sizeof(send_data[0]);
        Serial.write(0x77);
        Serial.write(0x77);
        // Serial.write(length);
        Serial.write(send_data,2);
        Serial.write(0x5B);
        delay(500);
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
        // uint8_t length;
        uint8_t send_data[1]={0x2D};
        // length=sizeof(send_data)/sizeof(send_data[0]);
        Serial.write(0x77);
        Serial.write(0x77);
        // Serial.write(length);
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


community_Command_Status NANO_Identification_Status_2()//当横杆移动到最右端时并改变移动方向，与NANO通信该=改变识别方向
    {
      buffer[0]=serial->read();
      while(buffer[0]!=0x77)
      {
        // uint8_t length;
        uint8_t send_data[2]={0x1D,0x1f};
        // length=sizeof(send_data)/sizeof(send_data[0]);
        Serial.write(0x77);
        Serial.write(0x77);
        // Serial.write(length);
        serial_Send(send_data,2);
        Serial.write(0x5B);
        delay(5);
        buffer[0]=serial->read();
        if(Serial.available()>0&&buffer[0]==0x77)
        {
          return buffer[0];
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
          buffer[3]=serial->read();//第三帧接受后接收第四帧，判断是否为圆
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

community_Command_Status ScalesGrasping()//砝码个数识别 
    {
    buffer[0]=serial->read();
    while(buffer[0]==0x77)
      {
        buffer[1]=serial->read();
        delay(1);
        while(buffer[1]==0x77)// 接收数据长度（数据长度+4）
        {
          buffer[2]=serial->read();//第三帧接受后接收第四帧，判断是否为圆
          while(buffer[2]==0x02)
          {
            buffer[3]=serial->read();
            while(buffer[3]==0x1A)
            { 

              int led_pin_1=17;
              pinMode(led_pin_1,OUTPUT);
              digitalWrite(led_pin_1,HIGH);
              delay(3000);
              digitalWrite(17,LOW);
            } 
            while(buffer[3]==0x1B)
            { 
              int led_pin_2=5;
              pinMode(led_pin_2,OUTPUT);
              digitalWrite(led_pin_2,HIGH);
              delay(3000);
              digitalWrite(5,LOW);
            } 
            while(buffer[3]==0x1C) 
            { 
              int led_pin_3=18;
              pinMode(led_pin_3,OUTPUT);
              digitalWrite(led_pin_3,HIGH);
              delay(3000);
              digitalWrite(18,LOW);
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
帧头 0x77 0x77 第三帧 数据长度（数据长度+4）
第四帧 ：
        0x00   停止（横杆停止，机械爪运动）
        0x1B：向esp32说明已经开始检测，停止发送数据包（此时不需要检测第五帧）

处于扫描阶段时：
第五帧：砝码个数{1个：0x1a；2个：0x1b；3个：0x1c
帧尾 0x5B


esp32: 传输
帧头 0x77 0x77 第三帧 数据长度（数据长度+4）
第四帧：
	0x1D 开始检测
	0x2D 检测停止（此时不需要检测第五帧）（用于机械爪搬运砝码时，以及将砝码放置到对应地点前）

第五帧：扫描方向
        0x1e （右边）
        0x1f （左边）
初始运动方向为右边，默认扫描方向为右边，当切换方向后，扫描方向同时切换为左边，以此类推。
帧尾 0x5B；
（识别到2/3个砝码时，分别向内扫描，并向最近的砝码进行抓取）


与NANO的通讯频率为5ms/次，
1.机械横杆先向前走，两个机械爪分别置于最左端和最右端（ID=1,ID=2）,当识别到一个砝码时，只让1号机运动；当识别到两个砝码时，两个机械爪同时向内侧运动，在只扫描到第一个时停止，并进行抓取。
2.让并且让行动到位于圆圈中心置物柱的位置，然后让摄像头进行扫描（“0x1D”）只对右侧扫描，到达场地的中心后。nano向“我”发送终止符“0x00”，确定出6个砝码的大概位置，俩机械爪分别向左、右移动。首先启用砝码识别代码，当识别到砝码后，确保在机械爪的几何中心，舵机驱动机械爪向下，机械爪打开，抓取砝码，再向上移动，
3。确定距横杆最近的砝码，并对其抓取，并保持原来的运动方向，运动到置物台（此时速度可以加快）
3.1:情况一：第一次只识别到一个砝码，此时只让一号机抓取，并放置到右上角的置物台；反转横杆电机运动方向，第二次识别也只识别到一个砝码，则继续让一号机抓取，放置在左上角，则其余的砝码全交由二号机抓取。
3.2情况二：第一次只识别到一个砝码，让一号机抓取，并放置在右上角。反转电机运动方向，第二次识别到两个砝码，1.2号机都进行抓取，并放置在左端。第三次、四次都让二号机去抓取。
3.3情况二：第一次识别到两个砝码，让1.2号机都进行抓取，并放置在右端；反转电机运动方向，第二次识别到一个砝码，让一号机去抓取，然后放置在左上端；三、四次都让二号机抓取。
3.4情况四：第一次识别到两个砝码，同时让1.2号机进行抓取，并放置在最右端，反转电机方向，第二次识别到两个砝码，同时进行抓取，放置左端，第三次让一号机进行抓取


【已使用uint 8 数字：0x00，0x12，0x1a，0x1b，0x1c，0x77，0x5B，0x1D，0x2D，0x1r，0x1l，，，，，，，，，，，】

*/