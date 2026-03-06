# stm32f103c8t6

## 基于标准库(有修改)的stm32f103c8t6开发板的外设驱动，包含以下模块：


| 存储类型 | 计算方式 | 占用大小 | 总容量 | 使用率 |
| :--- | :--- | :--- | :--- | :--- |
| **Flash (ROM)** | Code + RO-data | 44.30 KB | 64 KB | **69.22%** |
| **RAM** | RW-data + ZI-data | 3.27 KB | 20 KB | **16.35%** |


```c




#include "stm32f10x.h"                  //Device header
#include "systick_scheduler.h"          //Base    systick
#include "DWT_Delay.h"                  //Base    DWT
#include "math.h"
#include "string.h"
#include "stdlib.h" 

#include "PC13_LED.h"                   //Base    PC13

#include "Serial.h"                     //Base    hardware_Serial   USART   TTL

#include "hardware_I2C.h"               //Base    hardware_I2Cx        (I2C1_I2C2)
    
    #include "OLED.h"                       //Base    (PB11) (PB10)   I2C2.h         OLED_2          //--no-multibyte-chars

    //====mpu6050dmp====//                  
    #include "mpu6050.h"                    //Base    (PB11) (PB10)   I2C2.h    inv_mpu.h
    #include "inv_mpu.h"                    //Base                    I2C2.h    inv_mpu.h    mpu6050.h



//#include "hardware_AD.h"                //Base    hardware_AD
//#include "PID_system.h"                 //base     
//#include "key.h"                        //Base    (PB1 ) (PB11)
//#include "PWM.h"                        //Base    TIM2_CH1(PA0)_CH2(PA1)_CH3(PA2)_CH4(PA3)
//#include "IC_PWMI.h"                    //Base    TIM3_CH1(PA6 )    TIM4_CH1(PB6 )    IC_PWMI.h 与 Encoder.h  只能起用一个
//#include "Encoder.h"                    //Base    TIM3_CH1(PA67)    TIM4_CH1(PB67)    IC_PWMI.h 与 Encoder.h  只能起用一个
//#include "MyDMA.h"                      //Base    hardware_DMA
//#include "adc_dma.h"                    //Base    hardware_AD    hardware_DMA











```


















