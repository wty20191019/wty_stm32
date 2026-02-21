//===================================================================================================
//#include
//===================================================================================================


#include "stm32f10x.h"                  //Device header
#include "systick_scheduler.h"          //Base    systick
#include "DWT_Delay.h"                  //Base    DWT
#include "math.h" 

#include "FreeRTOS.h"                   //Base    FreeRTOS
#include "task.h"                       //Base    FreeRTOS
#include "queue.h"                      //Base    FreeRTOS


#include "hardware_I2C.h"                   //Base    hardware_I2Cx        (I2C1_I2C2)

    #include "OLED.h"                       //Base    (PB11) (PB10)   I2C2.h         OLED_2           //--no-multibyte-chars

    //====mpu6050dmp====//                  
    #include "mpu6050.h"                    //Base    (PB11) (PB10)   I2C2.h    inv_mpu.h
    #include "inv_mpu.h"                    //Base                    I2C2.h    inv_mpu.h    mpu6050.h



//#include "PID_system.h"                 //base     
//#include "key.h"                        //Base    (PB1 ) (PB11)
//#include "LED.h"                        //Base    (PA12)
//#include "PWM.h"                        //Base    TIM2_CH1(PA0)_CH2(PA1)_CH3(PA2)_CH4(PA3)
//#include "IC_PWMI.h"                    //Base    TIM3_CH1(PA6 )    TIM4_CH1(PB6 )    IC_PWMI.h 与 Encoder.h  只能起用一个
//#include "Encoder.h"                    //Base    TIM3_CH1(PA67)    TIM4_CH1(PB67)    IC_PWMI.h 与 Encoder.h  只能起用一个








//////===================================================================================================
//变量定义
//////===================================================================================================

//static uint16_t PWM_SetCompare1_i =0 ;
//uint8_t ID;                                //定义用于存放ID号的变量
//int16_t AX, AY, AZ, GX, GY, GZ;            //定义用于存放各个数据的变量


//float Pitch,Roll,Yaw;                                //俯仰角默认跟中值一样，翻滚角，偏航角
int16_t ax,ay,az,gx,gy,gz;                            //加速度，陀螺仪角速度

#define PI 3.141592653589793


typedef struct {
    float Pitch;
    float Roll;
    float Yaw;
} Pose_t;


// 队列句柄
QueueHandle_t xPoseQueue;

// 任务句柄
TaskHandle_t xMPUTaskHandle;
TaskHandle_t xOLEDTaskHandle;


                                                                                                         
//=================================================================================================//////


