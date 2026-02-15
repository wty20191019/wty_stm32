#ifndef __SYSTICK_SCHEDULER_H
#define __SYSTICK_SCHEDULER_H

#include <stdint.h>

/* 配置：最多5个任务，1ms滴答 */
#define MAX_TASKS 5

/* 任务函数类型 */
typedef void (*TaskFunc)(void);

/* 优先级定义 */
#define PRIORITY_HIGH   0    // 最高优先级
#define PRIORITY_MID    1    // 中等优先级
#define PRIORITY_LOW    2    // 最低优先级

/* API */
void SCH_Init(void);                                          // 初始化
void SCH_AddTask(TaskFunc func, uint32_t period, uint8_t priority);  // 添加任务（带优先级）
void SCH_Start(void);                                         // 启动
void SCH_Tick(void);                                          // 滴答（放SysTick中断）

#endif

//  8888888888888888888888888888888888888888888888888//

//  void SysTick_Handler(void)
//  {
//      SCH_Tick();
//  }

//8888888888888888888888888888888888888888888888888//




//8888888888888888888888888888888888888888888888888//
//  main()
//  {


//  void SCH_Init(void);                                  // 初始化
//  void SCH_AddTask(TaskFunc func, uint32_t period, uint8_t priority);     // 添加任务
//                                   // 启动


//  }


//8888888888888888888888888888888888888888888888888//






