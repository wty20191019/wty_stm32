//#ifndef __SERIAL_H
//#define __SERIAL_H

//#include "stm32f10x.h"
//#include "FreeRTOS.h"
//#include "queue.h"
//                                         

//#define USART1_BaudRate      38400          //波特率  //4800 //9600  //38400  //115200
//                                           
//// 声明队列句柄
//extern QueueHandle_t xSerialRxQueue;
//extern QueueHandle_t xSerialTxQueue;

//// 函数声明
//void Serial_Init(void);
//void Serial_SendByte_Async(uint8_t Byte);
//void Serial_SendArray(uint8_t *Array, uint16_t Length);
//void Serial_SendString_Async(char *String);
//void Serial_SendNumber_Async(uint32_t Number, uint8_t Length);
//void Serial_Printf_Async(char *format, ...);
//void Serial_SendStruct_Async(void *pStruct, uint16_t Size);

//#endif

#ifndef __SERIAL_H
#define __SERIAL_H

#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "queue.h"


#define USART1_BaudRate      9600          //波特率  //4800 //9600  //38400  //115200


// 兼容性类型定义
typedef void* xComPortHandle;

//定义configLIBRARY_KERNEL_INTERRUPT_PRIORITY
#define configLIBRARY_KERNEL_INTERRUPT_PRIORITY 9

// 串口队列句柄声明
extern QueueHandle_t xSerialRxQueue;
extern QueueHandle_t xSerialTxQueue;

// 初始化函数
void Serial_Init(void);
xComPortHandle xSerialPortInitMinimal(unsigned long ulWantedBaud, unsigned portBASE_TYPE uxQueueLength);

// 接收函数
BaseType_t xSerialGetChar(xComPortHandle pxPort, signed char *pcRxedChar, TickType_t xBlockTime);

// 发送函数
BaseType_t xSerialPutChar(xComPortHandle pxPort, signed char cOutChar, TickType_t xBlockTime);
void vSerialPutString(xComPortHandle pxPort, const signed char * const pcString, unsigned short usStringLength);

// 高级发送函数（基于队列）
void Serial_SendByte_Async(uint8_t Byte);
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void Serial_SendString_Async(char *String);
void Serial_SendNumber_Async(uint32_t Number, uint8_t Length);
void Serial_SendStruct_Async(void *pStruct, uint16_t Size);
void Serial_Printf_Async(char *format, ...);

// 关闭函数（可选）
void vSerialClose(xComPortHandle xPort);

#endif







