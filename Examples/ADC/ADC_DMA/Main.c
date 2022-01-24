/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2019/10/15
* Description        : Main program body.
*******************************************************************************/ 

/*
 *@Note 
 ADC使用DMA采样例程：
 ADC通道2(PA2),规则组通道通过DMA获取 ADC连续1024次转换数据。
 
*/

#include "debug.h"

/* Global Variable */ 
u16 TxBuf[1024];   

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

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );	  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE );	 
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);	  
                        
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	ADC_DeInit(ADC1);  
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;		
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	
	ADC_InitStructure.ADC_NbrOfChannel = 1;	
	ADC_Init(ADC1, &ADC_InitStructure);	 

	ADC_DMACmd(ADC1, ENABLE);  
	ADC_Cmd(ADC1, ENABLE);
	
	ADC_ResetCalibration(ADC1);	
	while(ADC_GetResetCalibrationStatus(ADC1));		
	ADC_StartCalibration(ADC1);	 
	while(ADC_GetCalibrationStatus(ADC1));
}

/*******************************************************************************
* Function Name  : Get_ADC_Val
* Description    : Returns ADCx conversion result data.
* Input          : ch: ADC channel.
*                    ADC_Channel_0: ADC Channel0 selected.
*                    ADC_Channel_1: ADC Channel1 selected.
*                    ADC_Channel_2: ADC Channel2 selected.
*                    ADC_Channel_3: ADC Channel3 selected.
*                    ADC_Channel_4: ADC Channel4 selected.
*                    ADC_Channel_5: ADC Channel5 selected.
*                    ADC_Channel_6: ADC Channel6 selected.
*                    ADC_Channel_7: ADC Channel7 selected.
*                    ADC_Channel_8: ADC Channel8 selected.
*                    ADC_Channel_9: ADC Channel9 selected.
*                    ADC_Channel_10: ADC Channel10 selected.
*                    ADC_Channel_11: ADC Channel11 selected.
*                    ADC_Channel_12: ADC Channel12 selected.
*                    ADC_Channel_13: ADC Channel13 selected.
*                    ADC_Channel_14: ADC Channel14 selected.
*                    ADC_Channel_15: ADC Channel15 selected.
*                    ADC_Channel_16: ADC Channel16 selected.
*                    ADC_Channel_17: ADC Channel17 selected.
* Return         : val: The Data conversion value.
*******************************************************************************/
u16 Get_ADC_Val(u8 ch)   
{
  u16 val;	
	
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );		  			    
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);			
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));
	val = ADC_GetConversionValue(ADC1);
	
	return val;
}

/*******************************************************************************
* Function Name  : DMA_Tx_Init
* Description    : Initializes the DMAy Channelx configuration.
* Input          : DMA_CHx:
*                    x can be 1 to 7.
*                  ppadr: Peripheral base address.
*                  memadr: Memory base address.
*                  bufsize: DMA channel buffer size.
* Return         : None
*******************************************************************************/ 
void DMA_Tx_Init( DMA_Channel_TypeDef* DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize)
{
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA1, ENABLE );
	
	DMA_DeInit(DMA_CHx);	
	DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;	
	DMA_InitStructure.DMA_MemoryBaseAddr = memadr;	
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	
  DMA_InitStructure.DMA_BufferSize = bufsize; 
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;	
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;	
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;	
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;	
	DMA_Init( DMA_CHx, &DMA_InitStructure );	
}

/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Return         : None
*******************************************************************************/
int main(void)
{
  u16 i;
	
	Delay_Init();
	USART_Printf_Init(115200);
	printf("SystemClk:%d\r\n",SystemCoreClock);

	ADC_Function_Init();
	DMA_Tx_Init( DMA1_Channel1, (u32)&ADC1->RDATAR, (u32)TxBuf, 1024 );  
	DMA_Cmd( DMA1_Channel1, ENABLE );		
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_239Cycles5 );	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);			
  Delay_Ms(50);
	ADC_SoftwareStartConvCmd(ADC1, DISABLE);	
	
	for(i=0; i<1024; i++)
	{
		printf( "%04d\r\n", TxBuf[i] );
		Delay_Ms(10);
	}

	while(1);
}