void MPU6050_PoseTask(void *pvParameters)
{
    float Pitch, Roll, Yaw;
    Pose_t pose;
    const TickType_t xFrequency = pdMS_TO_TICKS(2);  // ms周期
    TickType_t xLastWakeTime = xTaskGetTickCount();


    while(1)
    {
        // 读一次姿态（保证采样率）
        MPU6050_DMP_Get_Data(&Pitch,&Roll,&Yaw);                //读取姿态信息(其中偏航角有飘移是正常现象)
        
        pose.Pitch = Pitch;
        pose.Roll  = Roll;
        pose.Yaw   = Yaw;
        
        // 写入队列（覆盖式，只保留最新一帧）
        xQueueOverwrite(xPoseQueue, &pose);
        
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}


void OLED_DisplayTask(void *pvParameters)
{
    Pose_t recv_pose;
    const TickType_t xFrequency = pdMS_TO_TICKS(50);  //ms周期
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while(1)
    {
        
        // 从队列拿最新数据
        if(xQueueReceive(xPoseQueue, &recv_pose, 0) == pdPASS)
        {
            //---------------------------------------
            
            OLED_Clear(); 
            static uint16_t i=0;
            float Pitch, Roll, Yaw;
            int_fast8_t y_p1,y_p2,Yaw_p;
            
            
            Pitch = recv_pose.Pitch;
            Roll = recv_pose.Roll;
            Yaw = recv_pose.Yaw ;
            
            
            OLED_ShowNum(i*8,0,i,1,OLED_8X16);
            i++;
            i = (i>9?0:i);


            //OLED显示mpu6050数据


                
            
            OLED_DrawLine(0,31,127,31);
            OLED_DrawLine(63,0,63,63);

            y_p1     = 32+32*(Pitch/90);
            y_p2     = 63*tan(Roll* PI / 180.0);
            Yaw_p   = 63+63*(Yaw   /180); 

            OLED_DrawLine(0     ,y_p1-y_p2  ,127    ,y_p1+y_p2 );


            OLED_DrawLine(Yaw_p ,0          ,Yaw_p  ,63         );


            OLED_ShowSignedNum(8*12, 14*1  , Pitch, 3,OLED_8X16);
            OLED_ShowSignedNum(8*0, 14*2  , Roll , 3,OLED_8X16);
            OLED_ShowSignedNum(8*8, 14*4-6  , Yaw  , 3,OLED_8X16);


            //    OLED_ShowSignedNum(33, 0  , gx, 5,OLED_8X16);
            //    OLED_ShowSignedNum(33, 14 , gy, 5,OLED_8X16);
            //    OLED_ShowSignedNum(33, 28 , gz, 5,OLED_8X16);

            //    OLED_ShowSignedNum(66, 0, ax, 4,OLED_8X16);
            //    OLED_ShowSignedNum(66, 14, ay, 4,OLED_8X16);
            //    OLED_ShowSignedNum(66, 28, az, 4,OLED_8X16);

            OLED_Update();
            
            //========================
        }
        
        
        
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}



int main(void)
{   


//////====================================================================================================
//////模块初始化
//////==================================================================================================== 

    //SCH_Init();                         // systick_scheduler调度器
    DWT_Delay_Init();                   // DWT_Delay_Init
    I2C_QuickInit(I2C2, 400*1000);
    
    OLED_Init();                        OLED_ShowNum(0,1,1,1,OLED_8X16);OLED_Update();
    
    MPU6050_Init();                     OLED_ShowNum(0,2,2,1,OLED_8X16);OLED_Update();
    
    MPU6050_DMP_Init();                 OLED_ShowNum(0,3,3,1,OLED_8X16);OLED_Update();
    

//    TIM2_PWM_Init();
//    TIM34_IC_PWMI_Init();
//    Encoder1_TIM3_Init();
//    Encoder2_TIM4_Init();

//--------------------------------------------------------------------------------------------------------
    
    
// =====================================================
// 创建队列
// =====================================================
    
    xPoseQueue = xQueueCreate(1, sizeof(Pose_t));
    

// =====================================================
// 创建任务
// =====================================================
    
    // MPU6050 任务
    xTaskCreate(MPU6050_PoseTask, "MPUTask", 128, NULL, 1, &xMPUTaskHandle);

    // OLED 显示任务
    xTaskCreate(OLED_DisplayTask, "OLEDTask", 128, NULL, 2, &xOLEDTaskHandle);

    
    
// =====================================================
    vTaskStartScheduler();
//------------------------------------------------------












    
//////====================================================================================================
    
    
    
//////====================================================================================================    
//////systick_scheduler调度器                                            
//////====================================================================================================
//这里添加调度


//    SCH_AddTask(GET_MPU6050_info    ,2     ,PRIORITY_MID);
//    SCH_AddTask(OLED_Show_0         ,20    ,PRIORITY_LOW);
//    SCH_AddTask(OLED_OLED_Clear     ,500   ,PRIORITY_LOW);
//    SCH_AddTask(Turn_PWM_SetCompare1_i  ,100    ,PRIORITY_LOW);
    
    
    
//-------------------------------------------------------------------------------------------------------
//SCH_Start();                    //开始调度                                     
//=================================================================================================//////









    while (1)//主循环=============================================================================
    {
        
        //DWT_Delay_s(1);
        
    }

}
