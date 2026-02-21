#include "key.h"                  // Device header


// 常量定义
#define DEBOUNCE_COUNT_THRESHOLD 25    // 消抖阈值
#define LONG_PRESS_THRESHOLD 100       // 长按阈值


void Key_Init(void)
{
    /*开启时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);        //开启GPIOB的时钟
    
    /*GPIO初始化*/
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);                        //将PB1和PB11引脚初始化为上拉输入
}







uint8_t ReadInputDataBit_GPIOB_Pin_1(void)
{
    // 按键状态枚举定义
    // 用于管理按键在不同阶段的状态转换
    typedef enum {
        KEY_STATE_RELEASED = 0,    // 按键释放状态：按键处于未被按下的稳定状态
        KEY_STATE_DEBOUNCE,        // 消抖检测状态：检测到按键动作，正在进行消抖验证
        KEY_STATE_PRESSED,         // 按键按下确认状态：已通过消抖，确认按键被有效按下
        KEY_STATE_LONG_PRESS       // 长按状态：检测长按时间
    } KeyState;

    // 静态变量 - 在函数调用间保持其值不变
    static uint8_t debounce_count = 0;               // 消抖计数器：记录连续检测到相同状态的次数
    static uint16_t press_duration = 0;              // 按键按下持续时间计数器
    static KeyState key_state = KEY_STATE_RELEASED;  // 当前按键状态机状态，初始为释放状态
    static uint8_t last_pin_state = 1;               // 上一次读取的引脚状态，假设按键未按下时为高电平(1)
    

    // 局部变量
    uint8_t current_pin_state = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1);  // 读取当前引脚输入状态
    uint8_t key_result = 0;                                                 // 按键结果：0=无动作，1=短按，2=长按
    
    // 状态机处理：根据不同的按键状态执行相应的处理逻辑
    switch(key_state)
    {
        case KEY_STATE_RELEASED:
            // 当前状态：按键释放
            // 检测条件：当前状态为低电平(0)且上次状态为高电平(1)，即检测到下降沿
            if(current_pin_state == 0 && last_pin_state == 1)
            {  
                key_state = KEY_STATE_DEBOUNCE;  // 转移到消抖检测状态
                debounce_count = 0;              // 重置消抖计数器
                press_duration = 0;              // 重置按下持续时间计数器
            }
            break;
            
        case KEY_STATE_DEBOUNCE:
            // 当前状态：消抖检测中
            if(current_pin_state == 0)  // 仍然为低电平（按键确实被按下）
            {  
                debounce_count++;               // 增加连续检测计数
                
                // 消抖完成条件：连续DEBOUNCE_COUNT_THRESHOLD次检测到低电平
                if(debounce_count >= DEBOUNCE_COUNT_THRESHOLD)
                {  
                    key_state = KEY_STATE_PRESSED;  // 转移到按下确认状态
                    // 注意：此时不设置key_result，等待释放时判断长短按
                }
            }
            else
            {
                // 在消抖期间检测到高电平，说明是抖动信号，不是有效按键
                key_state = KEY_STATE_RELEASED;  // 回到释放状态重新开始检测
            }
            break;
            
        case KEY_STATE_PRESSED:
            // 当前状态：按键按下确认，开始计时并检测释放
            if(current_pin_state == 0)
            {
                // 按键仍然按下，增加持续时间计数
                press_duration++;
                
                // 检查是否达到长按阈值（1秒）
                if(press_duration >= LONG_PRESS_THRESHOLD)
                {
                    key_state = KEY_STATE_LONG_PRESS;  // 转移到长按状态
                    key_result = 2;                     // 返回长按结果
                }
            }
            else
            {
                // 按键释放，根据按下时间判断短按还是长按
                if(press_duration < LONG_PRESS_THRESHOLD)
                {
                    key_result = 1;  // 短按：按下时间小于1秒
                }
                // 如果已经达到长按阈值，已经在上面设置了key_result=2
                
                key_state = KEY_STATE_RELEASED;  // 回到释放状态
                debounce_count = 0;              // 重置计数器
                press_duration = 0;              // 重置持续时间
            }
            break;
            
        case KEY_STATE_LONG_PRESS:
            // 当前状态：长按已确认
            if(current_pin_state == 1)
            {
                // 按键释放，回到释放状态
                key_state = KEY_STATE_RELEASED;
                debounce_count = 0;              // 重置计数器
                press_duration = 0;              // 重置持续时间
                // key_result保持为2，直到下次有效按键
            }
            break;
            
        default:
            // 异常处理：如果状态机处于未知状态，强制重置为释放状态
            key_state = KEY_STATE_RELEASED;
            debounce_count = 0;
            press_duration = 0;
            break;
    }
    
    // 更新历史状态：保存当前引脚状态供下次调用时使用
    last_pin_state = current_pin_state;
    
    // 返回按键检测结果
    return key_result;
}






