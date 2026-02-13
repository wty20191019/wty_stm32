#ifndef __I2C2_H
#define __I2C2_H

#define clock_frequency  400  //(kHz)时钟速度,标准速度（1-100kHz）,快速（101-400kHZ） 

//===============================================
//硬件 I2C2
//===============================================
//Base   (PB11) (PB10)        
//        SDA    SCL          
//===============================================

void I2C2_Init(void);
uint8_t I2C2_WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT);

#endif
