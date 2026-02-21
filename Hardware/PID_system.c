#include "PID_system.h"
#include <string.h>

// =========================================================
// 静态全局变量
// =========================================================

/**
 * @brief PID控制器实例数组
 * @note 静态分配，最大数量由MAX_PID_CONTROLLERS定义
 */
static PID_Controller pid_controllers[MAX_PID_CONTROLLERS];

/**
 * @brief 已创建的PID控制器数量
 * @note 用于管理控制器实例
 */
static uint8_t pid_count = 0;

// =========================================================
// 私有函数声明（仅在文件内部使用）
// =========================================================

/**
 * @brief 位置式PID计算
 * @param pid PID控制器实例
 * @param dt 时间间隔（秒）
 * @return 计算出的输出值
 * @note 位置式PID公式：u(t) = Kp*e(t) + Ki*∫e(t)dt + Kd*de(t)/dt
 */
static float PID_Calculate_Position(PID_Controller* pid, float dt);

/**
 * @brief 增量式PID计算
 * @param pid PID控制器实例
 * @param dt 时间间隔（秒）
 * @return 计算出的输出增量值
 * @note 增量式PID公式：Δu(t) = Kp*Δe(t) + Ki*e(t) + Kd*Δ²e(t)/dt
 */
static float PID_Calculate_Incremental(PID_Controller* pid, float dt);

// =========================================================
// 公共函数实现
// =========================================================

/**
 * @brief PID系统初始化
 * 
 * 初始化PID控制器数组，必须在创建任何PID实例前调用。
 * 该函数会将所有控制器状态清零，并重置计数器。
 */
void PID_System_Init(void)
{
    // 使用memset快速清零整个数组
    memset(pid_controllers, 0, sizeof(pid_controllers));
    
    // 重置创建计数器
    pid_count = 0;
}

/**
 * @brief 创建新的PID控制器实例
 * 
 * 分配一个新的PID控制器，并初始化其参数和状态。
 * 如果已达到最大实例数，则返回NULL。
 */
PID_Controller* PID_Create_Instance(
    float Kp,
    float Ki,
    float Kd,
    float integral_limit,
    float integral_threshold,
    float output_min,
    float output_max,
    PID_Mode mode)
{
    // 检查是否还有可用实例
    if (pid_count >= MAX_PID_CONTROLLERS) {
        return NULL;
    }
    
    // 获取新的PID实例
    PID_Controller* pid = &pid_controllers[pid_count];
    pid_count++;
    
    // 初始化所有字段为0
    memset(pid, 0, sizeof(PID_Controller));
    
    // 设置基本PID参数
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    
    // 设置积分相关参数
    pid->integral_limit = (integral_limit > 0) ? integral_limit : 1000.0f;  // 默认值
    pid->integral_threshold = (integral_threshold >= 0) ? integral_threshold : 0.0f;
    pid->integral_enabled = 1;  // 默认启用积分
    
    // 设置输出限制
    pid->output_min = output_min;
    pid->output_max = output_max;
    
    // 设置控制器模式
    pid->mode = mode;
    
    // 默认启用抗饱和
    pid->anti_windup_enabled = 1;
    
    // 设置默认微分滤波系数（0.1表示较强的滤波）
    pid->d_filter_coef = 0.1f;
    
    // 默认使能控制器
    pid->enabled = 1;
    
    // 重置控制器状态
    PID_Reset(pid);
    
    return pid;
}

/**
 * @brief 获取PID控制器实例
 * 
 * 通过索引获取已创建的PID控制器。
 * 索引按照创建顺序分配，从0开始。
 */
PID_Controller* PID_Get_Instance(uint8_t index)
{
    // 检查索引是否有效
    if (index >= pid_count) {
        return NULL;
    }
    
    return &pid_controllers[index];
}

/**
 * @brief 设置PID参数
 * 
 * 可以在运行时动态调整PID控制器的参数。
 * 注意：修改参数不会重置控制器状态。
 */
