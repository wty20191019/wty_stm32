#include "systick_scheduler.h"
#include "stm32f10x.h"

/* 任务结构 */
typedef struct {
    TaskFunc func;         // 任务函数
    uint32_t period;       // 周期(ms)
    uint32_t counter;      // 计数器
    uint8_t priority;      // 优先级 (0=最高, 2=最低)
} Task_t;

static Task_t tasks[MAX_TASKS];  // 任务数组
static uint8_t task_count = 0;   // 任务数
static uint32_t tick = 0;        // 系统滴答

/* 初始化 */
void SCH_Init(void)
{
    // 清空任务
    for(int i=0; i<MAX_TASKS; i++) {
        tasks[i].func = 0;
        tasks[i].period = 0;
        tasks[i].counter = 0;
        tasks[i].priority = PRIORITY_LOW;  // 默认最低优先级
    }
    task_count = 0;
    tick = 0;
    
    // 配置SysTick 1ms中断
    SysTick_Config(SystemCoreClock / 1000);
}

/* 启动调度器 */
void SCH_Start(void)
{
    // SysTick已在Init中启用
}

/* 添加任务（带优先级） */
void SCH_AddTask(TaskFunc func, uint32_t period, uint8_t priority)
{
    if(task_count < MAX_TASKS && func) {
        // 限制优先级在有效范围内
        if(priority > PRIORITY_LOW) {
            priority = PRIORITY_LOW;
        }
        
        tasks[task_count].func = func;
        tasks[task_count].period = period;
        tasks[task_count].counter = 0;
        tasks[task_count].priority = priority;
        task_count++;
    }
}

/* 滴答处理 - 放在SysTick中断里 */
void SCH_Tick(void)
{
    tick++;  // 系统时间+1ms
    
    // 按优先级顺序执行任务 (0->1->2)
    for(int prio = PRIORITY_HIGH; prio <= PRIORITY_LOW; prio++) {
        for(int i=0; i<task_count; i++) {
            // 只处理当前优先级的任务
            if(tasks[i].priority == prio && tasks[i].func != 0) {
                tasks[i].counter++;
                if(tasks[i].counter >= tasks[i].period) {
                    tasks[i].counter = 0;  // 重置计数器
                    tasks[i].func();        // 执行任务
                }
            }
        }
    }
}
