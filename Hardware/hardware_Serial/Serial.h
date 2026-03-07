#ifndef __SERIAL_H
#define __SERIAL_H

#include "stm32f10x.h"
#include <stdio.h>
#include <stdarg.h>

#define USART1_BaudRate      9600          //波特率

// 缓冲区大小定义
#define SERIAL_RX_BUFFER_SIZE 128
#define SERIAL_TX_BUFFER_SIZE 128

// 串口状态标志
typedef struct {
    uint8_t rxBuffer[SERIAL_RX_BUFFER_SIZE];
    uint16_t rxWriteIndex;
    uint16_t rxReadIndex;
    uint16_t rxCount;
    
    uint8_t txBuffer[SERIAL_TX_BUFFER_SIZE];
    uint16_t txWriteIndex;
    uint16_t txReadIndex;
    uint16_t txCount;
    
    uint8_t dmaTxBusy;  // DMA发送忙标志
} Serial_TypeDef;

extern Serial_TypeDef Serial;

// 初始化函数
void Serial_Init(void);

// 接收函数
uint8_t Serial_ReceiveByte(void);
uint8_t Serial_Available(void);
void Serial_Flush(void);

// 发送函数
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void Serial_SendString(char *String);
void Serial_SendNumber(uint32_t Number, uint8_t Length);
void Serial_SendStruct(void *pStruct, uint16_t Size);
void Serial_Printf(char *format, ...);



// 中断回调函数
void USART1_IRQHandler(void);

#endif






