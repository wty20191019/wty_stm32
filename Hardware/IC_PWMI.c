#include "IC_PWMI.h"                  // Device header

//===============================================================
//Base   TIM3_CH1  And  TIM4_CH1
//===============================================================




void TIM3_IC_PWMI_Init(void)
{
    
    //===============================================================
    //1.配置GPIO
    //===============================================================

    // 开启TIM3时钟 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    
    // 开启GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);


    // 配置GPIO
    GPIO_InitTypeDef GPIO_InitStructure;                     /* GPIO初始化_结构体 */  
    
    // 配置PA6 (TIM3_CH1) 为上拉输入
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    
    
    //===============================================================
    //2.TIM3 PWMI模式初始化 - 测量PWM1(PA6)
    //===============================================================
    
    // TIM3选择内部时钟源
    TIM_InternalClockConfig(TIM3);

    // TIM3时基单元初始化 
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;           // 时钟分频不分频
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;       // 向上计数模式
    TIM_TimeBaseInitStructure.TIM_Period = 65536 - 1;                     // 自动重装载值ARR
    TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;                     // 预分频器PSC (72MHz/72=1MHz)
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;                  // 重复计数器(高级定时器用)
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
    
    // TIM3_CH1 PWMI模式配置 (PA6-PWM1测量) 
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;                       // 选择_CH1
    TIM_ICInitStructure.TIM_ICFilter = 0xF;                               // 输入滤波器参数(过滤抖动)
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;           // 上升沿触发捕获
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;                // 捕获预分频不分频
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;        // 输入信号直连接
    TIM_PWMIConfig(TIM3, &TIM_ICInitStructure);                           // PWMI配置(自动配置_CH2为相反极性)
    
    // TIM3选择触发源为TI1FP1 
    TIM_SelectInputTrigger(TIM3, TIM_TS_TI1FP1);
    
    // TIM3选择从模式为复位模式
    TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);                      // TI1上升沿时CNT归零
    
    //  使能TIM3定时器
    TIM_Cmd(TIM3, ENABLE);

}




void TIM4_IC_PWMI_Init(void)
{
    
    //===============================================================
    //1.配置GPIO
    //===============================================================

    // 开启TIM4时钟 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    
    // 开启GPIOB时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);


    // 配置GPIO
    GPIO_InitTypeDef GPIO_InitStructure;                     /* GPIO初始化_结构体 */  
    
    // 配置PB6 (TIM4_CH1) 为上拉输入 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    
    
    //===============================================================
    //2.TIM4 PWMI模式初始化 - 测量PWM2(PB6)
    //===============================================================
    
    // TIM4选择内部时钟源
    TIM_InternalClockConfig(TIM4);

    // TIM4时基单元初始化 
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;           // 时钟分频不分频
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;       // 向上计数模式
    TIM_TimeBaseInitStructure.TIM_Period = 65536 - 1;                     // 自动重装载值ARR
    TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;                     // 预分频器PSC (72MHz/72=1MHz)
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;                  // 重复计数器(高级定时器用)
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);
    
    // TIM4_CH1 PWMI模式配置 (PB6-PWM2测量) 
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;                       // 选择_CH1
    TIM_ICInitStructure.TIM_ICFilter = 0xF;                               // 输入滤波器参数(过滤抖动)
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;           // 上升沿触发捕获
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;                // 捕获预分频不分频
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;        // 输入信号直连接
    TIM_PWMIConfig(TIM4, &TIM_ICInitStructure);                           // PWMI配置(自动配置_CH2为相反极性)
    
    // TIM4选择触发源为TI1FP1 
    TIM_SelectInputTrigger(TIM4, TIM_TS_TI1FP1);
    
    // TIM4选择从模式为复位模式
    TIM_SelectSlaveMode(TIM4, TIM_SlaveMode_Reset);                      // TI1上升沿时CNT归零
    
    //  使能TIM4定时器
    TIM_Cmd(TIM4, ENABLE);

}







void TIM34_IC_PWMI_Init(void)
{
    TIM3_IC_PWMI_Init();
    TIM4_IC_PWMI_Init();
}






/**
 * @brief 读取TIM3_CH1(PA6)测量的PWM1频率
 * @return PWM频率值(单位:Hz)，范围取决于输入信号
 * @note 频率 = 定时器时钟频率 / 周期计数值 = 1MHz / (CCR1)
 */
uint32_t IC_GetPWM1_Frequency(void)
{
    uint32_t frequency = 0;
    uint16_t period_count = TIM_GetCapture1(TIM3);            // 读取TIM3的CCR1寄存器值(周期计数值)
    if(period_count != 0)
    {
        frequency = 1000000 / period_count;                  // 1MHz = 1000000Hz// 计算频率: Freq = 1MHz / period_count (因为定时器时钟为1MHz)
    }
    return frequency;
}

