/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2019/10/15
* Description        : Main program body.
*******************************************************************************/ 

/*
 *@Note
 噪声波形输出例程：
 DAC通道0(PA4)输出
 通过软件触发1次DAC转换，PA4输出噪声波形。

*/

#include "debug.h"

/* Global define */
#define Num 7

/* Global Variable */ 
u16 DAC_Value[Num]={64,128,256,512,1024,2048,4095};   


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
	
	DAC_InitType.DAC_Trigger=DAC_Trigger_Software;	                         
	DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_Noise;             
	DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude=DAC_LFSRUnmask_Bits11_0;   
	DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Disable ;	         
  DAC_Init(DAC_Channel_1,&DAC_InitType);
	/* TEN = 1 */
	DAC->CTLR |= 0x04;
	DAC_Cmd(DAC_Channel_1, ENABLE); 
	
	DAC_SetChannel1Data(DAC_Align_12b_R, 0);
}


/*******************************************************************************
* Function Name  : DAC1_Nosie_Gen_Test
* Description    : Noise wava generation test.
* Input          : None
* Return         : None
*******************************************************************************/
void DAC1_Nosie_Gen_Test(void)
{
  DAC->SWTR |= 0x01;                   /* Set by software, Reset by hardware */
  __NOP();
  __NOP();
  __NOP();
  printf("DOR1=0x%04x\r\n",DAC->DOR1); /* Value will be 0x0AAA,0x0D55,0x0EAA ... */
}

/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Return         : None
*******************************************************************************/
int main(void)
{
  Delay_Init();
	USART_Printf_Init(115200);
	Dac_Init();
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf("Noise Wava Generation Test\r\n");
	while(1)
  {	
		DAC1_Nosie_Gen_Test();
	  //Delay_Ms(1000); 
	}
}

