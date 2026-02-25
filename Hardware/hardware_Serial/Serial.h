#ifndef __SERIAL_H
#define __SERIAL_H

#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "queue.h"

// 声明队列句柄
extern QueueHandle_t xSerialRxQueue;
extern QueueHandle_t xSerialTxQueue;

// 函数声明
void Serial_Init(void);
void Serial_SendByte_Async(uint8_t Byte);
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void Serial_SendString_Async(char *String);
void Serial_SendNumber_Async(uint32_t Number, uint8_t Length);
void Serial_Printf_Async(char *format, ...);
void Serial_SendStruct_Async(void *pStruct, uint16_t Size);

#endif

