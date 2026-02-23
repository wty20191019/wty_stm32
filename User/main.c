//===================================================================================================
//#include
//===================================================================================================


#include "stm32f10x.h"                  //Device header
#include "DWT_Delay.h"                  //Base    DWT
#include "math.h" 

#include "FreeRTOS.h"                   //Base    FreeRTOS
#include "task.h"                       //Base    FreeRTOS
#include "queue.h"                      //Base    FreeRTOS
#include "semphr.h"                     //Base    FreeRTOS




#include "hardware_I2C.h"                   //Base    hardware_I2Cx        (I2C1_I2C2)

    #include "OLED.h"                       //Base    (PB11) (PB10)   I2C2.h         OLED_2          //--no-multibyte-chars

    //====mpu6050dmp====//                  
    #include "mpu6050.h"                    //Base    (PB11) (PB10)   I2C2.h    inv_mpu.h
    #include "inv_mpu.h"                    //Base                    I2C2.h    inv_mpu.h    mpu6050.h


//#include "hardware_AD.h"                //Base    hardware_AD
//#include "PID_system.h"                 //base     
//#include "key.h"                        //Base    (PB1 ) (PB11)
//#include "LED.h"                        //Base    (PA12)
//#include "PWM.h"                        //Base    TIM2_CH1(PA0)_CH2(PA1)_CH3(PA2)_CH4(PA3)
//#include "IC_PWMI.h"                    //Base    TIM3_CH1(PA6 )    TIM4_CH1(PB6 )    IC_PWMI.h 与 Encoder.h  只能起用一个
//#include "Encoder.h"                    //Base    TIM3_CH1(PA67)    TIM4_CH1(PB67)    IC_PWMI.h 与 Encoder.h  只能起用一个
//#include "MyDMA.h"                      //Base    hardware_DMA
//#include "adc_dma.h"                    //Base    hardware_AD    hardware_DMA







//////===================================================================================================
//变量定义
//////===================================================================================================

#define PI 3.141592653589793

//=====FreeRTOS=====//

typedef struct {
    float Pitch;
    float Roll;
    float Yaw;
} Pose_t_mpu6050;


// 队列句柄
QueueHandle_t xPoseQueue_mpu6050;
//QueueHandle_t xPoseQueue_AD;

// 任务句柄
TaskHandle_t xMPUTaskHandle;
TaskHandle_t xOLEDTaskHandle;
TaskHandle_t PC13_led;
//TaskHandle_t xADTaskHandle;

//I2C2互斥量
SemaphoreHandle_t xI2C2Mutex;  




//=================================================================================================//////







//void GetAD_Task(void *pvParameters)
//{
//    uint16_t ad_data[4];
//    
//    const TickType_t xFrequency = pdMS_TO_TICKS(20);
//    TickType_t xLastWakeTime = xTaskGetTickCount();
//    
//    
//    while(1)
//    {
//        
//        taskENTER_CRITICAL();
//        
//        memcpy(ad_data, AD_Value, sizeof(ad_data));
//        
//        taskEXIT_CRITICAL();
//        
//        
//        
//        // 发送到队列
//        xQueueSend(xPoseQueue_AD, &ad_data, pdMS_TO_TICKS(5));
//        
//        vTaskDelayUntil(&xLastWakeTime, xFrequency);
//    }
//}



void Test_PC13_ledTask(void *pvParameters)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    while(1)
    {
        if (GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13) == 0)
        {
        GPIO_SetBits(GPIOC, GPIO_Pin_13);
        }
        else
        {
        GPIO_ResetBits(GPIOC, GPIO_Pin_13);
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}


