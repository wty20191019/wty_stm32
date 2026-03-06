#include "stm32f10x.h"
#include "serial.h"
#include <string.h>

// 串口数据结构
Serial_TypeDef Serial;

// 内部函数声明
static uint32_t Serial_Pow(uint32_t X, uint32_t Y);
static void Serial_SendData(void);

/*-----------------------------------------------------------*/

/**
  * 函    数：串口初始化
  */
void Serial_Init(void)
{
    // 初始化串口数据结构
    memset(&Serial, 0, sizeof(Serial));
    
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
    USART_ITConfig(USART1, USART_IT_TXE, DISABLE);  // 初始关闭发送中断
    
    /* NVIC配置 */
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /* 使能USART */
    USART_Cmd(USART1, ENABLE);
}

/*-----------------------------------------------------------*/

/**
  * 函    数：检查接收缓冲区是否有数据
  */
uint8_t Serial_Available(void)
{
    return Serial.rxCount;
}

/*-----------------------------------------------------------*/

/**
  * 函    数：读取一个字节
  */
uint8_t Serial_ReceiveByte(void)
{
    uint8_t data = 0;
    
    if(Serial.rxCount > 0)
    {
        data = Serial.rxBuffer[Serial.rxReadIndex];
        
        // 更新读取索引
        Serial.rxReadIndex++;
        if(Serial.rxReadIndex >= SERIAL_RX_BUFFER_SIZE)
        {
            Serial.rxReadIndex = 0;
        }
        
        // 更新计数
        __disable_irq();
        Serial.rxCount--;
        __enable_irq();
    }
    
    return data;
}

/*-----------------------------------------------------------*/

/**
  * 函    数：清空接收缓冲区
  */
void Serial_Flush(void)
{
    __disable_irq();
    Serial.rxReadIndex = 0;
    Serial.rxWriteIndex = 0;
    Serial.rxCount = 0;
    __enable_irq();
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
  * 函    数：发送一个字节
  */
void Serial_SendByte(uint8_t Byte)
{
    // 等待发送缓冲区有空闲位置
    while(Serial.txCount >= SERIAL_TX_BUFFER_SIZE)
    {
        // 如果缓冲区满，等待
    }
    
    // 写入发送缓冲区
    Serial.txBuffer[Serial.txWriteIndex] = Byte;
    Serial.txWriteIndex++;
    if(Serial.txWriteIndex >= SERIAL_TX_BUFFER_SIZE)
    {
        Serial.txWriteIndex = 0;
    }
    
    __disable_irq();
    Serial.txCount++;
    __enable_irq();
    
    // 使能发送中断
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}

/*-----------------------------------------------------------*/

/**
  * 函    数：发送数据（中断中调用）
  */
static void Serial_SendData(void)
{
    if(Serial.txCount > 0)
    {
        uint8_t data = Serial.txBuffer[Serial.txReadIndex];
        USART_SendData(USART1, data);
        
        Serial.txReadIndex++;
        if(Serial.txReadIndex >= SERIAL_TX_BUFFER_SIZE)
        {
            Serial.txReadIndex = 0;
        }
        
        __disable_irq();
        Serial.txCount--;
        __enable_irq();
    }
    else
    {
        // 发送完成，关闭发送中断
        USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
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
        Serial_SendByte(Array[i]);
    }
}

/*-----------------------------------------------------------*/

/**
  * 函    数：发送字符串
  */
void Serial_SendString(char *String)
{
    while(*String != '\0')
    {
        Serial_SendByte(*String);
        String++;
    }
}

/*-----------------------------------------------------------*/

/**
  * 函    数：发送数字
  */
void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for(i = 0; i < Length; i++)
    {
        Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
    }
}

/*-----------------------------------------------------------*/

/**
  * 函    数：发送结构体
  */
void Serial_SendStruct(void *pStruct, uint16_t Size)
{
    uint8_t *pByte = (uint8_t *)pStruct;
    uint16_t i;
    
    for(i = 0; i < Size; i++)
    {
        Serial_SendByte(pByte[i]);
    }
}

/*-----------------------------------------------------------*/

/**
  * 函    数：格式化输出
  */
void Serial_Printf(char *format, ...)
{
    char String[256];
    va_list arg;
    va_start(arg, format);
    vsnprintf(String, sizeof(String) - 1, format, arg);
    String[sizeof(String) - 1] = '\0';
    va_end(arg);
    Serial_SendString(String);
}

/*-----------------------------------------------------------*/

/**
  * 函    数：USART1中断服务程序
  */
void USART1_IRQHandler(void)
{
    uint8_t ucData;
    
    /* 接收中断处理 */
    if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
    {
        ucData = USART_ReceiveData(USART1);
        
        // 检查缓冲区是否已满
        if(Serial.rxCount < SERIAL_RX_BUFFER_SIZE)
        {
            // 写入接收缓冲区
            Serial.rxBuffer[Serial.rxWriteIndex] = ucData;
            Serial.rxWriteIndex++;
            if(Serial.rxWriteIndex >= SERIAL_RX_BUFFER_SIZE)
            {
                Serial.rxWriteIndex = 0;
            }
            
            // 更新计数
            Serial.rxCount++;
        }
        
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
    
    /* 发送中断处理 */
    if(USART_GetITStatus(USART1, USART_IT_TXE) == SET)
    {
        Serial_SendData();
    }
}

