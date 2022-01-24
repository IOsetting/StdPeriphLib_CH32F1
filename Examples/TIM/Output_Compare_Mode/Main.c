/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2019/10/15
* Description        : Main program body.
*******************************************************************************/ 

/*
 *@Note
 输出比较模式例程：
 TIM1_CH1(PA8)
 本例程演示 TIM_CH1(PA8) 引脚在4种输出比较模式下，输出波形。输出比较模式包括
 OutCompare_Timing、OutCompare_Active、OutCompare_Inactive、OutCompare_Toggle。
 
*/

#include "debug.h"

/* Output Compare Mode Definition */
#define OutCompare_Timing   0
#define OutCompare_Active   1
#define OutCompare_Inactive 2
#define OutCompare_Toggle   3

/* Output Compare Mode Selection */
#define OutCompare_MODE OutCompare_Timing
//#define OutCompare_MODE OutCompare_Active
//#define OutCompare_MODE OutCompare_Inactive
//#define OutCompare_MODE OutCompare_Toggle

/*******************************************************************************
* Function Name  : TIM1_OutCompare_Init
* Description    : Initializes TIM1 output compare.
* Input          : arr: the period value.
*                  psc: the prescaler value.
*									 ccp: the pulse value.
* Return         : None
*******************************************************************************/
void TIM1_OutCompare_Init( u16 arr, u16 psc, u16 ccp )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_TIM1, ENABLE );

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOA, &GPIO_InitStructure );
		
	TIM_TimeBaseInitStructure.TIM_Period = arr;	
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;	
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;	
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	
	TIM_TimeBaseInit( TIM1, &TIM_TimeBaseInitStructure);	

#if (OutCompare_MODE == OutCompare_Timing)
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;	
	
#elif (OutCompare_MODE == OutCompare_Active)		
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Active;	
	
#elif (OutCompare_MODE == OutCompare_Inactive)		
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Inactive;	
	
#elif (OutCompare_MODE == OutCompare_Toggle)			
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;	
	
#endif

	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;		
	TIM_OCInitStructure.TIM_Pulse = ccp;	
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;   
	TIM_OC1Init( TIM1, &TIM_OCInitStructure );  

	TIM_CtrlPWMOutputs(TIM1, ENABLE );		
	TIM_OC1PreloadConfig( TIM1, TIM_OCPreload_Disable );
	TIM_ARRPreloadConfig( TIM1, ENABLE );		
	TIM_Cmd( TIM1, ENABLE );
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

	TIM1_OutCompare_Init( 100, 48000-1, 50 );	
	
	while(1);
}

