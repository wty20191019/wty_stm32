#include "stm32f10x.h"                  // Device header
#include "PWM.h"


void Motor_Init(void)
{
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0| GPIO_Pin_1 |GPIO_Pin_15 ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4| GPIO_Pin_5;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
}

void Motor_SetSpeed_TIM2_ch1_PWMA(int16_t ch1)
{
    if (ch1 >= 0)
    {
        GPIO_SetBits(GPIOB, GPIO_Pin_4);
        GPIO_ResetBits(GPIOB, GPIO_Pin_5);
        TIM_SetCompare1(TIM2, ch1);
    }
    else
    {
        GPIO_ResetBits(GPIOB, GPIO_Pin_4);
        GPIO_SetBits(GPIOB, GPIO_Pin_5);
        TIM_SetCompare1(TIM2, -ch1);
    }
}



void Motor_SetSpeed_TIM2_ch2_PWMB(int16_t ch2)
{
    if (ch2 >= 0)
    {
        GPIO_SetBits(GPIOA, GPIO_Pin_15);
        GPIO_ResetBits(GPIOB, GPIO_Pin_3);
        TIM_SetCompare1(TIM2, ch2);
    }
    else
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_15);
        GPIO_SetBits(GPIOB, GPIO_Pin_3);
        TIM_SetCompare1(TIM2, -ch2);
    }
}

