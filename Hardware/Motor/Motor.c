#include "stm32f10x.h"

void Motor_Init(void)
{
    /*开启时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
    
    /*初始化电机方向控制引脚*/
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    // 电机A方向控制引脚: PB14(正转), PB15(反转)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // 电机B方向控制引脚: PB8(正转), PB9(反转)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void Motor_Set_TIM2_ch1_PWMA(int16_t ch1)
{
    if (ch1 > 0)
    {
        GPIO_SetBits(GPIOB, GPIO_Pin_8);    // 正转
        GPIO_ResetBits(GPIOB, GPIO_Pin_9);
        TIM_SetCompare1(TIM2, ch1);
    }
    else
    {
        GPIO_ResetBits(GPIOB, GPIO_Pin_8);
        GPIO_SetBits(GPIOB, GPIO_Pin_9);    // 反转
        TIM_SetCompare1(TIM2, -ch1);
    }
}

void Motor_Set_TIM2_ch2_PWMB(int16_t ch2)
{
    if (ch2 > 0)
    {
        GPIO_SetBits(GPIOB, GPIO_Pin_14);     // 正转
        GPIO_ResetBits(GPIOB, GPIO_Pin_15);
        TIM_SetCompare2(TIM2, ch2);
    }
    else
    {
        GPIO_ResetBits(GPIOB, GPIO_Pin_14);
        GPIO_SetBits(GPIOB, GPIO_Pin_15);     // 反转
        TIM_SetCompare2(TIM2, -ch2);
    }
}
