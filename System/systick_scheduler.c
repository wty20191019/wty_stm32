#include "systick_scheduler.h"
#include "stm32f10x.h"
#include <stddef.h> 

/* 任务结构（包含模式字段） */
typedef struct {
    TaskFunc func;          // 任务函数
    uint32_t period;        // 周期(ms)
    uint8_t priority;       // 优先级 (0=最高, 14=最低)
    TaskMode_t mode;        // 调度模式（绝对/相对周期）
    uint32_t last_exec_tick;// 上次执行的系统滴答值（用于周期计算）
    uint8_t is_init;        // 初始化标记（0=未初始化，1=已初始化）
} Task_t;

static Task_t tasks[MAX_TASKS];  // 任务数组
static uint8_t task_count = 0;   // 任务数
static uint32_t sys_tick = 0;    // 系统总滴答数（ms）

/* 初始化调度器 */
void SCH_Init(void)
{
    // 清空所有任务
    for (int i = 0; i < MAX_TASKS; i++) {
        tasks[i].func = NULL;
        tasks[i].period = 0;
        tasks[i].priority = PRIORITY_MAX;
        tasks[i].mode = TaskDelayUntil;  // 默认绝对周期
        tasks[i].last_exec_tick = 0;
        tasks[i].is_init = 0;
    }
    task_count = 0;
    sys_tick = 0;
    
    // 配置SysTick 1ms中断（72MHz系统时钟）
    SysTick_Config(SystemCoreClock / 1000);
}

/* 启动调度器 */
void SCH_Start(void)
{
    // SysTick已在Init中启用
}

/* 修改：新增mode参数，指定相对/绝对周期 */
void SCH_AddTask(TaskFunc func, uint32_t period, uint8_t priority, TaskMode_t mode)
{
    // 边界检查：任务数未超限、函数非空、周期大于0
    if (task_count >= MAX_TASKS || func == NULL || period == 0) {
        return;
    }
    
    // 优先级边界检查（限制在0~14之间）
    if (priority > PRIORITY_MAX) {
        priority = PRIORITY_MAX;
    }
    
    // 初始化任务参数（包含传入的mode）
    tasks[task_count].func = func;
    tasks[task_count].period = period;
    tasks[task_count].priority = priority;
    tasks[task_count].mode = mode;  // 赋值传入的调度模式
    tasks[task_count].last_exec_tick = sys_tick;
    tasks[task_count].is_init = 1;
    task_count++;
}

/* 滴答处理 - 放在SysTick中断中 */
void SCH_Tick(void)
{
    sys_tick++;  // 系统时间+1ms
    
    // 按优先级从高到低执行（0 -> 14）
    for (uint8_t prio = PRIORITY_HIGHEST; prio <= PRIORITY_MAX; prio++) {
        for (int i = 0; i < task_count; i++) {
            // 跳过未初始化/无效的任务
            if (tasks[i].is_init == 0 || tasks[i].func == NULL) {
                continue;
            }
            
            // 只处理当前优先级的任务
            if (tasks[i].priority != prio) {
                continue;
            }
            
            // 1. 绝对周期调度：固定周期执行
            if (tasks[i].mode == TaskDelayUntil) {
                if ((sys_tick - tasks[i].last_exec_tick) >= tasks[i].period) {
                    tasks[i].func();                  // 执行任务
                    tasks[i].last_exec_tick = sys_tick;  // 更新上次执行时间
                }
            }
            // 2. 相对周期调度：执行完成后延时period再执行
            else if (tasks[i].mode == TaskDelay) {
                if ((sys_tick - tasks[i].last_exec_tick) >= tasks[i].period) {
                    tasks[i].func();                  // 执行任务
                    tasks[i].last_exec_tick = sys_tick;  // 任务执行后更新时间
                }
            }
        }
    }
}



///* SysTick中断服务函数（需放在stm32f10x_it.c中） */
//void SysTick_Handler(void)
//{
//    SCH_Tick();  // 调用调度器滴答处理
//}


