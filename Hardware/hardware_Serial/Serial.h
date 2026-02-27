#ifndef __SERIAL_H
#define __SERIAL_H


#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>


#define USART1_BaudRate 9600        // 串口波特率     //9600    //115200
#define USART1_RX_BUF_SIZE 64       // 接收缓冲区大小









//外部变量声明 
extern uint8_t USART1_RxBuffer[USART1_RX_BUF_SIZE];  // 接收缓冲区
extern uint16_t USART1_RxLen;                        // 接收数据长度
extern uint8_t USART1_RxFlag;                        // 接收完成标志（0：未完成，1：完成）


void Serial_Init(void);//串口初始化
uint8_t Serial_SendByte(uint8_t Byte);//串口发送单字节
void Serial_SendArray(uint8_t *Array, uint16_t Length);//串口发送数组
void Serial_SendString(char *String);//串口发送字符串
void Serial_SendNumber(uint32_t Number, uint8_t Length);//串口发送数字（指定长度）
void Serial_SendStruct(void *pStruct, uint16_t Size);//串口发送结构体（按字节发送）
void Serial_Printf(char *format, ...);//串口格式化打印（安全版）

uint8_t Serial_GetRxData(uint8_t *pBuf, uint16_t *pLen);//获取串口接收缓冲区数据

#endif 