uint8_t ReadInputDataBit_GPIOB_Pin_11(void)
{
    // 按键状态枚举定义
    // 用于管理按键在不同阶段的状态转换
    typedef enum {
        KEY_STATE_RELEASED = 0,    // 按键释放状态：按键处于未被按下的稳定状态
        KEY_STATE_DEBOUNCE,        // 消抖检测状态：检测到按键动作，正在进行消抖验证
        KEY_STATE_PRESSED,         // 按键按下确认状态：已通过消抖，确认按键被有效按下
        KEY_STATE_LONG_PRESS       // 长按状态：检测长按时间
    } KeyState;

    // 静态变量 - 在函数调用间保持其值不变
    static uint8_t debounce_count = 0;               // 消抖计数器：记录连续检测到相同状态的次数
    static uint16_t press_duration = 0;              // 按键按下持续时间计数器
    static KeyState key_state = KEY_STATE_RELEASED;  // 当前按键状态机状态，初始为释放状态
    static uint8_t last_pin_state = 1;               // 上一次读取的引脚状态，假设按键未按下时为高电平(1)
    

    // 局部变量
    uint8_t current_pin_state = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11);  // 读取当前引脚输入状态
    uint8_t key_result = 0;                                                 // 按键结果：0=无动作，1=短按，2=长按
    
    // 状态机处理：根据不同的按键状态执行相应的处理逻辑
    switch(key_state)
    {
        case KEY_STATE_RELEASED:
            // 当前状态：按键释放
            // 检测条件：当前状态为低电平(0)且上次状态为高电平(1)，即检测到下降沿
            if(current_pin_state == 0 && last_pin_state == 1)
            {  
                key_state = KEY_STATE_DEBOUNCE;  // 转移到消抖检测状态
                debounce_count = 0;              // 重置消抖计数器
                press_duration = 0;              // 重置按下持续时间计数器
            }
            break;
            
        case KEY_STATE_DEBOUNCE:
            // 当前状态：消抖检测中
            if(current_pin_state == 0)  // 仍然为低电平（按键确实被按下）
            {  
                debounce_count++;               // 增加连续检测计数
                
                // 消抖完成条件：连续DEBOUNCE_COUNT_THRESHOLD次检测到低电平
                if(debounce_count >= DEBOUNCE_COUNT_THRESHOLD)
                {  
                    key_state = KEY_STATE_PRESSED;  // 转移到按下确认状态
                    // 注意：此时不设置key_result，等待释放时判断长短按
                }
            }
            else
            {
                // 在消抖期间检测到高电平，说明是抖动信号，不是有效按键
                key_state = KEY_STATE_RELEASED;  // 回到释放状态重新开始检测
            }
            break;
            
        case KEY_STATE_PRESSED:
            // 当前状态：按键按下确认，开始计时并检测释放
            if(current_pin_state == 0)
            {
                // 按键仍然按下，增加持续时间计数
                press_duration++;
                
                // 检查是否达到长按阈值（1秒）
                if(press_duration >= LONG_PRESS_THRESHOLD)
                {
                    key_state = KEY_STATE_LONG_PRESS;  // 转移到长按状态
                    key_result = 2;                     // 返回长按结果
                }
            }
            else
            {
                // 按键释放，根据按下时间判断短按还是长按
                if(press_duration < LONG_PRESS_THRESHOLD)
                {
                    key_result = 1;  // 短按：按下时间小于1秒
                }
                // 如果已经达到长按阈值，已经在上面设置了key_result=2
                
                key_state = KEY_STATE_RELEASED;  // 回到释放状态
                debounce_count = 0;              // 重置计数器
                press_duration = 0;              // 重置持续时间
            }
            break;
            
        case KEY_STATE_LONG_PRESS:
            // 当前状态：长按已确认
            if(current_pin_state == 1)
            {
                // 按键释放，回到释放状态
                key_state = KEY_STATE_RELEASED;
                debounce_count = 0;              // 重置计数器
                press_duration = 0;              // 重置持续时间
                // key_result保持为2，直到下次有效按键
            }
            break;
            
        default:
            // 异常处理：如果状态机处于未知状态，强制重置为释放状态
            key_state = KEY_STATE_RELEASED;
            debounce_count = 0;
            press_duration = 0;
            break;
    }
    
    // 更新历史状态：保存当前引脚状态供下次调用时使用
    last_pin_state = current_pin_state;
    
    // 返回按键检测结果
    return key_result;
}















 
// 
// 
//uint8_t ReadInputDataBit_GPIOB_Pin_1(void)
//{   
//    /**
//     * @brief 读取GPIOB Pin1引脚状态并进行按键消抖检测
//     * 
//     * 该函数每20ms调用一次，通过状态机和计数器实现硬件按键的软件消抖功能
//     * 能够有效过滤机械开关的抖动信号，确保每次按键只触发一次有效事件
//     * 
//     * @param 无
//     * @return uint8_t 按键检测结果：1=确认按键按下，0=无按键动作或抖动
//     * 
//     * @note 使用静态变量保持状态，函数调用间状态持续有效
//     * @note 消抖时间：3次检测 × 20ms = 60ms
//     */
//     
//     
//    // 按键状态枚举定义
//    // 用于管理按键在不同阶段的状态转换
//    typedef enum {
//        KEY_STATE_RELEASED = 0,    // 按键释放状态：按键处于未被按下的稳定状态
//        KEY_STATE_DEBOUNCE,        // 消抖检测状态：检测到按键动作，正在进行消抖验证
//        KEY_STATE_PRESSED,         // 按键按下确认状态：已通过消抖，确认按键被有效按下
//        KEY_STATE_LONG_PRESS       // 长按状态（可选）：可用于扩展长按功能
//    } KeyState;

