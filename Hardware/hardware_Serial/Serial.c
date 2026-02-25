#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

#include "Serial.h"

QueueHandle_t xSerialRxQueue = NULL;
QueueHandle_t xSerialTxQueue = NULL;


/**
  * 函    数：串口初始化
  * 参    数：无
  * 返 回 值：无
  */
void Serial_Init(void)
{
    /*开启时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);      //开启USART1的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);       //开启GPIOA的时钟
    
    /*GPIO初始化*/
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);                    //将PA9引脚初始化为复用推挽输出
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);                    //将PA10引脚初始化为上拉输入
    
    /*USART初始化*/
    USART_InitTypeDef USART_InitStructure;                                              //定义结构体变量
    USART_InitStructure.USART_BaudRate = 9600;                                          //波特率  //9600  //19200  //38400  
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;     //硬件流控制，不需要
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;                     //模式，发送模式和接收模式均选择
    USART_InitStructure.USART_Parity = USART_Parity_No;                                 //奇偶校验，不需要
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                              //停止位，选择1位
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                         //字长，选择8位
    USART_Init(USART1, &USART_InitStructure);                                           //将结构体变量交给USART_Init，配置USART1
    
    /*中断输出配置*/
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);              //开启串口接收数据的中断
    
    /*NVIC配置*/
    NVIC_InitTypeDef NVIC_InitStructure;                            //定义结构体变量
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;               //选择配置NVIC的USART1线
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                 //指定NVIC线路使能
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 11;      //指定NVIC线路的抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;              //指定NVIC线路的响应优先级
    NVIC_Init(&NVIC_InitStructure);                                 //将结构体变量交给NVIC_Init，配置NVIC外设
    
    /*USART使能*/
    USART_Cmd(USART1, ENABLE);                                //使能USART1，串口开始运行
    

}


/**
  * 函    数：次方函数（内部使用）
  * 返 回 值：返回值等于X的Y次方
  */
uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
    uint32_t Result = 1;    //设置结果初值为1
    while (Y --)            //执行Y次
    {
        Result *= X;        //将X累乘到结果
    }
    return Result;
}


/**
  * 函    数：USART1中断函数
  * 参    数：无
  * 返 回 值：无
  * 注意事项：此函数为中断函数，无需调用，中断触发后自动执行
  *           函数名为预留的指定名称，可以从启动文件复制
  *           请确保函数名正确，不能有任何差异，否则中断函数将不能进入
  */
  void USART1_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;  // 默认为 pdFALSE = ( ( BaseType_t ) 0 )
    uint8_t ucReceivedData;

    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
    {
        ucReceivedData = USART_ReceiveData(USART1); // 读取数据

        // 增加队列非空检查，避免向NULL队列发送
        if(xSerialRxQueue != NULL)
        {
            // 注意：从中断调用，必须使用 xQueueSendToBackFromISR
            xQueueSendToBackFromISR(xSerialRxQueue,
                                &ucReceivedData,
                                &xHigherPriorityTaskWoken);
            
            
        }
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }

    // 如果有任务因为此中断而被解除阻塞，且其优先级高于当前运行的任务，
    // 则 xHigherPriorityTaskWoken 会被设置为 pdTRUE      //pdTRUE = ( ( BaseType_t ) 1 )
    // 随后我们必须请求进行一次上下文切换。
    
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}



// 供其他任务调用的非阻塞发送 API
void Serial_SendByte_Async(uint8_t Byte)
{
    xQueueSend(xSerialTxQueue, &Byte,portMAX_DELAY);               //portMAX_DELAY
}

void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
    uint16_t i;
    for (i = 0; i < Length; i ++)               
    {
        Serial_SendByte_Async(Array[i]);        
    }
}

void Serial_SendString_Async(char *String)
{
    while (*String != '\0')
    {
        Serial_SendByte_Async(*String);
        String++;
    }
}

void Serial_SendNumber_Async(uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i ++)        //根据数字长度遍历数字的每一位
    {
        Serial_SendByte_Async(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');    //依次调用Serial_SendByte发送每位数字
    }
}

//void Serial_Printf_Async(char *format, ...)
//{
//    char String[256];                   //定义字符数组
//    va_list arg;                        //定义可变参数列表数据类型的变量arg
//    va_start(arg, format);              //从format开始，接收参数列表到arg变量
//    vsprintf(String, format, arg);      //使用vsprintf打印格式化字符串和参数列表到字符数组中
//    va_end(arg);                        //结束变量arg
//    Serial_SendString_Async(String);    //串口发送字符数组（字符串）
//}


void Serial_Printf_Async(char *format, ...)
{
    char String[256];                  
    va_list arg;                        
    va_start(arg, format);              
    // 改用vsnprintf，限制最大长度，避免缓冲区溢出
    vsnprintf(String, sizeof(String) - 1, format, arg);
    String[sizeof(String) - 1] = '\0'; // 强制结尾符，防止溢出
    va_end(arg);                        
    Serial_SendString_Async(String);    
}





void Serial_SendStruct_Async(void *pStruct, uint16_t Size)
{
    // 将结构体指针转换为字节（uint8_t）指针
    uint8_t *pByte = (uint8_t *)pStruct;
    
    // 循环发送每一个字节
    for(uint16_t i = 0; i < Size; i++)
    {
        Serial_SendByte_Async(pByte[i]); // 调用您已有的异步字节发送函数
    }
}



