#ifndef __LIGHT_SENSOR_H
#define __LIGHT_SENSOR_H

#include "stm32f10x.h"

void LightSensor_Init(void);
float LightSensor_GetPos(void);

#endif /* __LIGHT_SENSOR_H */


//GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13);














