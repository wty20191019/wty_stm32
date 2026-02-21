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

#include "PID_system.h"
#include "PWM.h"                        //Base    TIM2_CH1(PA0)_CH2(PA1)_CH3(PA2)_CH4(PA3)
#include "key.h"                        //Base    (PB1 )



#include "hardware_I2C.h"                   //Base    hardware_I2Cx        (I2C1_I2C2)

    #include "OLED.h"                       //Base    (PB11) (PB10)   I2C2.h         OLED_2           //--no-multibyte-chars

    //====mpu6050dmp====//                  
    #include "mpu6050.h"                    //Base    (PB11) (PB10)   I2C2.h    inv_mpu.h
    #include "inv_mpu.h"                    //Base                    I2C2.h    inv_mpu.h    mpu6050.h



//#include "PID_system.h"                 //base     
//#include "LED.h"                        //Base    (PA12)
//#include "IC_PWMI.h"                    //Base    TIM3_CH1(PA6 )    TIM4_CH1(PB6 )    IC_PWMI.h 与 Encoder.h  只能起用一个
//#include "Encoder.h"                    //Base    TIM3_CH1(PA67)    TIM4_CH1(PB67)    IC_PWMI.h 与 Encoder.h  只能起用一个








//////===================================================================================================
//变量定义
//////===================================================================================================

//static uint16_t PWM_SetCompare1_i =0 ;
//uint8_t ID;                                //定义用于存放ID号的变量
//int16_t AX, AY, AZ, GX, GY, GZ;            //定义用于存放各个数据的变量


//float Pitch,Roll,Yaw;                                //俯仰角默认跟中值一样，翻滚角，偏航角
//int16_t ax,ay,az,gx,gy,gz;                            //加速度，陀螺仪角速度


#define PI 3.141592653589793

//=====FreeRTOS=====//

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
TaskHandle_t xPC13_ledaskHandle;
TaskHandle_t xPIDTaskHandle;

//I2C2互斥量
SemaphoreHandle_t xI2C2Mutex;  



//=================================================================================================//////

void Test_PID(void *pvParameters)
{
    float Pitch, Roll, Yaw;
    Pose_t recv_pose;
    const TickType_t xFrequency = pdMS_TO_TICKS(100);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);GPIO_InitTypeDef GPIO_InitStructure;GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;GPIO_Init(GPIOC, &GPIO_InitStructure);

    TIM_SetCompare1(TIM2,75);
    
    // 创建PID控制器实例
    PID_Controller* Turn_pid = PID_Create_Instance(
        -0.555f,        // Kp：比例系数  
        1.0f,           // Ki：积分系数
        0.055f,          // Kd：微分系数
        
        
        90.0f,           // 积分限幅：最大积分累积值
        45.0f,          // 积分分离阈值
        
        
        -50.0f,         // 输出下限
        50.0f,          // 输出上限
        PID_MODE_POSITION  // 位置式PID
    );
    
    
    if (Turn_pid == NULL) {
        // 无法创建PID控制器，删除任务
        vTaskDelete(NULL);
        return;
    }
    
    // 设置目标值
    PID_Set_Target(Turn_pid, 0.0f);  
    
    // 启用抗饱和功能
    PID_Set_Anti_Windup(Turn_pid, 0);
    
    // 设置微分滤波系数（0.2表示中等滤波强度）
    PID_Set_Derivative_Filter(Turn_pid, 1.0f);

    while(1)
    {   
        if(xQueueReceive(xPoseQueue, &recv_pose, 0) == pdPASS)
        {
            Pitch = recv_pose.Pitch;
            Roll = recv_pose.Roll;
            Yaw = recv_pose.Yaw ;
        }
        TIM_SetCompare1(TIM2,(75+PID_Update(Turn_pid,Roll,0.1f)));
        if(GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13) == 0){GPIO_SetBits(GPIOC, GPIO_Pin_13);}else{GPIO_ResetBits(GPIOC, GPIO_Pin_13);}
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}



