#include "stm32f10x.h"		//STM32头文件
#include "Delay.h"				//延时函数头文件
#include "math.h"
#include "oled.h"					//OLED屏头文件
#include "key.h"					//按键头文件
#include "LED.h"					//LED灯头文件
#include "MQ2.h"  
#include "DS18B20.h"
#include "esp8266.h"
#include "AD.h"
#include "PH.h"
#include "TDS.h"
void GetData(void);			//获取数据
void Alarm(void);				//自动控制函数
void Menu_key_set(void);//按键扫描
void Mune(void);		    //按键切换页面操作
void Task(void);        //滴答定时器扫描函数
void Task_Time(void);       //时间函数，每秒Timer加1

// 全局变量定义
uint8_t buff[128]={0};
uint8_t key1=0,key2=0,key3=0,key4=0;
uint16_t OLED_Display=0; 			//i页面标志位
uint32_t timer_tt=0;
u8 water_level = 0;     // 水位（0-100）
uint8_t timer_ref=0;
float HZD;        //浑浊度
uint32_t Timer=0;
uint16_t key_tt=0;
uint8_t key_ref=0;
uint16_t GetData_tt=0;
uint8_t GetData_ref=0;
uint8_t Timer1=0;
uint8_t key_Set=0;
 u8  temp_threshold=80 ;    
 u8 humi_threshold=80 ;         
 u8 water_threshold=90 ;  
uint8_t PHH=10;
float Temp_H=30.00f;
 u8 hunzhuodu_threshold=100 ;
 u8 shuiwei_threshold =60; 
uint8_t connect_flag;
uint8_t baojing_Flag=0;
float HZD_H=400;
uint16_t adc;
u8 humi;
float Temp ;             // DS18B20水温
float PH=0;
float rongjieyang ;             // DS18B20水温
extern uint8_t Inter_Flag;
uint16_t mq2_val_H=700;				//电导率
uint16_t mq2_val=0;				//烟雾浓度
float rongjieyangh=20 ;  
void Task()
{
	if(++timer_tt==1000){timer_tt=0;timer_ref=1;}
	if(++key_tt==20){key_tt=0;key_ref=1;}      //每20ms获得按键数据
	if(++GetData_tt==2000){GetData_tt=0;GetData_ref=1;}    //每2秒获得一次传感器
}

