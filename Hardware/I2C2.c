#include "stm32f10x.h"                  // Device header
#include "I2C2.h"


void I2C2_Init(void)
{
    /*开启时钟*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);                        //开启I2C2的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);                       //开启GPIOB的时钟
    
    /*GPIO初始化*/
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);                                      //将PB10和PB11引脚初始化为复用开漏输出
    
    /*I2C初始化*/
    I2C_InitTypeDef I2C_InitStructure;                                          //定义结构体变量
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;                                  //模式，选择为I2C模式
    I2C_InitStructure.I2C_ClockSpeed = clock_frequency*1000;                    //时钟速度，选择为clock_frequencykHz    标准速度（1-100kHz）    快速（101-400kHZ） 
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_16_9;                       //时钟占空比，选择I2C_DutyCycle_16_9
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;                                 //应答，选择使能
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;   //应答地址，选择7位，从机模式下才有效
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;                                   //自身地址，从机模式下才有效
    I2C_Init(I2C2, &I2C_InitStructure);                                         //将结构体变量交给I2C_Init，配置I2C2
    
    /*I2C使能*/
    I2C_Cmd(I2C2, ENABLE);                                                      //使能I2C2，开始运行

}


//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功

uint8_t I2C2_WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT)
{
    uint32_t Timeout ;                                       //给定超时计数时间
    Timeout = 100;
    while (I2C_CheckEvent(I2Cx, I2C_EVENT) != SUCCESS)              //循环等待指定事件
    {

        Timeout --;                                                 //等待时，计数值自减
        if (Timeout == 0)                                           //自减到0后，等待超时
        {
            
            
            return 1;
            //break;                     //跳出等待，不等了
        }
    }  
    
    return 0;
    
}










