#include "LightSensor.h"
#include "string.h"


void LightSensor_Init(void)
{
    
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

GPIO_InitTypeDef GPIO_InitStructure;

// 初始化GPIOA的A4和A5引脚
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_Init(GPIOA, &GPIO_InitStructure);

// 初始化GPIOB的B0和B1引脚
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_Init(GPIOB, &GPIO_InitStructure);

}

//===================================================================================================
// 光敏传感器量化函数
//===================================================================================================
float LightSensor_GetPositionCentered(void)
{
    // 读取传感器状态
    float sensors[4];
    sensors[0] = (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0) ? 1.0f : 0.0f;  // B1
    sensors[1] = (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0) ? 1.0f : 0.0f;  // B0
    sensors[2] = (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5) == 0) ? 1.0f : 0.0f;  // A5
    sensors[3] = (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == 0) ? 1.0f : 0.0f;  // A4
    
    if(            sensors[0]== 0
                && sensors[1]== 1
                && sensors[2]== 1
                && sensors[3]== 0)
                {
                    
                    return 0;
                }
    else if(       sensors[0]== 0
                && sensors[1]== 0
                && sensors[2]== 1
                && sensors[3]== 0)
                {
                    
                    return -1;
                }
    else if(       sensors[0]== 0
                && sensors[1]== 1
                && sensors[2]== 0
                && sensors[3]== 0)
                {
                    
                    
                    return +2;
                }
    else if(       sensors[0]== 1
                && sensors[1]== 0
                && sensors[2]== 0
                && sensors[3]== 0)
                {
                    
                    return +1;
                }
    else if(       sensors[0]== 0
                && sensors[1]== 0
                && sensors[2]== 0
                && sensors[3]== 1)
                {
                    
                    return -2;
                }
    else
                {
                    
                    return 0;
                }

}





