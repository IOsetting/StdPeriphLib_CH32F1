/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2019/10/15
* Description        : Main program body.
*******************************************************************************/ 

/*
 *@Note
 外部触发ADC转换例程：
 ADC通道2(PA2)-注入组通道，外部触发引脚(PA15)高电平触发 EXTI线15 事件，
 该模式下，通过 EXTI线15 事件触发一次ADC转换，转换结束后产生JEOC中断。

*/

#include "debug.h"


/*******************************************************************************
* Function Name  : ADC_Function_Init
* Description    : Initializes ADC collection.
* Input          : None
* Return         : None
*******************************************************************************/ 
void ADC_Function_Init(void)
{
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;	

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );	 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE );	  	
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);  
                   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	ADC_DeInit(ADC1);  
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigInjecConv_Ext_IT15_TIM8_CC4;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	
	ADC_InitStructure.ADC_NbrOfChannel = 1;	
	ADC_Init(ADC1, &ADC_InitStructure);	 

  ADC_InjectedSequencerLengthConfig(ADC1, 1);
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_239Cycles5);
  ADC_ExternalTrigInjectedConvCmd(ADC1, ENABLE);		

	NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	ADC_ITConfig( ADC1, ADC_IT_JEOC, ENABLE);  
	ADC_Cmd(ADC1, ENABLE);	

	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));	
	ADC_StartCalibration(ADC1);	 
	while(ADC_GetCalibrationStatus(ADC1));	 
}

/*******************************************************************************
* Function Name  : EXTI_Event_Init
* Description    : Initializes EXTI.
* Input          : None
* Return         : None
*******************************************************************************/ 
void EXTI_Event_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE );	 
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO, ENABLE );

	GPIO_EXTILineConfig( GPIO_PortSourceGPIOA, GPIO_PinSource15 );	

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_Init(GPIOA, &GPIO_InitStructure);					
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line15; 
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Event;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	
}

/*******************************************************************************
* Function Name  : ADC1_2_IRQHandler
* Description    : This function handles analog watchdog exception.
* Input          : None
* Return         : None
*******************************************************************************/
void ADC1_2_IRQHandler()
{
	u16 ADC_val;

	if(ADC_GetITStatus( ADC1, ADC_IT_JEOC)){  
		printf("ADC Extline trigger conversion...\r\n");
		ADC_val = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);  
		printf( "JADC%04d\r\n", ADC_val );
	}
	
	ADC_ClearITPendingBit( ADC1, ADC_IT_JEOC);
}

/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Return         : None
*******************************************************************************/
int main(void)
{
	USART_Printf_Init(115200);
	printf("SystemClk:%d\r\n",SystemCoreClock);
	
	ADC_Function_Init();
	EXTI_Event_Init(); 
	
	while(1);	
}




