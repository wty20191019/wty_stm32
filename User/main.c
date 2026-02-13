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




//===================================================================================================
//变量定义

//static uint16_t PWM_SetCompare1_i =0 ;
//uint8_t ID;                                //定义用于存放ID号的变量
//int16_t AX, AY, AZ, GX, GY, GZ;            //定义用于存放各个数据的变量

float Pitch,Roll,Yaw;                                //俯仰角默认跟中值一样，翻滚角，偏航角
int16_t ax,ay,az,gx,gy,gz;                            //加速度，陀螺仪角速度


//===================================================================================================


//u8 MPU_Get_Gyroscope(short *gx,short *gy,short *gz);
//u8 MPU_Get_Accelerometer(short *ax,short *ay,short *az);


void OLED_Show_0()
{
    
    static uint16_t i=0;
    OLED_ShowNum(1,i+1,i,1);
    i++;

    if(i>9)
        {
        i=0;
        }
        
        
//    OLED_ShowNum(2 , 1 ,IC_GetPWM1_Frequency(),6);
//    OLED_ShowNum(3 , 1 ,IC_GetPWM1_DutyCycle(),6);
//    OLED_ShowNum(2 , 9 ,IC_GetPWM2_Frequency(),6);
//    OLED_ShowNum(3 , 9 ,IC_GetPWM2_DutyCycle(),6);
//    OLED_ShowSignedNum(4 , 1 ,Encoder1_TIM3_Encoder_Get(),6);
//    OLED_ShowSignedNum(4 , 9 ,Encoder2_TIM4_Encoder_Get(),6);


        

    
    //OLED显示mpu6050数据
    OLED_ShowSignedNum(2, 1 , Pitch, 3);
    OLED_ShowSignedNum(3, 1 , Roll , 3);
    OLED_ShowSignedNum(4, 1 , Yaw  , 3);
    
    OLED_ShowSignedNum(2, 5 , gx, 4);
    OLED_ShowSignedNum(3, 5 , gy, 4);
    OLED_ShowSignedNum(4, 5 , gz, 4);
    
    OLED_ShowSignedNum(2, 11, ax, 4);
    OLED_ShowSignedNum(3, 11, ay, 4);
    OLED_ShowSignedNum(4, 11, az, 4);

};



void GET_MPU6050_info()
{   


    MPU6050_DMP_Get_Data(&Pitch,&Roll,&Yaw);                //读取姿态信息(其中偏航角有飘移是正常现象)
    MPU_Get_Gyroscope(&gx,&gy,&gz);
    MPU_Get_Accelerometer(&ax,&ay,&az);
}


void OLED_OLED_Clear()
{
    OLED_Clear();    
}




//void Turn_PWM_SetCompare1_i()                         //按键输入控制pwm占空比      //static uint16_t PWM_SetCompare1_i =0 ;
//{
//    // TIM2_SetAllChannels(25,58,91,125);             //TIM2的4路pwm占空比设置


//    if(ReadInputDataBit_GPIOB_Pin_1()==1) //短按
//    {
//     PWM_SetCompare1_i=((PWM_SetCompare1_i+1)<100
//                        ?PWM_SetCompare1_i+1
//                        :100);
//    }
//    else if(ReadInputDataBit_GPIOB_Pin_1()==2) //长按
//    {
//     PWM_SetCompare1_i=((PWM_SetCompare1_i+10)<100
//                        ?PWM_SetCompare1_i+10
//                        :100);
//    }
//    else if (ReadInputDataBit_GPIOB_Pin_11()==1)
//    {
//    PWM_SetCompare1_i=((PWM_SetCompare1_i-1)>0
//                        ?PWM_SetCompare1_i-1
//                        :0);
//    }
//    else if (ReadInputDataBit_GPIOB_Pin_11()==2)
//    {
//    PWM_SetCompare1_i=((PWM_SetCompare1_i-10)>0
//                        ?PWM_SetCompare1_i-10
//                        :0);
//    }
//};











int main(void)
{   
//===============================================================
//模块初始化
//===============================================================
    DWT_Init();
    I2C2_Init();
    OLED_Init();
    DWT_Delay_s(1);
                                    OLED_ShowNum(1,1,1,1);
    MPU6050_Init();
                                    OLED_ShowNum(1,2,2,1);
    MPU6050_DMP_Init();
                                    OLED_ShowNum(1,3,3,1);

//    TIM2_PWM_Init();
//    TIM34_IC_PWMI_Init();
//    Encoder1_TIM3_Init();
//    Encoder2_TIM4_Init();
    
    
    
    
//====================================================================
//systick_scheduler调度器                                           //
SCH_Init();                                                         //
//--------------------------------------------------------------------
    
    
    SCH_AddTask(GET_MPU6050_info    ,10     ,PRIORITY_MID);
    SCH_AddTask(OLED_Show_0         ,50    ,PRIORITY_LOW);
    SCH_AddTask(OLED_OLED_Clear     ,500   ,PRIORITY_LOW);
//    SCH_AddTask(Turn_PWM_SetCompare1_i  ,100    ,PRIORITY_LOW);
    
    
//--------------------------------------------------------------------
SCH_Start();                                                        //
//====================================================================





    while (1)
    {
        
        DWT_Delay_s(1);
        
        
    }
}
