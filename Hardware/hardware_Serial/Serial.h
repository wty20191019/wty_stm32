#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdio.h>

// hardware_Serial   USART   TTL

void Serial_Init(void);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array, uint16_t Length);  //数组
void Serial_SendString(char *String);
void Serial_SendNumber(uint32_t Number, uint8_t Length);
void Serial_Printf(char *format, ...);


uint16_t Serial_ReceiveNonBlocking(void);
uint16_t Serial_Available(void);
uint8_t Serial_ReadByte(void);
uint16_t Serial_ReadBytes(uint8_t *buffer, uint16_t maxLen);
void Serial_Flush(void);



#endif
