#ifndef __PWM_H
#define __PWM_H

#include "stm32f10x.h"  


// TIM2_ch1 (PA0)
// TIM2_ch2 (PA1)

void TIM2_PWM_Init(void);
void TIM2_SetAllChannels(uint16_t ch1, uint16_t ch2);        // chx (0-999)



#endif

