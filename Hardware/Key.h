#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"                  // Device header


void Key_Init(void);
uint8_t ReadInputDataBit_GPIOB_Pin_1(void);
//uint8_t ReadInputDataBit_GPIOB_Pin_11(void);


#endif





//void Turn_PWM_SetCompare1_i()                         //按键输入控制pwm占空比      //static uint16_t PWM_SetCompare1_i =0 ;
//{
//    // TIM2_SetAllChannels(25,58,91,125);             //TIM2的4路pwm占空比设置


//    if(ReadInputDataBit_GPIOB_Pin_1()==1) //短按
//    {
//     PWM_SetCompare1_i=((PWM_SetCompare1_i+1)<100
//                        ?PWM_SetCompare1_i+1
//                        :100);
//    }
//    else if(ReadInputDataBit_GPIOB_Pin_1()==2) //长按
//    {
//     PWM_SetCompare1_i=((PWM_SetCompare1_i+10)<100
//                        ?PWM_SetCompare1_i+10
//                        :100);
//    }
//    else if (ReadInputDataBit_GPIOB_Pin_11()==1)
//    {
//    PWM_SetCompare1_i=((PWM_SetCompare1_i-1)>0
//                        ?PWM_SetCompare1_i-1
//                        :0);
//    }
//    else if (ReadInputDataBit_GPIOB_Pin_11()==2)
//    {
//    PWM_SetCompare1_i=((PWM_SetCompare1_i-10)>0
//                        ?PWM_SetCompare1_i-10
//                        :0);
//    }
//};









