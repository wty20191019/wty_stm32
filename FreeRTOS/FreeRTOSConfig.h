/*
 * FreeRTOS V202212.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * 此处是版权声明和许可证信息
 * 软件按"原样"提供，没有任何明示或暗示的保证
 * 更多信息请访问：https://www.FreeRTOS.org
 * 源代码仓库：https://github.com/FreeRTOS
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * 应用程序特定定义
 *
 * 这些定义应根据您的具体硬件和应用程序需求进行调整
 * 这些参数在FreeRTOS API文档的"配置"部分有详细描述
 * 参见：http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

/* 内核控制相关配置 */
#define configUSE_PREEMPTION            1                                   /* 1: 使用抢占式调度器; 0: 使用协作式调度器 */
#define configUSE_IDLE_HOOK             0                                   /* 1: 启用空闲任务钩子函数; 0: 禁用 */
#define configUSE_TICK_HOOK             0                                   /* 1: 启用时钟节拍钩子函数; 0: 禁用 */
#define configCPU_CLOCK_HZ              ( ( unsigned long ) 72000000 )      /* CPU时钟频率: 72MHz (STM32F103) */
#define configTICK_RATE_HZ              ( ( TickType_t ) 1000 )             /* 系统节拍频率: 1000Hz (1ms) */
#define configMAX_PRIORITIES            ( 8 )                               /* 最大任务优先级数 (0-7, 0为最低) */
#define configMINIMAL_STACK_SIZE        ( ( unsigned short ) 256 )          /* 空闲任务堆栈大小 (字) */
#define configTOTAL_HEAP_SIZE           ( ( size_t ) ( 10 * 1024 ) )        /* 堆内存总大小: 10KB (STM32F103C8T6有20KB RAM) */
#define configMAX_TASK_NAME_LEN         ( 16 )                              /* 任务名称最大长度 */
#define configUSE_TRACE_FACILITY        0                                   /* 1: 启用可视化跟踪调试; 0: 禁用 (节省资源) */
#define configUSE_16_BIT_TICKS          0                                   /* 1: 使用16位Tick计数器; 0: 使用32位Tick计数器 */
#define configIDLE_SHOULD_YIELD         1                                   /* 1: 空闲任务主动让出CPU; 0: 不主动让出 */




/* 协程相关配置 (FreeRTOS的轻量级任务, 现已不常用) */
#define configUSE_CO_ROUTINES           0           /* 1: 启用协程; 0: 禁用 */
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )       /* 协程最大优先级数 */

/* 
 * API函数包含配置
 * 设置为1以包含API函数，0以排除（节省代码空间）
 */
#define INCLUDE_vTaskPrioritySet        1    /* 包含动态设置任务优先级的函数 */
#define INCLUDE_uxTaskPriorityGet       1    /* 包含获取任务优先级的函数 */
#define INCLUDE_vTaskDelete             1    /* 包含删除任务的函数 */
#define INCLUDE_vTaskCleanUpResources   0    /* 包含任务清理资源的函数 (通常不需要) */
#define INCLUDE_vTaskSuspend            1    /* 包含挂起和恢复任务的函数 */
#define INCLUDE_vTaskDelayUntil         1    /* 包含绝对延时函数vTaskDelayUntil */
#define INCLUDE_vTaskDelay              1    /* 包含相对延时函数vTaskDelay */

/* 
 * 中断优先级配置 - Cortex-M3/M4内核特定
 * 注意：Cortex-M3使用8位优先级寄存器，但STM32只使用高4位
 */
#define configKERNEL_INTERRUPT_PRIORITY         255    /* 内核中断优先级: 255(最低) 对应优先级15 */

/* 
 * 可管理的中断最高优先级
 * 重要：不能设置为0！详见http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html
 */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY     191    /* 191对应优先级11 (0xb0) */

/* 
 * ST标准库使用的优先级值 (0-15, 0为最高优先级)
 * 这必须与configKERNEL_INTERRUPT_PRIORITY设置对应
 * 这里15对应NVIC的最低优先级值255
 */
#define configLIBRARY_KERNEL_INTERRUPT_PRIORITY    15

/* 
 * 中断服务例程重命名
 * 将FreeRTOS的中断处理函数映射到CMSIS标准名称
 */
#define xPortPendSVHandler  PendSV_Handler          /* PendSV中断，用于上下文切换 */
#define vPortSVCHandler     SVC_Handler             /* SVC中断，用于启动调度器 */
#define xPortSysTickHandler SysTick_Handler         /* 系统节拍中断，用于时间管理 */

/* 内存分配方案选择 */
#define configSUPPORT_DYNAMIC_ALLOCATION        1    /* 使用动态内存分配 */
#define configSUPPORT_STATIC_ALLOCATION         0    /* 不使用静态内存分配 */

/* 任务通知功能（轻量级信号量/事件标志）*/
#define configUSE_TASK_NOTIFICATIONS            1    /* 启用任务通知功能 */

/* 递归互斥量 */
#define configUSE_RECURSIVE_MUTEXES             0   

// 启用互斥量（信号量的一种）
#define configUSE_MUTEXES                       1

/* 计数信号量/队列 */
#define configUSE_COUNTING_SEMAPHORES           1    /* 启用计数信号量 */
#define configUSE_QUEUE_SETS                    0    /* 禁用队列集以节省资源 */

/* 软件定时器 */
#define configUSE_TIMERS                        0                                   /* 禁用软件定时器以节省RAM（如需定时器可设为1）*/
#define configTIMER_TASK_PRIORITY               ( configMAX_PRIORITIES - 1 )        /* 定时器任务优先级 */
#define configTIMER_QUEUE_LENGTH                5                                   /* 定时器命令队列长度 */
#define configTIMER_TASK_STACK_DEPTH            ( configMINIMAL_STACK_SIZE * 2 )    /* 定时器任务栈大小 */

#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5  // 可管理的最高中断优先级


#endif /* FREERTOS_CONFIG_H */