void PID_Set_Parameters(
    PID_Controller* pid,
    float Kp,
    float Ki,
    float Kd,
    float integral_limit,
    float integral_threshold,
    float output_min,
    float output_max)
{
    if (pid == NULL) {
        return;
    }
    
    // 设置基本PID参数
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    
    // 设置积分相关参数
    if (integral_limit > 0) {
        pid->integral_limit = integral_limit;
    }
    
    if (integral_threshold >= 0) {
        pid->integral_threshold = integral_threshold;
    }
    
    // 设置输出限制
    pid->output_min = output_min;
    pid->output_max = output_max;
    
    // 如果当前积分值超出新的限制，则进行限幅
    if (pid->integral > pid->integral_limit) {
        pid->integral = pid->integral_limit;
    } else if (pid->integral < -pid->integral_limit) {
        pid->integral = -pid->integral_limit;
    }
}

/**
 * @brief 设置控制器模式
 * 
 * 切换PID控制模式（位置式或增量式）。
 * 切换模式时会自动重置控制器状态。
 */
void PID_Set_Mode(PID_Controller* pid, PID_Mode mode)
{
    if (pid == NULL) {
        return;
    }
    
    // 只有在模式改变时才重置状态
    if (pid->mode != mode) {
        pid->mode = mode;
        PID_Reset(pid);
    }
}

/**
 * @brief 设置目标值
 * 
 * 设置PID控制器的目标值（设定点）。
 * 在控制过程中可以随时改变目标值。
 */
void PID_Set_Target(PID_Controller* pid, float target)
{
    if (pid == NULL) {
        return;
    }
    
    pid->target = target;
}

/**
 * @brief 更新PID计算（带时间参数）
 * 
 * 这是PID控制的核心函数，根据当前输入值和时间间隔计算输出值。
 * 该函数实现了完整的PID算法，包括：
 * 1. 误差计算
 * 2. 积分分离
 * 3. 积分限幅
 * 4. 抗饱和处理
 * 5. 微分滤波
 * 6. 输出限幅
 * 
 * @note dt参数必须大于0，建议使用vTaskDelayUntil保证固定的时间间隔
 */
float PID_Update(PID_Controller* pid, float input, float dt)
{
    // 检查输入参数有效性
    if (pid == NULL || !pid->enabled || dt < PID_MIN_DT) {
        return 0.0f;
    }
    
    // 保存时间间隔（用于调试）
    pid->last_dt = dt;
    pid->input = input;
    pid->update_count++;
    
    // 根据模式选择计算方法
    float output;
    if (pid->mode == PID_MODE_POSITION) {
        output = PID_Calculate_Position(pid, dt);
    } else {
        output = PID_Calculate_Incremental(pid, dt);
    }
    
    return output;
}

/**
 * @brief 快速PID更新（固定周期）
 * 
 * 适用于固定周期的简单应用，使用预定义的PID_UPDATE_BASE_PERIOD_MS作为时间间隔。
 */
float PID_Update_Fast(PID_Controller* pid, float input)
{
    float dt = PID_UPDATE_BASE_PERIOD_MS / 1000.0f;
    return PID_Update(pid, input, dt);
}

/**
 * @brief 位置式PID计算（内部函数）
 * 
 * 实现位置式PID算法：
 * u(t) = Kp * e(t) + Ki * ∫e(t)dt + Kd * de(t)/dt
 * 
 * 特点：输出直接对应控制量，适用于位置控制。
 */
