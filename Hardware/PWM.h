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

//void Turn_PWM_SetCompare1_i()                         //按键输入控制pwm占空比      //static uint16_t PWM_SetCompare1_i =0 ;
//{
//    // TIM2_SetAllChannels(25,58,91,125);             //TIM2的4路pwm占空比设置


//    if(ReadInputDataBit_GPIOB_Pin_1()==1) //短按
//    {
//     PWM_SetCompare1_i=((PWM_SetCompare1_i+1)<100
//                        ?PWM_SetCompare1_i+1
//                        :100);
//    }
//    else if(ReadInputDataBit_GPIOB_Pin_1()==2) //长按
//    {
//     PWM_SetCompare1_i=((PWM_SetCompare1_i+10)<100
//                        ?PWM_SetCompare1_i+10
//                        :100);
//    }
//    else if (ReadInputDataBit_GPIOB_Pin_11()==1)
//    {
//    PWM_SetCompare1_i=((PWM_SetCompare1_i-1)>0
//                        ?PWM_SetCompare1_i-1
//                        :0);
//    }
//    else if (ReadInputDataBit_GPIOB_Pin_11()==2)
//    {
//    PWM_SetCompare1_i=((PWM_SetCompare1_i-10)>0
//                        ?PWM_SetCompare1_i-10
//                        :0);
//    }
//};


