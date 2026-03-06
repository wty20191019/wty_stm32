#include "stm32f10x.h"                  // Device header
#include "PWM.h"


void Motor_Init(void)
{
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
}

void Motor_SetSpeed_TIM2_ch1(int8_t ch1)
{
    if (ch1 >= 0)
    {
        GPIO_SetBits(GPIOA, GPIO_Pin_1);
        GPIO_ResetBits(GPIOA, GPIO_Pin_2);
        TIM_SetCompare1(TIM2, ch1);
    }
    else
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_1);
        GPIO_SetBits(GPIOA, GPIO_Pin_2);
        TIM_SetCompare1(TIM2, -ch1);
    }
}

void Motor_SetSpeed_TIM2_ch2(int8_t ch2)
{
    if (ch2 >= 0)
    {
        GPIO_SetBits(GPIOC, GPIO_Pin_15);
        GPIO_ResetBits(GPIOC, GPIO_Pin_14);
        TIM_SetCompare1(TIM2, ch2);
    }
    else
    {
        GPIO_ResetBits(GPIOC, GPIO_Pin_15);
        GPIO_SetBits(GPIOC, GPIO_Pin_14);
        TIM_SetCompare1(TIM2, -ch2);
    }
}

