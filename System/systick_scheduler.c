#include "systick_scheduler.h"
#include "stm32f10x.h"
#include <string.h>  // 用于memset

/* 任务状态 */
typedef enum {
    TASK_READY,         // 就绪态
    TASK_BLOCKED,       // 阻塞态（延时中）
    TASK_SUSPENDED,     // 挂起态
    TASK_DELETED        // 删除态
} TaskState_t;

/* 任务控制块结构 */
typedef struct {
    TaskFunc func;              // 任务函数指针
    uint32_t wakeup_tick;       // 任务唤醒的绝对时间（tick）
    uint8_t priority;           // 优先级 (0=最高, 4=最低)
    TaskState_t state;          // 任务状态
    uint8_t task_id;            // 任务ID
    void* next;                 // 指向下一个任务（用于就绪链表）
    uint8_t in_ready_list;      // 标记是否在就绪列表中
} TaskControlBlock_t;

/* 全局变量 */
static TaskControlBlock_t tasks[MAX_TASKS];                   // 任务数组
static TaskControlBlock_t* ready_list[5] = {NULL, NULL, NULL, NULL, NULL}; // 就绪链表，按优先级分组
static uint8_t task_count = 0;                                // 当前任务数
static uint32_t system_tick = 0;                              // 系统滴答计数器
static TaskControlBlock_t* current_task = NULL;               // 当前正在执行的任务
static uint8_t scheduler_running = 0;                         // 调度器运行标志

/* 私有函数声明 */
static TaskControlBlock_t* SCH_FindTaskByFunc(TaskFunc func);
static void SCH_InsertToReadyList(TaskControlBlock_t* task);
static void SCH_RemoveFromReadyList(TaskControlBlock_t* task);
static TaskControlBlock_t* SCH_GetNextReadyTask(void);
static void SCH_CheckBlockedTasks(void);

/* 初始化调度器 */
void SCH_Init(void)
{
    // 清空所有任务控制块
    memset(tasks, 0, sizeof(tasks));
    
    for(int i = 0; i < MAX_TASKS; i++) {
        tasks[i].state = TASK_DELETED;
        tasks[i].task_id = i;
    }
    
    // 清空就绪链表
    for(int i = 0; i <= PRIORITY_LOWEST; i++) {
        ready_list[i] = NULL;
    }
    
    task_count = 0;
    system_tick = 0;
    current_task = NULL;
    scheduler_running = 0;
    
    // 配置SysTick 1ms中断
    SysTick_Config(SystemCoreClock / 1000);
}

/* 根据函数指针查找任务 */
static TaskControlBlock_t* SCH_FindTaskByFunc(TaskFunc func)
{
    if(func == NULL) return NULL;
    
    for(int i = 0; i < MAX_TASKS; i++) {
        if(tasks[i].state != TASK_DELETED && tasks[i].func == func) {
            return &tasks[i];
        }
    }
    return NULL;
}

/* 创建任务 - 简化API，只需要函数指针和优先级 */
bool SCH_CreateTask(TaskFunc func, uint8_t priority)
{
    if(task_count >= MAX_TASKS || func == NULL) {
        return false;
    }
    
    // 检查是否已存在相同函数指针的任务
    if(SCH_FindTaskByFunc(func) != NULL) {
        return false;  // 任务已存在
    }
    
    // 查找空闲的任务控制块
    TaskControlBlock_t* new_task = NULL;
    for(int i = 0; i < MAX_TASKS; i++) {
        if(tasks[i].state == TASK_DELETED) {
            new_task = &tasks[i];
            break;
        }
    }
    
    if(new_task == NULL) {
        return false;
    }
    
    // 初始化任务控制块
    new_task->func = func;
    new_task->wakeup_tick = system_tick; // 立即就绪
    new_task->priority = (priority > PRIORITY_LOWEST) ? PRIORITY_LOWEST : priority;
    new_task->state = TASK_READY;
    new_task->next = NULL;
    new_task->in_ready_list = 0;
    
    // 插入就绪链表
    SCH_InsertToReadyList(new_task);
    
    task_count++;
    return true;
}

/* 挂起任务 - 通过函数指针 */
void SCH_SuspendTask(TaskFunc func)
{
    TaskControlBlock_t* task = SCH_FindTaskByFunc(func);
    if(task != NULL && task->state == TASK_READY) {
        task->state = TASK_SUSPENDED;
        SCH_RemoveFromReadyList(task);
    }
}

/* 恢复任务 - 通过函数指针 */
void SCH_ResumeTask(TaskFunc func)
{
    TaskControlBlock_t* task = SCH_FindTaskByFunc(func);
    if(task != NULL && task->state == TASK_SUSPENDED) {
        task->state = TASK_READY;
        task->wakeup_tick = system_tick; // 立即就绪
        SCH_InsertToReadyList(task);
    }
}