void MPU6050_PoseTask(void *pvParameters)
{
    float Pitch, Roll, Yaw;
    Pose_t_mpu6050 pose;
    const TickType_t xFrequency = pdMS_TO_TICKS(1);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    
    while(1)
    {
        
        if(xSemaphoreTake(xI2C2Mutex, pdMS_TO_TICKS(1)) == pdTRUE)// 获取I2C互斥锁
        {
            MPU6050_DMP_Get_Data(&Pitch, &Roll, &Yaw);
            xSemaphoreGive(xI2C2Mutex);// 释放锁
            
            pose.Pitch = Pitch;
            pose.Roll = Roll;
            pose.Yaw = Yaw;
            xQueueOverwrite(xPoseQueue_mpu6050, &pose);
        }
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}



void OLED_DisplayTask(void *pvParameters)
{
    Pose_t_mpu6050 recv_pose_mpu6050;
//    uint16_t recv_data_AD[4];
    const TickType_t xFrequency = pdMS_TO_TICKS(50);
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while(1)
    {
        if( xQueueReceive(xPoseQueue_mpu6050,&recv_pose_mpu6050 , 0) == pdPASS )          //  | xQueueReceive(xPoseQueue_AD     ,&recv_data_AD        , 0) == pdPASS
        {
            if(xSemaphoreTake(xI2C2Mutex, pdMS_TO_TICKS(5)) == pdTRUE)// 获取I2C互斥锁
            {
                //---------------------------------------
                OLED_Clear(); 
                
                
                
                //OLED显示数字滚动
                static uint16_t i=0;
                OLED_ShowNum(i*8,0,i,1,OLED_8X16);
                i++;
                i = (i>9?0:i);


                //OLED显示mpu6050数据
                
                
                int_fast8_t y_p1,y_p2,Yaw_p;
                y_p1    = 32+32*(recv_pose_mpu6050.Pitch/90);
                y_p2    = 63*tan(recv_pose_mpu6050.Roll* PI / 180.0);
                Yaw_p   = 63+63*(recv_pose_mpu6050.Yaw   /180); 
                
                OLED_DrawLine(0     ,31         ,127    ,31         );
                OLED_DrawLine(63    ,0          ,63     ,63         );
                OLED_DrawLine(0     ,y_p1-y_p2  ,127    ,y_p1+y_p2  );
                OLED_DrawLine(Yaw_p ,0          ,Yaw_p  ,63         );
                
                OLED_ShowSignedNum(8*12, 14*1   , recv_pose_mpu6050.Pitch, 3,OLED_8X16);
                OLED_ShowSignedNum(8*0, 14*2    , recv_pose_mpu6050.Roll , 3,OLED_8X16);
                OLED_ShowSignedNum(8*8, 14*4-6  , recv_pose_mpu6050.Yaw  , 3,OLED_8X16);
                
                
                 
                
//                //OLED显示AD
//                
//                OLED_ShowNum(8*8, 12*0  , recv_data_AD[0]  , 5,OLED_8X16);
//                OLED_ShowNum(8*8, 12*1  , recv_data_AD[1]  , 5,OLED_8X16);
//                OLED_ShowNum(8*8, 12*2  , recv_data_AD[2]  , 5,OLED_8X16);
//                OLED_ShowNum(8*8, 12*3  , recv_data_AD[3]  , 5,OLED_8X16);
                
                
                OLED_Update();
                //========================
                xSemaphoreGive(xI2C2Mutex);  // 释放锁
            }
        }
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}



            
       















int main(void)
{   


//////====================================================================================================
//////模块初始化
//////==================================================================================================== 


    DWT_Delay_Init();
    
    I2C_QuickInit(I2C2, 400*1000);
    
    OLED_Init();                        OLED_ShowNum(0,1,1,1,OLED_8X16);OLED_Update();
    
    MPU6050_Init();                     OLED_ShowNum(0,2,2,1,OLED_8X16);OLED_Update();
    
    MPU6050_DMP_Init();                 OLED_ShowNum(0,3,3,1,OLED_8X16);OLED_Update();
    
//    AD_DMA_Init();                      OLED_ShowNum(0,3,4,1,OLED_8X16);OLED_Update();


//    TIM2_PWM_Init();
//    TIM34_IC_PWMI_Init();
//    Encoder1_TIM3_Init();
//    Encoder2_TIM4_Init();

//--------------------------------------------------------------------------------------------------------
    
// =====================================================
// 设置NVIC优先级分组
// =====================================================

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    
    
//FreeRTOS==============================================
// =====================================================
// 创建互斥量
// =====================================================
    
    xI2C2Mutex = xSemaphoreCreateMutex();
    
// =====================================================
// 创建队列
// =====================================================
    
    xPoseQueue_mpu6050 = xQueueCreate(1, sizeof(Pose_t_mpu6050));
    
//    xPoseQueue_AD = xQueueCreate(2, sizeof(uint16_t[4]));
    


// =====================================================
// 创建任务
// =====================================================
//    xTaskCreate(LED_Task,      // 任务函数
//                "LED",         // 任务名称
//                128,           // 堆栈大小（字）
//                NULL,          // 任务参数
//                2,             // 优先级（数字越大优先级越高）
//                NULL);         // 任务句柄



    
    // OLED 显示任务
    xTaskCreate(OLED_DisplayTask, "OLEDTask", 128, NULL, 1, &xOLEDTaskHandle);
    
    // MPU6050 任务
    xTaskCreate(MPU6050_PoseTask, "MPUTask", 128, NULL, 2, &xMPUTaskHandle);
    
    // PC3_led 任务
    xTaskCreate(Test_PC13_ledTask, "led_PC13", 32, NULL, 1, &PC13_led);
    
//    // AD 任务
//    xTaskCreate(GetAD_Task, "AD_Task", 128, NULL, 1, &xADTaskHandle);
    
    
    
// =====================================================
    vTaskStartScheduler();
//------------------------------------------------------





    while (1)//主循环=============================================================================
    {
        
        //DWT_Delay_s(1);
        
    }

}



