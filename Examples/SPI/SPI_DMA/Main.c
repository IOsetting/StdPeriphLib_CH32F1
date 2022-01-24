/********************************** (C) COPYRIGHT  *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2019/10/15
* Description        : Main program body.
*******************************************************************************/ 

/*
 *@Note
 SPI使用DMA，Master/Slave 模式收发例程：
 Master：SPI1_SCK(PA5)、SPI1_MISO(PA6)、SPI1_MOSI(PA7)。
 Slave：SPI1_SCK(PA5)、SPI1_MISO(PA6)、SPI1_MOSI(PA7)。
 
 本例程演示 Master 和 Slave 同时使用 DAM 全双工收发。
 注：两块板子分别下载 Master 和 Slave 程序，同时上电。
     硬件连线：PA5 —— PA5
	           PA6 —— PA6
			   PA7 —— PA7
 
*/

#include "debug.h"
#include "string.h"

/* SPI Mode Definition */
#define HOST_MODE   0
#define SLAVE_MODE   1

/* SPI Communication Mode Selection */
#define SPI_MODE   HOST_MODE
//#define SPI_MODE   SLAVE_MODE

/* Global define */
#define Size 18

/* Global Variable */ 
u16 TxData[Size] = { 0x0101, 0x0202, 0x0303, 0x0404, 0x0505, 0x0606,
                     0x1111, 0x1212, 0x1313, 0x1414, 0x1515, 0x1616,
                     0x2121, 0x2222, 0x2323, 0x2424, 0x2525, 0x2626 };										
u16 RxData[Size];

/*******************************************************************************
* Function Name  : SPI_FullDuplex_Init
* Description    : Configuring the SPI for full-duplex communication.
* Input          : None
* Return         : None
*******************************************************************************/ 
void SPI_FullDuplex_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE );	
		
#if (SPI_MODE == HOST_MODE)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOA, &GPIO_InitStructure );	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		
	GPIO_Init( GPIOA, &GPIO_InitStructure );	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOA, &GPIO_InitStructure );
	
#elif (SPI_MODE == SLAVE_MODE)	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init( GPIOA, &GPIO_InitStructure );	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOA, &GPIO_InitStructure );	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		
	GPIO_Init( GPIOA, &GPIO_InitStructure );

#endif

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	
	
#if (SPI_MODE == HOST_MODE)	
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	
	
#elif (SPI_MODE == SLAVE_MODE)	
	SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;	
	
#endif	

	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;	
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;	
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;	
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB;	
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init( SPI1, &SPI_InitStructure );

#if (SPI_MODE == HOST_MODE)	
	SPI_I2S_DMACmd( SPI1, SPI_I2S_DMAReq_Tx, ENABLE );	
	SPI_I2S_DMACmd( SPI1, SPI_I2S_DMAReq_Rx, ENABLE );	
	
#endif
	
	SPI_Cmd( SPI1, ENABLE );
}

/*******************************************************************************
* Function Name  : DMA_Tx_Init
* Description    : Initializes the SPI1 DMA Channelx configuration.
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
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
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
* Function Name  : DMA_Rx_Init
* Description    : Initializes the SPI1 DMA Channelx configuration.
* Input          : DMA_CHx:
*                    x can be 1 to 7.
*                  ppadr; Peripheral base address.
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
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;	
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;	
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
	u8 i=0,j=0;
	
	Delay_Init();
	USART_Printf_Init(460800);
	printf("SystemClk:%d\r\n",SystemCoreClock);

#if (SPI_MODE == SLAVE_MODE)
  printf("Slave Mode\r\n");	
	Delay_Ms(1000);
	
#endif	
	
	SPI_FullDuplex_Init();	
	
#if (SPI_MODE == HOST_MODE)
  printf("Host Mode\r\n");		
	Delay_Ms(2000);	
	
	DMA_Tx_Init( DMA1_Channel3, (u32)&SPI1->DATAR, (u32)TxData, Size );	
	DMA_Rx_Init( DMA1_Channel2, (u32)&SPI1->DATAR, (u32)RxData, Size );	
	DMA_Cmd( DMA1_Channel3, ENABLE );	
	DMA_Cmd( DMA1_Channel2, ENABLE );	
	
#endif
	
	while(1)
	{
#if (SPI_MODE == HOST_MODE)	
		while( (!DMA_GetFlagStatus(DMA1_FLAG_TC2)) &&  (!DMA_GetFlagStatus(DMA1_FLAG_TC3)) );

		for( i=0; i<Size; i++ )
		{	
			printf( " RxData:%04x\r\n", RxData[i] );
		}		
		
#elif (SPI_MODE == SLAVE_MODE)
		while( ( i<Size ) || ( j<Size ))
		{
			if( i<Size )
			{
				if( SPI_I2S_GetFlagStatus( SPI1, SPI_I2S_FLAG_TXE ) != RESET )		
				{
					SPI_I2S_SendData( SPI1, TxData[i] );	
					i++;
				}
			}

			if( j<Size )
			{
				if( SPI_I2S_GetFlagStatus( SPI1, SPI_I2S_FLAG_RXNE ) != RESET ) 
				{
					RxData[j] = SPI_I2S_ReceiveData( SPI1 );	
					j++;
				}
			}
		}

		for( i=0; i<Size; i++ )
		{	
			printf( " RxData:%04x\r\n", RxData[i] );
		}		
		
#endif
		
		while(1);
	}
}

