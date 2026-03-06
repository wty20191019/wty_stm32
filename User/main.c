//===================================================================================================
//#include
//===================================================================================================

#include "stm32f10x.h"                  //Device header
#include "systick_scheduler.h"          //Base    systick
#include "DWT_Delay.h"                  //Base    DWT
#include "math.h"
#include "string.h"
#include "stdlib.h" 

#include "PC13_LED.h"                   //Base    PC13

#include "Serial.h"                     //Base    hardware_Serial   USART   TTL

#include "hardware_I2C.h"               //Base    hardware_I2Cx        (I2C1_I2C2)
    
    #include "OLED.h"                       //Base    (PB11) (PB10)   I2C2.h         OLED_2          //--no-multibyte-chars

    //====mpu6050dmp====//                  
    #include "mpu6050.h"                    //Base    (PB11) (PB10)   I2C2.h    inv_mpu.h
    #include "inv_mpu.h"                    //Base                    I2C2.h    inv_mpu.h    mpu6050.h



//#include "hardware_AD.h"                //Base    hardware_AD
//#include "PID_system.h"                 //base     
//#include "key.h"                        //Base    (PB1 ) (PB11)
//#include "PWM.h"                        //Base    TIM2_CH1(PA0)_CH2(PA1)_CH3(PA2)_CH4(PA3)
//#include "IC_PWMI.h"                    //Base    TIM3_CH1(PA6 )    TIM4_CH1(PB6 )    IC_PWMI.h 与 Encoder.h  只能起用一个
//#include "Encoder.h"                    //Base    TIM3_CH1(PA67)    TIM4_CH1(PB67)    IC_PWMI.h 与 Encoder.h  只能起用一个
//#include "MyDMA.h"                      //Base    hardware_DMA
//#include "adc_dma.h"                    //Base    hardware_AD    hardware_DMA






//===================================================================================================
// 变量定义
//===================================================================================================

#define PI 3.141592653589793

typedef struct
{
    float Pitch;
    float Roll;
    float Yaw;
} pose_of_Pitch_Roll_Yaw;

pose_of_Pitch_Roll_Yaw recv_pose_mpu6050;

int8_t recv_Yaw;
int8_t recv_Pitch; 

// 串口接收帧解析状态变量
#define     RX_BUFFER_SIZE              64  // 帧解析缓冲区大小
uint8_t     rxBuffer[RX_BUFFER_SIZE];       
uint8_t     inFrame;                        // 帧接收状态：0-未在帧内，1-正在接收帧
uint16_t    rxIndex;                        // 帧数据索引
uint8_t RE_tast;  



//===================================================================================================
// 串口接收帧解析任务
//===================================================================================================

void Serial_ParseFrame(void)
{
        if (Serial_RxFlag == 1)
        {
            
            char *Tag = strtok(Serial_RxPacket, ",");
            if (strcmp(Tag, "key") == 0)
            {
                char *Name = strtok(NULL, ",");
                char *Action = strtok(NULL, ",");
                
                if (strcmp(Name, "1") == 0 && strcmp(Action, "up") == 0)
                {
                    Serial_Printf("key,1,up\r\n");
                }
                else if (strcmp(Name, "2") == 0 && strcmp(Action, "down") == 0)
                {
                    Serial_Printf("key,2,down\r\n");
                }
            }
            else if (strcmp(Tag, "slider") == 0)
            {
                char *Name = strtok(NULL, ",");
                char *Value = strtok(NULL, ",");
                
                if (strcmp(Name, "1") == 0)
                {
                    uint8_t IntValue = atoi(Value);
                    
                    Serial_Printf("slider,1,%d\r\n", IntValue);
                }
                else if (strcmp(Name, "2") == 0)
                {
                    float FloatValue = atof(Value);
                    
                    Serial_Printf("slider,2,%f\r\n", FloatValue);
                }
            }
            else if (strcmp(Tag, "joystick") == 0)
            {
                int8_t LH = atoi(strtok(NULL, ","));
                int8_t LV = atoi(strtok(NULL, ","));
                int8_t RH = atoi(strtok(NULL, ","));
                int8_t RV = atoi(strtok(NULL, ","));
                
                Serial_Printf("joystick,%d,%d,%d,%d\r\n", LH, LV, RH, RV);
            }
            
            Serial_RxFlag = 0;
        }
}












