#ifndef __MOTOR_H
#define __MOTOR_H

//===============================
/*
PWMA    (PA0 )      ch1
AN1     (PB4 )
AN2     (PB5 )

PWMB    (PA1 )      ch2
BN1     (PA15)
BN2     (PB3 )



*/
//=================


void Motor_Init(void);
void Motor_SetSpeed_TIM2_ch1_PWMA(int8_t ch1);
void Motor_SetSpeed_TIM2_ch2_PWMB(int8_t ch2);

#endif
