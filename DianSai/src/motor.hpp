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
#define Max_Speed 300

enum
{
    Back_To_Check = 0,
    Start_To_Scaning,
    Start_To_Painting,
    Get_Location,
    Stop_Till,
    Test
} State_Status;

enum
{
    Circle = 1,
    Square,
    Triangle
} Graphical_Status;

typedef int State_Command_Status;
typedef int Graphical_Command_Status;

class Motor;
void Task_Get_Location(void *prmotor);
void Task_State_Choice(void *prstate);

class State
{
public:
    void State_Init(Motor *mt, Uart *ut, Oled *ol, Ultrasonic *uc1, Ultrasonic *uc2)
    {
        this->state_num = Start_To_Scaning;
        this->motor = mt;
        this->uart = ut;
        this->oled = ol;
        this->graph = 0;
        this->ultrasonic1 = uc1;
        this->ultrasonic2 = uc2;
    }

    void State_Choice(void)
    {
        if (State_handle == NULL)
        {
            xTaskCreatePinnedToCore(Task_State_Choice, "Task_State_Choice", 1024 * 100, this, 6, &State_handle, 1);
        }
    }
    // private:
    State_Command_Status state_num;
    Motor *motor;
    Uart *uart;
    Graphical_Command_Status graph;
    int Center_Location_Y;
    int Center_Location_Z;
    int Target_Location_Y[300];
    int Target_Location_Z[300];
    int Radius;
    int Length;
    Oled *oled;
    Ultrasonic *ultrasonic1;
    Ultrasonic *ultrasonic2;
};

class Motor
{
public:
    void Motor_Init(Steeping42 *motor2, Steeping42 *motor1, HardwareSerial *slmotor)
    {
        this->motor_2 = motor2;
        this->motor_2->Steeping_Init(slmotor, 2);
        this->motor_1 = motor1;
        this->motor_1->Steeping_Init(slmotor, 1);
        this->Motor_Back_To_Max();
        this->motor_1->Clear_All();
        this->motor_2->Clear_All();
        this->z_max_mm = 592;
        this->y_max_mm = 614;
        this->z_mm = 592;
        this->y_mm = 614;
        this->Motor_Get_Location();
        delay(10);
        // this->Motor_Set_Location(this->y_max_mm / 2, 180);
        this->Motor_Set_Location(313, 281);

    }
    void Motor_Back_To_Max(void)
    {
        this->motor_2->Speed_Mode_Cmd(1, 60, 0, true);
        this->motor_1->Speed_Mode_Cmd(1, 60, 0, true);
        this->motor_1->Dual_Machine_Enable();
        while (this->motor_1->Read_Instance_Current() < 800)
        {
            delay(5);
        };
        this->motor_2->Motor_Disable_Cmd();
        this->motor_1->Motor_Disable_Cmd();
        delay(1000);
        this->motor_2->Motor_Enable_Cmd();
        this->motor_1->Motor_Enable_Cmd();
        this->motor_2->Speed_Mode_Cmd(1, 60, 0, true);
        this->motor_1->Speed_Mode_Cmd(0, 60, 0, true);
        this->motor_1->Dual_Machine_Enable();
        while (this->motor_2->Read_Instance_Current() < 800)
        {
            delay(5);
        };
        this->motor_2->Motor_Disable_Cmd();
        this->motor_1->Motor_Disable_Cmd();
        delay(1000);
        this->motor_2->Motor_Enable_Cmd();
        this->motor_1->Motor_Enable_Cmd();
    }
    void Motor_Get_Location(void)
    {
        xTaskCreatePinnedToCore(Task_Get_Location, "Task_Get_Location", 1024, this, 5, NULL, 1);
    }
    void Motor_Set_Location(int target_y, int target_z, bool need_delay = true)
    {
        if (need_delay)
            delay(10);
        int diff_y = target_y - this->y_mm;
        int diff_z = target_z - this->z_mm;
        delay(10);
        if (diff_z >= 0)
        {
            this->motor_1->Location_Mode_Cmd(0, Max_Speed, abs(diff_z));
            this->motor_2->Location_Mode_Cmd(1, Max_Speed, abs(diff_z));
        }
        else
        {
            this->motor_1->Location_Mode_Cmd(1, Max_Speed, abs(diff_z));
            this->motor_2->Location_Mode_Cmd(0, Max_Speed, abs(diff_z));
        }
        this->motor_1->Dual_Machine_Enable();
        while (abs(this->z_mm - target_z) > 5)
            delay(1);
        // delay((float)abs(diff_z)/trip_mm*60/Max_Speed*1000+100);
        if (diff_y >= 0)
        {
            this->motor_1->Location_Mode_Cmd(1, Max_Speed, abs(diff_y));
            this->motor_2->Location_Mode_Cmd(1, Max_Speed, abs(diff_y));
        }
        else
        {
            this->motor_1->Location_Mode_Cmd(0, Max_Speed, abs(diff_y));
            this->motor_2->Location_Mode_Cmd(0, Max_Speed, abs(diff_y));
        }
        this->motor_1->Dual_Machine_Enable();
        while (abs(this->y_mm - target_y) > 5)
            delay(1);
        // delay((float)abs(diff_y)/trip_mm*60/Max_Speed*1000+100);
    }
    // privated:

