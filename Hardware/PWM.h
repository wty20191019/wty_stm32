#ifndef __PWM_H
#define __PWM_H


//void PWM_Init(void);
//void PWM_SetCompare1(uint16_t Compare);                                                // (0-99)



//===============================================================
//* TIM2
//===============================================================
//* PA0 -> ch1
//* PA1 -> ch2
//* PA2 -> ch3
//* PA3 -> ch4

void TIM2_PWM_Init(void);
void TIM2_SetAllChannels(uint16_t ch1, uint16_t ch2, uint16_t ch3, uint16_t ch4);        //chx (0-999)
void TIM_SetCompare1(TIM_TypeDef* TIMx, uint16_t Compare1);
void TIM_SetCompare2(TIM_TypeDef* TIMx, uint16_t Compare2);
void TIM_SetCompare3(TIM_TypeDef* TIMx, uint16_t Compare3);
void TIM_SetCompare4(TIM_TypeDef* TIMx, uint16_t Compare4);



#endif

