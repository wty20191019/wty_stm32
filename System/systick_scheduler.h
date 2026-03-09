#ifndef __SYSTICK_SCHEDULER_H
#define __SYSTICK_SCHEDULER_H

//让systick不断自增，到一定数值时调用对应的任务，在主循环里调用一个函数来监控systick的值

#include <stdint.h>

/* 定义最大任务数 */
#define MAX_TASKS            16
#define PRIORITY_HIGH        0
#define PRIORITY_MAX         14

/* 任务函数指针类型 */
typedef void (*TaskFunc)(void);

/* 函数声明 */
void SCH_Init(void);
void SCH_Start(void);
void SCH_AddTask(TaskFunc func, uint32_t period, uint8_t priority);
void SCH_Tick(void);
uint32_t SCH_GetTick(void);
void SCH_Dispatch(void);
void SCH_Delay(uint32_t ms);

#endif /* __SYSTICK_SCHEDULER_H */

