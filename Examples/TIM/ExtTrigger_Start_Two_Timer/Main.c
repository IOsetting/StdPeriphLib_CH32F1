/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2019/10/15
* Description        : Main program body.
*******************************************************************************/ 

/*
 *@Note
 外部触发同步启动两个定时器例程：
 TIM1_CH1(PA8)
 本例程演示 TIM_CH1(PA8) 引脚上拉输入，该引脚检测到上升沿，则同时启动 TIM1 和 
 TIM2。
 注：在 debug 模式下查看 TIM1->CNT 和 TIM2->CNT 观察现象。
 
*/

#include "debug.h"

/*******************************************************************************
* Function Name  : ExtTrigger_Start_Two_TIM
* Description    : Starting 2 timers synchronously in response to an external trigger.
* Input          : arr: the period value.
*                  psc: the prescaler value.
* Return         : None
*******************************************************************************/
void ExtTrigger_Start_Two_TIM( u16 arr, u16 psc )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_TIM1, ENABLE );
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2, ENABLE );
	
	TIM_CounterModeConfig( TIM2, TIM_CounterMode_Up );	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init( GPIOA, &GPIO_InitStructure);

	TIM_TimeBaseInitStructure.TIM_Period = arr;	
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;	
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter =  0x00;	
	TIM_TimeBaseInit( TIM1, &TIM_TimeBaseInitStructure);

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;  
	TIM_ICInitStructure.TIM_ICFilter = 0x00;	
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;	
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; 
	TIM_ICInit( TIM1, &TIM_ICInitStructure );
	
	TIM_SelectInputTrigger( TIM1, TIM_TS_TI1FP1 );	
	TIM_SelectSlaveMode( TIM1, TIM_SlaveMode_Trigger );		
	TIM_SelectMasterSlaveMode( TIM1, TIM_MasterSlaveMode_Enable );		
	TIM_SelectOutputTrigger( TIM1, TIM_TRGOSource_Enable );		
	TIM_SelectInputTrigger( TIM2, TIM_TS_ITR0 );			
	TIM_SelectSlaveMode( TIM2, TIM_SlaveMode_Trigger );   
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

	ExtTrigger_Start_Two_TIM( 0xFFFF, 48000-1);	
	
	while(1);
}