/**
 * @brief 读取TIM3_CH1(PA6)测量的PWM1占空比
 * @return PWM占空比值(0-10000表示0.00%-100.00%)，例如5000表示50.00%
 * @note 占空比 = (高电平时间 / 周期) * 10000，高电平时间存储在CCR2中
 */
uint16_t IC_GetPWM1_DutyCycle(void)
{
    uint16_t duty_cycle = 0;
    uint16_t period_count = TIM_GetCapture1(TIM3);                       // 读取TIM3的CCR1(周期)和CCR2(高电平时间)寄存器值
    uint16_t high_time_count = TIM_GetCapture2(TIM3);
    if(period_count != 0)                                                // 计算占空比: Duty = (high_time / period) * 10000 (放大100倍便于处理小数)
    {
        duty_cycle = (high_time_count * 10000) / period_count;
    }
    return (duty_cycle + 1);  // 返回值范围: 0-10000，对应0.00%-100.00%
}



/**
 * @brief 读取TIM4_CH1(PB6)测量的PWM2频率
 * @return PWM频率值(单位:Hz)，范围取决于输入信号
 */
uint32_t IC_GetPWM2_Frequency(void)
{
    uint32_t frequency = 0;
    
    // 读取TIM4的CCR1寄存器值(周期计数值)
    uint16_t period_count = TIM_GetCapture1(TIM4);
    
    // 计算频率: Freq = 1MHz / period_count
    if(period_count != 0)
    {
        frequency = 1000000 / period_count;
    }
    
    return frequency;
}

/**
 * @brief 读取TIM4_CH1(PB6)测量的PWM2占空比
 * @return PWM占空比值(0-10000表示0.00%-100.00%)
 */
uint16_t IC_GetPWM2_DutyCycle(void)
{
    uint16_t duty_cycle = 0;
    
    // 读取TIM4的CCR1(周期)和CCR2(高电平时间)寄存器值
    uint16_t period_count = TIM_GetCapture1(TIM4);
    uint16_t high_time_count = TIM_GetCapture2(TIM4);
    
    // 计算占空比: Duty = (high_time / period) * 10000
    if(period_count != 0)
    {
        duty_cycle = (high_time_count * 10000) / period_count;
    }
    
    return (duty_cycle + 1);
}


float IC_GetDutyCycle_Float(uint16_t duty_cycle)
{
    return (float)duty_cycle / 100.0f;
}
















//===============================================================//===============================================================//===============================================================




 

//void IC_Init(void)
//{
//    /*开启时钟*/
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);            //开启TIM3的时钟
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);            //开启GPIOA的时钟
//    
//    /*GPIO初始化*/ //将PA6引脚初始化为上拉输入
//    GPIO_InitTypeDef GPIO_InitStructure;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(GPIOA, &GPIO_InitStructure);                            
//    
//    /*配置时钟源*/ //选择TIM3为内部时钟，若不调用此函数，TIM默认也为内部时钟
//    TIM_InternalClockConfig(TIM3);        
//    
//    /*时基单元初始化*/
//    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;                 //定义结构体变量
//    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;        //时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
//    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;    //计数器模式，选择向上计数
//    TIM_TimeBaseInitStructure.TIM_Period = 65536 - 1;                  //计数周期，即ARR的值
//    TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;                  //预分频器，即PSC的值
//    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;               //重复计数器，高级定时器才会用到
//    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);                //将结构体变量交给TIM_TimeBaseInit，配置TIM3的时基单元
//    
//    /*PWMI模式初始化*/
//    TIM_ICInitTypeDef TIM_ICInitStructure;                            //定义结构体变量
//    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;                  //选择配置定时器_CH1
//    TIM_ICInitStructure.TIM_ICFilter = 0xF;                            //输入滤波器参数，可以过滤信号抖动
//    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;        //极性，选择为上升沿触发捕获
//    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;             //捕获预分频，选择不分频，每次信号都触发捕获
//    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;    //输入信号交叉，选择直通，不交叉
//    TIM_PWMIConfig(TIM3, &TIM_ICInitStructure);                        //将结构体变量交给TIM_PWMIConfig，配置TIM3的输入捕获_CH
//                                                                      //此函数同时会把另一个通道配置为相反的配置，实现PWMI模式

