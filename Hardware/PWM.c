#include "PWM.h"      

/**
 * @brief TIM2定时器PWM输出初始化函数
 * 
 * 配置TIM2定时器的指定通道产生PWM信号（通过宏定义控制通道启用）
 * PWM频率 = 72MHz / ((1440) * (1000)) = 50Hz 
 * 占空比分辨率 = 1/1000 = 0.1%
 * 
 * @note 启用的通道对应引脚：
 *       CH1 -> PA0, CH2 -> PA1, CH3 -> PA2, CH4 -> PA3
 * @warning 使用前需确认系统时钟配置为72MHz
 */
void TIM2_PWM_Init(void)
{
    // 0. 定义结构体变量
    GPIO_InitTypeDef GPIO_InitStructure;           
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure; 
    TIM_OCInitTypeDef TIM_OCInitStructure;        
    uint16_t gpio_pin = 0;  // 用于拼接需要初始化的GPIO引脚

    // 1. 使能外设时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);       
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);                             

    // 2. 拼接需要初始化的GPIO引脚（仅启用的通道）
#ifdef TIM2_PWM_CH1_ENABLE
    gpio_pin |= GPIO_Pin_0;
#endif
#ifdef TIM2_PWM_CH2_ENABLE
    gpio_pin |= GPIO_Pin_1;
#endif
#ifdef TIM2_PWM_CH3_ENABLE
    gpio_pin |= GPIO_Pin_2;
#endif
#ifdef TIM2_PWM_CH4_ENABLE
    gpio_pin |= GPIO_Pin_3;
#endif

    // 仅当有通道启用时，才配置GPIO
    if (gpio_pin != 0)
    {
        GPIO_InitStructure.GPIO_Pin = gpio_pin;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;       
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     
        GPIO_Init(GPIOA, &GPIO_InitStructure);                
    }

    // 3. 配置TIM2定时器基础参数（无论多少通道，定时器基础配置不变）
    TIM_TimeBaseStructure.TIM_Period = 1000 - 1;                // ARR = 999，计数周期1000
    TIM_TimeBaseStructure.TIM_Prescaler = 1440 - 1;             // PSC = 1439，分频后时钟50kHz（72M/1440=50k）
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     // 设置时钟分割为0(不分频)
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);             

    // 4. 配置启用的通道的PWM参数
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;          
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  

#ifdef TIM2_PWM_CH1_ENABLE
    TIM_OCInitStructure.TIM_Pulse = 0;                         
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);                   
    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);          
#endif

#ifdef TIM2_PWM_CH2_ENABLE
    TIM_OCInitStructure.TIM_Pulse = 0;                         
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);                   
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);          
#endif

#ifdef TIM2_PWM_CH3_ENABLE
    TIM_OCInitStructure.TIM_Pulse = 0;                         
    TIM_OC3Init(TIM2, &TIM_OCInitStructure);                   
    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);          
#endif

#ifdef TIM2_PWM_CH4_ENABLE
    TIM_OCInitStructure.TIM_Pulse = 0;                         
    TIM_OC4Init(TIM2, &TIM_OCInitStructure);                   
    TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);          
#endif

    // 5. 使能定时器
    TIM_ARRPreloadConfig(TIM2, ENABLE);                        
    TIM_Cmd(TIM2, ENABLE);                                     
}

void TIM2_SetAllChannels(uint16_t ch1, uint16_t ch2, uint16_t ch3, uint16_t ch4)
{
    // 仅对启用的通道设置比较值，避免无效操作
#ifdef TIM2_PWM_CH1_ENABLE
    TIM_SetCompare1(TIM2, ch1);
#endif
#ifdef TIM2_PWM_CH2_ENABLE
    TIM_SetCompare2(TIM2, ch2);
#endif
#ifdef TIM2_PWM_CH3_ENABLE
    TIM_SetCompare3(TIM2, ch3);
#endif
#ifdef TIM2_PWM_CH4_ENABLE
    TIM_SetCompare4(TIM2, ch4);
#endif
}