//    // 静态变量 - 在函数调用间保持其值不变
//    
//    static uint8_t debounce_count = 0;               // 消抖计数器：记录连续检测到相同状态的次数
//    static KeyState key_state = KEY_STATE_RELEASED;  // 当前按键状态机状态，初始为释放状态
//    static uint8_t last_pin_state = 1;               // 上一次读取的引脚状态，假设按键未按下时为高电平(1)
//                                            
//    
//    // 局部变量
//    uint8_t current_pin_state = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1);  // 读取当前引脚输入状态
//    uint8_t key_pressed = 0;                                               // 按键按下标志：0=未确认按下，1=确认按下
//    
//    // 状态机处理：根据不同的按键状态执行相应的处理逻辑
//    switch(key_state)
//    {   
//        case KEY_STATE_RELEASED:
//            // 当前状态：按键释放
//            // 检测条件：当前状态为低电平(0)且上次状态为高电平(1)，即检测到下降沿
//            // 这表示可能发生了按键按下动作
//            if(current_pin_state == 0 && last_pin_state == 1)
//            {  
//                key_state = KEY_STATE_DEBOUNCE;  // 转移到消抖检测状态
//                debounce_count = 0;              // 重置消抖计数器
//            }
//            break;
//            
//        case KEY_STATE_DEBOUNCE:
//            // 当前状态：消抖检测中
//            // 检查当前引脚状态是否仍然为低电平（按键确实被按下）
//            if(current_pin_state == 0)
//            {  
//                debounce_count++;               // 增加连续检测计数
//                
//                // 消抖完成条件：连续3次检测到低电平（3 × 20ms = 60ms）
//                // 这个时间长度可以有效滤除大部分机械按键的抖动
//                if(debounce_count >= DEBOUNCE_COUNT_THRESHOLD)
//                {  
//                    key_state = KEY_STATE_PRESSED;  // 转移到按下确认状态
//                    key_pressed = 1;                // 设置按键按下标志，通知调用者
//                }
//            }
//            else
//            {
//                // 在消抖期间检测到高电平，说明是抖动信号，不是有效按键
//                key_state = KEY_STATE_RELEASED;  // 回到释放状态重新开始检测
//            }
//            break;
//            
//        case KEY_STATE_PRESSED:
//            // 当前状态：按键按下确认
//            // 等待按键释放：当检测到引脚回到高电平时，认为按键释放
//            if(current_pin_state == 1)
//            {  
//                key_state = KEY_STATE_RELEASED;  // 回到释放状态，准备检测下一次按键
//            }
//            // 注意：在此状态下，key_pressed保持为0，避免重复触发
//            // 只有在状态刚从DEBOUNCE转到PRESSED时才设置key_pressed=1
//            break;
//            
//        default:
//            // 异常处理：如果状态机处于未知状态，强制重置为释放状态
//            key_state = KEY_STATE_RELEASED;
//            break;
//    }
//    
//    // 更新历史状态：保存当前引脚状态供下次调用时使用
//    // 这是实现边沿检测的关键
//    last_pin_state = current_pin_state;
//    
//    // 返回按键检测结果：仅在确认按键按下时返回1，其他情况返回0
//    return key_pressed;  
//}



