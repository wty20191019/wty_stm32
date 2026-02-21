# FreeRTOS 使用文档

## 一、FreeRTOS简介

FreeRTOS是一个小型实时操作系统内核，专为嵌入式系统设计。它提供任务管理、时间管理、信号量、消息队列、内存管理等功能，占用资源少，可移植性强，适合在资源受限的微控制器上运行。

## 二、系统配置

### 1. FreeRTOSConfig.h配置文件

```c
/* FreeRTOS基础配置 */
#define configUSE_PREEMPTION             1       // 使用抢占式调度
#define configUSE_IDLE_HOOK             0       // 不使用空闲钩子
#define configUSE_TICK_HOOK             0       // 不使用滴答钩子
#define configCPU_CLOCK_HZ              (72000000)  // CPU时钟72MHz
#define configTICK_RATE_HZ              (1000)      // 系统时钟1ms
#define configMAX_PRIORITIES            (5)         // 优先级数量
#define configMINIMAL_STACK_SIZE        (128)       // 最小堆栈大小
#define configTOTAL_HEAP_SIZE           (10240)     // 堆大小10KB

/* 任务功能配置 */
#define configUSE_MUTEXES               1       // 使用互斥量
#define configUSE_RECURSIVE_MUTEXES     0       // 不使用递归互斥量
#define configUSE_COUNTING_SEMAPHORES   1       // 使用计数信号量
#define configUSE_QUEUE_SETS            0       // 不使用队列集

/* 内存管理配置 */
#define configSUPPORT_DYNAMIC_ALLOCATION 1      // 动态内存分配
#define configAPPLICATION_ALLOCATED_HEAP 0      // 使用FreeRTOS堆

/* 中断配置 - Cortex-M3 */
#define configKERNEL_INTERRUPT_PRIORITY         255
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    191
#define configLIBRARY_KERNEL_INTERRUPT_PRIORITY 15

/* 包含的API函数 */
#define INCLUDE_vTaskPrioritySet        1
#define INCLUDE_uxTaskPriorityGet       1
#define INCLUDE_vTaskDelete             1
#define INCLUDE_vTaskSuspend            1
#define INCLUDE_vTaskDelayUntil         1
#define INCLUDE_vTaskDelay              1

/* 中断处理程序重命名 */
#define xPortPendSVHandler    PendSV_Handler
#define vPortSVCHandler       SVC_Handler
#define xPortSysTickHandler   SysTick_Handler
```

### 2. 中断优先级设置

在main函数开始处必须设置中断优先级分组：

```c
int main(void)
{
    // 关键：设置中断优先级分组为4
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    
    // 其他初始化代码...
}
```

## 三、任务管理

### 1. 创建任务

```c
#include "FreeRTOS.h"
#include "task.h"

/* 任务函数原型 */
void LED_Task(void *pvParameters);
void UART_Task(void *pvParameters);

int main(void)
{
    // 硬件初始化
    SystemInit();
    
    // 创建任务
    xTaskCreate(LED_Task,      // 任务函数
                "LED",         // 任务名称
                128,           // 堆栈大小（字）
                NULL,          // 任务参数
                2,             // 优先级（数字越大优先级越高）
                NULL);         // 任务句柄
    
    xTaskCreate(UART_Task, "UART", 256, NULL, 1, NULL);
    
    // 启动调度器
    vTaskStartScheduler();
    
    // 不会执行到这里
    while(1);
}
```

### 2. 任务函数结构

任务函数必须是无限循环，并使用FreeRTOS延时函数：

```c
void LED_Task(void *pvParameters)
{
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(500); // 500ms周期
    
    // 获取当前tick计数
    xLastWakeTime = xTaskGetTickCount();
    
    // 任务初始化代码
    GPIO_Init(); // 初始化LED
    
    for(;;) // 无限循环
    {
        // 任务主体代码==========================

        GPIO_Toggle(LED_PIN);

        //---------------------------------------
        // 精确周期延时
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}
```

### 3. 任务延时函数

