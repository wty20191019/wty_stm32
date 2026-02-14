#ifndef __DWT_DELAY_H
#define __DWT_DELAY_H

//=============================================
// DWT_Delay  阻塞延时        base  DWT
//=============================================


#include <stdint.h>

    // 如果 CMSIS 已定义，就不重复定义
    #ifndef DWT_BASE
    #define DWT_BASE             (0xE0001000UL)
    #endif

// 注意：不再定义 CoreDebug_BASE，因为 core_cm3.h 已定义

    // 手动定义 DWT 寄存器结构体（因为 core_cm3.h 中可能没有）
    #ifndef __DWT_TYPE_DEFINED
    typedef struct
    {
        volatile uint32_t CTRL;           // Offset: 0x000 (R/W)  Control Register
        volatile uint32_t CYCCNT;          // Offset: 0x004 (R/W)  Cycle Count Register
        volatile uint32_t CPICNT;          // Offset: 0x008 (R/W)  CPI Count Register
        volatile uint32_t EXCCNT;          // Offset: 0x00C (R/W)  Exception Overhead Count Register
        volatile uint32_t SLEEPCNT;        // Offset: 0x010 (R/W)  Sleep Count Register
        volatile uint32_t LSUCNT;          // Offset: 0x014 (R/W)  LSU Count Register
        volatile uint32_t FOLDCNT;         // Offset: 0x018 (R/W)  Folded-instruction Count Register
        volatile uint32_t PCSR;            // Offset: 0x01C (R/W)  Program Counter Sample Register
        volatile uint32_t COMP0;           // Offset: 0x020 (R/W)  Comparator Register 0
        volatile uint32_t MASK0;           // Offset: 0x024 (R/W)  Mask Register 0
        volatile uint32_t FUNCTION0;       // Offset: 0x028 (R/W)  Function Register 0
               uint32_t RESERVED0[932];
        volatile uint32_t COMP1;           // Offset: 0xF20 (R/W)  Comparator Register 1
        volatile uint32_t MASK1;           // Offset: 0xF24 (R/W)  Mask Register 1
        volatile uint32_t FUNCTION1;       // Offset: 0xF28 (R/W)  Function Register 1
               uint32_t RESERVED1[981];
        volatile uint32_t COMP2;           // Offset: 0x1020 (R/W) Comparator Register 2
        volatile uint32_t MASK2;           // Offset: 0x1024 (R/W) Mask Register 2
        volatile uint32_t FUNCTION2;       // Offset: 0x1028 (R/W) Function Register 2
               uint32_t RESERVED2[981];
        volatile uint32_t COMP3;           // Offset: 0x2020 (R/W) Comparator Register 3
        volatile uint32_t MASK3;           // Offset: 0x2024 (R/W) Mask Register 3
        volatile uint32_t FUNCTION3;       // Offset: 0x2028 (R/W) Function Register 3
    } DWT_Type;
    #define __DWT_TYPE_DEFINED
    #endif

    // 定义 DWT 寄存器指针（如果 CMSIS 没有定义）
    #ifndef DWT
    #define DWT                 ((DWT_Type *) DWT_BASE)
    #endif

    // CoreDebug 指针在 core_cm3.h 中已定义，不需要重复定义
    // 注意：CoreDebug_BASE 在 core_cm3.h 中定义为 0xE000EDF0
    // CoreDebug 指针是 ((CoreDebug_Type *) CoreDebug_BASE)

    // 定义 DWT CTRL 寄存器位掩码（如果 CMSIS 没有定义）
    #ifndef DWT_CTRL_CYCCNTENA_Msk
    #define DWT_CTRL_CYCCNTENA_Pos          0
    #define DWT_CTRL_CYCCNTENA_Msk         (1UL << DWT_CTRL_CYCCNTENA_Pos)
    #endif

    // CoreDebug_DEMCR_TRCENA_Msk 在 core_cm3.h 中已定义，不需要重复定义

    // 不要在这里定义 SystemCoreClock，应该使用外部声明
    // 在 system_stm32f10x.h 中已经有: extern uint32_t SystemCoreClock;
    
    
//=============================================
// DWT_Delay  阻塞延时        base  DWT
//=============================================
// 函数声明
void DWT_Delay_Init(void);
void DWT_Delay_us(uint32_t us);
void DWT_Delay_ms(uint32_t ms);
void DWT_Delay_s(uint32_t s);
//=============================================





#endif /* __DWT_DELAY_H */