void Test_PC13_ledTask(void *pvParameters)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);GPIO_InitTypeDef GPIO_InitStructure;GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    while(1)
    {
        
        if(GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13) == 0){GPIO_SetBits(GPIOC, GPIO_Pin_13);}else{GPIO_ResetBits(GPIOC, GPIO_Pin_13);}
        
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}


void MPU6050_PoseTask(void *pvParameters)
{
    float Pitch, Roll, Yaw;
    Pose_t pose;
    const TickType_t xFrequency = pdMS_TO_TICKS(1);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    
    while(1)
    {
        // 获取I2C互斥锁（等待时间稍长，因为OLED可能正在使用）
        if(xSemaphoreTake(xI2C2Mutex, pdMS_TO_TICKS(1)) == pdTRUE)
        {
            MPU6050_DMP_Get_Data(&Pitch, &Roll, &Yaw);
            xSemaphoreGive(xI2C2Mutex);  // 释放锁
            
            pose.Pitch = Pitch;
            pose.Roll = Roll;
            pose.Yaw = Yaw;
            xQueueOverwrite(xPoseQueue, &pose);
        }
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}


void OLED_DisplayTask(void *pvParameters)
{
    Pose_t recv_pose;
    const TickType_t xFrequency = pdMS_TO_TICKS(50);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    
    while(1)
    {
        if(xQueueReceive(xPoseQueue, &recv_pose, 0) == pdPASS)
        {
            // 获取I2C互斥锁
            if(xSemaphoreTake(xI2C2Mutex, pdMS_TO_TICKS(5)) == pdTRUE)
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


    DWT_Delay_Init();                   // DWT_Delay_Init
    I2C_QuickInit(I2C2, 400*1000);
    
    OLED_Init();                        OLED_ShowNum(0,1,1,1,OLED_8X16);OLED_Update();
    
    MPU6050_Init();                     OLED_ShowNum(0,2,2,1,OLED_8X16);OLED_Update();
    
    MPU6050_DMP_Init();                 OLED_ShowNum(0,3,3,1,OLED_8X16);OLED_Update();
    
    PID_System_Init();                  OLED_ShowNum(0,3,4,1,OLED_8X16);OLED_Update();
    
    TIM2_PWM_Init();                    OLED_ShowNum(0,3,5,1,OLED_8X16);OLED_Update();
    
    Key_Init();                         OLED_ShowNum(0,3,6,1,OLED_8X16);OLED_Update();

//    TIM2_PWM_Init();
//    TIM34_IC_PWMI_Init();
//    Encoder1_TIM3_Init();
//    Encoder2_TIM4_Init();

//--------------------------------------------------------------------------------------------------------
    
    
    
    
// =====================================================
// 创建互斥量
// =====================================================
    
    xI2C2Mutex = xSemaphoreCreateMutex();
    
// =====================================================
// 创建队列
// =====================================================
    
    xPoseQueue = xQueueCreate(2, sizeof(Pose_t));
    
// =====================================================
// 创建任务
// =====================================================
    
    
    // OLED 显示任务
    xTaskCreate(OLED_DisplayTask, "OLEDTask", 256, NULL, 1, &xOLEDTaskHandle);
    
    // MPU6050 任务
    xTaskCreate(MPU6050_PoseTask, "MPUTask", 256, NULL, 2, &xMPUTaskHandle);
    
    // PC3_led 任务
    //xTaskCreate(Test_PC13_ledTask, "led_PC13", 256, NULL, 1, &xPC13_ledaskHandle);
    
    // Test_PID 任务
    xTaskCreate(Test_PID, "Test_PID", 256, NULL, 1, &xPIDTaskHandle);



    
    
// =====================================================
    vTaskStartScheduler();
//------------------------------------------------------





    while (1)//主循环=============================================================================
    {
        
        //DWT_Delay_s(1);
        
    }

}
