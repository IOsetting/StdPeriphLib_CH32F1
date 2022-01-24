/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2019/10/15
* Description        : Main program body.
*******************************************************************************/ 

/*
 *@Note
 定时器同步模式：
 TIM1_CH1(PA8)
 本例程演示 4 种定时器同步模式。
 
 注：在 debug 模式下查看 TIM1->CNT和 TIM2->CNT观察现象。
 
*/

#include "debug.h"


/*******************************************************************************
* Function Name  : TIM_TimSynchroMode1_Init
* Description    : Using TIM2 as prescaler for TIM1.
* Input          : None
* Return         : None
*******************************************************************************/
void TIM_TimSynchroMode1_Init(void)
{
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_TIM1, ENABLE );	
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2, ENABLE );

  TIM_CounterModeConfig( TIM1, TIM_CounterMode_Up );
	TIM_CounterModeConfig( TIM2, TIM_CounterMode_Up );	
	TIM_SetAutoreload( TIM1, 0x3E8 );		
	TIM_PrescalerConfig( TIM1, 48000-1, TIM_PSCReloadMode_Immediate );	
	TIM_SelectOutputTrigger( TIM1, TIM_TRGOSource_Update );		
	TIM_ITRxExternalClockConfig( TIM2, TIM_TS_ITR0 );		
	TIM_SelectSlaveMode( TIM2, TIM_SlaveMode_External1 );   
		
	TIM_Cmd( TIM1, ENABLE );
	TIM_Cmd( TIM2, ENABLE );
}

/*******************************************************************************
* Function Name  : TIM_TimSynchroMode2_Init
* Description    : Using TIM2 to use TIM1.
* Input          : None
* Return         : None
*******************************************************************************/
void TIM_TimSynchroMode2_Init(void)
{
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_TIM1, ENABLE );		
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2, ENABLE );

  TIM_CounterModeConfig( TIM1, TIM_CounterMode_Up );	
	TIM_CounterModeConfig( TIM2, TIM_CounterMode_Up );		
	TIM_SetAutoreload( TIM1, 0x3E8 );		
	TIM_PrescalerConfig( TIM1, 48000-1, TIM_PSCReloadMode_Immediate );	

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;		
	TIM_OCInitStructure.TIM_Pulse = 0x64;	
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  
	TIM_OC1Init( TIM1, &TIM_OCInitStructure );  
	
	TIM_SelectOutputTrigger( TIM1, TIM_TRGOSource_OC1Ref );		
	TIM_SelectInputTrigger( TIM2, TIM_TS_ITR0 );			
	TIM_SelectSlaveMode( TIM2, TIM_SlaveMode_Gated );   
	
	TIM_Cmd( TIM2, ENABLE );	
	TIM_Cmd( TIM1, ENABLE );	
}

/*******************************************************************************
* Function Name  : TIM_TimSynchroMode3_Init
* Description    : Using TIM2 to start TIM1.
* Input          : None
* Return         : None
*******************************************************************************/
void TIM_TimSynchroMode3_Init(void)
{
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_TIM1, ENABLE );		
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2, ENABLE );

  TIM_CounterModeConfig( TIM1, TIM_CounterMode_Up );		
	TIM_CounterModeConfig( TIM2, TIM_CounterMode_Up );	
	TIM_SetAutoreload( TIM1, 0xFFFF );			
	TIM_PrescalerConfig( TIM1, 48000-1, TIM_PSCReloadMode_Immediate );	
	TIM_SelectOutputTrigger( TIM1, TIM_TRGOSource_Update );		
	TIM_SelectInputTrigger( TIM2, TIM_TS_ITR0 );			
	TIM_SelectSlaveMode( TIM2, TIM_SlaveMode_Trigger );   	
	TIM_Cmd( TIM1, ENABLE );	
}

/*******************************************************************************
* Function Name  : TIM_TimSynchroMode4_Init
* Description    : Starting TIM1 and TIM2 synchronously in response to an external trigger.  
* Input          : None
* Return         : None
*******************************************************************************/
void TIM_TimSynchroMode4_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_TIM1, ENABLE );	
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2, ENABLE );

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init( GPIOA, &GPIO_InitStructure);
  GPIO_ResetBits( GPIOA, GPIO_Pin_8 );	

  TIM_CounterModeConfig( TIM1, TIM_CounterMode_Up );	
	TIM_CounterModeConfig( TIM2, TIM_CounterMode_Up );		
	TIM_SetAutoreload( TIM1, 0xFFFF );		
	TIM_PrescalerConfig( TIM1, 48000-1, TIM_PSCReloadMode_Immediate );	

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
/* Timer synchronization Mode Selection */
	TIM_TimSynchroMode1_Init();
//	TIM_TimSynchroMode2_Init();	
//	TIM_TimSynchroMode3_Init();
//	TIM_TimSynchroMode4_Init();	
	
	while(1);
}