    Steeping42 *motor_2;
    Steeping42 *motor_1;
    int y_mm;
    int z_mm;
    int y_max_mm;
    int z_max_mm;
};

void Task_Get_Location(void *prmotor)
{
    Motor *motor = (Motor *)prmotor;
    while (1)
    {
        motor->motor_2->Read_Instance_Location();
        motor->motor_1->Read_Instance_Location();
        if (motor->motor_2->Buffer[0] == motor->motor_2->ID && motor->motor_2->Buffer[1] == 0x36)
        {
            motor->motor_2->Now_Location = (motor->motor_2->Buffer[2] ? -1 : 1) * (motor->motor_2->Buffer[3] * pow(16, 6) +
                                                                                   motor->motor_2->Buffer[4] * pow(16, 4) + motor->motor_2->Buffer[5] * pow(16, 2) + motor->motor_2->Buffer[6]);
        }
        if (motor->motor_1->Buffer[0] == motor->motor_1->ID && motor->motor_1->Buffer[1] == 0x36)
        {
            motor->motor_1->Now_Location = (motor->motor_1->Buffer[2] ? -1 : 1) * (motor->motor_1->Buffer[3] * pow(16, 6) +
                                                                                   motor->motor_1->Buffer[4] * pow(16, 4) + motor->motor_1->Buffer[5] * pow(16, 2) + motor->motor_1->Buffer[6]);
        }

        motor->z_mm = motor->z_max_mm + (-motor->motor_2->Now_Location + motor->motor_1->Now_Location) / 65536 * trip_mm / 2;
        motor->y_mm = motor->y_max_mm + (-motor->motor_2->Now_Location - motor->motor_1->Now_Location) / 65536 * trip_mm / 2;
        // Serial.print("y: ");
        // Serial.println(motor->y_mm);
        // Serial.print("z: ");
        // Serial.println(motor->z_mm);
        delay(5);
    }
}

