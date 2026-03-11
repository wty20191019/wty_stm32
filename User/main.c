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
    
    
#include "PWM.h"                        //Base    TIM2_CH1(PA0)_CH2(PA1)
#include "Motor.h"                      //Base    PWM.h     PWMA    (PA0)(PB4)(PB5)(PA1)(PA15)(PB3)
#include "Encoder.h"                    //Base    TIM3_CH1(PA67)    TIM4_CH1(PB67)    IC_PWMI.h 与 Encoder.h  只能起用一个
#include "PID_system.h"                 //base     

#include "LightSensor.h"




//#include "key.h"                        //Base    (PB1 ) (PB11)
//#include "IC_PWMI.h"                    //Base    TIM3_CH1(PA6 )    TIM4_CH1(PB6 )    IC_PWMI.h 与 Encoder.h  只能起用一个






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

int16_t recv_Yaw;
int16_t recv_Pitch; 


// 串口接收帧解析状态变量
uint8_t     rxBuffer[64];                   // 帧解析缓冲区大小 64
uint8_t     inFrame;                        // 帧接收状态：0-未在帧内，1-正在接收帧  
uint16_t    rxIndex;                        // 帧数据索引
uint8_t     RE_tast;


int16_t Speed_A_0;
int16_t Speed_B_0;
int16_t Speed_A;
int16_t Speed_B;
int16_t average_speed;
int16_t differential_speed;


PID_t PID_average_speed;
PID_t PID_differential_speed;



//===================================================================================================
// PID参数初始化
//===================================================================================================
void PID_System_Init(void)
{

    // PID_average_speed
    PID_average_speed.Target = 0.0f;        // 目标值
    PID_average_speed.Actual = 0.0f;        // 实际值
    PID_average_speed.Out = 0.0f;           // 输出值
    PID_average_speed.Kp = 1.44f;           // 比例系数
    PID_average_speed.Ki = 0.095f;          // 积分系数
    PID_average_speed.Kd = 0.42f;           // 微分系数
    PID_average_speed.Error0 = 0.0f;        // 本次误差
    PID_average_speed.Error1 = 0.0f;        // 上次误差
    PID_average_speed.ErrorInt = 0.0f;      // 误差积分
    PID_average_speed.OutMax = 1000.0f;     // 输出最大值
    PID_average_speed.OutMin = -1000.0f;    // 输出最小值
    
    // PID_differential_speed
    PID_differential_speed.Target = 0.0f;       // 目标值
    PID_differential_speed.Actual = 0.0f;       // 实际值
    PID_differential_speed.Out = 0.0f;          // 输出值
    PID_differential_speed.Kp = 1.40f;          // 比例系数
    PID_differential_speed.Ki = 0.0f;           // 积分系数
    PID_differential_speed.Kd = 0.0f;           // 微分系数
    PID_differential_speed.Error0 = 0.0f;       // 本次误差
    PID_differential_speed.Error1 = 0.0f;       // 上次误差
    PID_differential_speed.ErrorInt = 0.0f;     // 误差积分
    PID_differential_speed.OutMax = 500.0f;     // 输出最大值
    PID_differential_speed.OutMin = -500.0f;    // 输出最小值
    
}





//===================================================================================================
// Encoder测速任务               (2x25ms)
//===================================================================================================
void Encoder_get_speed(void)
{
    static uint16_t count_Encoder_get = 0;
    
    if(count_Encoder_get < 25)
    {
        Speed_A += Encoder1_TIM3_Encoder_Get();
        Speed_B += Encoder2_TIM4_Encoder_Get();
        count_Encoder_get++;
    }
    else
    {
        Speed_A /= 25;
        Speed_B /= 25;
        
        Speed_A_0=Speed_A;
        Speed_B_0=Speed_B;
        
        average_speed = (Speed_A_0 + Speed_B_0)/2;
        differential_speed = Speed_A_0 - Speed_B_0;
        
        Speed_A = 0;
        Speed_B = 0;
        count_Encoder_get = 0;
    }
    
    Serial_Printf("[plot,%d,%d]",Speed_A_0,Speed_B_0);
    
//    Serial_Printf("[plot,%d]",Speed_A_0);
}

