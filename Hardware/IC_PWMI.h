#ifndef __IC_H
#define __IC_H

#include "stm32f10x.h"                  // Device header

//===============================================================
//Base   TIM3_CH1(PA6)  And  TIM4_CH1(PB6)
//===============================================================


void TIM34_IC_PWMI_Init(void);
void TIM3_IC_PWMI_Init(void);
void TIM4_IC_PWMI_Init(void);

//TIM3_CH1(PA6)==PWM1
uint32_t IC_GetPWM1_Frequency(void);
uint16_t IC_GetPWM1_DutyCycle(void);
//TIM4_CH1(PB6)==PWM2
uint32_t IC_GetPWM2_Frequency(void);
uint16_t IC_GetPWM2_DutyCycle(void);



#endif



            //OLED显示PWM测量值
            //OLED_ShowNum(1,1,IC_GetPWM1_Frequency,6,OLED_8X16);
            //OLED_ShowNum(1,1,IC_GetPWM1_DutyCycle,6,OLED_8X16);
            //OLED_ShowNum(1,1,IC_GetPWM2_Frequency,6,OLED_8X16);
            //OLED_ShowNum(1,1,IC_GetPWM2_DutyCycle,6,OLED_8X16);
            


