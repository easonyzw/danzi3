#ifndef __HZD_H
#define __HZD_H


void HZD_Init(void);

uint16_t AD_GetValue_HZD(uint8_t ADC_Channel);

u16 Get_AD_Average_HZD(uint8_t ADC_Channel,uint8_t times);

float Get_HZD_V(uint8_t ADC_Channel,uint8_t times);

float Get_HZD(uint8_t ADC_Channel,uint8_t times);

#endif
