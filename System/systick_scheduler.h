#ifndef __SYSTICK_SCHEDULER_H
#define __SYSTICK_SCHEDULER_H

#include <stdint.h>

// 任务在中断中运行-对时序无要求的应用周期设为最小

/* 配置：最多任务数，1ms滴答 */
#define MAX_TASKS 10

/* 优先级定义：范围 0-14，0为最高，14为最低 */
#define PRIORITY_HIGH   0    // 最高优先级
#define PRIORITY_MAX    14   // 定义最大（最低）优先级**

/* 任务函数类型 */
typedef void (*TaskFunc)(void);

/* API */
void SCH_Init(void);                                          // 初始化
void SCH_AddTask(TaskFunc func, uint32_t period, uint8_t priority);  // 添加任务（带优先级，范围0-14）
void SCH_Start(void);                                         // 启动
void SCH_Tick(void);                                          // 滴答（放SysTick中断）

#endif