//    /*选择触发源及从模式*/
//    TIM_SelectInputTrigger(TIM3, TIM_TS_TI1FP1);                     //触发源选择TI1FP1
//    TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);                  //从模式选择复位
//                                                                     //即TI1产生上升沿时，会触发CNT归零
//    
//    /*TIM使能*/
//    TIM_Cmd(TIM3, ENABLE);            //使能TIM3，定时器开始运行
//}



//uint32_t IC_GetFreq(void)
//{
//    return 1000000 / (TIM_GetCapture1(TIM3) + 1);        //测周法得到频率fx = fc / N，这里不执行+1的操作也可
//}


//uint32_t IC_GetDuty(void)
//{
//    return (TIM_GetCapture2(TIM3) + 1) * 100 / (TIM_GetCapture1(TIM3) + 1);    //占空比Duty = CCR2 / CCR1 * 100，这里不执行+1的操作也可
//}



//------------------------------------------------------------------------------------- --------------------------------------------------------------------------------------------------------------------------------


//void IC_PWMI_Init(void)
//{
//    //===============================================================
//    //1.配置GPIO
//    //===============================================================

//    // 开启TIM3_4时钟 
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
//    
//    // 开启GPIOA_B时钟 (TIM3 CH1: PA6, TIM3 CH2: PA7)
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

//    //1.3.配置GPIO
//    GPIO_InitTypeDef GPIO_InitStructure;                     /* GPIO初始化_结构体 */  
//    
//    // 配置PA6 (TIM3_CH1) 为上拉输入
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(GPIOA, &GPIO_InitStructure);

//    // 配置PB6 (TIM4_CH1) 为上拉输入
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
//    GPIO_Init(GPIOB, &GPIO_InitStructure);




//    //===============================================================
//    //2.TIM3 PWMI模式初始化 - 测量PWM1(PA6)
//    //===============================================================
//    
//    // TIM3选择内部时钟源
//    TIM_InternalClockConfig(TIM3);

//    // TIM3时基单元初始化 
//    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
//    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;           // 时钟分频不分频
//    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;       // 向上计数模式
//    TIM_TimeBaseInitStructure.TIM_Period = 65536 - 1;                     // 自动重装载值ARR
//    TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;                     // 预分频器PSC (72MHz/72=1MHz)
//    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;                  // 重复计数器(高级定时器用)
//    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
//    
//    // TIM3_CH1 PWMI模式配置 (PA6-PWM1测量) 
//    TIM_ICInitTypeDef TIM_ICInitStructure;
//    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;                       // 选择_CH1
//    TIM_ICInitStructure.TIM_ICFilter = 0xF;                               // 输入滤波器参数(过滤抖动)
//    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;           // 上升沿触发捕获
//    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;                // 捕获预分频不分频
//    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;        // 输入信号直连接
//    TIM_PWMIConfig(TIM3, &TIM_ICInitStructure);                           // PWMI配置(自动配置_CH2为相反极性)
//    
//    // TIM3选择触发源为TI1FP1 
//    TIM_SelectInputTrigger(TIM3, TIM_TS_TI1FP1);
//    
//    // TIM3选择从模式为复位模式
//    TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);                      // TI1上升沿时CNT归零
//    
//    //  使能TIM3定时器
//    TIM_Cmd(TIM3, ENABLE);
//    
//    
//    
//    //===============================================================
//    //3.TIM4 PWMI模式初始化 - 测量PWM2(PB6)
//    //===============================================================
//    
//    // TIM4选择内部时钟源 
//    TIM_InternalClockConfig(TIM4);
//    
//    // TIM4时基单元初始化 
//    TIM_TimeBaseInitStructure.TIM_Period = 65536 - 1;                     // 自动重装载值ARR
//    TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;                     // 预分频器PSC (72MHz/72=1MHz)
//    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);
//    
//    // TIM4_CH1 PWMI模式配置 (PB6-PWM3测量)
//    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;                       // 选择_CH1
//    TIM_ICInitStructure.TIM_ICFilter = 0xF;                               // 输入滤波器参数(过滤抖动)
//    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;           // 上升沿触发捕获
//    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;                // 捕获预分频不分频
//    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;        // 输入信号直连接
//    TIM_PWMIConfig(TIM4, &TIM_ICInitStructure);                           // PWMI配置(自动配置_CH2为相反极性)
//    
//    // TIM4选择触发源为TI1FP1
//    TIM_SelectInputTrigger(TIM4, TIM_TS_TI1FP1);
//    
//    // TIM4选择从模式为复位模式
//    TIM_SelectSlaveMode(TIM4, TIM_SlaveMode_Reset);                      // TI1上升沿时CNT归零
//    
//    // 使能TIM4定时器 
//    TIM_Cmd(TIM4, ENABLE);
//}






