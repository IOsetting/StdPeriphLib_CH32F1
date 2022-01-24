/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2019/10/15
* Description        : Main program body.
*******************************************************************************/ 

/*
 *@Note
 I2C使用DMA，Master/Slave 模式收发例程：
 I2C1_SCL(PB6)、I2C1_SDA(PB7)。
 本例程演示7位地址模式， Master 通过 DMA 发，Slave 通过 DMA 收。
 注：两块板子分别下载 Master 和 Slave 程序，同时上电。
     硬件连线：PB6 —— PB6
	           PB7 —— PB7 
 
*/

#include "debug.h"

/* I2C Mode Definition */
#define HOST_MODE   0
#define SLAVE_MODE   1

/* I2C Communication Mode Selection */
//#define I2C_MODE   HOST_MODE
#define I2C_MODE   SLAVE_MODE

/* Global define */
#define Size   7
#define Tize   6
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
	I2C_InitTSturcture.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;	
  I2C_Init( I2C1, &I2C_InitTSturcture );

	I2C_DMACmd( I2C1, ENABLE );	

	I2C_Cmd( I2C1, ENABLE );
	
#if (I2C_MODE == HOST_MODE)	
	I2C_AcknowledgeConfig( I2C1, ENABLE );
	
#endif
}

/*******************************************************************************
* Function Name  : DMA_Tx_Init
* Description    : Initializes the SPI1 DMAy Channelx configuration.
* Input          : DMA_CHx:
*                  ppadr: Peripheral base address.
*                  memadr: Memory base address.
*                  bufsize: DMA channel buffer size.
* Return         : None
*******************************************************************************/  
void DMA_Tx_Init( DMA_Channel_TypeDef* DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize)
{
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA1, ENABLE );
	
	DMA_DeInit(DMA_CHx);	
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;	
	DMA_InitStructure.DMA_MemoryBaseAddr = memadr;	
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;	
	DMA_InitStructure.DMA_BufferSize = bufsize; 
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;	
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;	
	DMA_Init( DMA_CHx, &DMA_InitStructure );	
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	DMA_ITConfig( DMA1_Channel6, DMA_IT_TC, ENABLE );	
}

/*******************************************************************************
* Function Name  : DMA_Tx_Init
* Description    : Initializes the SPI1 DMAy Channelx configuration.
* Input          : DMA_CHx:
*                  ppadr: Peripheral base address.
*                  memadr: Memory base address.
*                  bufsize: DMA channel buffer size.
* Return         : None
*******************************************************************************/  
void DMA_Rx_Init( DMA_Channel_TypeDef* DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize )
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
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;	
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;	
	DMA_Init( DMA_CHx, &DMA_InitStructure );	
}

/*******************************************************************************
* Function Name  : DMA1_Channel6_IRQHandler
* Description    : This function handles DMA1 channel6 exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel6_IRQHandler()
{
	if( DMA_GetITStatus(DMA1_IT_TC6) != RESET )
	{
		DMA_Cmd( DMA1_Channel6, DISABLE );
		DMA_ClearITPendingBit(DMA1_IT_TC6);
	}
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
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	Delay_Init();
	USART_Printf_Init(460800);
	printf("SystemClk:%d\r\n",SystemCoreClock);

#if (I2C_MODE == HOST_MODE)
  printf("IIC Host mode\r\n");	
  DMA_Tx_Init( DMA1_Channel6, (u32)&I2C1->DATAR, (u32)TxData, Tize );		
	IIC_Init( 80000, TxAdderss);	
	
	while( I2C_GetFlagStatus( I2C1, I2C_FLAG_BUSY ) != RESET );		
	I2C_GenerateSTART( I2C1, ENABLE );	

	while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_MODE_SELECT ) );	
	I2C_Send7bitAddress( I2C1, 0x02, I2C_Direction_Transmitter );	
	
	while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) );	
	
	DMA_Cmd( DMA1_Channel6, ENABLE );	
	
	while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );	
	I2C_GenerateSTOP( I2C1, ENABLE );	

#elif (I2C_MODE == SLAVE_MODE)	
	printf("IIC Slave mode\r\n");	
	DMA_Rx_Init( DMA1_Channel7, (u32)&I2C1->DATAR, (u32)RxData, Tize );	
	IIC_Init( 80000, RXAdderss);	
	
	while( !I2C_CheckEvent( I2C1, I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED ) );	
	DMA_Cmd( DMA1_Channel7, ENABLE );	
	
	while( (!DMA_GetFlagStatus(DMA1_FLAG_TC7)) );
	
	printf( "RxData:\r\n" );
	for( i=0; i<6; i++ )
	{
		printf( "%02x\r\n", RxData[i] );
	}

#endif
	
	while(1);		
}