#### **使用相对延时 (vTaskDelay) 当：**"每隔固定时间"执行一次
1. 只需要简单的延时等待
2. 任务执行时间很短且变化不大
3. 不需要精确的时间间隔
4. 任务是非周期性的

```c
// 相对延时：从调用时刻开始延时
vTaskDelay(pdMS_TO_TICKS(100)); // 延时100ms
```

### **使用绝对延时 (vTaskDelayUntil) 当：**"暂停一段时间"
1. 需要精确的固定周期（如数据采样、控制循环）
2. 任务执行时间可能有变化，但需要稳定的执行间隔
3. 需要避免时间累积误差
4. 任务执行频率需要严格保证

```c
// 绝对延时：保证精确的周期
TickType_t xLastWakeTime = xTaskGetTickCount();
const TickType_t xFrequency = pdMS_TO_TICKS(100);

for(;;)
{
    // 任务代码
    vTaskDelayUntil(&xLastWakeTime, xFrequency); // 精确100ms周期
}
```

### 4. 任务优先级管理

```c
// 设置任务优先级
vTaskPrioritySet(xTaskHandle, newPriority);

// 获取任务优先级
UBaseType_t uxPriority = uxTaskPriorityGet(xTaskHandle);

// 挂起任务
vTaskSuspend(xTaskHandle);

// 恢复任务
vTaskResume(xTaskHandle);

// 删除任务
vTaskDelete(xTaskHandle);
```

## 四、任务间通信

### 1. 消息队列

```c
#include "queue.h"

// 创建队列
QueueHandle_t xQueue;
xQueue = xQueueCreate(10, sizeof(int)); // 队列长度10，元素大小int

// 发送数据到队列（任务中）
int data = 100;
xQueueSend(xQueue, &data, pdMS_TO_TICKS(100)); // 等待100ms

// 从队列接收数据（任务中）
int received_data;
if(xQueueReceive(xQueue, &received_data, portMAX_DELAY) == pdPASS)
{
    // 处理数据
}

// 从中断发送数据
void USART_IRQHandler(void)
{
    char c;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    c = USART_ReceiveData();
    xQueueSendFromISR(xQueue, &c, &xHigherPriorityTaskWoken);
    
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
```

### 2. 信号量

```c
#include "semphr.h"

// 创建二进制信号量
SemaphoreHandle_t xBinarySemaphore;
xBinarySemaphore = xSemaphoreCreateBinary();

// 创建计数信号量
SemaphoreHandle_t xCountingSemaphore;
xCountingSemaphore = xSemaphoreCreateCounting(10, 0); // 最大计数10，初始0

// 获取信号量（等待）
if(xSemaphoreTake(xBinarySemaphore, pdMS_TO_TICKS(100)) == pdTRUE)
{
    // 获取成功，执行受保护的操作
}

// 释放信号量
xSemaphoreGive(xBinarySemaphore);
```

### 3. 互斥量

```c
// 创建互斥量
SemaphoreHandle_t xMutex;
xMutex = xSemaphoreCreateMutex();

// 获取互斥锁
if(xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE)
{
    // 访问共享资源
    shared_variable++;
    
    // 释放互斥锁
    xSemaphoreGive(xMutex);
}
```

### 4. 事件标志组

```c
#include "event_groups.h"

// 创建事件组
EventGroupHandle_t xEventGroup;
xEventGroup = xEventGroupCreate();

// 设置事件位
xEventGroupSetBits(xEventGroup, 0x01); // 设置位0

// 等待事件位
EventBits_t uxBits;
uxBits = xEventGroupWaitBits(
    xEventGroup,        // 事件组句柄
    0x03,               // 等待位0和位1
    pdTRUE,             // 退出前清除等待的位
    pdTRUE,             // 所有位都必须设置
    portMAX_DELAY);     // 无限等待
```

## 五、内存管理

### 1. 动态内存分配

```c
// 分配内存
void *pvBuffer = pvPortMalloc(100); // 分配100字节
if(pvBuffer != NULL)
{
    // 使用内存
    memcpy(pvBuffer, data, 100);
    
    // 释放内存
    vPortFree(pvBuffer);
}

// 获取堆信息
size_t xFreeHeapSize = xPortGetFreeHeapSize();
size_t xMinEverFreeHeapSize = xPortGetMinimumEverFreeHeapSize();
```

