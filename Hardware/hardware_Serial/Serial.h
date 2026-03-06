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
extern char Serial_RxPacket[];
extern uint8_t Serial_RxFlag;


void Serial_Init(void);//串口初始化
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void Serial_SendString(char *String);
void Serial_SendNumber(uint32_t Number, uint8_t Length);
void Serial_Printf(char *format, ...);


#endif 