//===================================================================================================
// 串口接收处理任务
//===================================================================================================
void Serial_ProcessRxData(void)
{
    static uint8_t rxBuffer[128];
    static uint16_t rxIndex = 0;
    static uint8_t inFrame = 0;
    
    // 循环处理所有接收到的数据
    while(Serial_Available() > 0)
    {
        uint8_t ucRxData = Serial_ReceiveByte();
        
        // 检查是否为帧起始标记 '['
        if(ucRxData == '[')
        {
            inFrame = 1;
            rxIndex = 0;
            continue;
        }
        // 检查是否为帧结束标记 ']'
        else if (ucRxData == ']')
        {
            if (inFrame && rxIndex > 0)
            {
                rxBuffer[rxIndex] = '\0';
                
                // 打印接收到的完整帧内容
                Serial_Printf("[%s]\r\n", rxBuffer);
                
                // 解析帧内容
                char *Tag = strtok((char *)rxBuffer, ",");
                
                if (strcmp(Tag, "key") == 0)
                {
                    char *Name = strtok(NULL, ",");
                    char *Action = strtok(NULL, ",");
                    
                    if (strcmp(Name, "1") == 0 && strcmp(Action, "up") == 0)
                    {
                        Serial_Printf("key,1,up\r\n");
                        RE_tast++;
                        Serial_Printf("%d\r\n", RE_tast);
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
                        float FloatValue_1 = atoi(Value);
                        Serial_Printf("slider,1,%d\r\n", FloatValue_1);
                        PID_differential_speed.Target = FloatValue_1;
                    }
                    else if (strcmp(Name, "2") == 0)
                    {
                        float FloatValue_2 = atof(Value);
                        Serial_Printf("slider,2,%f\r\n", FloatValue_2);
                        PID_differential_speed.Kp = FloatValue_2;
                    }
                    else if (strcmp(Name, "3") == 0)
                    {
                        float FloatValue_3 = atof(Value);
                        Serial_Printf("slider,3,%f\r\n", FloatValue_3);
                        PID_differential_speed.Ki = FloatValue_3;
                    }
                    else if (strcmp(Name, "4") == 0)
                    {
                        float FloatValue_4 = atof(Value);
                        Serial_Printf("slider,4,%f\r\n", FloatValue_4);
                        PID_differential_speed.Kd = FloatValue_4;
                    }
                }
                else if (strcmp(Tag, "joystick") == 0)
                {
                    int16_t LH = atoi(strtok(NULL, ","));
                    int16_t LV = atoi(strtok(NULL, ","));
                    int16_t RH = atoi(strtok(NULL, ","));
                    int16_t RV = atoi(strtok(NULL, ","));
                    
                    
//                    Motor_Set_TIM2_ch1_PWMA( LV-RH);
//                    Motor_Set_TIM2_ch2_PWMB( LV+RH);
                    PID_average_speed.Target      = LV ;
                    PID_differential_speed.Target = RH ;

                    
                    
                    
                    Serial_Printf("joystick,%d,%d,%d,%d\r\n", LH, LV, RH, RV);
                }
                else if (strcmp(Tag, "Pose") == 0)
                {
                    recv_Yaw= atoi(strtok(NULL, ","));
                    recv_Pitch = atoi(strtok(NULL, ","));
                }
                
                inFrame = 0;
                rxIndex = 0;
            }
            else
            {
                inFrame = 0;
                rxIndex = 0;
            }
        }
        // 如果正在接收帧且不是帧结束标记，则存储数据
        else if (inFrame && rxIndex < (sizeof(rxBuffer) - 1))
        {
            rxBuffer[rxIndex++] = ucRxData;
        }
    }
}


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
                            