### 2. 静态内存分配

```c
// 静态分配任务堆栈
static StackType_t xTaskStack[128];

// 静态分配任务控制块
static StaticTask_t xTaskTCB;

// 创建静态任务
xTaskCreateStatic(TaskFunction,     // 任务函数
                  "Task",           // 任务名
                  128,              // 堆栈深度
                  NULL,             // 参数
                  1,                // 优先级
                  xTaskStack,       // 堆栈数组
                  &xTaskTCB);       // 任务控制块
```

## 六、软件定时器

```c
#include "timers.h"

// 定时器回调函数
void TimerCallback(TimerHandle_t xTimer)
{
    // 定时器超时处理
    uint32_t *pTimerID = (uint32_t *)pvTimerGetTimerID(xTimer);
    
    switch(*pTimerID)
    {
        case 1: // 定时器1
            GPIO_Toggle(LED_PIN);
            break;
    }
}

// 创建定时器
TimerHandle_t xTimer;
uint32_t timerID = 1;

xTimer = xTimerCreate("Timer1",              // 定时器名称
                      pdMS_TO_TICKS(1000),   // 周期1000ms
                      pdTRUE,                // 自动重载
                      (void *)&timerID,      // 定时器ID
                      TimerCallback);        // 回调函数

// 启动定时器
if(xTimer != NULL)
{
    xTimerStart(xTimer, 0); // 0表示不阻塞
}

// 修改定时器周期
xTimerChangePeriod(xTimer, pdMS_TO_TICKS(500), 0);

// 停止定时器
xTimerStop(xTimer, 0);
```

## 七、中断管理

### 1. 中断优先级规则

```c
// 在main函数开始设置
NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

// 中断优先级范围：0-15（15最低）
// FreeRTOS内核中断必须为最低优先级（15）

// 可调用FreeRTOS API的中断优先级：0-11
// 不可调用FreeRTOS API的中断优先级：0-15
```

### 2. 中断服务程序

```c
// 可调用FreeRTOS API的中断
void TIM2_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        // 处理中断
        xSemaphoreGiveFromISR(xSemaphore, &xHigherPriorityTaskWoken);
        
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
    
    // 如果需要上下文切换
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// 不可调用FreeRTOS API的中断
void EXTI0_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        // 只能执行简单操作，不能调用FreeRTOS API
        flag = 1;
        
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}
```

## 八、调试和监控

### 1. 任务状态查询

```c
// 获取任务数量
UBaseType_t uxTaskCount = uxTaskGetNumberOfTasks();

// 获取任务状态
TaskStatus_t *pxTaskStatusArray;
uxArraySize = uxTaskGetNumberOfTasks();
pxTaskStatusArray = pvPortMalloc(uxArraySize * sizeof(TaskStatus_t));

if(pxTaskStatusArray != NULL)
{
    uxArraySize = uxTaskGetSystemState(pxTaskStatusArray, uxArraySize, NULL);
    
    for(UBaseType_t x = 0; x < uxArraySize; x++)
    {
        printf("Task: %s, Priority: %d, State: %d\n",
               pxTaskStatusArray[x].pcTaskName,
               pxTaskStatusArray[x].uxCurrentPriority,
               pxTaskStatusArray[x].eCurrentState);
    }
    
    vPortFree(pxTaskStatusArray);
}
```

### 2. 堆栈使用监控

```c
void Monitor_Task(void *pvParameters)
{
    while(1)
    {
        // 获取任务堆栈高水位线
        UBaseType_t uxHighWaterMark;
        uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        
        printf("Stack High Water Mark: %d words\n", uxHighWaterMark);
        
        vTaskDelay(pdMS_TO_TICKS(5000)); // 每5秒检查一次
    }
}
```

## 九、从原调度器迁移指南

### 1. 任务迁移

原调度器任务：
```c
void Original_Task(void)
{
    // 任务代码
    // 周期执行
}
```

