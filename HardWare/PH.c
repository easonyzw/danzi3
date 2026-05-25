#include "stm32f10x.h"                  // Device header
#include "AD.h"

void PH_Init(void)
{
	AD_Init();
}

float Get_PH(uint8_t ADC_Channel,uint8_t times)
{
	uint16_t adx;
	float PH_Value;
	adx = Get_AD_Average(ADC_Channel,times);
	
	PH_Value = adx*3.3/4096;                    //得到采集PH值的电压
  PH_Value = -5.7541*PH_Value+15.654;           //通过公式转换成PH值

  if(PH_Value<=0) 
		PH_Value = 0;                            //PH值小于0 矫正为0
  else if(PH_Value>=14) 
		PH_Value = 14;                    //PH值大于14 矫正为14
	
	return PH_Value;
}
