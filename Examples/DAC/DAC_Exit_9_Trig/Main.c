/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2019/10/15
* Description        : Main program body.
*******************************************************************************/

/*
 *@Note
 外部触发DAC转换例程：
 DAC通道0(PA4)输出
 通过EXTI_9事件触发1次DAC转换，PA4输出相应的电压。

*/
 
#include "debug.h"

/* Global define */
#define Num 7

/* Global Variable */ 
u16 DAC_Value[Num]={64,128,256,512,1024,2048,4095};   



/*******************************************************************************
* Function Name  : Gpio_Init
* Description    : Initializes GPIO collection.
* Input          : None
* Return         : None
*******************************************************************************/ 
void Gpio_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 
  GPIO_Init(GPIOA, &GPIO_InitStructure);					 
  GPIO_SetBits(GPIOA,GPIO_Pin_0);
}

/*******************************************************************************
* Function Name  : Dac_Init
* Description    : Initializes DAC collection.
* Input          : None
* Return         : None
*******************************************************************************/ 
void Dac_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	DAC_InitTypeDef DAC_InitType;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE );
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				          
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 		     
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_4);
	
	DAC_InitType.DAC_Trigger=DAC_Trigger_Ext_IT9;	                         
	DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_None;             
	DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Disable ;	         
  DAC_Init(DAC_Channel_1,&DAC_InitType);

	DAC_Cmd(DAC_Channel_1, ENABLE); 
}


/*******************************************************************************
* Function Name  : EXTI_Event_Init
* Description    : Initializes EXTI collection.
* Input          : None
* Return         : None
*******************************************************************************/
void EXTI_Event_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
 	EXTI_InitTypeDef EXTI_InitStructure;
    
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOC,ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;              
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;           
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);                  
    
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource9); 
  EXTI_InitStructure.EXTI_Line=EXTI_Line9;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Event;	
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);	 	
  
}

/*******************************************************************************
* Function Name  : TIM3_Init
* Description    : Initializes TIM3 collection.
* Input          : None
* Return         : None
*******************************************************************************/
void TIM3_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 
	
	TIM_TimeBaseStructure.TIM_Period = arr;             
	TIM_TimeBaseStructure.TIM_Prescaler =psc;           
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); 
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);  

	TIM_Cmd(TIM3, ENABLE); 				 
}


/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Return         : None
*******************************************************************************/
int main(void)
{
	u8 i=0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  Delay_Init();
	USART_Printf_Init(115200);
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf("Timer Trig\r\n");
  Gpio_Init();
  TIM3_Init(9999,7199);    
	EXTI_Event_Init();        
	Dac_Init();
	
	while(1)
  {	
	  DAC->R12BDHR1=DAC_Value[i];
		i++;
		if(i>Num)
		{
		  i=0;
		}
	  Delay_Ms(1000);
		
	//TIM_GenerateEvent(TIM3,TIM_EventSource_Update);	
		printf("run\r\n");
		printf("DAC->R12BDHR1:0x%04x\r\n",DAC->R12BDHR1);
		printf("DAC->DOR1:0x%04x\r\n",DAC->DOR1);
		printf("TIM3->CNT:%d\r\n",TIM3->CNT);
		
	}
}

/*******************************************************************************
* Function Name  : TIM3_IRQHandler
* Description    : This function handles TIM3 Handler.
* Input          : None
* Return         : None
*******************************************************************************/
void TIM3_IRQHandler(void)  
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  
		PAout(0)^=1;      
	}
}


