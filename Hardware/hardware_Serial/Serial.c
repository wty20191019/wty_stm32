#include "stm32f10x.h"
#include <stdio.h>
#include <stdarg.h>

//缓冲区
#define SERIAL_BUFFER_SIZE 128
static uint8_t rxBuffer[SERIAL_BUFFER_SIZE];
static uint16_t rxIndex = 0;

void Serial_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
    
    // GPIO配置
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    // TX - PA9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // RX - PA10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // USART配置
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &USART_InitStructure);
    
    USART_Cmd(USART1, ENABLE);
    
    // 清空缓冲区
    for(int i = 0; i < SERIAL_BUFFER_SIZE; i++) {
        rxBuffer[i] = 0;
    }
    rxIndex = 0;
}

// ==========发送函数 ==========

// 串口发送一个字节
void Serial_SendByte(uint8_t Byte)
{
    USART_SendData(USART1, Byte);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

// 串口发送一个数组
void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
    uint16_t i;
    for(i = 0; i < Length; i++) {
        Serial_SendByte(Array[i]);
    }
}

// 串口发送一个字符串
void Serial_SendString(char *String)
{
    uint8_t i;
    for(i = 0; String[i] != '\0'; i++) {
        Serial_SendByte(String[i]);
    }
}

// 次方函数（内部使用）
uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
    uint32_t Result = 1;
    while(Y--) {
        Result *= X;
    }
    return Result;
}

// 串口发送数字
void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for(i = 0; i < Length; i++) {
        Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
    }
}

// 自己封装的printf函数
void Serial_Printf(char *format, ...)
{
    char String[100];            // 定义字符数组
    va_list arg;                 // 定义可变参数列表数据类型的变量arg
    va_start(arg, format);       // 从format开始，接收参数列表到arg变量
    vsprintf(String, format, arg); // 使用vsprintf打印格式化字符串和参数列表到字符数组中
    va_end(arg);                 // 结束变量arg
    Serial_SendString(String);   // 串口发送字符数组（字符串）
}

// 使用printf需要重定向的底层函数
int fputc(int ch, FILE *f)
{
    Serial_SendByte(ch);         // 将printf的底层重定向到自己的发送字节函数
    return ch;
}

// ==========轮询接收函数 ==========

// 轮询接收（在任务中调用）
void Serial_ReceiveTask(void)
{
    // 检查是否有数据
    if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET)
    {
        uint8_t data = USART_ReceiveData(USART1);
        
        // 存入缓冲区
        if(rxIndex < SERIAL_BUFFER_SIZE-1)
        {
            rxBuffer[rxIndex++] = data;
            
            // 如果收到回车或换行，处理命令
            if(data == '\r' || data == '\n')
            {
                if(rxIndex > 1)  // 有实际数据
                {
                    rxBuffer[rxIndex-1] = '\0';  // 替换换行为结束符
                    // 这里可以处理接收到的数据
                }
                rxIndex = 0;  // 清空缓冲区
            }
        }
        else
        {
            rxIndex = 0;  // 缓冲区满，清空
        }
    }
}

// 获取接收到的字符串（如果有完整的一行）
char* Serial_GetLine(void)
{
    for(int i = 0; i < rxIndex; i++)
    {
        if(rxBuffer[i] == '\r' || rxBuffer[i] == '\n')
        {
            rxBuffer[i] = '\0';
            char* result = (char*)rxBuffer;
            rxIndex = 0;  // 清空缓冲区
            return result;
        }
    }
    return NULL;  // 没有完整的一行
}