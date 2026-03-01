#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"
#include <stdio.h>
#include <stdarg.h>

#include "serial.h"

/* 队列句柄定义 */
QueueHandle_t xSerialRxQueue = NULL;
QueueHandle_t xSerialTxQueue = NULL;

/* 内部函数声明 */
static uint32_t Serial_Pow(uint32_t X, uint32_t Y);

/*-----------------------------------------------------------*/

/**
  * 函    数：串口初始化
  */
void Serial_Init(void)
{
    /* 创建队列 */
    xSerialRxQueue = xQueueCreate(128, sizeof(uint8_t));
    xSerialTxQueue = xQueueCreate(128, sizeof(uint8_t));
    
    if(xSerialRxQueue == NULL || xSerialTxQueue == NULL)
    {
        return;
    }
    
    /* 开启时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    /* GPIO初始化 */
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // PA9: TX 复用推挽输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // PA10: RX 浮空输入
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* USART初始化 */
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = USART1_BaudRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
    
    /* 中断配置 */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    
    /* NVIC配置 */
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configLIBRARY_KERNEL_INTERRUPT_PRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /* 使能USART */
    USART_Cmd(USART1, ENABLE);
}

/*-----------------------------------------------------------*/

/**
  * 函    数：串口完整初始化
  */
xComPortHandle xSerialPortInitMinimal(unsigned long ulWantedBaud, unsigned portBASE_TYPE uxQueueLength)
{
    xComPortHandle xReturn = (xComPortHandle)1;
    
    /* 创建队列 */
    xSerialRxQueue = xQueueCreate(uxQueueLength, sizeof(uint8_t));
    xSerialTxQueue = xQueueCreate(uxQueueLength, sizeof(uint8_t));
    
    if(xSerialRxQueue != NULL && xSerialTxQueue != NULL)
    {
        /* 开启时钟 */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        
        /* GPIO初始化 */
        GPIO_InitTypeDef GPIO_InitStructure;
        
        // PA9: TX
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        
        // PA10: RX
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        
        /* USART初始化 */
        USART_InitTypeDef USART_InitStructure;
        USART_InitStructure.USART_BaudRate = ulWantedBaud;
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        USART_InitStructure.USART_Parity = USART_Parity_No;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        USART_Init(USART1, &USART_InitStructure);
        
        /* 中断配置 */
        USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
        
        /* NVIC配置 */
        NVIC_InitTypeDef NVIC_InitStructure;
        NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configLIBRARY_KERNEL_INTERRUPT_PRIORITY;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
        
        /* 使能USART */
        USART_Cmd(USART1, ENABLE);
    }
    else
    {
        xReturn = (xComPortHandle)0;
    }
    
    return xReturn;
}

/*-----------------------------------------------------------*/

/**
  * 函    数：接收字符
  */
BaseType_t xSerialGetChar(xComPortHandle pxPort, signed char *pcRxedChar, TickType_t xBlockTime)
{
    (void)pxPort;
    
    if(xQueueReceive(xSerialRxQueue, pcRxedChar, xBlockTime) == pdPASS)
    {
        return pdTRUE;
    }
    else
    {
        return pdFALSE;
    }
}

/*-----------------------------------------------------------*/

/**
  * 函    数：发送字符
  */
BaseType_t xSerialPutChar(xComPortHandle pxPort, signed char cOutChar, TickType_t xBlockTime)
{
    (void)pxPort;
    
    if(xQueueSend(xSerialTxQueue, &cOutChar, xBlockTime) == pdPASS)
    {
        USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
        return pdPASS;
    }
    else
    {
        return pdFAIL;
    }
}

/*-----------------------------------------------------------*/

/**
  * 函    数：发送字符串
  */
void vSerialPutString(xComPortHandle pxPort, const signed char * const pcString, unsigned short usStringLength)
{
    signed char *pxNext = (signed char *)pcString;
    
    (void)usStringLength;
    (void)pxPort;
    
    while(*pxNext)
    {
        xSerialPutChar(NULL, *pxNext, 0);
        pxNext++;
    }
}

/*-----------------------------------------------------------*/

/**
  * 函    数：次方计算
  */
static uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
    uint32_t Result = 1;
    while(Y--)
    {
        Result *= X;
    }
    return Result;
}

/*-----------------------------------------------------------*/

/**
  * 函    数：异步发送一个字节
  */
void Serial_SendByte_Async(uint8_t Byte)
{
    if(xQueueSend(xSerialTxQueue, &Byte, portMAX_DELAY) == pdPASS)
    {
        USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
    }
}

/*-----------------------------------------------------------*/

/**
  * 函    数：发送数组
  */
void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
    uint16_t i;
    for(i = 0; i < Length; i++)
    {
        Serial_SendByte_Async(Array[i]);
    }
}

/*-----------------------------------------------------------*/

/**
  * 函    数：异步发送字符串
  */
void Serial_SendString_Async(char *String)
{
    while(*String != '\0')
    {
        Serial_SendByte_Async(*String);
        String++;
    }
}

/*-----------------------------------------------------------*/

/**
  * 函    数：异步发送数字
  */
void Serial_SendNumber_Async(uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for(i = 0; i < Length; i++)
    {
        Serial_SendByte_Async(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
    }
}

/*-----------------------------------------------------------*/

/**
  * 函    数：异步发送结构体
  */
void Serial_SendStruct_Async(void *pStruct, uint16_t Size)
{
    uint8_t *pByte = (uint8_t *)pStruct;
    uint16_t i;
    
    for(i = 0; i < Size; i++)
    {
        Serial_SendByte_Async(pByte[i]);
    }
}

/*-----------------------------------------------------------*/

/**
  * 函    数：异步格式化输出
  */
void Serial_Printf_Async(char *format, ...)
{
    char String[256];
    va_list arg;
    va_start(arg, format);
    vsnprintf(String, sizeof(String) - 1, format, arg);
    String[sizeof(String) - 1] = '\0';
    va_end(arg);
    Serial_SendString_Async(String);
}

/*-----------------------------------------------------------*/

/**
  * 函    数：关闭串口
  */
void vSerialClose(xComPortHandle xPort)
{
    (void)xPort;
    
    // 禁用串口
    USART_Cmd(USART1, DISABLE);
    
    // 禁用中断
    USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
    
    // 清空队列
    if(xSerialRxQueue != NULL)
    {
        vQueueDelete(xSerialRxQueue);
        xSerialRxQueue = NULL;
    }
    
    if(xSerialTxQueue != NULL)
    {
        vQueueDelete(xSerialTxQueue);
        xSerialTxQueue = NULL;
    }
}

/*-----------------------------------------------------------*/

/**
  * 函    数：USART1中断服务程序
  */
void USART1_IRQHandler(void)
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    uint8_t ucData;
    
    /* 接收中断处理 */
    if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
    {
        ucData = USART_ReceiveData(USART1);
        if(xSerialRxQueue != NULL)
        {
            xQueueSendToBackFromISR(xSerialRxQueue, &ucData, &xHigherPriorityTaskWoken);
        }
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
    
    /* 发送中断处理 */
    if(USART_GetITStatus(USART1, USART_IT_TXE) == SET)
    {
        if(xQueueReceiveFromISR(xSerialTxQueue, &ucData, &xHigherPriorityTaskWoken) == pdTRUE)
        {
            USART_SendData(USART1, ucData);
        }
        else
        {
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
        }
    }
    
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}



