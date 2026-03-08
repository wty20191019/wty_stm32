#include "LightSensor.h"
#include "string.h"


//===================================================================================================
// 光敏传感器量化函数
//===================================================================================================
float LightSensor_GetPositionCentered(void)
{
    // 读取传感器状态
    float sensors[6];
    sensors[0] = (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0) ? 1.0f : 0.0f;  // b1
    sensors[1] = (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0) ? 1.0f : 0.0f;  // b0
    sensors[2] = (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5) == 0) ? 1.0f : 0.0f;  // a5
    sensors[3] = (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == 0) ? 1.0f : 0.0f;  // a4
    sensors[4] = (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3) == 0) ? 1.0f : 0.0f;  // a3
    sensors[5] = (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == 0) ? 1.0f : 0.0f;  // a2
    
    // 中心化位置（-2.5到2.5）
    float positions[6] = {-3.0f, -1.5f, -0.5f, 0.5f, 1.5f, 3.0f};
    
    float weighted_sum = 0.0f;
    float total_sensors = 0.0f;
    
    for (int i = 0; i < 6; i++) {
        weighted_sum += sensors[i] * positions[i];
        total_sensors += sensors[i];
    }
    
    if (total_sensors < 0.001f) {
        return 0.0f;
    }
    
    // 直接返回-2.5到2.5的中心化位置
    return weighted_sum / total_sensors;
}


