/********************************** (C) COPYRIGHT  *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2019/10/15
* Description        : RCC configuration reference system_ch32f10x.h and 
                       system_ch32f10x.c file.
*******************************************************************************/ 

/*
 *@Note
 HSI或HSI/2作为PLL输入时钟例程：
 本例程演示，HSI或HSI/2作为PLL输入时钟，系统时钟以PLL作为时钟源。
 参考system_ch32f10x.c、system_ch32f10x.h。
 HSI作为PLL输入时钟，演示系统时钟为 24MHz、48MHz、72MHz。
 HSI/2作为PLL输入时钟，演示系统时钟为 24MHz、48MHz
 
*/

#include "debug.h"


#define HSI      0   //HSI
#define HSI_1_2  1   //HSI/2

#define PLL_Source   HSI
//#define PLL_Source   HSI_1_2

/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Return         : None
*******************************************************************************/
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	USART_Printf_Init(115200);
	printf("SystemClk:%d\r\n",SystemCoreClock);

	while(1);
}

