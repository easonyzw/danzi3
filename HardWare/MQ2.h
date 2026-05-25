#ifndef __MQ2_H
#define __MQ2_H

//使用ADC1的通道2（同时是UART2的TX端口）



#include "math.h"
#include "stm32f10x.h"

void MQ2_Init(void);
uint16_t AD2_GetValue(uint8_t ADC_Channel);
u16 Get2_AD_Average(uint8_t ADC_Channel,uint8_t times);
float MQ2_GetPPM(uint8_t ADC_Channel,uint8_t times);//获取当前烟雾浓度，填入通道和采样次数后取平均值

#endif

