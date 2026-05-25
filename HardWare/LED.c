#include "stm32f10x.h"                  // Device header
#include "delay.h"
 
 void LED_Config(void)
 {
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	 
		GPIO_InitTypeDef GPIO_InitStructure;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 
	 GPIO_Init(GPIOA,&GPIO_InitStructure);

	 
 }
 void buzzer_Init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_0);
}

 void LED_ON(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_5);
}

void LED_OFF(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_5);
}
void LED_Turn(void)
{
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)==0)
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_5);
	}
	else
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_5);
	}
}

void buzzer_ON()
{
	GPIO_SetBits(GPIOA, GPIO_Pin_0);//

}

void buzzer_OFF()
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_0);
}

 void jairepian_ON(void)
	 
{
		GPIO_SetBits(GPIOA,GPIO_Pin_5);
	
}

void jiarepian_OFF(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_5);
}
void jairepian_Turn(void)
{
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)==0)
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_5);
	}
	else
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_5);
	}
}


void shuibeng_ON(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_8);
	
}

void shuibeng_OFF(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_8);
}
void shuibeng_Turn(void)
{
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)==0)
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_8);
	}
	else
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_8);
	}
}


void fengshan_ON(void)
{
	GPIO_SetBits(GPIOB,GPIO_Pin_13);

}

void fengshan_OFF(void)
{
		GPIO_ResetBits(GPIOB,GPIO_Pin_13);
}
void fengshan_Turn(void)
{
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)==0)
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_13);
	}
	else
	{
		GPIO_ResetBits(GPIOB,GPIO_Pin_13);
	}
}