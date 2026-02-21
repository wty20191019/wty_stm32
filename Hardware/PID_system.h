#ifndef __PID_SYSTEM_
#define __PID_SYSTEM_

#include "stm32f10x.h"
#include <string.h>

#define PID_UPDATE_PERIOD  500       // ms
#define MAX_PID_CONTROLLERS 4      // 系统中支持的最大PID控制器数量

// PID控制器结构体定义
typedef struct {
    float Kp;             // 比例系数
    float Ki;             // 积分系数
    float Kd;             // 微分系数
    float target;         // 目标值
    float input;          // 输入值（实际测量值）
    float error;          // 当前误差（target - input）
    float integral;       // 积分累积值
    float K_enabled_integral;
    float integral_limit; // 积分限幅值
    float derivative;     // 微分值
    float prev_error;     // 上一次的误差值
    float output;         // 输出值
    float output_min;     // 输出下限
    float output_max;     // 输出上限
    uint8_t enabled;      // 使能标志（1：使能，0：禁用）
} PID_Controller;

// 系统初始化
void PID_System_Init(void);

// 创建新的PID控制器实例
PID_Controller* PID_Create_Instance(float Kp,
                                   float Ki,
                                   float Kd,
                                   float K_enabled_integral,
                                   float integral_limit,
                                   float output_min,
                                   float output_max);

// 获取PID控制器实例指针  index 0~~MAX_PID_CONTROLLERS-1
PID_Controller* PID_Get_Instance(uint8_t index);

// 设置PID参数
void PID_Parameter_Set(PID_Controller *pid,
                        float Kp,
                        float Ki,
                        float Kd,
                        float K_enabled_integral,
                        float integral_limit,
                        float output_min,
                        float output_max);

// 设置目标值
void PID_SetTarget(PID_Controller *pid, float target);

// 更新PID计算
void PID_Update(PID_Controller *pid, float input);

// 使能/禁用PID控制器
void PID_Enable(PID_Controller *pid, uint8_t enable);

// 重置PID控制器状态
void PID_Reset(PID_Controller *pid);

#endif

