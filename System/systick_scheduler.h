#ifndef __SYSTICK_SCHEDULER_H
#define __SYSTICK_SCHEDULER_H

#include <stdint.h>
#include <stdbool.h>

/* 配置：最多10个任务，1ms滴答 */
#define MAX_TASKS 10
#define MAX_DELAY 0xFFFFFFFF

/* 任务函数类型 */
typedef void (*TaskFunc)(void);

/* 简化API - 不再需要任务句柄 */
void SCH_Init(void);                                    // 初始化
bool SCH_CreateTask(TaskFunc func, uint8_t priority);   // 创建任务（返回成功/失败）
void SCH_Delay(uint32_t delay_ms);                      // 相对延时
void SCH_DelayUntil(uint32_t target_tick);              // 绝对延时
void SCH_SuspendTask(TaskFunc func);                    // 挂起任务（通过函数指针）
void SCH_ResumeTask(TaskFunc func);                     // 恢复任务（通过函数指针）
void SCH_DeleteTask(TaskFunc func);                     // 删除任务（通过函数指针）

/* 调度器滴答（放在SysTick中断中） */
void SCH_Tick(void);

/* 优先级定义 - 增加到5个优先级 */
#define PRIORITY_HIGHEST  0    // 最高优先级
#define PRIORITY_HIGH     1    // 高优先级
#define PRIORITY_MID      2    // 中等优先级
#define PRIORITY_LOW      3    // 低优先级
#define PRIORITY_LOWEST   4    // 最低优先级

/* 获取系统时间 */
uint32_t SCH_GetTickCount(void);

#endif



//    // 创建任务
//    SCH_CreateTask(Task1, PRIORITY_HIGHEST);   // 最高优先级任务
//    SCH_CreateTask(Task2, PRIORITY_HIGH);      // 高优先级任务
//    SCH_CreateTask(Task3, PRIORITY_MID);       // 中等优先级任务
//    SCH_CreateTask(Task4, PRIORITY_LOW);       // 低优先级任务
//    SCH_CreateTask(Task5, PRIORITY_LOWEST);    // 最低优先级任务



//void Task1(void)//相对延时=
//{
//uint32_t period = 100;
////=========================
////功能实现


////-------------------------
//SCH_Delay(period);
//}



//void Task2(void) //绝对延时=
//{
//    uint32_t period = 100;
//    static uint32_t next_wake_time = 0;
//    if(next_wake_time == 0)
//    {
//        next_wake_time = SCH_GetTickCount() + period;
//    }
////=========================
////功能实现(1)


////-------------------------
//    // 设置绝对唤醒时间（每隔200ms执行一次）
//    next_wake_time += period;
//    SCH_DelayUntil(next_wake_time);
//}















