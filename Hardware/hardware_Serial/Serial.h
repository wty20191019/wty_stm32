#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdio.h>

extern QueueHandle_t xSerialRxQueue;    // 用于接收数据的队列
extern QueueHandle_t xSerialTxQueue;    // 用于发送数据的队列

void Serial_Init(void);

// 供其他任务调用的非阻塞发送 API
void Serial_SendByte_Async(uint8_t Byte) ;
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void Serial_SendString_Async(char *String);
void Serial_SendNumber_Async(uint32_t Number, uint8_t Length);
void Serial_Printf_Async(char *format, ...);
void Serial_SendStruct_Async(void *pStruct, uint16_t Size);



#endif