// 
//uint8_t ReadInputDataBit_GPIOB_Pin_11(void)
//{   


//    /**
//     * @brief 读取GPIOB Pin1引脚状态并进行按键消抖检测
//     * 
//     * 该函数每20ms调用一次，通过状态机和计数器实现硬件按键的软件消抖功能
//     * 能够有效过滤机械开关的抖动信号，确保每次按键只触发一次有效事件
//     * 
//     * @param 无
//     * @return uint8_t 按键检测结果：1=确认按键按下，0=无按键动作或抖动
//     * 
//     * @note 使用静态变量保持状态，函数调用间状态持续有效
//     * @note 消抖时间：3次检测 × 20ms = 60ms
//     */
//     
//     
//    // 按键状态枚举定义
//    // 用于管理按键在不同阶段的状态转换
//    typedef enum {
//        KEY_STATE_RELEASED = 0,    // 按键释放状态：按键处于未被按下的稳定状态
//        KEY_STATE_DEBOUNCE,        // 消抖检测状态：检测到按键动作，正在进行消抖验证
//        KEY_STATE_PRESSED,         // 按键按下确认状态：已通过消抖，确认按键被有效按下
//        KEY_STATE_LONG_PRESS       // 长按状态（可选）：可用于扩展长按功能
//    } KeyState;

//    // 静态变量 - 在函数调用间保持其值不变
//    
//    static uint8_t debounce_count = 0;               // 消抖计数器：记录连续检测到相同状态的次数
//    static KeyState key_state = KEY_STATE_RELEASED;  // 当前按键状态机状态，初始为释放状态
//    static uint8_t last_pin_state = 1;               // 上一次读取的引脚状态，假设按键未按下时为高电平(1)
//                                            
//    
//    // 局部变量
//    uint8_t current_pin_state = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11);  // 读取当前引脚输入状态
//    uint8_t key_pressed = 0;                                               // 按键按下标志：0=未确认按下，1=确认按下
//    
//    // 状态机处理：根据不同的按键状态执行相应的处理逻辑
//    switch(key_state)
//    {   
//        case KEY_STATE_RELEASED:
//            // 当前状态：按键释放
//            // 检测条件：当前状态为低电平(0)且上次状态为高电平(1)，即检测到下降沿
//            // 这表示可能发生了按键按下动作
//            if(current_pin_state == 0 && last_pin_state == 1)
//            {  
//                key_state = KEY_STATE_DEBOUNCE;  // 转移到消抖检测状态
//                debounce_count = 0;              // 重置消抖计数器
//            }
//            break;
//            
//        case KEY_STATE_DEBOUNCE:
//            // 当前状态：消抖检测中
//            // 检查当前引脚状态是否仍然为低电平（按键确实被按下）
//            if(current_pin_state == 0)
//            {  
//                debounce_count++;               // 增加连续检测计数
//                
//                // 消抖完成条件：连续3次检测到低电平（3 × 20ms = 60ms）
//                // 这个时间长度可以有效滤除大部分机械按键的抖动
//                if(debounce_count >= DEBOUNCE_COUNT_THRESHOLD)
//                {  
//                    key_state = KEY_STATE_PRESSED;  // 转移到按下确认状态
//                    key_pressed = 1;                // 设置按键按下标志，通知调用者
//                }
//            }
//            else
//            {
//                // 在消抖期间检测到高电平，说明是抖动信号，不是有效按键
//                key_state = KEY_STATE_RELEASED;  // 回到释放状态重新开始检测
//            }
//            break;
//            
//        case KEY_STATE_PRESSED:
//            // 当前状态：按键按下确认
//            // 等待按键释放：当检测到引脚回到高电平时，认为按键释放
//            if(current_pin_state == 1)
//            {  
//                key_state = KEY_STATE_RELEASED;  // 回到释放状态，准备检测下一次按键
//            }
//            // 注意：在此状态下，key_pressed保持为0，避免重复触发
//            // 只有在状态刚从DEBOUNCE转到PRESSED时才设置key_pressed=1
//            break;
//            
//        default:
//            // 异常处理：如果状态机处于未知状态，强制重置为释放状态
//            key_state = KEY_STATE_RELEASED;
//            break;
//    }
//    
//    // 更新历史状态：保存当前引脚状态供下次调用时使用
//    // 这是实现边沿检测的关键
//    last_pin_state = current_pin_state;
//    
//    // 返回按键检测结果：仅在确认按键按下时返回1，其他情况返回0
//    return key_pressed;  
//}
































