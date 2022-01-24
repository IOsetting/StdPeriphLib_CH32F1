/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2019/10/15
* Description        : Main program body.
*******************************************************************************/ 

/*
 *@Note
 单脉冲输出例程：
 TIM1_CH1(PA8)，TIM1_CH2(PA9)
 本例程演示 在单脉冲模式下，TIM1_CH2(PA9) 引脚检测到一个上升沿，则在 
 TIM1_CH1(PA8) 输出正脉冲。
 
*/

#include "debug.h"

/*******************************************************************************
* Function Name  : One_Pulse_Init
* Description    : Initializes TIM1 one pulse.
* Input          : arr: the period value.
*                  psc: the prescaler value.
*									 ccp: the pulse value.
* Return         : None
*******************************************************************************/
void One_Pulse_Init( u16 arr, u16 psc, u16 ccp )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_TIM1, ENABLE );	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOA, &GPIO_InitStructure );

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init( GPIOA, &GPIO_InitStructure);

	TIM_TimeBaseInitStructure.TIM_Period = arr;	
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;	
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;	
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;	
	TIM_TimeBaseInit( TIM1, &TIM_TimeBaseInitStructure);	

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;	
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;		
	TIM_OCInitStructure.TIM_Pulse = ccp;	
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  
	TIM_OC1Init( TIM1, &TIM_OCInitStructure );  

	TIM_CtrlPWMOutputs(TIM1, ENABLE );	

	TIM_ICStructInit( &TIM_ICInitStructure );  
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;	
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;  
	TIM_ICInitStructure.TIM_ICFilter = 0x00;	
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;  	
	TIM_ICInit( TIM1, &TIM_ICInitStructure );	
	
	TIM_SelectOnePulseMode( TIM1,TIM_OPMode_Single );
	TIM_SelectInputTrigger( TIM1, TIM_TS_TI2FP2 );
	TIM_SelectSlaveMode( TIM1, TIM_SlaveMode_Trigger );
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
		
	One_Pulse_Init( 200, 48000-1, 100 );	
	
	while(1);
}

