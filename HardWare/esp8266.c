#include "esp8266.h"
#include "string.h"
#include "Delay.h"

__MESSAGE wifi={0};
//底层串口初始化，采用串口2连接ESP8266
void ESP8266_Config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStructure);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART1, ENABLE);
	Delay_ms(1000);
}



void USART1_IRQHandler(void)
{
	uint8_t data =0;
	if(USART_GetITStatus(USART1,USART_IT_RXNE))
	{
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		wifi.rxbuff[wifi.rxcount++] = USART_ReceiveData(USART1);
		data = USART_ReceiveData(USART1); //接收wifi模块回应的数据
	}
	if(USART_GetITStatus(USART1,USART_IT_IDLE))
	{
		USART1->DR;
		wifi.rxover = 1;
		wifi.rxcount =0;
	}
	DataAnylize();
}

//1发送一个字节数据
void USART13_SendByte(uint8_t data)
{
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET)
	{
	}
	USART_SendData(USART1,data);
}

//2发送字符串
void WifiSendStr(char *p)
{
	while(*p !='\0')
	{
		USART13_SendByte(*p++);
	}
}
//3发送一定长度的字符串
void WifiSendbuff(uint8_t *p,uint8_t lenth)
{
	for(uint8_t i=0;i<lenth;i++)
	{
		USART13_SendByte(p[i]);
	}
}


//清空缓存区数据
void Clear_BuffData(void)
{
	memset(wifi.rxbuff,0,RXMAX);
	wifi.rxcount = 0;
	wifi.rxover = 0;
}

/**
 * 功能：查找字符串中是否包含另一个字符串
 * 参数：
 *       dest：待查找目标字符串
 *       src：待查找内容
 *       timeout: 查询超时时间
 * 返回值：查找结果  返回所查找字符串在整体字符串中的位置
 *						 = NULL  没有找到字符串		查找失败
 */
char *FindStr(char *dest,char *src,uint32_t outtime)
{
	while((outtime--) && (strstr(dest,src)==NULL))
	{
		Delay_ms(1);
	}
	return strstr(dest,src);
}

/**
 * 功能：ESP8266发送指令获取应答
 * 参数：
 *       cmd -- 指令字符串
 *       ack -- 应答字符串
 *       timeout -- 应答溢出时长
 *			 check_cnt -- 循环发送指令的次数（可能模组没反应过来，多发几次）
 * 返回值：0 -- 成功		>0 -- 失败
 */
uint8_t WifiSendRevAck(char *cmd,char *ack,uint32_t timeout,uint8_t check_cnt)
{
	uint16_t lenth = strlen((char *)cmd);
	wifi.rxcount = 0;
	memset(wifi.rxbuff,0,sizeof(wifi.rxbuff));
	do{
			if(*cmd)  //判断指令是否为空 -- 无效指令
			{ 
				WifiSendbuff((u8 *)cmd,lenth);
			}
			if(*ack) //判断是否有应答 -- 参数是否正确
			{
				if(FindStr((char *)wifi.rxbuff,(char *)ack,timeout)!=NULL)
				{
					wifi.rxcount = 0;
					return 0;
				}
			}
	}while(--check_cnt);
	return 1;
}


//打开透传
uint8_t Wifi_OpenTransmission(void)
{
	return WifiSendRevAck("AT+CIPMODE=1\r\n","OK",1000,2);
}
//关闭透传
void Wifi_CloseTransmission(void)
{
	WifiSendStr("+++");
	Delay_ms(2000);
}


//连接热点
uint8_t ConnectToHotspot(void)
{
	static uint8_t stacnt = 3;
	char buff[36]={0};
	
	Wifi_CloseTransmission();
	
	if(WifiSendRevAck("AT\r\n","OK",100,2)!=0) return 1;
	if(WifiSendRevAck("AT+CWMODE=1\r\n","OK",1000,2)!=0) return 2;
	sprintf(buff,"AT+CWJAP=\"%s\",\"%s\"\r\n",ACCOUNT,PASSWORD);
	if(WifiSendRevAck(buff,"OK",20000,2)!=0) return 3;
	return 0;
}

//连接服务器
uint8_t Wifi_ConnectServer(char *mode,char *ip,uint16_t port)
{
	
	char buff[128]={0};
	sprintf(buff,"AT+CIPSTART=\"%s\",\"%s\",%d\r\n",mode,ip,port);

	if(WifiSendRevAck(buff,"CONNECT",10000,2)==WIFI_ACK_OK)
	{

	}
	else
	{
		return WIFI_ACK_ERROR;
	}
	//设置透传
	if(Wifi_OpenTransmission()==WIFI_ACK_OK)
	{

	}
	else
	{
		return WIFI_ACK_ERROR;
	}
	if(WifiSendRevAck("AT+CIPSEND\r\n",">",500,2)==WIFI_ACK_OK)
	{

	}
	else
	{
		return WIFI_ACK_ERROR;
	}
	return WIFI_ACK_OK;
}


/*******************************************************************************
函数名称：ConnectToOneNetServer
函数作用：链接巴法服务器
函数入口：无
函数出口：无

*******************************************************************************/
uint8_t ConnectToBaffServer(void)
{
	uint8_t Timers = 2;
		
	Wifi_CloseTransmission();    //多次连接需退出透传
	Delay_ms(500);
	
	//连接服务器
	while(Timers--)
	{       
		if(Wifi_ConnectServer("TCP",SERVERIP,SERVERPORT) == WIFI_ACK_OK)
		{
			return 1;
		}
	}
	return 0;
}