/* 删除任务 - 通过函数指针 */
void SCH_DeleteTask(TaskFunc func)
{
    TaskControlBlock_t* task = SCH_FindTaskByFunc(func);
    if(task != NULL) {
        task->state = TASK_DELETED;
        SCH_RemoveFromReadyList(task);
        task_count--;
        
        // 清空任务控制块
        memset(task, 0, sizeof(TaskControlBlock_t));
        task->task_id = task - tasks;  // 保持task_id不变
    }
}

/* 相对延时 - 阻塞调用任务 */
void SCH_Delay(uint32_t delay_ms)
{
    if(current_task == NULL) {
        return;
    }
    
    // 将任务从就绪链表移除，并设置为阻塞态
    current_task->state = TASK_BLOCKED;
    current_task->wakeup_tick = system_tick + delay_ms;
    SCH_RemoveFromReadyList(current_task);
    
    // 触发任务调度
    scheduler_running = 0;
}

/* 绝对延时 - 设置当前任务的绝对唤醒时间 */
void SCH_DelayUntil(uint32_t target_tick)
{
    if(current_task == NULL) {
        return;
    }
    
    // 确保目标时间在未来
    if(target_tick <= system_tick) {
        target_tick = system_tick + 1;
    }
    
    if(current_task->state == TASK_READY) {
        current_task->state = TASK_BLOCKED;
        current_task->wakeup_tick = target_tick;
        SCH_RemoveFromReadyList(current_task);
        scheduler_running = 0;
    }
}

/* 获取系统滴答数 */
uint32_t SCH_GetTickCount(void)
{
    return system_tick;
}

/* 插入任务到就绪链表 */
static void SCH_InsertToReadyList(TaskControlBlock_t* task)
{
    if(task == NULL || task->in_ready_list) return;
    
    uint8_t prio = task->priority;
    
    // 插入链表头部
    task->next = ready_list[prio];
    ready_list[prio] = task;
    task->in_ready_list = 1;
}

/* 从就绪链表移除任务 */
static void SCH_RemoveFromReadyList(TaskControlBlock_t* task)
{
    if(task == NULL || !task->in_ready_list) return;
    
    uint8_t prio = task->priority;
    TaskControlBlock_t* prev = NULL;
    TaskControlBlock_t* curr = ready_list[prio];
    
    // 遍历链表查找任务
    while(curr != NULL) {
        if(curr == task) {
            if(prev == NULL) {
                // 任务在链表头部
                ready_list[prio] = curr->next;
            } else {
                // 任务在链表中间
                prev->next = curr->next;
            }
            task->next = NULL;
            task->in_ready_list = 0;
            break;
        }
        prev = curr;
        curr = curr->next;
    }
}

/* 获取下一个就绪任务 */
static TaskControlBlock_t* SCH_GetNextReadyTask(void)
{
    // 按优先级从高到低查找
    for(int prio = PRIORITY_HIGHEST; prio <= PRIORITY_LOWEST; prio++) {
        if(ready_list[prio] != NULL) {
            return ready_list[prio];
        }
    }
    return NULL;
}

/* 检查阻塞任务是否需要唤醒 */
static void SCH_CheckBlockedTasks(void)
{
    for(int i = 0; i < MAX_TASKS; i++) {
        TaskControlBlock_t* task = &tasks[i];
        
        if(task->state == TASK_BLOCKED && system_tick >= task->wakeup_tick) {
            // 任务需要被唤醒
            task->state = TASK_READY;
            SCH_InsertToReadyList(task);
        }
    }
}

/* 调度器滴答中断处理 */
void SCH_Tick(void)
{
    system_tick++;
    
    // 检查是否有阻塞任务需要唤醒
    SCH_CheckBlockedTasks();
    
    // 如果没有任务在运行，开始调度
    if(!scheduler_running) {
        scheduler_running = 1;
        
        // 执行任务调度
        while(1) {
            TaskControlBlock_t* task_to_run = SCH_GetNextReadyTask();
            
            if(task_to_run == NULL) {
                break; // 没有就绪任务，退出调度循环
            }
            
            current_task = task_to_run;
            
            // 从就绪链表头部移除（将执行的任务）
            SCH_RemoveFromReadyList(task_to_run);
            
            // 执行任务函数
            if(task_to_run->func != NULL) {
                task_to_run->func();
            }
            
            // 如果任务执行后状态为就绪（没有调用延时函数），则重新插入就绪链表
            if(task_to_run->state == TASK_READY) {
                SCH_InsertToReadyList(task_to_run);
            }
            
            current_task = NULL;
            
            // 检查调度器是否需要停止（例如在延时函数中设置了scheduler_running=0）
            if(!scheduler_running) {
                break;
            }
        }
        
        scheduler_running = 0;
    }
}

