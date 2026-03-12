#ifndef __LIGHT_SENSOR_H
#define __LIGHT_SENSOR_H

#include "stm32f10x.h"

float LightSensor_GetPositionCentered(void);
void LightSensor_Init(void);


#endif /* __LIGHT_SENSOR_H */


//GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13);