uint8_t ASCLL_TO_Oct(char c)
{
	uint8_t num;
	
	switch(c)
	{
		case 0x30:num = 0;break;
		case 0x31:num = 1;break;
		case 0x32:num = 2;break;
		case 0x33:num = 3;break;
		case 0x34:num = 4;break;
		case 0x35:num = 5;break;
		case 0x36:num = 6;break;
		case 0x37:num = 7;break;
		case 0x38:num = 8;break;
		case 0x39:num = 9;break;
	}
	return num;
}

extern uint8_t inter_baojing_Flag;
char* topic;
uint8_t msg[16];
extern float Temp_H;
extern uint16_t MQ2_H;
extern uint8_t inter_Flag;
uint16_t T_H;
extern uint8_t PHH;
extern uint16_t mq2_val_H;
uint8_t Inter_Flag=0;
extern float rongjieyangh;
void DataAnylize(void)
{
	char *token, *key, *value;
	char *delimiters = "&=";
	uint8_t data=0;
	uint8_t len = 0;
	if(wifi.rxover==1)
	{
		//cmd=2&uid=%s&topic=Set&msg=%s
		wifi.rxover=0;
		if((topic = strstr((char*)wifi.rxbuff,"MQ432"))!=NULL)//鹿媒脗脣碌么脦脼脨搂脢媒戮脻
		{
			//Set&msg=%s
			sscanf(topic,"%*6s%s",msg);
			//msg=100
			len = strlen((char*)msg);
			if(len == 5)
			{
				PHH = ASCLL_TO_Oct(msg[4]);
			}
			else if(len == 6)
			{
				PHH = ASCLL_TO_Oct(msg[4]) * 10 +  ASCLL_TO_Oct(msg[5]);
			}
			else if(len == 7)
			{
				PHH = ASCLL_TO_Oct(msg[4]) * 100 +  ASCLL_TO_Oct(msg[5]) * 10 + ASCLL_TO_Oct(msg[6]);
			}
			Clear_BuffData();
		}

		if((topic = strstr((char*)wifi.rxbuff,"MQ732"))!=NULL)//鹿媒脗脣碌么脦脼脨搂脢媒戮脻
		{
			//Set&msg=%s
			sscanf(topic,"%*6s%s",msg);
			//msg=100
			len = strlen((char*)msg);
			if(len == 5)
			{
				mq2_val_H = ASCLL_TO_Oct(msg[4]);
			}
			else if(len == 6)
			{
				mq2_val_H = ASCLL_TO_Oct(msg[4]) * 10 +  ASCLL_TO_Oct(msg[5]);
			}
			else if(len == 7)
			{
				mq2_val_H = ASCLL_TO_Oct(msg[4]) * 100 +  ASCLL_TO_Oct(msg[5]) * 10 + ASCLL_TO_Oct(msg[6]);
			}
			else if(len == 8)
			{
				mq2_val_H = ASCLL_TO_Oct(msg[4]) * 1000 +  ASCLL_TO_Oct(msg[5]) * 100 + ASCLL_TO_Oct(msg[6])*10+ASCLL_TO_Oct(msg[7]);
			}
			Clear_BuffData();
	
		}
			if((topic = strstr((char*)wifi.rxbuff,"MQ1032"))!=NULL)//鹿媒脗脣碌么脦脼脨搂脢媒戮脻
		{
			//Set&msg=%s
			sscanf(topic,"%*7s%s",msg);
			//msg=100
			len = strlen((char*)msg);
			if(len == 5)
			{
				Temp_H = ASCLL_TO_Oct(msg[4]);
			}
			else if(len == 6)
			{
				Temp_H = ASCLL_TO_Oct(msg[4]) * 10 +  ASCLL_TO_Oct(msg[5]);
			}
			Clear_BuffData();
	
		}
		if((topic = strstr((char*)wifi.rxbuff,"MQ13632"))!=NULL)//鹿媒脗脣碌么脦脼脨搂脢媒戮脻
		{
			//Set&msg=%s
			sscanf(topic,"%*8s%s",msg);
			//msg=100
			len = strlen((char*)msg);
			if(len == 5)
			{
				rongjieyangh = ASCLL_TO_Oct(msg[4]);
			}
			else if(len == 6)
			{
				rongjieyangh = ASCLL_TO_Oct(msg[4]) * 10 +  ASCLL_TO_Oct(msg[5]);
			}
			else if(len == 7)
			{
				rongjieyangh = ASCLL_TO_Oct(msg[4]) * 100 +  ASCLL_TO_Oct(msg[5]) * 10 + ASCLL_TO_Oct(msg[6]);
			}
			else if(len == 8)
			{
				rongjieyangh = ASCLL_TO_Oct(msg[4]) * 1000 +  ASCLL_TO_Oct(msg[5]) * 100 + ASCLL_TO_Oct(msg[6])*10+ASCLL_TO_Oct(msg[7]);
			}
			Clear_BuffData();
	
		}
				if((topic = strstr((char*)wifi.rxbuff,"1GTH20"))!=NULL)//鹿媒脗脣碌么脦脼脨搂脢媒戮脻
		{
			//Set&msg=%s
			uint8_t len;
			sscanf(topic,"%*7s%s",msg);
			//msg=100
			len = strlen((char*)msg);
			if(len == 5)
			{
				Inter_Flag = ASCLL_TO_Oct(msg[4]);
			}
			else if(len == 6)
			{
				Inter_Flag = ASCLL_TO_Oct(msg[4]) * 10 +  ASCLL_TO_Oct(msg[5]);
			}
			else if(len == 7)
			{
				Inter_Flag = ASCLL_TO_Oct(msg[4]) * 100 +  ASCLL_TO_Oct(msg[5]) * 10 + ASCLL_TO_Oct(msg[6]);
			}
			Clear_BuffData();
		}
	}
}