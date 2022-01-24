/********************************** (C) COPYRIGHT  *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2019/10/15
* Description        : Main program body.
*******************************************************************************/ 

/*
 *@Note
 独立看门狗例程：
 本例程演示 PA0 引脚下拉输入，当输入高电平时，进行喂狗。若不喂狗，
 则 3.2s 后触发独立看门狗复位。

*/

#include "debug.h"

#define KEY0 GPIO_ReadInputDataBit( GPIOA, GPIO_Pin_0)	//PA0

/*******************************************************************************
* Function Name  : KEY_Init
* Description    : Initializes KEY GPIO.
* Input          : None
* Return         : None
*******************************************************************************/ 
void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE );
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init( GPIOA, &GPIO_InitStructure);	
}

/*******************************************************************************
* Function Name  : KEY_PRESS
* Description    : Key processing funcation.
* Input          : None
* Return         : 0：Press the key.
*                  1：Release Key.
*******************************************************************************/ 
u8 KEY_PRESS(void)
{
	if(KEY0 == 1)
	{
		Delay_Ms(10);
		return 1;
	}
	
	return 0;
}

/*******************************************************************************
* Function Name  : IWDG_Init
* Description    : Initializes IWDG. 
* Input          : IWDG_Prescaler: specifies the IWDG Prescaler value.
*                    IWDG_Prescaler_4: IWDG prescaler set to 4.
*                    IWDG_Prescaler_8: IWDG prescaler set to 8.
*                    IWDG_Prescaler_16: IWDG prescaler set to 16.
*                    IWDG_Prescaler_32: IWDG prescaler set to 32.
*                    IWDG_Prescaler_64: IWDG prescaler set to 64.
*                    IWDG_Prescaler_128: IWDG prescaler set to 128.
*                    IWDG_Prescaler_256: IWDG prescaler set to 256.
*										Reload: specifies the IWDG Reload value.
*                    This parameter must be a number between 0 and 0x0FFF.                 
* Return         : None
*******************************************************************************/ 
void IWDG_Feed_Init( u16 prer, u16 rlr )	
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);	
	IWDG_SetPrescaler(prer);	
	IWDG_SetReload(rlr);		
	IWDG_ReloadCounter();	
	IWDG_Enable();	
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
	
  Delay_Init();
	KEY_Init();
	printf("IWDG test...\r\n");
	Delay_Ms(1000); 
	
	IWDG_Feed_Init( IWDG_Prescaler_32, 4000 );   // 3.2s IWDG reset
	
	while(1)
	{
		if( KEY_PRESS() == 1 )	//PA0 
		{
			printf("Feed dog\r\n");
	    IWDG_ReloadCounter();	//Feed dog
			Delay_Ms(10);
		}	
	}		
}



