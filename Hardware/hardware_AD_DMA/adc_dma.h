#ifndef __AD_H
#define __AD_H


//==============================
/*

AD_Value[0] → PB1 (ADC1通道9)

AD_Value[1] → PB0 (ADC1通道8)

AD_Value[2] → PA5 (ADC1通道5)

AD_Value[3] → PA4 (ADC1通道4)

AD_Value[4] → PA3 (ADC1通道3)

AD_Value[5] → PA2 (ADC1通道2)

*/
//==============================

extern uint16_t AD_Value[6];

void AD_DMA_Init(void);

#endif














