/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2019/10/15
* Description        : Main program body.
*******************************************************************************/ 

/*
 *@Note
 10位地址模式，Master/Slave 模式收发例程：
 I2C1_SCL(PB6)、I2C1_SDA(PB7)。
 本例程演示 Master 发，Slave 收。
 注：两块板子分别下载 Master 和 Slave 程序，同时上电。
     硬件连线：PB6 —— PB6
	           PB7 —— PB7 
*/

#include "debug.h"

/* I2C Mode Definition */
#define HOST_MODE   0
#define SLAVE_MODE   1

/* I2C Communication Mode Selection */
#define I2C_MODE   HOST_MODE
//#define I2C_MODE   SLAVE_MODE

/* Global define */
#define Size   7
#define RXAdderss   0x02	
#define TxAdderss   0x02	

/* Global Variable */ 
u8 TxData[Size] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
u8 RxData[Size];

/*******************************************************************************
* Function Name  : IIC_Init
* Description    : Initializes the IIC peripheral.
* Input          : None
* Return         : None
*******************************************************************************/ 
void IIC_Init( u32 bound, u16 address )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitTSturcture;
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE );	
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_I2C1, ENABLE );	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOB, &GPIO_InitStructure );

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOB, &GPIO_InitStructure );

	I2C_InitTSturcture.I2C_ClockSpeed = bound;
	I2C_InitTSturcture.I2C_Mode = I2C_Mode_I2C;		
	I2C_InitTSturcture.I2C_DutyCycle = I2C_DutyCycle_16_9;
	I2C_InitTSturcture.I2C_OwnAddress1 = address;
	I2C_InitTSturcture.I2C_Ack = I2C_Ack_Enable;	
	I2C_InitTSturcture.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_10bit;	
  I2C_Init( I2C1, &I2C_InitTSturcture );
	
	I2C_Cmd( I2C1, ENABLE );
	
#if (I2C_MODE == HOST_MODE)	
	I2C_AcknowledgeConfig( I2C1, ENABLE );
	
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
	u8 i=0;
	
	Delay_Init();
	USART_Printf_Init(460800);
	printf("SystemClk:%d\r\n",SystemCoreClock);

#if (I2C_MODE == HOST_MODE)
  printf("IIC Host mode\r\n");	
	IIC_Init( 80000, TxAdderss);
	
	while( I2C_GetFlagStatus( I2C1, I2C_FLAG_BUSY ) != RESET );	
	
	I2C_GenerateSTART( I2C1, ENABLE );	
	
	while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_MODE_SELECT ) );	
	I2C_Send7bitAddress( I2C1, 0xF0, I2C_Direction_Transmitter );	
	
	while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_MODE_ADDRESS10 ) );	
	I2C_Send7bitAddress( I2C1, 0x02, I2C_Direction_Transmitter );	
	
	while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) );	

	while( i<6 )
	{
		if( I2C_GetFlagStatus( I2C1, I2C_FLAG_TXE ) !=  RESET )	
		{
			I2C_SendData( I2C1, TxData[i] );	
			i++;
		}	
	}
	
	while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );	
	I2C_GenerateSTOP( I2C1, ENABLE );	

#elif (I2C_MODE == SLAVE_MODE)	
	printf("IIC Slave mode\r\n");	
	IIC_Init( 80000, RXAdderss);	
	
	while( !I2C_CheckEvent( I2C1, I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED ) );	

	while( i<6 )
	{
		if( I2C_GetFlagStatus( I2C1, I2C_FLAG_RXNE ) !=  RESET )	
		{
			RxData[i] = I2C_ReceiveData( I2C1 );
			i++;
		}	
	}
	
	printf( "RxData:\r\n" );
	for( i=0; i<6; i++ )
	{
		printf( "%02x\r\n", RxData[i] );
	}

#endif
	
	while(1);		
}