static float PID_Calculate_Position(PID_Controller* pid, float dt)
{
    // 计算当前误差
    pid->error = pid->target - pid->input;
    
    // ==== 积分项计算 ====
    float integral_contribution = 0.0f;
    
    if (pid->integral_enabled) {
        // 积分分离：只有误差在阈值内时才进行积分
        if (fabsf(pid->error) <= pid->integral_threshold) {
            // 累积积分项
            pid->integral += pid->error * dt;
            
            // 积分限幅
            if (pid->integral > pid->integral_limit) {
                pid->integral = pid->integral_limit;
            } else if (pid->integral < -pid->integral_limit) {
                pid->integral = -pid->integral_limit;
            }
        }
        
        integral_contribution = pid->Ki * pid->integral;
    }
    
    // ==== 微分项计算 ====
    float derivative_contribution = 0.0f;
    
    if (pid->Kd != 0.0f) {
        // 计算原始微分（对误差的微分）
        float raw_derivative = (pid->error - pid->prev_error) / dt;
        
        // 应用一阶低通滤波减少噪声
        pid->derivative = pid->d_filter_coef * pid->derivative + 
                         (1.0f - pid->d_filter_coef) * raw_derivative;
        
        derivative_contribution = pid->Kd * pid->derivative;
    }
    
    // ==== 计算PID输出 ====
    float raw_output = pid->Kp * pid->error + 
                      integral_contribution + 
                      derivative_contribution;
    
    // ==== 输出限幅和抗饱和处理 ====
    pid->is_saturated = 0;
    
    if (raw_output > pid->output_max) {
        pid->output = pid->output_max;
        pid->is_saturated = 1;
        
        // 抗饱和处理：如果积分项导致输出饱和，且误差与积分方向相同，则停止积分
        if (pid->anti_windup_enabled && pid->integral_enabled && 
            pid->error * pid->Ki > 0) {
            // 回退积分更新
            pid->integral -= pid->error * dt;
        }
    } 
    else if (raw_output < pid->output_min) {
        pid->output = pid->output_min;
        pid->is_saturated = 1;
        
        // 抗饱和处理
        if (pid->anti_windup_enabled && pid->integral_enabled && 
            pid->error * pid->Ki < 0) {
            pid->integral -= pid->error * dt;
        }
    } 
    else {
        pid->output = raw_output;
        pid->is_saturated = 0;
    }
    
    // 保存当前误差为历史误差（用于下一次微分计算）
    pid->prev_error = pid->error;
    
    return pid->output;
}

/**
 * @brief 增量式PID计算（内部函数）
 * 
 * 实现增量式PID算法：
 * Δu(t) = Kp*Δe(t) + Ki*e(t) + Kd*Δ²e(t)/dt
 * u(t) = u(t-1) + Δu(t)
 * 
 * 特点：输出是控制量的增量，适用于速度控制，对系统扰动不敏感。
 */
static float PID_Calculate_Incremental(PID_Controller* pid, float dt)
{
    // 计算当前误差
    pid->error = pid->target - pid->input;
    
    // ==== 计算误差变化 ====
    float delta_error = pid->error - pid->prev_error;
    float delta2_error = delta_error - (pid->prev_error - pid->prev_prev_error);
    
    // ==== 计算输出增量 ====
    float delta_output = pid->Kp * delta_error + 
                        pid->Ki * pid->error * dt + 
                        pid->Kd * delta2_error / dt;
    
    // ==== 计算新输出 ====
    float new_output = pid->output + delta_output;
    
    // ==== 更新历史误差 ====
    pid->prev_prev_error = pid->prev_error;  // 保存上上次误差
    pid->prev_error = pid->error;            // 保存上次误差
    
    // ==== 输出限幅 ====
    pid->is_saturated = 0;
    
    if (new_output > pid->output_max) {
        pid->output = pid->output_max;
        pid->is_saturated = 1;
    } 
    else if (new_output < pid->output_min) {
        pid->output = pid->output_min;
        pid->is_saturated = 1;
    } 
    else {
        pid->output = new_output;
        pid->is_saturated = 0;
    }
    
    return pid->output;
}

/**
 * @brief 使能/禁用PID控制器
 * 
 * 禁用控制器时，输出保持为0，并且重置所有状态。
 * 使能控制器时，也会重置状态，从零开始控制。
 */
void PID_Enable(PID_Controller* pid, uint8_t enable)
{
    if (pid == NULL) {
        return;
    }
    
    pid->enabled = enable;
    
    if (!enable) {
        // 禁用时重置控制器
        PID_Reset(pid);
    }
}

/**
 * @brief 重置PID控制器状态
 * 
 * 清除所有中间状态变量（积分、微分、误差等），
 * 但保留PID参数（Kp, Ki, Kd等）。
 * 常用于系统启动或模式切换时。
 */
void PID_Reset(PID_Controller* pid)
{
    if (pid == NULL) {
        return;
    }
    
    // 重置所有状态变量
    pid->input = 0.0f;
    pid->error = 0.0f;
    pid->prev_error = 0.0f;
    pid->prev_prev_error = 0.0f;  // 增量式PID需要
    pid->integral = 0.0f;
    pid->derivative = 0.0f;
    pid->output = 0.0f;
    pid->is_saturated = 0;
    pid->update_count = 0;
    pid->last_dt = 0.0f;
}