int main(void)
{	
	SystemInit();  
	Delay_Init();					//滴答定时器初始化
	OLED_Init();					//OLED屏初始化
	Key_Init();						//按键初始化
	LED_Config();
	buzzer_Init();       //默认高电平触发
	PH_Init();
	DS18B20_Init();
	MQ2_Init();
	DS18B20_Start();	
	DO_Init();
	OLED_ShowChinese(40,28,"联网中");
	OLED_Update(); //更新数据
	
	//连接网络
	 // WiFi初始化
    ESP8266_Config();
    Delay_ms(500);
    ConnectToHotspot();
    connect_flag = ConnectToBaffServer();
    Delay_ms(500);
    // ESP8266初始化订阅
  sprintf((char*)buff,"cmd=2&uid=%s&topic=%s&msg=#%.2f#%d#%.2f#%0.2f#%d\r\n",PRODUCTID,"10DHT1135",PH,(int)mq2_val,Temp,rongjieyang,baojing_Flag);
		WifiSendStr((char*)buff);
     sprintf((char*)buff,"cmd=1&uid=%s&topic=%s\r\n",PRODUCTID,"MQ432"); //空气温度阙值
    WifiSendStr((char*)buff);
    sprintf((char*)buff,"cmd=1&uid=%s&topic=%s\r\n",PRODUCTID,"MQ732"); //空气湿度阙值
    WifiSendStr((char*)buff);
		sprintf((char*)buff,"cmd=1&uid=%s&topic=%s\r\n",PRODUCTID,"MQ1032");//营养液阙值
    WifiSendStr((char*)buff);
		sprintf((char*)buff,"cmd=1&uid=%s&topic=%s\r\n",PRODUCTID,"MQ13632");//营养液阙值
    WifiSendStr((char*)buff);
	   sprintf((char*)buff,"cmd=1&uid=%s&topic=%s\r\n",PRODUCTID,"1GTH20");//光照强度阙值
    WifiSendStr((char*)buff);
    
	while(1)
	{		


		if(timer_ref==1){timer_ref=0;Task_Time();}
		
		if(key_ref==1){key_ref=0;Menu_key_set();}
		
		if(GetData_ref==1)
			{
				GetData_ref=0;
				GetData();
				mq2_val = Get_AD_Average(ADC_Channel_1,10);
					//读取数据
			DS18B20_Start();
			Temp = DS18B20_Get_Temp();
			PH=Get_PH( ADC_Channel_7,10);  //PA7	
			rongjieyang=DO_Read_5V(ADC_Channel_6,10);
			}
		
		Alarm();
		Mune();
	}
}
void Task_Time()
{
	uint8_t Time_Frequency_1=2;    //每Time_Frequency进入一次
	Timer++;
   sprintf((char*)buff,"cmd=2&uid=%s&topic=%s&msg=#%.2f#%d#%.2f#%0.2f#%d\r\n",PRODUCTID,"10DHT1135",PH,(int)mq2_val,Temp,rongjieyang,baojing_Flag);
		WifiSendStr((char*)buff);
     sprintf((char*)buff,"cmd=1&uid=%s&topic=%s\r\n",PRODUCTID,"MQ432"); //空气温度阙值
    WifiSendStr((char*)buff);
    sprintf((char*)buff,"cmd=1&uid=%s&topic=%s\r\n",PRODUCTID,"MQ732"); //空气湿度阙值
    WifiSendStr((char*)buff);
		sprintf((char*)buff,"cmd=1&uid=%s&topic=%s\r\n",PRODUCTID,"MQ1032");//营养液阙值
    WifiSendStr((char*)buff);
		sprintf((char*)buff,"cmd=1&uid=%s&topic=%s\r\n",PRODUCTID,"MQ13632");//营养液阙值
    WifiSendStr((char*)buff);
	   sprintf((char*)buff,"cmd=1&uid=%s&topic=%s\r\n",PRODUCTID,"1GTH20");//光照强度阙值
    WifiSendStr((char*)buff);
	if(Timer%Time_Frequency_1==0)
	{
		
	}
}
void Mune(void)											//按键页面切换
{
	if(OLED_Display == 0)														//初始数据页面
	{
		
		OLED_Clear();	
		OLED_ShowString(00,00,"PH",OLED_8X16);
		OLED_ShowString(32,00,":",OLED_8X16);
		OLED_ShowFloatNum(42,00,PH,2,2,OLED_8X16);
		
		OLED_ShowChinese(0,16,"烟雾");
		OLED_ShowString(32,16,":",OLED_8X16);
		OLED_ShowNum(40,16,mq2_val,4,OLED_8X16);
		
		OLED_ShowChinese(0,32,"水温");
		OLED_ShowString(30,32,":",OLED_8X16);
		OLED_ShowNum(35,32,Temp,3,OLED_8X16);
				
			OLED_ShowChinese(0,48,"溶解氧");
		OLED_ShowString(48,48,":",OLED_8X16);
		OLED_ShowFloatNum(58,48,rongjieyang,4,2,OLED_8X16);
	}
	
	else if(OLED_Display==1)
	{
		OLED_Clear();	
		OLED_ShowString(0,0,"PH",OLED_8X16);
		OLED_ShowChinese(20,0,"阈值");
		OLED_ShowString(65,0,":",OLED_8X16);
		OLED_ShowNum(70,0,PHH,2,OLED_8X16);
		
		OLED_ShowChinese(0,16,"烟雾阈值");
		OLED_ShowString(65,16,":",OLED_8X16);
		OLED_ShowNum(70,16,mq2_val_H,4,OLED_8X16);
		
		
		OLED_ShowChinese(0,32,"水温阈值");
		OLED_ShowString(65,32,":",OLED_8X16);
		OLED_ShowFloatNum(70,32,Temp_H,2,2,OLED_8X16);	
				OLED_ShowChinese(0,48,"溶解氧阈值");
		OLED_ShowString(80,48,":",OLED_8X16);
		OLED_ShowNum(90,48,rongjieyangh,4,OLED_8X16);
			
	}
		else if(OLED_Display==2)
	{
		OLED_Clear();		
		OLED_ShowChinese(20,0,"手动模式");	

		OLED_ShowChinese(0,16,"打开充氧泵");	
		OLED_ShowChinese(0,32,"关闭充氧泵");
	
			
	}
	
	OLED_Update();
}
void Menu_key_set(void)									//按键扫描
{
	key1=Get_Key_1();
	key2=Get_Key_2();
	key3=Get_Key_3();
	key4=Get_Key_4();
	
	if(key1 == 1)   //按键1
	{
		key1=0;
		OLED_Clear();		
		if(OLED_Display==1)
		{
			//设置几个阙值，并且三个阙值来回切换
			if(++key_Set>=4)key_Set=0;
		}
		else if(OLED_Display==2)
		{
			//手动模式打开和关闭
			shuibeng_Turn();
		}
		
		
	
	}
	//加阙值
	if(key2 == 1)   //按键2
	{
		key2=0;
		OLED_Clear();
		if(OLED_Display==1)
		{
			if(key_Set==0){PHH++;}
			else if(key_Set==1){mq2_val_H+=10;}
			else if(key_Set==2){Temp_H++;}
			else if(key_Set==3){rongjieyangh++;}
		}
		
	}
	//减阙值
	if(key3 == 1)   //按键3
	{
		key3=0;
		OLED_Clear();
		if(OLED_Display==1)
		{
			if(key_Set==0){PHH--;}
			else if(key_Set==1){mq2_val_H-=10;}
			else if(key_Set==2){Temp_H--;}
			else if(key_Set==3){rongjieyangh--;}
		}
		
	}
	if(key4 == 1)   //按键3
	{
		key4=0;
		OLED_Clear();
		if(++OLED_Display>=3)OLED_Display=0;

	}
	else if(key4==2)
	{
		key4=0;
		OLED_Clear();

	}
}


void GetData(void)
{

 
}

void Alarm(void)
{
	// PH、烟雾、水温超出阈值时，只触发蜂鸣器和LED报警；
	// 溶解氧低于阈值时，单独启动充氧泵。
	if(OLED_Display==0)
	{
		if((PH>PHH)||(mq2_val>mq2_val_H)||(Temp>Temp_H))
		{
			buzzer_ON();
			baojing_Flag=1;
			LED_ON();
		}
		else
		{
			buzzer_OFF();
			baojing_Flag=0;
			LED_OFF();
		}
		
		if(rongjieyang<rongjieyangh)
		{
			shuibeng_ON();
		}
		else
		{
			shuibeng_OFF();
		}
	}
	else if(OLED_Display==1)
	{
		if(Inter_Flag==1)
		{
			shuibeng_ON();
		}
		else if(Inter_Flag==2)
		{
			shuibeng_OFF();
		}
	}
}
