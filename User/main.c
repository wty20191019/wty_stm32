//===================================================================================================
//#include
//===================================================================================================


#include "stm32f10x.h"                  // Device header
#include "systick_scheduler.h"          //Base    systick
#include "DWT_Delay.h"                  //Base    DWT
#include "math.h" 

#include "I2C2.h"                       //Base    (PB11) (PB10)   I2C2.h

    #include "OLED.h"                       //Base    (PB11) (PB10)   I2C2.h         OLED_2           //--no-multibyte-chars
    
    
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





//////===================================================================================================
//变量定义
//////===================================================================================================

//static uint16_t PWM_SetCompare1_i =0 ;
//uint8_t ID;                                //定义用于存放ID号的变量
//int16_t AX, AY, AZ, GX, GY, GZ;            //定义用于存放各个数据的变量


float Pitch,Roll,Yaw;                                //俯仰角默认跟中值一样，翻滚角，偏航角
int16_t ax,ay,az,gx,gy,gz;                            //加速度，陀螺仪角速度

#define PI 3.141592653589793


                                                                                                         
//=================================================================================================//////


//u8 MPU_Get_Gyroscope(short *gx,short *gy,short *gz);
//u8 MPU_Get_Accelerometer(short *ax,short *ay,short *az);





void OLED_Show_0()
{
    OLED_Clear(); 
    static uint16_t i=0;
    int_fast8_t y_p1,y_p2,Yaw_p;
    
    OLED_ShowNum(i*8,0,i,1,OLED_8X16);
    i++;
    i = (i>9?0:i);

        
    //OLED显示PWM测量值
//OLED_ShowNum(1,1,IC_GetPWM1_Frequency,6,OLED_8X16);
//OLED_ShowNum(1,1,IC_GetPWM1_DutyCycle,6,OLED_8X16);
//OLED_ShowNum(1,1,IC_GetPWM2_Frequency,6,OLED_8X16);
//OLED_ShowNum(1,1,IC_GetPWM2_DutyCycle,6,OLED_8X16);

    //OLED显示Encoder的计数
//    OLED_ShowSignedNum(3 , 1 ,Encoder1_TIM3_Encoder_Get(),6,OLED_8X16);
//    OLED_ShowSignedNum(3 , 9 ,Encoder2_TIM4_Encoder_Get(),6,OLED_8X16);


        

    
    //OLED显示mpu6050数据
    
    

    
    OLED_DrawLine(0,31,127,31);
    OLED_DrawLine(63,0,63,63);
    
    y_p1     = 32+32*(Pitch/90);
    y_p2     = 63*tan(Roll* PI / 180.0);
    Yaw_p   = 63+63*(Yaw   /180); 
    
    OLED_DrawLine(0     ,y_p1-y_p2  ,127    ,y_p1+y_p2 );
    
//    OLED_DrawTriangle(0     ,y_p1-y_p2  ,127    ,y_p1+y_p2 ,63,63,OLED_FILLED);
//    OLED_DrawTriangle(0     ,y_p1-y_p2  ,0    ,63,63,63,OLED_FILLED);
//    OLED_DrawTriangle(127     ,63  ,127    ,y_p1+y_p2 ,63,63,OLED_FILLED);
    
    OLED_DrawLine(Yaw_p ,0          ,Yaw_p  ,63         );
    
    
    OLED_ShowSignedNum(8*12, 14*1  , Pitch, 3,OLED_8X16);
    OLED_ShowSignedNum(8*0, 14*2  , Roll , 3,OLED_8X16);
    OLED_ShowSignedNum(8*8, 14*4-6  , Yaw  , 3,OLED_8X16);

    
//    OLED_ShowSignedNum(33, 0  , gx, 5,OLED_8X16);
//    OLED_ShowSignedNum(33, 14 , gy, 5,OLED_8X16);
//    OLED_ShowSignedNum(33, 28 , gz, 5,OLED_8X16);
    
//    OLED_ShowSignedNum(66, 0, ax, 4,OLED_8X16);
//    OLED_ShowSignedNum(66, 14, ay, 4,OLED_8X16);
//    OLED_ShowSignedNum(66, 28, az, 4,OLED_8X16);



OLED_Update();
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


//////====================================================================================================
//////模块初始化
//////==================================================================================================== 

    SCH_Init();                         // systick_scheduler调度器
    DWT_Delay_Init();                   // DWT_Delay_Init
    I2C2_Init();
    OLED_Init();
    DWT_Delay_s(1);
                                    OLED_ShowNum(0,1,1,1,OLED_8X16);OLED_Update();
    MPU6050_Init();                   // MPU6050_Init    包含 I2C2_Init
                                    OLED_ShowNum(0,2,2,1,OLED_8X16);OLED_Update();
    MPU6050_DMP_Init();               // MPU6050_DMP_Init    包含 I2C2_Init
                                    OLED_ShowNum(0,3,3,1,OLED_8X16);OLED_Update();

//    TIM2_PWM_Init();
//    TIM34_IC_PWMI_Init();
//    Encoder1_TIM3_Init();
//    Encoder2_TIM4_Init();

//=================================================================================================//////
    
    
    
    
//////====================================================================================================    
//////systick_scheduler调度器                                            
//////====================================================================================================
//这里添加调度


    SCH_AddTask(GET_MPU6050_info    ,1     ,PRIORITY_MID);
    SCH_AddTask(OLED_Show_0         ,20    ,PRIORITY_LOW);
//    SCH_AddTask(OLED_OLED_Clear     ,500   ,PRIORITY_LOW);
//    SCH_AddTask(Turn_PWM_SetCompare1_i  ,100    ,PRIORITY_LOW);
    
    
    
//-------------------------------------------------------------------------------------------------------
SCH_Start();                    //开始调度                                     
//=================================================================================================//////









    while (1)//主循环=============================================================================
    {
        
        DWT_Delay_s(1);
        
    }

}
