#include "systick_scheduler.h"
#include "stm32f10x.h"

/* 任务结构 */
typedef struct {
    TaskFunc func;         // 任务函数
    uint32_t period;       // 周期(ms)
    uint32_t next_run;     // 下一次运行的tick值
    uint8_t priority;      // 优先级 (0=最高, 14=最低)
} Task_t;

static Task_t tasks[MAX_TASKS];  // 任务数组
static uint8_t task_count = 0;   // 任务数
static volatile uint32_t systick_cnt = 0;  // Systick计数器（volatile确保可见性）

/* 初始化 */
void SCH_Init(void)
{
    // 清空任务
    for(int i=0; i<MAX_TASKS; i++) {
        tasks[i].func = 0;
        tasks[i].period = 0;
        tasks[i].next_run = 0;
        tasks[i].priority = PRIORITY_MAX;
    }
    task_count = 0;
    systick_cnt = 0;
    
    // 配置SysTick 1ms中断
    SysTick_Config(SystemCoreClock / 1000);
}



/* 添加任务 */
void SCH_AddTask(TaskFunc func, uint32_t period, uint8_t priority)
{
    if(task_count < MAX_TASKS && func) {
        if(priority > PRIORITY_MAX) {
            priority = PRIORITY_MAX;
        }
        
        tasks[task_count].func = func;
        tasks[task_count].period = period;
        tasks[task_count].next_run = systick_cnt + period;  // 设置第一次运行时间
        tasks[task_count].priority = priority;
        task_count++;
    }
}

/* Systick中断处理函数 - 只增加计数器 */
void SCH_Tick(void)
{
    systick_cnt++;  // 只增加计数器
}

/* 获取当前systick值 */
uint32_t SCH_GetTick(void)
{
    return systick_cnt;
}

/* 调度任务执行 - 在主循环中调用 */
void SCH_Dispatch(void)
{
    static uint32_t last_check = 0;
    uint32_t current_tick = systick_cnt;
    
    // 避免在同一个tick内重复检查
    if(current_tick == last_check) {
        return;
    }
    last_check = current_tick;
    
    // 按优先级顺序执行任务 (0->1->2...->14)
    for(int prio = PRIORITY_HIGH; prio <= PRIORITY_MAX; prio++) {
        for(int i=0; i<task_count; i++) {
            // 只处理当前优先级的任务
            if(tasks[i].priority == prio && tasks[i].func != 0) {
                // 检查是否到达执行时间
                if(current_tick >= tasks[i].next_run) {
                    // 计算下一次运行时间
                    tasks[i].next_run = current_tick + tasks[i].period;
                    
                    // 防止时间溢出时的错误
                    if(tasks[i].next_run < current_tick) {
                        // 处理溢出，设置为最大合法值
                        tasks[i].next_run = 0xFFFFFFFF - tasks[i].period;
                    }
                    
                    // 执行任务
                    tasks[i].func();
                }
            }
        }
    }
}

/* 延迟函数 */
void SCH_Delay(uint32_t ms)
{
    uint32_t start_tick = systick_cnt;
    
    // 等待指定时间
    while((systick_cnt - start_tick) < ms) {
        // 可以在这里加入低功耗模式
        __WFI();  // 等待中断，降低功耗
    }
}

