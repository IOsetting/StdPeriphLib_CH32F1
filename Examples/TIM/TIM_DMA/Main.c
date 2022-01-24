/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2019/10/15
* Description        : Main program body.
*******************************************************************************/ 

/*
 *@Note
 定时器使用DMA例程：
 TIM1_CH1(PA8)
 本例程演示 使用 DMA 通过 TIM1_CH1(PA8) 引脚输出 PWM。
 
*/

#include "debug.h"

/* CH1CVR register Definition */
#define TIM1_CH1CVR_ADDRESS   0x40012C34

/* Private variables */
u16 pbuf[3] = { 10, 50, 80 };

/*******************************************************************************
* Function Name  : TIM1_PWMOut_Init
* Description    : Initializes TIM1 PWM output.
* Input          : arr: the period value.
*                  psc: the prescaler value.
*									 ccp: the pulse value.
* Return         : None
*******************************************************************************/
void TIM1_PWMOut_Init( u16 arr, u16 psc, u16 ccp )
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
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;		
	TIM_TimeBaseInit( TIM1, &TIM_TimeBaseInitStructure);	

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;		
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;		
	TIM_OCInitStructure.TIM_Pulse = ccp;	
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;   
	TIM_OC1Init( TIM1, &TIM_OCInitStructure );  
	
	TIM_OC1PreloadConfig( TIM1, TIM_OCPreload_Disable );
	TIM_ARRPreloadConfig( TIM1, ENABLE );		
}

/*******************************************************************************
* Function Name  : TIM1_DMA_Init
* Description    : Initializes the TIM DMAy Channelx configuration.
* Input          : DMA_CHx:
*                    x can be 1 to 7.
*                  ppadr; Peripheral base address.
*                  memadr: Memory base address.
*                  bufsize: DMA channel buffer size.
* Return         : None
*******************************************************************************/ 
void TIM1_DMA_Init( DMA_Channel_TypeDef* DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize)
{
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA1, ENABLE );
	
	DMA_DeInit(DMA_CHx);	
	DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;	
	DMA_InitStructure.DMA_MemoryBaseAddr = memadr;	
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;	
  DMA_InitStructure.DMA_BufferSize = bufsize; 
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;	
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;	
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;	
	DMA_Init( DMA_CHx, &DMA_InitStructure );	
			
	DMA_Cmd( DMA_CHx, ENABLE );	
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

	TIM1_PWMOut_Init( 100, 48000-1, pbuf[0] );	
  TIM1_DMA_Init( DMA1_Channel5, (u32)TIM1_CH1CVR_ADDRESS, (u32)pbuf, 3 );
	
  TIM_DMACmd(TIM1, TIM_DMA_Update, ENABLE);
  TIM_Cmd(TIM1, ENABLE);
	TIM_CtrlPWMOutputs(TIM1, ENABLE );		
	
	while(1);
}

