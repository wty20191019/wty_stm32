//===================================================================================================
//#include
//===================================================================================================

#include "stm32f10x.h"                  //Device header
#include "DWT_Delay.h"                  //Base    DWT
#include "math.h"
#include "string.h"
#include "stdlib.h" 

#include "FreeRTOS.h"                   //Base    FreeRTOS
#include "task.h"                       //Base    FreeRTOS
#include "queue.h"                      //Base    FreeRTOS
#include "semphr.h"                     //Base    FreeRTOS

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

uint8_t RE_tast = 0;


//=====FreeRTOS=====//
typedef struct {
    float Pitch;
    float Roll;
    float Yaw;
} Pose_t_mpu6050;

// 队列句柄
QueueHandle_t xPoseQueue_mpu6050;


// 任务句柄
TaskHandle_t xMPUTaskHandle;
TaskHandle_t xOLEDTaskHandle;
TaskHandle_t xPC13LedTaskHandle;
TaskHandle_t vSerialRxTaskHandle;



// 互斥量
SemaphoreHandle_t xI2C2Mutex;






//===================================================================================================
// 串口接收任务
//===================================================================================================
void vSerialRxTask(void *pvParameters)
{
    uint8_t ucRxData;
    uint8_t rxBuffer[128];
    uint16_t rxIndex = 0;
    bool inFrame = false;
    
    while(1)
    {
        if (xQueueReceive(xSerialRxQueue, &ucRxData, portMAX_DELAY) == pdPASS)
        {
            // 检查是否为帧起始标记 '['
            if(ucRxData == '[')
            {
                inFrame = true;
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
                    Serial_Printf_Async("[%s]\r\n", rxBuffer);
                    
                    
                    // 解析帧内容
                    char *Tag = strtok((char *)rxBuffer, ",");
                    
                    if (strcmp(Tag, "key") == 0)
                    {
                        char *Name = strtok(NULL, ",");
                        char *Action = strtok(NULL, ",");
                        
                        if (strcmp(Name, "1") == 0 && strcmp(Action, "up") == 0)
                        {
                            Serial_Printf_Async("key,1,up\r\n");
                            RE_tast++;
                            Serial_Printf_Async("%d\r\n",RE_tast);
                            
                        }
                        else if (strcmp(Name, "2") == 0 && strcmp(Action, "down") == 0)
                        {
                            Serial_Printf_Async("key,2,down\r\n");
                            
                        }
                    }
                    else if (strcmp(Tag, "slider") == 0)
                    {
                        char *Name = strtok(NULL, ",");
                        char *Value = strtok(NULL, ",");
                        
                        if (strcmp(Name, "1") == 0)
                        {
                            uint8_t IntValue = atoi(Value);
                            Serial_Printf_Async("slider,1,%d\r\n", IntValue);
                        }
                        else if (strcmp(Name, "2") == 0)
                        {
                            float FloatValue = atof(Value);
                            Serial_Printf_Async("slider,2,%f\r\n", FloatValue);
                        }
                    }
                    else if (strcmp(Tag, "joystick") == 0)
                    {
                        int8_t LH = atoi(strtok(NULL, ","));
                        int8_t LV = atoi(strtok(NULL, ","));
                        int8_t RH = atoi(strtok(NULL, ","));
                        int8_t RV = atoi(strtok(NULL, ","));
                        
                        Serial_Printf_Async("joystick,%d,%d,%d,%d\r\n", LH, LV, RH, RV);
                    }
                    
                    inFrame = false;
                    rxIndex = 0;
                }
                else
                {
                    inFrame = false;
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
}


//===================================================================================================
// PC13 LED任务
//===================================================================================================
void Test_PC13_ledTask(void *pvParameters)
{
    const TickType_t xFrequency = pdMS_TO_TICKS(50);  
    TickType_t xLastWakeTime = xTaskGetTickCount();

    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    while(1)
    {
        if(GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13) == Bit_SET)
        {
            GPIO_ResetBits(GPIOC, GPIO_Pin_13);  
        }
        else
        {
            GPIO_SetBits(GPIOC, GPIO_Pin_13);    
        }
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}



//===================================================================================================
// MPU6050任务
//===================================================================================================
void MPU6050_PoseTask(void *pvParameters)
{
    float Pitch, Roll, Yaw;
    Pose_t_mpu6050 pose;
    
    const TickType_t xFrequency = pdMS_TO_TICKS(10);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    
    while(1)
    {
        if(xSemaphoreTake(xI2C2Mutex, pdMS_TO_TICKS(2)) == pdTRUE)
        {
            MPU6050_DMP_Get_Data(&Pitch, &Roll, &Yaw);
            xSemaphoreGive(xI2C2Mutex);
            
            pose.Pitch = Pitch;
            pose.Roll = Roll;
            pose.Yaw = Yaw;
            
            xQueueOverwrite(xPoseQueue_mpu6050, &pose);
            //xQueueSend(xPoseQueue_mpu6050, &pose,pdMS_TO_TICKS(2));
        }
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

//===================================================================================================
// OLED显示任务
//===================================================================================================
void OLED_DisplayTask(void *pvParameters)
{
    Pose_t_mpu6050 recv_pose_mpu6050;
    
    const TickType_t xFrequency = pdMS_TO_TICKS(50);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    
    while(1)
    {
        if(xQueueReceive(xPoseQueue_mpu6050, &recv_pose_mpu6050, 0) == pdPASS)
        {
            OLED_Clear(); 
            
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
            
            if(xSemaphoreTake(xI2C2Mutex, pdMS_TO_TICKS(1)) == pdTRUE)
            {
                OLED_Update();
                xSemaphoreGive(xI2C2Mutex);
            }
        }
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}


int main(void)
{
// 设置NVIC优先级分组
NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

//===================================================================================================
// 模块初始化
//===================================================================================================
DWT_Delay_Init();

I2C_QuickInit(I2C2, 400*1000);

OLED_Init();
OLED_ShowNum(0, 1, 1, 1, OLED_8X16);OLED_Update();


MPU6050_Init();
OLED_ShowNum(0, 2, 2, 1, OLED_8X16);OLED_Update();


MPU6050_DMP_Init();
OLED_ShowNum(0, 3, 3, 1, OLED_8X16);OLED_Update();


Serial_Init();
OLED_ShowNum(0, 3, 4, 1, OLED_8X16);OLED_Update();



//    Encoder2_TIM4_Init();               OLED_ShowNum(0,3,5,1,OLED_8X16);OLED_Update();
//    TIM2_PWM_Init();
//    TIM34_IC_PWMI_Init();
//    Encoder1_TIM3_Init();
//    Encoder2_TIM4_Init();
//    AD_DMA_Init();






//===================================================================================================
// FreeRTOS初始化
//===================================================================================================
// 创建互斥量====================================
xI2C2Mutex = xSemaphoreCreateMutex();

// 创建队列======================================
xPoseQueue_mpu6050 = xQueueCreate(1     , sizeof(Pose_t_mpu6050)    );
xSerialRxQueue     = xQueueCreate(256   , sizeof(uint8_t)           );
xSerialTxQueue     = xQueueCreate(256   , sizeof(uint8_t)           );

// 创建任务=======================================                                                                           
//    xTaskCreate(LED_Task, // 任务函数
//                "LED",    // 任务名称
//                128,      // 堆栈大小（字）
//                NULL,     // 任务参数
//                2,        // 优先级（数字越大优先级越高）   (0-7)
//                NULL);    // 任务句柄


xTaskCreate(Test_PC13_ledTask,   "led_PC13",  32    , NULL, 4, &xPC13LedTaskHandle);
xTaskCreate(OLED_DisplayTask,    "OLED",      128   , NULL, 3, &xOLEDTaskHandle);
xTaskCreate(MPU6050_PoseTask,    "MPU",       128   , NULL, 2, &xMPUTaskHandle);
xTaskCreate(vSerialRxTask,       "SerialRx",  512   , NULL, 3, &vSerialRxTaskHandle);




// 启动调度器
vTaskStartScheduler();//==============================================================================


    while(1)
    {
        
    }
}







