#include "stm32f10x.h"                  // Device header
#include "delay.h"

// 溶解氧ADC初始化
void DO_Init(void)
{
    // 开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);   
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  

    // 设置ADC时钟 72M/6=12MHz
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);                    

    // PA1 模拟输入
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // ADC基础配置
    ADC_InitTypeDef ADC_InitStructure;
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    // ADC使能 + 校准
    ADC_Cmd(ADC1, ENABLE);
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1) == SET);
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1) == SET);
}

// 获取单次AD原始值
uint16_t ad_GetValue(uint8_t ADC_Channel)
{
    ADC_RegularChannelConfig(ADC1, ADC_Channel, 1, ADC_SampleTime_55Cycles5);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);              
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET); 
    return ADC_GetConversionValue(ADC1);                
}

// 获取多次AD平均值
u16 Get_ad_Average(uint8_t ADC_Channel, uint8_t times)
{
    u32 temp_val = 0;
    u8 t;
    for (t = 0; t < times; t++)
    {
        temp_val += ad_GetValue(ADC_Channel);
        Delay_ms(5);
    }
    return temp_val / times;
}

// 新增：5V版本 电压+溶解氧换算
// 5V参考：ADC最大值4095对应5000mV
float DO_Read_5V(uint8_t ch, uint8_t avgTimes)
{
    u16 adc = Get_ad_Average(ch, avgTimes);
    // 电压(mV) = ADC * 5000 / 4095
    float voltage = (float)adc * 5000.0f / 4095.0f;  
    
    // 溶解氧通用换算公式（5V模块默认适配，可自行微调）
    // 0mV≈0mg/L  5000mV≈20mg/L 常规量程
    float do_val = voltage / 250.0f;  
    return do_val;
}