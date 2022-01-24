/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2019/10/15
* Description        : Main program body.
*******************************************************************************/ 

/*
 *@Note
 从模式例程：
 TIM1_CH1(PA8)
 本例程演示当 TIM_CH1(PA8) 引脚检测到上升沿时，在不同从模式下，TIM1 不同工作方式。
 RESET_MODE：TIM1->CNT 复位重新计数
 GATED_MODE：PA8引脚输入低电平，TIM1->CNT 正常计数，否则停止计数。
 TRIGGER__MODE：TIM1->CNT 继续计数。
 注：在 debug 模式下查看 TIM1->CNT 观察现象。
 
*/

#include "debug.h"

/* Slave Mode Definition */
#define RESET_MODE      0
#define GATED_MODE      1
#define TRIGGER__MODE   2

/*  Slave Mode Selection */
//#define SLAVE_MODE   RESET_MODE
//#define SLAVE_MODE   GATED_MODE
#define SLAVE_MODE   TRIGGER__MODE

/*******************************************************************************
* Function Name  : TIM1_Synchro_ExtTrigger_Init
* Description    : Initializes TIM1 external trigger synchronization.
* Input          : arr: the period value.
*                  psc: the prescaler value.
* Return         : None
*******************************************************************************/
void TIM1_Synchro_ExtTrigger_Init( u16 arr, u16 psc )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_TIM1, ENABLE );	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init( GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits( GPIOA, GPIO_Pin_8 );

	TIM_TimeBaseInitStructure.TIM_Period = arr;	
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;	
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;	
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter =  0x00;	
	TIM_TimeBaseInit( TIM1, &TIM_TimeBaseInitStructure);

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;	
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1; 
	TIM_ICInitStructure.TIM_ICFilter = 0x00;	
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;  
	TIM_ICInit( TIM1, &TIM_ICInitStructure );
	
	TIM_SelectInputTrigger( TIM1, TIM_TS_TI1FP1 );		
	
#if (SLAVE_MODE == RESET_MODE)	
	TIM_SelectSlaveMode( TIM1, TIM_SlaveMode_Reset );	

#elif (SLAVE_MODE == GATED_MODE)
	TIM_SelectSlaveMode( TIM1, TIM_SlaveMode_Gated );

#elif (SLAVE_MODE == TRIGGER__MODE)
	TIM_SelectMasterSlaveMode( TIM1, TIM_SlaveMode_Trigger );

#endif

	TIM_SelectMasterSlaveMode( TIM1, TIM_MasterSlaveMode_Enable );

#if (SLAVE_MODE != TRIGGER__MODE)
	TIM_Cmd( TIM1, ENABLE );
	
#endif
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
	
	TIM1_Synchro_ExtTrigger_Init( 0xFFFF, 48000-1);	
	
	while(1);
}

