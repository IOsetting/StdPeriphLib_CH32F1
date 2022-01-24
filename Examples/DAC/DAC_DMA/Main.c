/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2019/10/15
* Description        : Main program body.
*******************************************************************************/ 

/*
 *@Note
 DAC使用DMA转换例程：
 DAC通道0(PA4)输出
 DMA配置成循环模式，通过TIM3_TRGO事件触发1次DAC转换，PA4输出相应的电压。

*/

#include "debug.h"

/* Global define */


/* Global Variable */ 
u32 dacbuff16bit[8]={0x123,0x234,0x345,0x456,0x567,0x678,0x789,0x89a};
u32 dacbuff32bit[8];  



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
	
	DAC_InitType.DAC_Trigger=DAC_Trigger_T3_TRGO;	                         
	DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_None;             
	DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Disable ;	         
  DAC_Init(DAC_Channel_1,&DAC_InitType);

	DAC_Cmd(DAC_Channel_1, ENABLE); 
	DAC_DMACmd(DAC_Channel_1,ENABLE); 
}


/*******************************************************************************
* Function Name  : DAC1_DMA_INIT
* Description    : Initializes DMA of DAC1 collection.
* Input          : None
* Return         : None
*******************************************************************************/
void DAC1_DMA_Init(void)
{
  DMA_InitTypeDef DMA_InitStructure;
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);        
	
  DMA_StructInit( &DMA_InitStructure);
  /* Note:DAC1--->DMA1.CH3   DAC2--->DMA1.CH4 */
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&(DAC->R12BDHR1);
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&dacbuff32bit;   
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = 32;    
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;   
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; 
  DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Word;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                              
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  

  DMA_Init(DMA1_Channel3, &DMA_InitStructure);
  DMA_Cmd(DMA1_Channel3, ENABLE); 
}

/*******************************************************************************
* Function Name  : DAC_Data_Init
* Description    : Initializes Data of DMA.
* Input          : None
* Return         : None
*******************************************************************************/
void DAC_Data_Init(void)
{
	uint32_t Idx = 0;  
  for (Idx = 0; Idx < 8; Idx++)
  {
    dacbuff32bit[Idx] = (dacbuff16bit[Idx] << 16) + (dacbuff16bit[Idx]);
  }
}

/*******************************************************************************
* Function Name  : TIM3_Init
* Description    : Initializes TIM3 collection.
* Input          : arr: TIM_Period
*                  psc: TIM_Prescaler
* Return         : None
*******************************************************************************/
void TIM3_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 
	
	TIM_TimeBaseStructure.TIM_Period = arr;             
	TIM_TimeBaseStructure.TIM_Prescaler =psc;           
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 
	
	TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);  
	
#if 0 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); 
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);  
#endif
	
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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  Delay_Init();
	USART_Printf_Init(115200);
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf("DAC DMA TEST\r\n");
  Gpio_Init();
	Dac_Init();
	DAC_Data_Init();
	DAC1_DMA_Init(); 
	TIM3_Init(4,7199);    
	
	while(1)
  {	
    Delay_Ms(1000);
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