//Serial_Printf("[plot,%f,%f,%f]",recv_pose_mpu6050.Pitch,recv_pose_mpu6050.Roll,recv_pose_mpu6050.Yaw ); 

    


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
    OLED_ShowString(6*16, 8*3, "Pitch", OLED_6X8);
    OLED_ShowFloatNum(6*15, 8*4, recv_pose_mpu6050.Pitch, 2, 2, OLED_6X8);
    OLED_ShowString(6*0, 8*3, "Roll", OLED_6X8);
    OLED_ShowFloatNum(6*0, 8*4, recv_pose_mpu6050.Roll, 2, 2, OLED_6X8);
    OLED_ShowString(6*7 , 8*7, "Yaw", OLED_6X8);
    OLED_ShowFloatNum(6*11, 8*7, recv_pose_mpu6050.Yaw, 2, 2, OLED_6X8);
    
    
    
    OLED_ShowNum(6*0, 8*0   , recv_Yaw  ,3  ,OLED_6X8);
    OLED_ShowNum(6*0, 8*1   ,recv_Pitch ,3  ,OLED_6X8);
    
    
//------------------------------------
    OLED_Update();
}



//===================================================================================================
// APP 任务                     (50ms)
//===================================================================================================
void APP (void)
{

    PID_average_speed.Actual = (float)average_speed;

    PID_differential_speed.Actual = (float)differential_speed;

    
    PID_Update(&PID_average_speed);
    PID_Update(&PID_differential_speed);
    
    Motor_Set_TIM2_ch1_PWMA(PID_average_speed.Out - PID_differential_speed.Out);  //                PID_average_speed.Out
    Motor_Set_TIM2_ch2_PWMB(PID_average_speed.Out + PID_differential_speed.Out);  //       PID_average_speed.Out
    
    
//    Serial_Printf(  "[plot,%f]", LightSensor_GetPositionCentered()  );

//    Serial_Printf("[plot,%f,%f,%f]",
//                    recv_pose_mpu6050.Pitch,
//                    recv_pose_mpu6050.Roll,
//                    recv_pose_mpu6050.Yaw);

//    Serial_Printf("[plot,%f,%d]"
//                ,PID_average_speed.Target
//                ,average_speed);






}





int main(void)
{
// 设置NVIC优先级分组================================================================================
NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

//===================================================================================================
// 模块初始化
//===================================================================================================

    DWT_Delay_Init();
    SCH_Init();
    I2C_QuickInit(I2C2, 400*1000);
    OLED_Init();                    OLED_ShowNum(0, 3, 1, 2, OLED_8X16);OLED_Update();
    PC13_LED_Init();                OLED_ShowNum(0, 3, 2, 2, OLED_8X16);OLED_Update();
    MPU6050_Init();                 OLED_ShowNum(0, 3, 3, 2, OLED_8X16);OLED_Update();
    MPU6050_DMP_Init();             OLED_ShowNum(0, 3, 4, 2, OLED_8X16);OLED_Update();
    Serial_Init();                  OLED_ShowNum(0, 3, 5, 2, OLED_8X16);OLED_Update();
    
    TIM2_PWM_Init();                OLED_ShowNum(0, 3, 6, 2, OLED_8X16);OLED_Update();
    Motor_Init();                   OLED_ShowNum(0, 3, 7, 2, OLED_8X16);OLED_Update();
    Encoder1_TIM3_Init();           OLED_ShowNum(0, 3, 8, 2, OLED_8X16);OLED_Update();
    Encoder2_TIM4_Init();           OLED_ShowNum(0, 3, 9, 2, OLED_8X16);OLED_Update();
    
    PID_System_Init();              OLED_ShowNum(0, 3,10, 2, OLED_8X16);OLED_Update();  
    

//===================================================================================================
//systick 调度器                                (0->1->2)
//=============|函数====================|周期===|优先级=|============================================
    
    SCH_AddTask(Serial_ProcessRxData    ,10     ,8      );
    SCH_AddTask(Test_PC13_LED           ,20     ,10     );
    SCH_AddTask(MPU6050_PoseTask        ,20     ,7      );
    SCH_AddTask(OLED_DisplayTask        ,20     ,8      );
    SCH_AddTask(Encoder_get_speed       ,2      ,8      );
    SCH_AddTask(APP                     ,50     ,9      );

//===================================================================================================

    while(1)
    {
        //===========================
        SCH_Dispatch();// 启动调度器
        //__WFI();
        //===========================
        
    }
}







