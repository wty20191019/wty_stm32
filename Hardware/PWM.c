#include "PWM.h"      

void TIM2_PWM_Init(void)
{
  // ===================================================================
    // 0. 瀹氫箟缁撴瀯浣撳彉閲忕敤浜庨厤缃瓽PIO銆佸畾鏃跺櫒鍜孭WM杈撳嚭
    // ===================================================================
    GPIO_InitTypeDef GPIO_InitStructure;           // GPIO閰嶇疆缁撴瀯浣
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure; // 瀹氭椂鍣ㄥ熀纭€閰嶇疆缁撴瀯浣
    TIM_OCInitTypeDef TIM_OCInitStructure;         // 瀹氭椂鍣ㄨ緭鍑烘瘮杈冮厤缃®缁撴瀯浣
    
    
    
    // ===================================================================
    // 1. 浣胯兘澶栬®炬椂閽
    // ===================================================================
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);       // 浣胯兘GPIOA绔¯鍙ｆ椂閽熷拰澶嶇敤鍔熻兘鏃堕挓(AFIO)// GPIOA鐢ㄤ簬杈撳嚭PWM淇″彿锛孉FIO鐢ㄤ簬寮曡剼澶嶇敤鍔熻兘閰嶇疆
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);                              // 浣胯兘TIM2瀹氭椂鍣ㄦ椂閽(浣嶄簬APB1鎬荤嚎涓)
    
    
    
    // ===================================================================
    // 2. 閰嶇疆GPIO寮曡剼涓哄¤嶇敤鎺ㄦ尳杈撳嚭妯″紡
    // ===================================================================
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;  // 閫夋嫨PA0-PA3鍥涗釜寮曡剼锛屽¯瑰簲TIM2鐨勫洓涓ª閫氶亾
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                                   // 璁剧疆涓哄¤嶇敤鎺ㄦ尳杈撳嚭妯″紡(GPIO_Mode_AF_PP)// 澶嶇敤鍔熻兘锛氬紩鑴氱敱瀹氭椂鍣ㄦ帶鍒讹紱鎺ㄦ尳锛氬彲杈撳嚭楂樹綆鐢靛钩
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                 // 璁剧疆寮曡剼閫熷害涓º50MHz锛岀‘淇濋珮閫烶WM淇″彿璐ㄩ噺
    GPIO_Init(GPIOA, &GPIO_InitStructure);                                            // 搴旂敤GPIO閰嶇疆鍒癎PIOA绔¯鍙£
    
    
    
    // ===================================================================
    // 3. 閰嶇疆TIM2瀹氭椂鍣ㄥ熀纭€鍙傛暟
    // =================================================================== 
    TIM_TimeBaseStructure.TIM_Period = 1000 - 1;                                        // 璁剧疆鑷ª鍔ㄩ噸瑁呰浇瀵勫瓨鍣¨(ARR)鍊间负999 // 璁℃暟鍣ㄤ粠0璁℃暟鍒°999锛屽叡1000涓ª璁℃暟鍛ㄦ湡
    TIM_TimeBaseStructure.TIM_Prescaler = 1440 - 1;                                     // 瀹氭椂鍣ㄦ椂閽 = 72MHz / 1440 = 50Hz // 璁剧疆棰勫垎棰戝櫒(PSC)鍊间负1440-1 
    TIM_TimeBaseStructure.TIM_ClockDivision = 1 - 1;                                    // 璁剧疆鏃堕挓鍒嗗壊涓º0(涓嶅垎棰)
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;                         // 璁剧疆璁℃暟鍣ㄦā寮忎负鍚戜笂璁℃暟 // 璁℃暟鍣ㄤ粠0閫掑¢炲埌ARR鍊硷紝鐒跺悗閲嶆柊浠0寮€濮
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);                                     // 搴旂敤閰嶇疆鍙傛暟鍒濆§嬪寲TIM2瀹氭椂鍣¨
    
    
    
    // ===================================================================
    // 
    // ===================================================================
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    
    // -------------------------------
    //(PA0 - TIM2_CH1)
    // -------------------------------
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
    // -------------------------------
    //(PA1 - TIM2_CH2)
    // -------------------------------  
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
    

    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
    
}

void TIM2_SetAllChannels(uint16_t ch1, uint16_t ch2)
{
    TIM_SetCompare1(TIM2, ch1);
    TIM_SetCompare2(TIM2, ch2);
}
