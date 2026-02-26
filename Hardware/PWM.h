#ifndef __PWM_H
#define __PWM_H

#include "stm32f10x.h"  

// ======================== 通道使能宏定义 ========================
// 注释对应行以禁用该通道
#define TIM2_PWM_CH1_ENABLE  // 通道1 (PA0)
#define TIM2_PWM_CH2_ENABLE  // 通道2 (PA1)
//#define TIM2_PWM_CH3_ENABLE  // 通道3 (PA2)
//#define TIM2_PWM_CH4_ENABLE  // 通道4 (PA3)

// ======================== 函数声明 ========================
void TIM2_PWM_Init(void);
void TIM2_SetAllChannels(uint16_t ch1, uint16_t ch2, uint16_t ch3, uint16_t ch4);        // chx (0-999)



#endif

