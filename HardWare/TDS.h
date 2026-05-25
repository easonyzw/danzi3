#ifndef __TDS_H
#define __TDS_H

void DO_Init(void);
uint16_t ad_GetValue(uint8_t ADC_Channel);
u16 Get_ad_Average(uint8_t ADC_Channel,uint8_t times);
float DO_Read_5V(uint8_t ch, uint8_t avgTimes);
#endif
