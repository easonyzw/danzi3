#include "stm32f10x.h"                  // Device header
#include "delay.h"
GPIO_InitTypeDef GPIO_InitStructure;
GPIO_InitTypeDef GPIO_InitStructure1;
void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13| GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure1.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure1.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure1.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure1);
}

uint8_t Get_Key_1(void)
{
	uint8_t KeyNum = 0;
	if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15) == 0);
		Delay_ms(20);
		KeyNum = 1;
	}	
	return KeyNum;
}
uint8_t Get_Key_2(void)
{
    uint8_t KeyNum = 0;
    if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14) == 0);
		Delay_ms(20);
		KeyNum = 1;
	}

    return KeyNum;
}

uint8_t Get_Key_3(void)
{
    uint8_t KeyNum = 0;
    if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 0);
		Delay_ms(20);
		KeyNum = 1;
	}

    return KeyNum;
}

uint8_t Get_Key_4(void)
{
    uint8_t KeyNum = 0;
		uint32_t Key4_Time=0;
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0)
		{
			Key4_Time++;
			Delay_ms(1);
		}
		Delay_ms(20);
		
		if(Key4_Time<=800)KeyNum = 1;
		else KeyNum=2;
	}

    return KeyNum;
}
