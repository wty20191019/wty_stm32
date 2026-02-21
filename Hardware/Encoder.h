#ifndef __ENCODER_H
#define __ENCODER_H

#include "stm32f10x.h"                  // Device header

//===============================================================
//Base   TIM3_CH1(PA67)  And  TIM4_CH1(PB67)
//===============================================================

void Encoder1_TIM3_Init(void);
void Encoder2_TIM4_Init(void);
int16_t Encoder1_TIM3_Encoder_Get(void);
int16_t Encoder2_TIM4_Encoder_Get(void);


#endif


    
            //OLED显示Encoder的计数
            //    OLED_ShowSignedNum(3 , 1 ,Encoder1_TIM3_Encoder_Get(),6,OLED_8X16);
            //    OLED_ShowSignedNum(3 , 9 ,Encoder2_TIM4_Encoder_Get(),6,OLED_8X16);





