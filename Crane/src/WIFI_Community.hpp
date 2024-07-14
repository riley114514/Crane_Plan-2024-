
#ifndef __WIFI_Community_H
#define __WIFI_Community_H
#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>
#include <map>
extern int ID;

uint8_t receive_MACAddress[] ={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};//接收ESP32mac地址


//接收状态定义
enum package_type {
    package_type_normal = 0,
    package_type_error = 1,
    package_type_request = 2,
    package_type_response = 3
};


esp_now_peer_info_t peerInfo;//peerInfo 必须是全局变量否则espnow不可用

//ESP-NOW初始化
void esp_now_setup() 
{
  //设置WiFi模式为WIFI_AP_STA（AP+Station模式）
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
      Serial.println("ESP-NOW initialization failed");
      return;
  }

  peerInfo.ifidx = WIFI_IF_STA;
  memcpy(peerInfo.peer_addr, receive_MACAddress, 6);

  if (esp_now_add_peer(&peerInfo) != ESP_OK){
      Serial.println("Failed to add peer");
      return;
  }
  esp_now_register_recv_cb(OnDataRecv);
} 
//数据包格式
struct data_package {
  uint16_t header=0xFE;//固定包头
  uint8_t id=0; //包ID标识 0为广播包
  uint8_t packge_type; //包类型
  uint8_t name_len; //包名称长度
  uint8_t data_len; //包参数长度
  char name[30] ;//包名称所含数据
  uint8_t data[50] ;//包参数所含数据
  uint8_t checksum=0;//校验码

  bool check()//检查数据包的校验和是否正确
  {
    uint8_t sum=header+packge_type+name_len+data_len+id;
    for(int i=0;i<name_len;i++){
        sum+=name[i];
    }
    for(int i=0;i<data_len;i++){
        sum+=data[i];
    }
    return sum==checksum;
  }

  uint8_t add_checksum()//计算并添加数据包的校验和
  {
    uint8_t sum=header+packge_type+name_len+data_len+id;
    for(int i=0;i<name_len;i++){
        sum+=name[i];
    }
    for(int i=0;i<data_len;i++){
        sum+=data[i];
    }
    checksum=sum;
    return sum;
  }

  bool add_package(const uint8_t *data,int len)//从字节数组中解析出数据包,并将其存入结构体
  {
    if(len<2||(data[0]!=0xFE&&data[1]!=0xFE)) 
    return false;

    data_package re_data;//创建一个名为re_data的结构体来计算
    
    re_data.id=data[2];
    re_data.packge_type=data[3];
    re_data.name_len=data[4];
    re_data.data_len=data[5];

    for(int i=0;i<re_data.name_len;i++)
    {
      re_data.name[i]=data[6+i];
    }

    for(int i=0;i<re_data.data_len;i++)
    {
      re_data.data[i]=data[6+re_data.name_len+i];
    }

    re_data.checksum=data[len-1];

    if(!re_data.check()) 
    {
      return false;
    }
  
    //存入数据
    memcpy(this,&re_data,sizeof(re_data));
    return true;
  }

  bool add_package(uint8_t* data,int len)
  {
    const uint8_t* d=(const uint8_t*)data;
    add_package(d,len);
  }

  int get_len()//获取数据包的总长度
  {
    return 6+name_len+data_len+1;
  }

  void get_data(uint8_t* arr)//将数据包结构体转换为字节数组,以便于发送
  {
    arr[0]=0xFE;
    arr[1]=0xFE;
    arr[2]=id;
    arr[3]=packge_type;
    arr[4]=name_len;
    arr[5]=data_len;

    for(int i=0;i<name_len;i++)
    {
        arr[6+i]=name[i];
    }

    if(data_len!=0)
    {
     for(int i=0;i<data_len;i++)
      {
       arr[6+name_len+i]=data[i];
      }
    }
    add_checksum();
    arr[6+name_len+data_len]=checksum;
  }
};


void esp_now_send_package(package_type type,int _id,String name,uint8_t* data,int datalen,uint8_t* receive_MAC);

using DataPackageCallback = void (*)(data_package);// 定义一个函数指针类型，它接受 data_package 作为参数


std::map<String, DataPackageCallback> callback_map;// 创建一个回调函数映射表

data_package re_data;//创建结构体，以用于接受数据

void forward(data_package re_data)//横杆向前移动
{

}

void back_away(data_package re_data)//横杆后移
{

}

void cross_bar_endstop(data_package re_data)//横杆归位
{

}

void gripper_1_move(data_package re_data)//一号爪子移动
{

}

void gripper_1_endstop(data_package re_data)//一号爪子归位
{

}

void gripper_2_move(data_package re_data)//二号爪子移动
{

}

void gripper_2_endstop(data_package re_data)//二号爪子归位
{

}

void gripper_work_together(data_package re_data)//双爪同时移动
{

}
void grippers_endstop(data_package re_data)//双爪归位
{

}




void add_callback()
{
callback_map["cross_forward"]=forward;//横杆前进
callback_map["cross_back"]=back_away;//横杆后退
callback_map["cross_endstop"]=cross_bar_endstop;//横杆归零
callback_map["first_gripper_move"]=gripper_1_move;
callback_map["first_gripper_stop"]=gripper_1_endstop;
callback_map["second_gripper_move"]=gripper_2_move;
callback_map["second_gripper_stop"]=gripper_2_endstop;
callback_map["grippers_move"]=gripper_work_together;
callback_map["grippers_stop"]=grippers_endstop;
}



void package_response(void* p)//处理数据时的回调函数
{
  String name = String(re_data.name,re_data.name_len); 

  //如果有对应的回调函数，则执行
  if(callback_map.find(name) != callback_map.end())
  {
    callback_map[name](re_data);
  };
  vTaskDelete(NULL);//应该是操作系统的，看不懂，就一起抄过来了
}
  


//接收数据时的回调函数，收到数据时自动运行
void OnDataRecv(const uint8_t *mac, const uint8_t *data, int len) 
{
  if(!re_data.add_package(data,len)) return ;
  if(re_data.id!=ID) return;
  // //如果有对应的回调函数，则执行
  xTaskCreate(package_response, "package_response_task", 4096,&re_data, 1, NULL);//应该是操作系统的，看不懂，就一起抄过来了
}

//通过espnow发送数据
void esp_now_send_package(package_type type,int _id,String name,uint8_t* data,int datalen,uint8_t* receive_MAC=receive_MACAddress)
{
  data_package send_data;
  send_data.packge_type=type;
  send_data.id=_id;
  send_data.name_len=name.length();
  send_data.data_len=datalen;

  for(int i=0;i<name.length();i++)
  {
    send_data.name[i]=name[i];
  }
  for(int i=0;i<datalen;i++)
  {
    send_data.data[i]=data[i];
  }

  uint8_t send_data_array[send_data.name_len+send_data.data_len+7];
  //将结构体中的数据转到数组中
  send_data.get_data(send_data_array);
  //发送
  esp_err_t err = esp_now_send(receive_MAC,send_data_array,send_data.get_len());
}


#endif