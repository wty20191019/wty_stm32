
 /*
 * FreeRTOS V202212.00
 * 版权所有 (C) 2020 Amazon.com, Inc. 或其关联公司。保留所有权利。
 *
 * 特此免费授予任何获得副本的人以下权限：
 * 本软件及其相关文档文件（以下简称“本软件”）的交易
 * 无限制地使用本软件，包括但不限于以下权利：
 * 使用、复制、修改、合并、发布、分发、授予从属许可和/或出售副本
 * 本软件，并允许获得本软件的人士进行上述操作，
 * 需满足以下条件：
 *
 * 上述版权声明和本许可声明应包含在所有
 * 本软件的副本或实质性部分。
 *
 * 本软件“按原样”提供，不附带任何形式的明示或暗示担保
 * 暗示的，包括但不限于对适销性、适用性的保证
 * 仅用于特定目的且不侵犯版权。在任何情况下，作者或
 * 版权所有者对任何索赔、损害或其他责任负责，无论
 * 在因合同、侵权或其他原因而产生的诉讼中，无论是源于、超出还是涉及
 * 与本软件相关的连接，或对本软件的使用或其他处理方式。
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * 特定应用定义。
 *
 * 这些定义应根据您的特定硬件进行调整
 * 应用要求。
 *
 * 这些参数在“配置”部分进行了描述
 * FreeRTOS API文档可在FreeRTOS.org网站上获取。 
 * 
 * 参见http://www.freertos.org/a00110.html  
 *----------------------------------------------------------*/ 

#define configUSE_PREEMPTION            1                                               // 使用抢占式调度
#define configUSE_IDLE_HOOK             0                                               // 不使用空闲钩子
#define configUSE_TICK_HOOK             0                                               // 不使用滴答钩子
#define configCPU_CLOCK_HZ              ( ( unsigned long ) 72000000 )                  // 72MHz
#define configTICK_RATE_HZ              ( ( TickType_t ) 1000 )                         // 1ms滴答
#define configMAX_PRIORITIES            ( 5 )                                           // 优先级数量
#define configMINIMAL_STACK_SIZE        ( ( unsigned short ) 128 )                      // 最小栈
#define configTOTAL_HEAP_SIZE           ( ( size_t ) ( 12 * 1024 ) )                     // 堆空间大小
#define configMAX_TASK_NAME_LEN         ( 16 )                                          // 任务名长度
#define configUSE_TRACE_FACILITY        0                                               // 关闭追踪功能（节省空间）
#define configUSE_16_BIT_TICKS          0                                               // 使用32位时间戳
#define configIDLE_SHOULD_YIELD         1                                               // 空闲任务让出CPU

/* Co-routine definitions. *//* 协同例程定义。 */
#define configUSE_CO_ROUTINES               0                        //协程（不使用）
#define configMAX_CO_ROUTINE_PRIORITIES     ( 2 )

/* Set the following definitions to 1 to include the API function, or zeroto exclude the API function. */
/* 将以下定义设置为1以包含API函数，或设置为0排除API功能。 */
#define INCLUDE_vTaskPrioritySet                    1       // 包含优先级设置
#define INCLUDE_uxTaskPriorityGet                   1       // 包含获取优先级
#define INCLUDE_vTaskDelete                         1       // 包含删除任务
#define INCLUDE_vTaskCleanUpResources               0       // 不包含清理资源
#define INCLUDE_vTaskSuspend                        1       // 包含挂起任务
#define INCLUDE_vTaskDelayUntil                     1       // 包含精确延时
#define INCLUDE_vTaskDelay                          1       // 包含相对延时


/* 中断配置 - Cortex-M3专用 */
/* This is the raw value as per the Cortex-M3 NVIC.  Values can be 255 (lowest) to 0 (1?) (highest). */
/* 这是根据Cortex-M3 NVIC得出的原始值。值可以是255（最低）到0（1？）（最高）。 */
#define configKERNEL_INTERRUPT_PRIORITY                            255
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!! See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
 /* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY 绝对不能设置为零！请参阅 http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY                191 /* equivalent to 0xb0, or priority 11. */