/**
 * @brief 设置抗饱和功能
 * 
 * 抗饱和（Anti-Windup）功能用于防止积分项过度累积。
 * 当输出达到限幅值时，如果积分项仍在增加（误差方向不变），
 * 则停止积分或反向抑制积分，避免系统响应迟钝。
 */
void PID_Set_Anti_Windup(PID_Controller* pid, uint8_t enable)
{
    if (pid == NULL) {
        return;
    }
    
    pid->anti_windup_enabled = enable;
}

/**
 * @brief 设置微分滤波系数
 * 
 * 微分项对噪声敏感，通过低通滤波可以减少噪声影响。
 * 滤波系数范围0.0~1.0：
 * - 0.0：完全滤波（无微分响应）
 * - 1.0：无滤波（原始微分）
 * - 推荐值：0.1~0.3
 */
void PID_Set_Derivative_Filter(PID_Controller* pid, float filter_coef)
{
    if (pid == NULL) {
        return;
    }
    
    // 限制滤波系数在有效范围内
    if (filter_coef < 0.0f) {
        filter_coef = 0.0f;
    } else if (filter_coef > 1.0f) {
        filter_coef = 1.0f;
    }
    
    pid->d_filter_coef = filter_coef;
}

/**
 * @brief 设置积分分离阈值
 * 
 * 当误差绝对值小于此阈值时，积分项才起作用。
 * 这可以防止在大误差时积分项过度累积导致系统震荡。
 * 设为0表示始终启用积分。
 */
void PID_Set_Integral_Threshold(PID_Controller* pid, float threshold)
{
    if (pid == NULL) {
        return;
    }
    
    if (threshold < 0.0f) {
        threshold = 0.0f;
    }
    
    pid->integral_threshold = threshold;
}

/**
 * @brief 设置积分限幅值
 * 
 * 限制积分项的最大绝对值，防止积分饱和。
 * 通常设为输出限幅值的2~5倍。
 */
void PID_Set_Integral_Limit(PID_Controller* pid, float limit)
{
    if (pid == NULL || limit <= 0.0f) {
        return;
    }
    
    pid->integral_limit = limit;
    
    // 如果当前积分值超出新限制，则进行限幅
    if (pid->integral > limit) {
        pid->integral = limit;
    } else if (pid->integral < -limit) {
        pid->integral = -limit;
    }
}

/**
 * @brief 设置积分使能
 * 
 * 完全禁用积分功能，将控制器变为PD控制器。
 * 用于调试或某些不需要积分控制的场合。
 */
void PID_Set_Integral_Enable(PID_Controller* pid, uint8_t enable)
{
    if (pid == NULL) {
        return;
    }
    
    pid->integral_enabled = enable;
    
    if (!enable) {
        // 禁用积分时清零积分项
        pid->integral = 0.0f;
    }
}

/**
 * @brief 获取PID输出值
 */
float PID_Get_Output(PID_Controller* pid)
{
    return (pid != NULL) ? pid->output : 0.0f;
}

/**
 * @brief 获取当前误差
 */
float PID_Get_Error(PID_Controller* pid)
{
    return (pid != NULL) ? pid->error : 0.0f;
}

/**
 * @brief 获取积分项值
 */
float PID_Get_Integral(PID_Controller* pid)
{
    return (pid != NULL) ? pid->integral : 0.0f;
}

/**
 * @brief 获取微分项值
 */
float PID_Get_Derivative(PID_Controller* pid)
{
    return (pid != NULL) ? pid->derivative : 0.0f;
}

/**
 * @brief 获取控制器状态信息
 */
void PID_Get_Status(PID_Controller* pid, uint8_t* is_saturated, uint32_t* update_count, float* last_dt)
{
    if (pid == NULL) {
        return;
    }
    
    if (is_saturated != NULL) {
        *is_saturated = pid->is_saturated;
    }
    
    if (update_count != NULL) {
        *update_count = pid->update_count;
    }
    
    if (last_dt != NULL) {
        *last_dt = pid->last_dt;
    }
}

