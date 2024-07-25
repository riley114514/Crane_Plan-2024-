#ifndef __COMMUNITY_H
#define __COMMUNITY_H


#include <Arduino.h>

#define Max_Wait_Time 120000

typedef int community_Command_Status;



#define    community_Command_Success 1
#define    community_command_error 2 
#define    community_Command_Insufficient 3
#define    community_command_part_error 4
#define    community_Send_False 6

class Community
{
public:
community_Command_Status Communcation_Init(HardwareSerial *serial2,int rx,int tx)/*串口初始化*/
    {
    if(serial2 == NULL)
        return community_command_error;
        this->CommunitySerial = serial2;
        this->CommunitySerial->begin(115200,SERIAL_8N1,rx,tx);
    return community_Command_Success;
    }



community_Command_Status NANO_Send()//向NANO发送开始检测的信号
    {
      this->buffer[0]=CommunitySerial->read();
      if(this->buffer[0]!=0x77)//while 
      {
        uint8_t send_data[4]={0x77,0x78,0x1D,0x5B};
        this->serial_Send(send_data,4);
        delay(100);
        this->buffer[0]=this->CommunitySerial->read();
        if(this->buffer[0]==0x77)
        {
          return buffer[0];
        }
      }
    }

/*
@brief 当nano传输完所有点的坐标,发送0x2D让nano停止检测
*/

community_Command_Status NANO_Identification_Status_1()
    {
      this->buffer[0]=this->CommunitySerial->read();
      if(this->buffer[0]!=0x77)
      {
        uint8_t send_data[4]={0x77,0x78,0x2D,0x5B};
        this->serial_Send(send_data,4);
        delay(5);
        if(buffer[0]==0x77)
        {
          return buffer[0];
        }
      }
    }



/*
 @brief 接收NANO的信号,停止向NANO发送数据，同时让NANO也停止发送数据
 @param 
*/

// community_Command_Status NANO_Recieve()
//     {
//       while(this->buffer[0]!=0x77)
//       {
//         this->buffer[0]=this->CommunitySerial->read();
//       }
//       for (int i = 1; i < 4; i++)
//       {
//         this->buffer[i]=this->CommunitySerial->read();
//         delay(1);
//       } 
//           if(this->buffer[2]==0x1B)//验证第四帧，是否为终止发送向NANO发送信号
//           { 
//             int led_pin=21;
//             pinMode(21,OUTPUT);
//             digitalWrite(21,HIGH);
//             delay(3000);
//             digitalWrite(21,LOW);
//             return community_Command_Success;
//           } else{
//             this->buffer[0]=0;
//             this->buffer[1]=0;
//           }
//     }


/*
*@brief 砝码识别，通过识别到固定点位上的砝码，传入到buffer[4]->buffer[9]中
 @param 
*/
community_Command_Status ScaleIdentification()
{
  this->buffer[0]=this->CommunitySerial->read();
  // if(this->buffer[0]==0x77)
  // {
  //   this->buffer[1]=this->CommunitySerial->read();
  //   delay(1);
  //   if(this->buffer[1]==0x87)
  //   {
  //     this->buffer[2]=this->CommunitySerial->read();
  //     delay(1);
  //     if(this->buffer[2]==0x8F)
  //     {
  //       for(int i=3;i<9;i++)
  //       {
  //         buffer[i]=this->CommunitySerial->read();
  //         delay(1);
  //       }
  //       for (int  j = 0; j < 6; j++)
  //       {
  //         location_buffer[j]=buffer[j+2];//将nano传输到buffer[3]-buffer[8]中代表坐标的各个点位转移到新数组中
  //       }
  //       this->serial_Send(location_buffer,5);
  //     }
  //   }
  // }

  if (this->buffer[0]==0x77)
  {
    for (int  i = 1; i < 10; i++)
    {
      this->buffer[i]=this->CommunitySerial->read();
    }
    for (int j = 0; j < 6; j++)
    {
      location_buffer[j]=buffer[j+2];
    }
    this->serial_Send(location_buffer,6);
  }
  
  //陈云要的可视化史山测试代码
  if (location_buffer[0]==0x11)
  {
    int led_pin=18;
    pinMode(led_pin,OUTPUT);
    digitalWrite(led_pin,HIGH);
    delay(1000);
    digitalWrite(led_pin,LOW);

    if (location_buffer[1]==0x21)
    {
      int led_pin=19;
      pinMode(led_pin,OUTPUT);
      digitalWrite(led_pin,HIGH);
      delay(1000);
      digitalWrite(led_pin,LOW);

      if (location_buffer[2]==0x31)
      {
        int led_pin=21;
        pinMode(led_pin,OUTPUT);
        digitalWrite(led_pin,HIGH);
        delay(1000);
        digitalWrite(led_pin,LOW);

        if (location_buffer[3]==0x41)
        {
          int led_pin=34;
          pinMode(led_pin,OUTPUT);
          digitalWrite(led_pin,HIGH);
          delay(1000);
          digitalWrite(led_pin,LOW);

          if (location_buffer[4]==0x51)
          {
            int led_pin=35;
            pinMode(led_pin,OUTPUT);
            digitalWrite(led_pin,HIGH);
            delay(1000);
            digitalWrite(led_pin,LOW);

            if (location_buffer[5]==0x61)
            {
              int led_pin=32;
              pinMode(led_pin,OUTPUT);
              digitalWrite(led_pin,HIGH);
              delay(1000);
              digitalWrite(led_pin,LOW);
            }
          }
        }
      }
    }
  }
}

uint8_t bubbleSort(uint8_t location_buffer[5])
{
int i=0, j=0;
    uint8_t temp=0x00;
    for (i = 0; i < 4; i++) 
    {
        for (j = 0; j < 4-i; j++) 
        {
            if (location_buffer[j] > location_buffer[j+1])
             {
                temp = location_buffer[j];
                location_buffer[j] = location_buffer[j+1];
                location_buffer[j+1] = temp;
            }
        }
    }

}

// private:
HardwareSerial *CommunitySerial;
HardwareSerial *serial;
HardwareSerial *serial2;
uint8_t ID;//每个esp32芯片的ID号，便于区分
uint8_t buffer[100];
int Rx_pin;
int Tx_pin;
int char1;
uint8_t location_buffer[6]; //用于存储6个砝码坐标的数组

/*
    向NANO发送命令
*/

void serial_Send(uint8_t *data, int length, bool need_delay = true)
  {
  for (int i = 0; i < length; i++)
  {
    this->CommunitySerial->write(data[i]);
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
        {
            delay(10);
        }
        for (int i = 0; i < Max_Wait_Time; i++)
        {
          if (this->CommunitySerial->available())
            {
              for(int j = 1; j < length; j++)
              {
                  this->buffer[j] = this->CommunitySerial->read();
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

#endif