//                else if (strcmp(Tag, "Pose") == 0)
//                {
//                    char *Yaw_str = strtok(NULL, ",");
//                    char *Pitch_str = strtok(NULL, ",");
//                    
//                    if (Yaw_str != NULL && Pitch_str != NULL)
//                    {
//                        recv_Yaw = atoi(Yaw_str);
//                        recv_Pitch = atoi(Pitch_str);
//                        
//                        Serial_Printf("Pose: Yaw=%d, Pitch=%d\r\n", recv_Yaw, recv_Pitch);
//                    }
//                }



//===================================================================================================
// PC13 LED任务
//===================================================================================================
void Test_PC13_LED(void)
{
PC13_LED_Turn();
}

//===================================================================================================
// MPU6050任务
//===================================================================================================
void MPU6050_PoseTask(void)
{
    MPU6050_DMP_Get_Data(   &recv_pose_mpu6050.Pitch,
                            &recv_pose_mpu6050.Roll,
                            &recv_pose_mpu6050.Yaw);
}

//===================================================================================================
// OLED显示任务
//===================================================================================================
void OLED_DisplayTask(void)
{
    OLED_Clear(); 
//------------------------------------
    // OLED显示数字滚动
    static uint16_t i = 0;
    OLED_ShowNum(i*8, 0, i, 1, OLED_8X16);
    i++;
    i = (i > 9 ? 0 : i);

    // OLED显示mpu6050数据
    int_fast8_t y_p1, y_p2, Yaw_p;
    y_p1 = 32 + 32 * (recv_pose_mpu6050.Pitch / 90.0);
    y_p2 = 63 * tan(recv_pose_mpu6050.Roll * PI / 180.0);
    Yaw_p = 63 + 63 * (recv_pose_mpu6050.Yaw / 180.0); 

    OLED_ShowString(6*16, 8*3, "Pitch", OLED_6X8);
    OLED_ShowFloatNum(6*15, 8*4, recv_pose_mpu6050.Pitch, 2, 2, OLED_6X8);
    OLED_ShowString(6*0, 8*3, "Roll", OLED_6X8);
    OLED_ShowFloatNum(6*0, 8*4, recv_pose_mpu6050.Roll, 2, 2, OLED_6X8);
    OLED_ShowString(6*7 + (Yaw_p-51), 8*6, "Yaw", OLED_6X8);
    OLED_ShowFloatNum(6*11 + (Yaw_p-87), 8*7, recv_pose_mpu6050.Yaw, 2, 2, OLED_6X8);

    OLED_DrawLine(0, 31, 127, 31);
    OLED_DrawLine(63, 0, 63, 63);
    OLED_DrawLine(0, y_p1-y_p2, 127, y_p1+y_p2);
    OLED_DrawLine(Yaw_p, 0, Yaw_p, 63);
    
//------------------------------------
    OLED_Update();
}


int main(void)
{
// 设置NVIC优先级分组
NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

//===================================================================================================
// 模块初始化
//===================================================================================================

    DWT_Delay_Init();
    SCH_Init();
    I2C_QuickInit(I2C2, 400*1000);
    OLED_Init();                    OLED_ShowNum(0, 1, 1, 1, OLED_8X16);OLED_Update();
    PC13_LED_Init();                OLED_ShowNum(0, 2, 3, 1, OLED_8X16);OLED_Update();
    MPU6050_Init();                 OLED_ShowNum(0, 2, 3, 1, OLED_8X16);OLED_Update();
    MPU6050_DMP_Init();             OLED_ShowNum(0, 3, 4, 1, OLED_8X16);OLED_Update();

//    Serial_Init();          OLED_ShowNum(0, 3, 5, 1, OLED_8X16);OLED_Update();




//    Encoder2_TIM4_Init();               OLED_ShowNum(0,3,5,1,OLED_8X16);OLED_Update();
//    TIM2_PWM_Init();
//    TIM34_IC_PWMI_Init();
//    Encoder1_TIM3_Init();
//    Encoder2_TIM4_Init();
//    AD_DMA_Init();  


//===================================================================================================
//systick 调度器
//===================================================================================================


    //SCH_AddTask(Serial_ParseFrame   ,1    ,8      );
    SCH_AddTask(Test_PC13_LED       ,20     ,9      );
    SCH_AddTask(MPU6050_PoseTask    ,10     ,7      );
    SCH_AddTask(OLED_DisplayTask    ,50     ,8      );
    

// 启动调度器========================================================================================
SCH_Start();
//===================================================================================================

    while(1)
    {
        DWT_Delay_us(1);
        Serial_ParseFrame();
    }
}