/* This is the value being used as per the ST library which permits 16 priority values, 0 to 15.  This must correspond to the configKERNEL_INTERRUPT_PRIORITY setting.  Here 15 corresponds to the lowest NVIC value of 255. */
/* 这是根据ST库所使用的值，该库允许使用16个优先级值，范围从0到15。此值必须与configKERNEL_INTERRUPT_PRIORITY设置相对应。在此，15对应于最低的NVIC值255。 */
#define configLIBRARY_KERNEL_INTERRUPT_PRIORITY            15


/*新增配置*/
#define configUSE_MUTEXES                1       // 使用互斥量
#define configUSE_RECURSIVE_MUTEXES      0       // 不使用递归互斥量
#define configUSE_COUNTING_SEMAPHORES    1       // 使用计数信号量
#define configUSE_QUEUE_SETS             0       // 不使用队列集
#define configUSE_TASK_NOTIFICATIONS     1       // 使用任务通知（推荐）

/* 软件定时器配置 */
#define configUSE_TIMERS                 1       // 使用软件定时器
#define configTIMER_TASK_PRIORITY        ( configMAX_PRIORITIES - 1 )  // 最高优先级
#define configTIMER_QUEUE_LENGTH         5       // 定时器队列长度
#define configTIMER_TASK_STACK_DEPTH     ( configMINIMAL_STACK_SIZE * 2 )

/* 内存管理 */
#define configSUPPORT_DYNAMIC_ALLOCATION    1       // 动态内存分配
#define configAPPLICATION_ALLOCATED_HEAP    0       // 使用FreeRTOS堆

/* 中断处理程序重命名（针对STM32） */
#define xPortPendSVHandler    PendSV_Handler
#define vPortSVCHandler       SVC_Handler
#define xPortSysTickHandler   SysTick_Handler

/* 堆栈溢出检查（调试用） */
#define configCHECK_FOR_STACK_OVERFLOW      0       // 关闭以节省性能

/* 兼容性 */
#define configENABLE_BACKWARD_COMPATIBILITY 1    // 向后兼容











#endif /* FREERTOS_CONFIG_H */






//#include "FreeRTOS.h"
//#include "task.h"
//#include "queue.h"

///* 任务函数原型（必须有 void * 参数） */
//void Task1_Function(void *pvParameters);
//void Task2_Function(void *pvParameters);

///* 全局变量（用于任务间通信） */
//QueueHandle_t xQueue;

//int main(void)
//{
//    // 1. 硬件初始化
//    SystemInit();
//    // ... 其他初始化代码
//    
//    // 2. 创建消息队列（如果需要任务间通信）
//    xQueue = xQueueCreate(5, sizeof(uint32_t));
//    
//    // 3. 创建任务    优先级数字越大，优先级越高
//    // 参数：       任务函数        , 任务名    , 堆栈大小  , 参数  , 优先级, 任务句柄
//    xTaskCreate(    Task1_Function  , "Task1"   , 128       , NULL  , 2     , NULL);
//    xTaskCreate(    Task2_Function  , "Task2"   , 128       , NULL  , 1     , NULL);
//    
//    // 4. 启动调度器（永不返回）
//    vTaskStartScheduler();
//    
//    // 如果调度器启动失败，会执行到这里
//    while(1);
//}

///* 任务1实现 */
//void Task1_Function(void *pvParameters)
//{
//    TickType_t xLastWakeTime;
//    const TickType_t xPeriod = pdMS_TO_TICKS(100); // 100ms周期
//    
//    // 获取当前tick计数
//    xLastWakeTime = xTaskGetTickCount();
//    
//    for(;;) // 无限循环
//    {
//        // 执行你的任务代码
//        GPIO_WriteBit(GPIOC, GPIO_Pin_13, 
//                      (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13)));
//        
//        // 精确周期延时（从上次唤醒时间计算）
//        vTaskDelayUntil(&xLastWakeTime, xPeriod);
//    }
//}

///* 任务2实现 */
//void Task2_Function(void *pvParameters)
//{
//    for(;;)
//    {
//        // 相对延时（从当前时间开始延时）
//        vTaskDelay(pdMS_TO_TICKS(500)); // 延时500ms
//        
//        // 执行任务
//        // ... 你的代码
//    }
//}





