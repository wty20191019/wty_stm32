#include "PID_system.h"
#include <string.h>
#include <math.h>

// =========================================================
// 静态全局变量定义
// =========================================================

static PID_Controller pid_controllers[MAX_PID_CONTROLLERS];
static uint8_t pid_count = 0;

// =========================================================
// 公共函数实现
// =========================================================

void PID_System_Init(void)
{
    memset(pid_controllers, 0, sizeof(pid_controllers));
    pid_count = 0;
}

PID_Controller* PID_Create_Instance(void)
{
    if (pid_count >= MAX_PID_CONTROLLERS) {
        return NULL;
    }
    
    PID_Controller* pid = &pid_controllers[pid_count++];
    memset(pid, 0, sizeof(PID_Controller));
    
    return pid;
}

void PID_Update(PID_Controller* pid)
{
    if (pid == NULL) {
        return;
    }
    
    // 保存上次误差
    pid->Error1 = pid->Error0;
    
    // 计算本次误差
    pid->Error0 = pid->Target - pid->Actual;
    
    // 积分计算
    if (pid->Ki != 0.0f) {
        pid->ErrorInt += pid->Error0;  // 误差积分
    } else {
        pid->ErrorInt = 0.0f;  // Ki为0时，积分清零
    }
    
    // PID计算
    pid->Out = pid->Kp * pid->Error0
             + pid->Ki * pid->ErrorInt
             + pid->Kd * (pid->Error0 - pid->Error1);
    
    // 输出限幅
    if (pid->Out > pid->OutMax) {
        pid->Out = pid->OutMax;
    }
    if (pid->Out < pid->OutMin) {
        pid->Out = pid->OutMin;
    }
}

