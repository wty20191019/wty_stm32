#ifndef __I2C_HAL_H
#define __I2C_HAL_H

#include "stm32f10x.h"
#include <stdint.h>
#include <stdbool.h>

// I2C超时配置
#define I2C_TIMEOUT         100000  // 循环等待超时
#define I2C_LONG_TIMEOUT    1000000 // 长超时

// 错误代码定义
typedef enum {
    I2C_OK          = 0x00,  // 操作成功
    I2C_ERROR       = 0x01,  // 通用错误
    I2C_BUSY        = 0x02,  // 总线忙
    I2C_TIMEOUT_ERR = 0x03,  // 超时错误
    I2C_ARB_LOST    = 0x04,  // 仲裁丢失
    I2C_ACK_FAIL    = 0x05,  // 应答失败
    I2C_OVERRUN     = 0x06,  // 过载/欠载错误
    I2C_PEC_ERROR   = 0x07,  // PEC错误
    I2C_SMBALERT    = 0x08   // SMBus告警
} I2C_Status;

// I2C端口配置结构体
typedef struct {
    GPIO_TypeDef* GPIOx;        // GPIO端口
    uint16_t SCL_Pin;           // SCL引脚
    uint16_t SDA_Pin;           // SDA引脚
    uint32_t RCC_APBPeriph;     // 时钟使能位
    I2C_TypeDef* I2Cx;          // I2C外设
    uint32_t ClockSpeed;        // 时钟速度（Hz）
} I2C_Config;

// 外部声明I2C配置（在hardware_I2C.c中定义）
extern I2C_Config I2C1_Config;
extern I2C_Config I2C2_Config;

// 函数声明
void I2C_GPIO_Init(I2C_Config* config);
void I2C_ConfigInit(I2C_TypeDef* I2Cx, uint32_t ClockSpeed);
I2C_Status I2C_CheckBusy(I2C_TypeDef* I2Cx, uint32_t timeout);
I2C_Status I2C_GenerateStart(I2C_TypeDef* I2Cx, uint32_t timeout);
I2C_Status I2C_GenerateStop(I2C_TypeDef* I2Cx, uint32_t timeout);
I2C_Status I2C_SendAddress(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction, uint32_t timeout);
I2C_Status I2C_SendByte(I2C_TypeDef* I2Cx, uint8_t data, uint32_t timeout);
uint8_t I2C_ReceiveByte(I2C_TypeDef* I2Cx, uint32_t timeout);
I2C_Status I2C_MemWrite(I2C_TypeDef* I2Cx, uint8_t devAddr, uint8_t memAddr, uint8_t* data, uint16_t len);
I2C_Status I2C_MemRead(I2C_TypeDef* I2Cx, uint8_t devAddr, uint8_t memAddr, uint8_t* data, uint16_t len);
I2C_Status I2C_WriteMulti(I2C_TypeDef* I2Cx, uint8_t devAddr, uint8_t regAddr, uint8_t* data, uint16_t len);
I2C_Status I2C_ReadMulti(I2C_TypeDef* I2Cx, uint8_t devAddr, uint8_t regAddr, uint8_t* data, uint16_t len);
I2C_Status I2C_WriteByte(I2C_TypeDef* I2Cx, uint8_t devAddr, uint8_t regAddr, uint8_t data);
uint8_t I2C_ReadByte(I2C_TypeDef* I2Cx, uint8_t devAddr, uint8_t regAddr);
I2C_Status I2C_WriteBuffer(I2C_TypeDef* I2Cx, uint8_t devAddr, uint8_t* data, uint16_t len);
I2C_Status I2C_ReadBuffer(I2C_TypeDef* I2Cx, uint8_t devAddr, uint8_t* data, uint16_t len);
I2C_Status I2C_IsDeviceReady(I2C_TypeDef* I2Cx, uint8_t devAddr, uint32_t trials);

// 简易初始化函数
void I2C_QuickInit(I2C_TypeDef* I2Cx, uint32_t ClockSpeed);

#endif /* __I2C_HAL_H */
