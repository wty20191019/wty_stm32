# stm32f103c8t6

## 基于标准库(有修改)的stm32f103c8t6开发板的外设驱动，包含以下模块：

### Flash   :   26.3KB / 64KB   |   41%

### RAM     :   1.82KB / 20 KB  |    9%

```c




//===================================================================================================
//#include
//===================================================================================================


#include "stm32f10x.h"                  // Device header
#include "systick_scheduler.h"          //Base    systick
#include "DWT_Delay.h"                  //Base    DWT

#include "I2C2.h"                       //Base    (PB11) (PB10)   I2C2.h

    #include "OLED.h"                       //Base    (PB11) (PB10)   I2C2.h
    
    //====mpu6050dmp====//                  
    #include "mpu6050.h"                    //Base    (PB11) (PB10)   I2C2.h    inv_mpu.h
    #include "inv_mpu.h"                    //Base                    I2C2.h    inv_mpu.h    mpu6050.h



//#include "PID_system.h"                 //base     systick_scheduler
//#include "key.h"                        //Base    (PB1 ) (PB11)
//#include "LED.h"                        //Base    (PA12)
//#include "PWM.h"                        //Base    TIM2_CH1(PA0)_CH2(PA1)_CH3(PA2)_CH4(PA3)
//#include "IC_PWMI.h"                    //Base    TIM3_CH1(PA6 )    TIM4_CH1(PB6 )    IC_PWMI.h 与 Encoder.h  只能起用一个
//#include "Encoder.h"                    //Base    TIM3_CH1(PA67)    TIM4_CH1(PB67)    IC_PWMI.h 与 Encoder.h  只能起用一个



//====================================================================================
//弃用
//#include "MyI2C.h"                      //弃用  Base   (PB11) (PB10)   DWT_Delay.h     用  硬件I2C2   替代
//#include "Delay.h"                      //弃用  Base   systick与systick_scheduler冲突  用  DWT_Delay  替代














```