void Task_State_Choice(void *prstate)
{
    State *state = (State *)prstate;
    while (1)
    {
        if (state->state_num == Back_To_Check)
        {
            state->motor->Motor_Set_Location(state->motor->y_max_mm / 2, 180);
            state->state_num = Stop_Till;
        }
        else if (state->state_num == Start_To_Scaning)
        {
            uint8_t send_data[5] = {0x77, 0x77, 5, 0x1D, 0x5B};
            while (1)
            {
                state->uart->Serial_Send(send_data, 5);
                delay(10);
                /* code */
            }
            
            state->uart->Serial_Send(send_data, 5);
            state->state_num = Get_Location;
        }
        else if (state->state_num == Start_To_Painting)
        {
            state->oled->Clear_All();
            if (state->graph == Circle)
            {

                for (int i = 0; i <= 60; i++)
                {
                    state->motor->Motor_Set_Location(state->Target_Location_Y[i % 60], state->Target_Location_Z[i % 60]);
                }
            }
            else if (state->graph == Square)
            {
                for (int i = 0; i <= 60; i++)
                {
                    state->motor->Motor_Set_Location(state->Target_Location_Y[i % 60], state->Target_Location_Z[i % 60]);
                }
            }
            else if (state->graph == Triangle)
            {
                for (int i = 0; i <= 45; i++)
                {
                    state->motor->Motor_Set_Location(state->Target_Location_Y[i % 45], state->Target_Location_Z[i % 45]);
                }
            }
            state->state_num = Back_To_Check;
        }
        else if (state->state_num == Get_Location)
        {
            state->uart->Buffer[0] = state->uart->Uart_Serial->read();
            while (state->uart->Buffer[0] != 0x77)
            {
                state->uart->Buffer[0] = state->uart->Uart_Serial->read();
                Serial.println("Wariting!");
            }
            state->uart->Buffer[1] = state->uart->Uart_Serial->read();
            state->uart->Buffer[2] = state->uart->Uart_Serial->read();
            for (int i = 3; i < state->uart->Buffer[2]; i++)
            {
                state->uart->Buffer[i] = state->uart->Uart_Serial->read();
            }

            /*
            测试单元
            */
            for(int i = 0 ;i < state->uart->Buffer[2]; i++)
            {
                Serial.print(state->uart->Buffer[i]);
                Serial.print(" ");
            }
            state->Center_Location_Y = state->uart->Buffer[4] * 100 + state->uart->Buffer[5];
            state->Center_Location_Z = state->uart->Buffer[6] * 100 + state->uart->Buffer[7];
            if (state->uart->Buffer[3] == 0x1F)
            {
                state->graph = Circle;
                state->Radius = state->uart->Buffer[8] * 100 + state->uart->Buffer[9];
                for (int i = 0; i < 15; i++)
                {
                    state->Target_Location_Y[i] = state->Center_Location_Y + state->Radius / 15 * (15 - i);
                    state->Target_Location_Z[i] = state->Center_Location_Z + sqrt(pow(state->Radius, 2) - pow(state->Radius / 15 * (15 - i), 2));
                    state->Target_Location_Y[i + 15] = state->Center_Location_Y + state->Radius / 15 * i;
                    state->Target_Location_Z[i + 15] = state->Center_Location_Z + sqrt(pow(state->Radius, 2) - pow(state->Radius / 15 * i, 2));
                    state->Target_Location_Y[i + 30] = state->Center_Location_Y + state->Radius / 15 * (15 - i);
                    state->Target_Location_Z[i + 30] = state->Center_Location_Z + sqrt(pow(state->Radius, 2) - pow(state->Radius / 15 * (15 - i), 2));
                    state->Target_Location_Y[i + 45] = state->Center_Location_Y + state->Radius / 15 * i;
                    state->Target_Location_Z[i + 45] = state->Center_Location_Z + sqrt(pow(state->Radius, 2) - pow(state->Radius / 15 * i, 2));
                }
            }
            else if (state->uart->Buffer[3] == 0x2F)
            {
                state->graph = Square;
                state->Length = state->uart->Buffer[8] * 100 + state->uart->Buffer[9];
                int Target_Point_Y[3];
                int Target_Point_Z[3];
                float slope;
                float duration;
                for (int i = 10; i < state->uart->Buffer[2] - 1; i += 4)
                {
                    Target_Point_Y[(i - 10) / 4] = state->uart->Buffer[i] * 100 + state->uart->Buffer[i + 1];
                    Target_Point_Z[(i - 10) / 4] = state->uart->Buffer[i + 2] * 100 + state->uart->Buffer[i + 3];
                }

                for (int i = 0; i < 3; i++)
                {
                    if ((Target_Point_Y[(i + 1) % 3] - Target_Point_Y[i % 3]) == 0)
                    {
                        duration = (float)(Target_Point_Z[(i + 1) % 3] - Target_Point_Z[i % 3]) / 15;
                        for (int j = 0; j < 15; j++)
                        {
                            state->Target_Location_Y[j + i * 15] = Target_Point_Y[(i + 1) % 3];
                            state->Target_Location_Z[j + i * 15] = Target_Point_Z[i % 3] + j * duration;
                        }
                    }
                    else
                    {
                        slope = (float)(Target_Point_Z[(i + 1) % 3] - Target_Point_Z[i % 3]) / (Target_Point_Y[(i + 1) % 3] - Target_Point_Y[i % 3]);
                        duration = (float)(Target_Point_Y[(i + 1) % 3] - Target_Point_Y[i % 3]) / 15;
                        for (int j = 0; j < 15; j++)
                        {
                            state->Target_Location_Y[j + i * 15] = Target_Point_Y[i % 3] + j * duration;
                            state->Target_Location_Z[j + i * 15] = Target_Point_Z[i % 3] + j * duration * slope;
                        }
                    }
                }
            }
            else if (state->uart->Buffer[3] == 0x3F)
            {
                state->graph = Triangle;
                state->Length = state->uart->Buffer[8] * 100 + state->uart->Buffer[9];
                int Target_Point_Y[4];
                int Target_Point_Z[4];
                float slope;
                float duration;
                for (int i = 10; i < state->uart->Buffer[2] - 1; i += 4)
                {
                    Target_Point_Y[(i - 10) / 4] = state->uart->Buffer[i] * 100 + state->uart->Buffer[i + 1];
                    Target_Point_Z[(i - 10) / 4] = state->uart->Buffer[i + 2] * 100 + state->uart->Buffer[i + 3];
                }

                for (int i = 0; i < 4; i++)
                {
                    if ((Target_Point_Y[(i + 1) % 4] - Target_Point_Y[i % 4]) == 0)
                    {
                        duration = (float)(Target_Point_Z[(i + 1) % 4] - Target_Point_Z[i % 4]) / 15;
                        for (int j = 0; j < 15; j++)
                        {
                            state->Target_Location_Y[j + i * 15] = Target_Point_Y[(i + 1) % 4];
                            state->Target_Location_Z[j + i * 15] = Target_Point_Z[i % 4] + j * duration;
                        }
                    }
                    else
                    {
                        slope = (float)(Target_Point_Z[(i + 1) % 4] - Target_Point_Z[i % 4]) / (Target_Point_Y[(i + 1) % 4] - Target_Point_Y[i % 4]);
                        duration = (float)(Target_Point_Y[(i + 1) % 4] - Target_Point_Y[i % 4]) / 15;
                        for (int j = 0; j < 15; j++)
                        {
                            state->Target_Location_Y[j + i * 15] = Target_Point_Y[i % 3] + j * duration;
                            state->Target_Location_Z[j + i * 15] = Target_Point_Z[i % 3] + j * duration * slope;
                        }
                    }
                }
            }
            state->ultrasonic1->Ultrasonic_Get();
            state->ultrasonic2->Ultrasonic_Get();
            state->oled->Show_Text(state->Center_Location_Y, state->Center_Location_Z, (state->ultrasonic1->distance + state->ultrasonic2->distance) / 2, state->graph);
        }
        else if (state->state_num == Test)
        {
            // float slope;
            // float duration;
            // int Target_Point_Y[4] = {100, 500, 500, 100};
            // int Target_Point_Z[4] = {100, 500, 100, 500};
            // for (int i = 0; i < 4; i++)
            // {
            //     if ((Target_Point_Y[(i + 1) % 4] - Target_Point_Y[i % 4]) == 0)
            //     {
            //         duration = (float)(Target_Point_Z[(i + 1) % 4] - Target_Point_Z[i % 4]) / 15;
            //         for (int j = 0; j < 15; j++)
            //         {
            //             state->Target_Location_Y[j + i * 15] = Target_Point_Y[(i + 1) % 4];
            //             state->Target_Location_Z[j + i * 15] = Target_Point_Z[i % 4] + j * duration;
            //         }
            //     }
            //     else
            //     {
            //         slope = (float)(Target_Point_Z[(i + 1) % 4] - Target_Point_Z[i % 4]) / (Target_Point_Y[(i + 1) % 4] - Target_Point_Y[i % 4]);
            //         duration = (float)(Target_Point_Y[(i + 1) % 4] - Target_Point_Y[i % 4]) / 15;
            //         for (int j = 0; j < 15; j++)
            //         {
            //             state->Target_Location_Y[j + i * 15] = Target_Point_Y[i % 4] + j * duration;
            //             state->Target_Location_Z[j + i * 15] = Target_Point_Z[i % 4] + j * duration * slope;
            //         }
            //     }
            // }
            state->Center_Location_Y = 200;
            state->Center_Location_Z = 200;
            state->Radius = 100;
            for (int i = 0; i < 15; i++)
            {
                state->Target_Location_Y[i] = state->Center_Location_Y + state->Radius / 15.0 * (15 - i);
                state->Target_Location_Z[i] = state->Center_Location_Z + sqrt(pow(state->Radius, 2) - pow(abs(state->Center_Location_Y - state->Target_Location_Y[i]),2));
                state->Target_Location_Y[i + 15] = state->Center_Location_Y - state->Radius / 15.0 * i;
                state->Target_Location_Z[i + 15] = state->Center_Location_Z + sqrt(pow(state->Radius, 2) - pow(abs(state->Center_Location_Y - state->Target_Location_Y[i + 15]),2));
                state->Target_Location_Y[i + 30] = state->Center_Location_Y - state->Radius / 15.0 * (15 - i);
                state->Target_Location_Z[i + 30] = state->Center_Location_Z - sqrt(pow(state->Radius, 2) - pow(abs(state->Center_Location_Y - state->Target_Location_Y[i + 30]),2));
                state->Target_Location_Y[i + 45] = state->Center_Location_Y + state->Radius / 15.0 * i;
                state->Target_Location_Z[i + 45] = state->Center_Location_Z - sqrt(pow(state->Radius, 2) - pow(abs(state->Center_Location_Y - state->Target_Location_Y[i + 45]),2));
            }

            // for(int i = 0;i<60;i++)
            // {
            //     Serial.print(i+1);
            //     Serial.print("Y: ");
            //     Serial.println(state->Target_Location_Y[i]);
            //     Serial.print(i+1);
            //     Serial.print("Z: ");
            //     Serial.println(state->Target_Location_Z[i]);

            // }
            // delay(100);
            // for (int i = 0; i <= 60; i++)
            // {
            //     state->motor->Motor_Set_Location(state->Target_Location_Y[i % 60], state->Target_Location_Z[i % 60]);
            // }
            state->state_num = Stop_Till;
        }

        delay(10);
    }
}
