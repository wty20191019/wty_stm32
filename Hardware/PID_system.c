#include "stm32f10x.h"                  // Device header
#include "PID_system.h"

void PID_Update(PID_t *p)
{
    /*获取本次误差和上次误差*/
    p->Error1 = p->Error0;
    p->Error0 = p->Target - p->Actual;
    
    /*外环误差积分（累加）*/
    if (p->Ki != 0)
    {
        p->ErrorInt += p->Error0;
    }
//    else                            
//    {
//        p->ErrorInt = 0;
//    }
    
    /*PID计算*/
    /*使用位置式PID公式，计算得到输出值*/
    p->Out = p->Kp * p->Error0
           + p->Ki * p->ErrorInt
           + p->Kd * (p->Error0 - p->Error1);
    
    /*输出限幅*/
    if (p->Out > p->OutMax) {p->Out = p->OutMax;}
    if (p->Out < p->OutMin) {p->Out = p->OutMin;}
}


