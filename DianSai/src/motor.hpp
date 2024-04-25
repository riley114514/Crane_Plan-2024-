#ifndef __MOTOR_H
#define __MOTOR_H
#endif

#include <Arduino.h>
#include "steeping42.hpp"
#include "uart.hpp"
#include "oled.hpp"
#include "ultrasonic.hpp"


TaskHandle_t Location_handle = NULL;
TaskHandle_t State_handle = NULL;

#define PI 3.14159265359
#define trip_mm 40
#define Max_Speed 1500

enum
{
    Back_To_Zero = 0,
    Start_To_Scaning,
    Start_To_Painting,
    Trans_To_Oled,
    Get_Location
}State_Status;

enum
{
    Circle = 1,
    Square,
    Triangle
}Graphical_Status;

typedef int State_Command_Status;
typedef int Graphical_Command_Status;

class State
{
    public:
        void State_Init(Motor *mt, Uart *ut,Oled *ol,Ultrasonic* uc1,Ultrasonic* uc2)
        {
            this->state_num = 0;
            this->motor = mt;
            this->uart = ut;
            this->oled = ol;
            this->graph = 0;
            this->ultrasonic1 = uc1;
            this->ultrasonic2 = uc2;

        }

        void State_Choice(void)
        {
            if(State_handle == NULL)
            {
                xTaskCreatePinnedToCore(Task_State_Choice, "Task_State_Choice", 1024*100,this,6,&State_handle,1);
            }
        }
    // private:
        State_Command_Status state_num;
        Motor *motor;
        Uart *uart;
        Graphical_Command_Status graph;
        float Center_Location_Y;
        float Center_Location_Z;
        float Target_Location_Y[100];
        float Target_Location_Z[100];
        float Radius;
        Oled *oled;
        Ultrasonic *ultrasonic1;
        Ultrasonic *ultrasonic2;

};

class Motor
{
public:
    void Motor_Init(Steeping42 *motory, Steeping42 *motorz, HardwareSerial *slmotor)
    {
        this->motor_y = motory;
        this->motor_y->Steeping_Init(slmotor,1);
        this->motor_z = motorz;
        this->motor_z->Steeping_Init(slmotor,2);
        this->motor_y->Dual_Machine_Enable();

    }
    void Motor_Get_Location(void)
    {
        xTaskCreatePinnedToCore(Task_Get_Location, "Task_Get_Location",1024,this,5,NULL,1);
    }
    void Motor_Set_Location(float target_y, float target_z)
    {
        float diff_y = target_y - this->motor_y->Now_Location ;
        float diff_z = target_z - this->motor_z->Now_Location ;
        if(abs(diff_y) > abs(diff_z) )
        {
            this->motor_y->Location_Mode_Cmd(Max_Speed,diff_y);
            this->motor_z->Location_Mode_Cmd(Max_Speed*(abs(diff_z)/abs(diff_y)),diff_z);
        }
        else
        {
            this->motor_z->Location_Mode_Cmd(Max_Speed,diff_z);
            this->motor_y->Location_Mode_Cmd(Max_Speed*(abs(diff_y)/abs(diff_z)),diff_y);
        }
    }
//privated:

    Steeping42 *motor_y;
    Steeping42 *motor_z; 
    
};

void Task_Get_Location(void *prmotor)
{
    Motor * motor = (Motor *) prmotor;
    while(1)
    {
        motor->motor_y->Read_Instance_Location();
        motor->motor_z->Read_Instance_Location();
        motor->motor_y->Now_Location = (motor->motor_y->Buffer[2]? -1: 1) * (motor->motor_y->Buffer[3]*pow(16,3) +
        motor->motor_y->Buffer[4]*pow(16,2) + motor->motor_y->Buffer[5]*pow(16,1) + motor->motor_y->Buffer[6]);
        motor->motor_z->Now_Location = (motor->motor_z->Buffer[2]? -1: 1) * (motor->motor_z->Buffer[3]*pow(16,3) +
        motor->motor_z->Buffer[4]*pow(16,2) + motor->motor_z->Buffer[5]*pow(16,1) + motor->motor_z->Buffer[6]);
        motor->motor_y->Now_Location = motor->motor_y->Now_Location/65536 * trip_mm;
        motor->motor_z->Now_Location = motor->motor_z->Now_Location/65536 * trip_mm;
        delay(10);
    }
}