迁移为FreeRTOS任务：
```c
void Wrapped_Task(void *pvParameters)
{
    TickType_t xLastWakeTime;
    const TickType_t xPeriod = pdMS_TO_TICKS(original_period);
    
    xLastWakeTime = xTaskGetTickCount();
    
    for(;;)
    {
        Original_Task(); // 调用原任务函数
        
        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
}
```


## 十、最佳实践

### 1. 堆栈大小设置

- 简单任务：128字
- 中等任务：256字
- 复杂任务：512字或更多
- 定期使用uxTaskGetStackHighWaterMark检查堆栈使用

### 2. 优先级设计

- 空闲任务：优先级0
- 低优先级任务：1-2
- 中等优先级任务：3-4
- 高优先级任务：5-6
- 系统关键任务：最高优先级

### 3. 中断设计原则

- 中断处理尽量简短
- 需要复杂处理时使用任务通知或队列将工作转移到任务中
- 可调用FreeRTOS API的中断优先级设为0-11
- 高速中断优先级设为12-15，但不调用FreeRTOS API

### 4. 资源保护

- 访问共享资源时使用互斥量
- 短时间访问共享变量可使用临界区
- 中断和任务共享资源使用FromISR函数

## 十一、常见问题

### 1. 编译错误：中断处理函数重复定义

解决方法：删除stm32f10x_it.c中的SysTick_Handler、PendSV_Handler、SVC_Handler函数。

### 2. 系统不稳定或重启

检查：
- 中断优先级是否正确设置
- 堆栈大小是否足够
- 堆空间是否足够
- 是否在不可调用API的中断中调用了FreeRTOS函数

### 3. 任务无法按时执行

检查：
- 任务优先级是否合理
- 是否有高优先级任务一直运行
- 是否使用了阻塞函数导致任务无法切换

### 4. 内存不足

检查：
- configTOTAL_HEAP_SIZE是否足够
- 是否及时释放动态分配的内存
- 任务堆栈是否过大



本文档涵盖了FreeRTOS的基本使用方法，可根据具体项目需求调整配置和使用方式。建议先从简单任务开始，逐步添加复杂功能，并在开发过程中持续监控系统资源使用情况。


---
---
---

# 两种延时(vTaskDelayUntil,vTaskDelay)**时序图对比**
### **场景设定**
- 周期：100ms
- 任务执行时间：有时10ms，有时30ms（模拟任务负载变化）

### **时序图1：相对延时(vTaskDelay)的效果**

```
时间轴 (ms): 0   10  20  30  40  50  60  70  80  90  100 110 120 130 140 150
             |---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
任务执行:     [A]     [A]     [A]     [A]     [A]     [A]     [A]     [A]     
执行时长:     10ms   30ms   10ms   30ms   10ms   30ms   10ms   30ms   
相对延时:     |----100ms----|----100ms----|----100ms----|----100ms----|

实际间隔:    10+100=110ms  30+100=130ms  10+100=110ms  30+100=130ms

结果：周期不稳定！在110ms和130ms之间波动。
```

### **时序图2：绝对延时(vTaskDelayUntil)的效果**

```
时间轴 (ms): 0   10  20  30  40  50  60  70  80  90  100 110 120 130 140 150
             |---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
任务执行:     [A]                 [A]                 [A]                 [A]     
执行时长:     10ms               30ms               10ms               30ms   
绝对延时:     从0开始，100ms后执行  从100开始，100ms后执行  从200开始，100ms后执行

实际时间点:   0ms                100ms               200ms               300ms

结果：周期稳定！总是在0, 100, 200, 300ms执行，不受任务执行时间影响。
```

### **更直观的图示**

```
相对延时（不保证周期）：
时间：0     100     200     300     400     500
执行：[====][       ][====][       ][====]
      ^10ms ^30ms    ^10ms  ^30ms    ^10ms
间隔： 110ms  130ms   110ms   130ms

绝对延时（保证周期）：
时间：0     100     200     300     400     500
执行：[====]        [       ]        [====]        [       ]
      ^10ms         ^30ms            ^10ms         ^30ms
间隔： 100ms         100ms            100ms         100ms
```
