#ifndef __PID_SYSTEM_H
#define __PID_SYSTEM_H

#include <stdint.h>
#include <math.h>

// =========================================================
// 配置宏定义
// =========================================================

/**
 * @brief PID更新周期（毫秒）
 */
#define PID_UPDATE_BASE_PERIOD_MS  500

/**
 * @brief 系统中支持的最大PID控制器数量
 */
#define MAX_PID_CONTROLLERS         4

/**
 * @brief 最小有效时间间隔（秒）
 */
#define PID_MIN_DT                  0.001f

// =========================================================
// 枚举定义
// =========================================================

/**
 * @brief PID控制模式枚举
 */
typedef enum {
    PID_MODE_POSITION = 0,      // 位置式PID
    PID_MODE_INCREMENTAL        // 增量式PID
} PID_Mode;

// =========================================================
// 结构体定义
// =========================================================

/**
 * @brief PID控制器结构体
 */
typedef struct {
    // ====== 基本PID参数 ======
    float Kp;
    float Ki;
    float Kd;
    float target;
    float input;
    float output;
    
    // ====== 误差项 ======
    float error;
    float prev_error;
    float prev_prev_error;      // 新增：用于增量式PID的二次差分
    
    // ====== 积分项管理 ======
    float integral;
    float integral_limit;
    float integral_threshold;
    uint8_t integral_enabled;
    
    // ====== 微分项管理 ======
    float derivative;
    float d_filter_coef;        // 修正：删除了prev_input，微分使用误差差分
    
    // ====== 抗饱和处理 ======
    uint8_t anti_windup_enabled;
    uint8_t is_saturated;
    
    // ====== 输出限制 ======
    float output_min;
    float output_max;
    
    // ====== 控制器状态 ======
    PID_Mode mode;
    uint8_t enabled;
    uint32_t update_count;
    float last_dt;
    
} PID_Controller;

// =========================================================
// 函数声明（与之前相同，保持不变）
// =========================================================

void PID_System_Init(void);
PID_Controller* PID_Create_Instance(float Kp, float Ki, float Kd, float integral_limit,
                                   float integral_threshold, float output_min, float output_max,
                                   PID_Mode mode);
PID_Controller* PID_Get_Instance(uint8_t index);
void PID_Set_Parameters(PID_Controller* pid, float Kp, float Ki, float Kd,
                       float integral_limit, float integral_threshold,
                       float output_min, float output_max);
void PID_Set_Mode(PID_Controller* pid, PID_Mode mode);
void PID_Set_Target(PID_Controller* pid, float target);
float PID_Update(PID_Controller* pid, float input, float dt);
float PID_Update_Fast(PID_Controller* pid, float input);
void PID_Enable(PID_Controller* pid, uint8_t enable);
void PID_Reset(PID_Controller* pid);
void PID_Set_Anti_Windup(PID_Controller* pid, uint8_t enable);
void PID_Set_Derivative_Filter(PID_Controller* pid, float filter_coef);
void PID_Set_Integral_Threshold(PID_Controller* pid, float threshold);
void PID_Set_Integral_Limit(PID_Controller* pid, float limit);
void PID_Set_Integral_Enable(PID_Controller* pid, uint8_t enable);
float PID_Get_Output(PID_Controller* pid);
float PID_Get_Error(PID_Controller* pid);
float PID_Get_Integral(PID_Controller* pid);
float PID_Get_Derivative(PID_Controller* pid);
void PID_Get_Status(PID_Controller* pid, uint8_t* is_saturated,
                   uint32_t* update_count, float* last_dt);

#endif /* __PID_SYSTEM_H */