void Task_State_Choice(void *prstate)
{
    State *state = (State *)prstate;
    while(1)
    {
        switch (state->state_num)
        {
        case Back_To_Zero:
            state->motor->motor_y->Back_To_Zero_Location(0x02,true);
            state->motor->motor_z->Back_To_Zero_Location(0x02,true);
            state->motor->motor_y->Dual_Machine_Enable();
            break;

        case Start_To_Scaning:
            uint8_t send_data[5] = {0x77,0x77,5,0x1D,0x5B};
            state->uart->Serial_Send(send_data,5);
            state->state_num = Get_Location;
            break;

        case Get_Location:
            state->uart->Buffer[0] = state->uart->Uart_Serial->read();
            state->uart->Buffer[1] = state->uart->Uart_Serial->read();
            while(state->uart->Buffer[0] != 0x77 || state->uart->Buffer[1] != 0x77)
            {
                state->uart->Buffer[0] = state->uart->Uart_Serial->read();
                state->uart->Buffer[1] = state->uart->Uart_Serial->read();
            }
            state->uart->Buffer[2] = state->uart->Uart_Serial->read();
            state->uart->Buffer[3] = state->uart->Uart_Serial->read();
            if (state->uart->Buffer[3] == 0x1F)
            {
                state->graph = Circle;          
                state->Center_Location_Y = state->uart->Buffer[4]*100+state->uart->Buffer[5]+state->uart->Buffer[6]/100.0;
                state->Center_Location_Z = state->uart->Buffer[7]*100+state->uart->Buffer[8]+state->uart->Buffer[9]/100.0;
                state->Radius =  state->uart->Buffer[10]*100+state->uart->Buffer[11]+state->uart->Buffer[12]/100.0;
                for(int i = 0; i < 15; i++)
                {
                    state->Target_Location_Y[i] = state->Center_Location_Y + state->Radius / 15 *(15-i);
                    state->Target_Location_Z[i] = state->Center_Location_Z + sqrt(pow(state->Radius , 2) - pow(state->Radius / 15 *(15-i) , 2));
                    state->Target_Location_Y[i+15] = state->Center_Location_Y + state->Radius / 15 *i;
                    state->Target_Location_Z[i+15] = state->Center_Location_Z + sqrt(pow(state->Radius , 2) - pow(state->Radius / 15 *i , 2));
                    state->Target_Location_Y[i+30] = state->Center_Location_Y + state->Radius / 15 *(15-i);
                    state->Target_Location_Z[i+30] = state->Center_Location_Z + sqrt(pow(state->Radius , 2) - pow(state->Radius / 15 *(15-i) , 2)); 
                    state->Target_Location_Y[i+45] = state->Center_Location_Y + state->Radius / 15 *i;   
                    state->Target_Location_Z[i+15] = state->Center_Location_Z + sqrt(pow(state->Radius , 2) - pow(state->Radius / 15 *i , 2));
                }
            }
            else if(state->uart->Buffer[3] == 0x2F)
            {
                state->graph = Square;
                state->Center_Location_Y = state->uart->Buffer[4]*100+state->uart->Buffer[5]+state->uart->Buffer[6]/100.0;
                state->Center_Location_Z = state->uart->Buffer[7]*100+state->uart->Buffer[8]+state->uart->Buffer[9]/100.0;              
                for(int i = 10; i < state->uart->Buffer[2] - 1; i += 6)
                {
                    state->Target_Location_Y[(i-10)/6] = state->uart->Buffer[i]*100+state->uart->Buffer[i+1]+state->uart->Buffer[i+2]/100.0;
                    state->Target_Location_Z[(i-10)/6] = state->uart->Buffer[i+3]*100+state->uart->Buffer[i+4]+state->uart->Buffer[i+5]/100.0;
                }
            }
            else if(state->uart->Buffer[3] == 0x3F)
            {
                state->graph = Triangle;
                state->Center_Location_Y = state->uart->Buffer[4]*100+state->uart->Buffer[5]+state->uart->Buffer[6]/100.0;
                state->Center_Location_Z = state->uart->Buffer[7]*100+state->uart->Buffer[8]+state->uart->Buffer[9]/100.0;              
                for(int i = 10; i < state->uart->Buffer[2] - 1; i += 6)
                {
                    state->Target_Location_Y[(i-10)/6] = state->uart->Buffer[i]*100+state->uart->Buffer[i+1]+state->uart->Buffer[i+2]/100.0;
                    state->Target_Location_Z[(i-10)/6] = state->uart->Buffer[i+3]*100+state->uart->Buffer[i+4]+state->uart->Buffer[i+5]/100.0;
                }
            }
            state->ultrasonic1->Ultrasonic_Get();
            state->ultrasonic2->Ultrasonic_Get();
            state->oled->Show_Text(state->Center_Location_Y,state->Center_Location_Z,(state->ultrasonic1->distance+state->ultrasonic2->distance)/2,state->graph);
            break;
        
        case Start_To_Painting:
            state->oled->Clear_All();
            state->motor->motor_y->Location_Mode_Cmd(1500,state->Center_Location_Y - state->motor->motor_y->Now_Location);
            state->motor->motor_z->Location_Mode_Cmd(1500,state->Center_Location_Z - state->motor->motor_y->Now_Location);
            while(state->motor->motor_y->Now_Location != state->Center_Location_Y && state->motor->motor_z->Now_Location != state->Center_Location_Z);
            if(state->graph == Circle)
            {
                
                for(int i = 0;i<60; i++)
                {
                    state->motor->Motor_Set_Location(state->Target_Location_Y[i],state->Target_Location_Z[i]);
                    while(state->motor->motor_y->Now_Location != state->Target_Location_Y[i] && state->motor->motor_z->Now_Location != state->Target_Location_Z[i]);
                }
            }
            else if (state->graph == Square)
            {
                for(int i = 0;i<4; i++)
                {
                    state->motor->Motor_Set_Location(state->Target_Location_Y[i],state->Target_Location_Z[i]);
                    while(state->motor->motor_y->Now_Location != state->Target_Location_Y[i] && state->motor->motor_z->Now_Location != state->Target_Location_Z[i]);
                }
            }
            else if (state->graph == Triangle)
            {
                for(int i = 0;i<3; i++)
                {
                    state->motor->Motor_Set_Location(state->Target_Location_Y[i],state->Target_Location_Z[i]);
                    while(state->motor->motor_y->Now_Location != state->Target_Location_Y[i] && state->motor->motor_z->Now_Location != state->Target_Location_Z[i]);
                }
            }
            state->state_num = Back_To_Zero;
            break;

        default:
            break;
        }
        delay(10);
    }

}
