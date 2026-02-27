#ifndef __SYSTICK_SCHEDULER_H
#define __SYSTICK_SCHEDULER_H

#include <stdint.h>

/* 配置：最多5个任务，1ms滴答 */
#define MAX_TASKS 5

/* 任务函数类型 */
typedef void (*TaskFunc)(void);

/* 优先级定义（共15级，数字越小优先级越高） */
#define PRIORITY_HIGHEST    0   // 最高优先级
#define PRIORITY_1          1
#define PRIORITY_2          2
#define PRIORITY_3          3
#define PRIORITY_4          4
#define PRIORITY_5          5
#define PRIORITY_6          6
#define PRIORITY_7          7
#define PRIORITY_8          8
#define PRIORITY_9          9
#define PRIORITY_10         10
#define PRIORITY_11         11
#define PRIORITY_12         12
#define PRIORITY_13         13
#define PRIORITY_14         14  // 最低优先级
#define PRIORITY_MAX        14  // 优先级最大值（用于边界检查）

/* 调度模式枚举（明确指定相对/绝对周期） */
typedef enum {
    TaskDelayUntil, // 绝对周期：按固定周期执行（如每10ms执行一次）
    TaskDelay       // 相对周期：执行完成后延时指定时间再执行
} TaskMode_t;

/* API修改：mode参数指定调度模式 */
void SCH_Init(void);                                                                    // 初始化
void SCH_AddTask(TaskFunc func, uint32_t period, uint8_t priority, TaskMode_t mode);    // mode参数
void SCH_Start(void);                                                                   // 启动
void SCH_Tick(void);                                                                    // 滴答（放SysTick中断）

#endif /* __SYSTICK_SCHEDULER_H */

