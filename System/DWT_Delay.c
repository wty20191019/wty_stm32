#include "DWT_Delay.h"
#include "stm32f10x.h"

// 初始化 DWT 用于 us 延时
void DWT_Init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;  // 启用 DWT
    DWT->CYCCNT = 0;                               // 清零计数器
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;            // 启动 CYCCNT
}

/**
  * @brief  微秒级延时（基于 DWT，不占用 SysTick）
  * @param  us: 延时微秒数（0~最大循环次数）
  * @retval 无
  */
void DWT_Delay_us(uint32_t us)
{

    uint32_t cycles = us * (SystemCoreClock / 1000000);  // 1us 需要的周期数
    uint32_t start = DWT->CYCCNT;
    while ((DWT->CYCCNT - start) < cycles);
    
}

/**
  * @brief  毫秒级延时
  */
void DWT_Delay_ms(uint32_t ms)
{
    while (ms--) {
        DWT_Delay_us(1000);
    }
}

/**
  * @brief  秒级延时
  */
void DWT_Delay_s(uint32_t s)
{
    while (s--) {
        DWT_Delay_ms(1000);
    }
}



