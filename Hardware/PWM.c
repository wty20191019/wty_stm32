#include "PWM.h"      

 


/**
 * @brief TIM2定时器PWM输出初始化函数
 * 
 * 配置TIM2定时器的4个通道(PA0-PA3)产生PWM信号
 * PWM频率 = 72MHz / ((144) * (1000)) = 0.5kHz
 * 占空比分辨率 = 1/1000 = 0.1%
 
 * PA0 -> ch1
 * PA1 -> ch2
 * PA2 -> ch3
 * PA3 -> ch4
 
 * @note 使用PA0-PA3引脚输出PWM，需要确保这些引脚未被其他功能占用
 * @warning 使用前需确认系统时钟配置为72MHz
 */
void TIM2_PWM_Init(void)
{

    // ===================================================================
    // 0. 定义结构体变量用于配置GPIO、定时器和PWM输出
    // ===================================================================
    GPIO_InitTypeDef GPIO_InitStructure;           // GPIO配置结构体
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure; // 定时器基础配置结构体
    TIM_OCInitTypeDef TIM_OCInitStructure;         // 定时器输出比较配置结构体
    
    
    
    // ===================================================================
    // 1. 使能外设时钟
    // ===================================================================
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);       // 使能GPIOA端口时钟和复用功能时钟(AFIO)// GPIOA用于输出PWM信号，AFIO用于引脚复用功能配置
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);                              // 使能TIM2定时器时钟(位于APB1总线上)
    
    
    
    // ===================================================================
    // 2. 配置GPIO引脚为复用推挽输出模式
    // ===================================================================
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;  // 选择PA0-PA3四个引脚，对应TIM2的四个通道
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                                   // 设置为复用推挽输出模式(GPIO_Mode_AF_PP)// 复用功能：引脚由定时器控制；推挽：可输出高低电平
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                 // 设置引脚速度为50MHz，确保高速PWM信号质量
    GPIO_Init(GPIOA, &GPIO_InitStructure);                                            // 应用GPIO配置到GPIOA端口
    
    
    
    // ===================================================================
    // 3. 配置TIM2定时器基础参数
    // =================================================================== 
    TIM_TimeBaseStructure.TIM_Period = 1000 - 1;                                        // 设置自动重装载寄存器(ARR)值为999 // 计数器从0计数到999，共1000个计数周期
    TIM_TimeBaseStructure.TIM_Prescaler = 1440 - 1;                                     // 定时器时钟 = 72MHz / 1440 = 50Hz // 设置预分频器(PSC)值为1440-1 
    TIM_TimeBaseStructure.TIM_ClockDivision = 1 - 1;                                    // 设置时钟分割为0(不分频)
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;                         // 设置计数器模式为向上计数 // 计数器从0递增到ARR值，然后重新从0开始
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);                                     // 应用配置参数初始化TIM2定时器
    
    
    
    // ===================================================================
    // 4. 配置PWM输出模式和参数
    // ===================================================================
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                                  // 设置输出比较模式为PWM模式1  // PWM模式1：当CNT < CCR时输出有效电平，CNT >= CCR时输出无效电平
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;                      // 使能定时器输出比较通道
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;                          // 设置输出极性为高电平有效  // 即：有效状态时输出高电平，无效状态时输出低电平
    
    // -------------------------------
    // 通道1配置 (PA0 - TIM2_CH1)
    // -------------------------------
    TIM_OCInitStructure.TIM_Pulse = 0;                                                  // 设置捕获/比较寄存器1(CCR1)的初始值为0
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);                                            // 初始化TIM2的通道1输出比较功能
    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);                                   // 使能TIM2通道1的预装载寄存器  // 预装载允许CCR值在更新事件时同步更新，避免产生毛刺
    
    // -------------------------------
    // 通道2配置 (PA1 - TIM2_CH2)
    // -------------------------------  
    TIM_OCInitStructure.TIM_Pulse = 0;                                                  // 设置CCR2初始值为
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);                                            // 初始化TIM2的通道2
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
    
    // -------------------------------
    // 通道3配置 (PA2 - TIM2_CH3)
    // -------------------------------
    TIM_OCInitStructure.TIM_Pulse = 0;                                                  // 设置CCR3初始值为
    TIM_OC3Init(TIM2, &TIM_OCInitStructure);                                            // 初始化TIM2的通道3
    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
    
    // -------------------------------
    // 通道4配置 (PA3 - TIM2_CH4)
    // -------------------------------
    TIM_OCInitStructure.TIM_Pulse = 0;                                                  // 设置CCR4初始值为
    TIM_OC4Init(TIM2, &TIM_OCInitStructure);                                            // 初始化TIM2的通道4
    TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
    
    // ===================================================================
    // 5. 使能定时器并启动PWM输出
    // ===================================================================
    TIM_ARRPreloadConfig(TIM2, ENABLE);                                                 // 使能TIM2的自动重装载预装载功能  // ARR值的更新将在更新事件发生时生效，确保PWM周期稳定 
    TIM_Cmd(TIM2, ENABLE);                                                              // 使能TIM2定时器，开始计数和PWM输出
    
}

void TIM2_SetAllChannels(uint16_t ch1, uint16_t ch2, uint16_t ch3, uint16_t ch4)        //chx (0-999)
{
    // 设置TIM2通道1-4的比较值(PA0-3输出)    // 直接写入CCR1寄存器，立即生效(因已启用预装载)
    TIM_SetCompare1(TIM2, ch1);
    TIM_SetCompare2(TIM2, ch2);
    TIM_SetCompare3(TIM2, ch3);
    TIM_SetCompare4(TIM2, ch4);
}








//###################################################################################################################################//





//void PWM_Init(void) 
//{
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);                              //  使能APB1 的TIM2时钟
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);                             //  打开GPIOA的GPIO时钟
//    
////    //引脚重映射
////    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);                              //  打开AFIO的GPIO时钟
////    GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2,ENABLE);                             //  配置TIM2部分重映射1
////    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);                          //  配置SWJ调试接口重映射，禁用JTAG但保留SWD功能
//                                                                                     //
//    GPIO_InitTypeDef GPIO_InitgSturcture;                                            //  结构体GPIO_InitTypeDef
//    GPIO_InitgSturcture.GPIO_Mode                       = GPIO_Mode_AF_PP;           //  GPIO_Mode 上拉输入
//    GPIO_InitgSturcture.GPIO_Pin                        = GPIO_Pin_0;                //  GPIO_Pin 引脚GPIO_Pin_0
//    GPIO_InitgSturcture.GPIO_Speed                      = GPIO_Speed_50MHz;          //  GPIO速度 GPIO_Speed_50MHz
//    GPIO_Init(GPIOA,&GPIO_InitgSturcture);                                           //  初始化  GPIOB
//                                                                                     //     
//    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitSturcture;                               //  结构体 TIM_TimeBaseInitSturcture
//    TIM_TimeBaseInitSturcture.TIM_ClockDivision     = TIM_CKD_DIV1;                  //  时钟分频：不分频（用于滤波器）
//    TIM_TimeBaseInitSturcture.TIM_CounterMode       = TIM_CounterMode_Up;            //  计数模式：向上计数
//    TIM_TimeBaseInitSturcture.TIM_Period            = 100 - 1;                       //  自动重装载值   // ARR   
//    TIM_TimeBaseInitSturcture.TIM_Prescaler         = 720 - 1;                       //  预分频器   //PSC
//    TIM_TimeBaseInitSturcture.TIM_RepetitionCounter = 0;                             //  重复计数器设为0 (高级时钟才有)
//    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitSturcture);                               //  初始化TIM2定时器时基单元
//                                                                                     //
//    TIM_Cmd(TIM2,ENABLE);                                                            //  使能TIM2定时器
//    
//    
//                                                                        
//    TIM_OCInitTypeDef TIM_OC_InitSturcture;                                     //结构体 TIM_OC_InitSturcture
//    TIM_OCStructInit(&TIM_OC_InitSturcture);                                    //结构体初始化，若结构体没有完整赋值  // 则最好执行此函数，给结构体所有成员都赋一个默认值 // 避免结构体初值不确定的问题
//    
//    
//    TIM_OC_InitSturcture.TIM_OCMode        = TIM_OCMode_PWM1;                   //输出比较模式
//    TIM_OC_InitSturcture.TIM_OCPolarity    = TIM_OCPolarity_High;               //输出极性
//    TIM_OC_InitSturcture.TIM_OutputState   = TIM_OutputState_Enable;            //输出使能
//    TIM_OC_InitSturcture.TIM_Pulse         = 0;                                 //初始的CCR值
//    TIM_OC1Init(TIM2, &TIM_OC_InitSturcture);                                   //配置TIM2的输出比较通道1
//                                                                                //
//    TIM_Cmd(TIM2, ENABLE);                                                      //使能TIM2

//}



//void PWM_SetCompare1(uint16_t Compare)
//{
//    TIM_SetCompare1(TIM2, Compare);         //设置CCR1的值
//}







