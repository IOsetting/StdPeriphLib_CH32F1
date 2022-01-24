/********************************** (C) COPYRIGHT  *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2019/10/15
* Description        : Main program body.
*******************************************************************************/ 

/*
 *@Note
 双线全双工模式，Master/Slave 模式数据收发：
 Master：SPI1_SCK(PA5)、SPI1_MISO(PA6)、SPI1_MOSI(PA7)。
 Slave：SPI1_SCK(PA5)、SPI1_MISO(PA6)、SPI1_MOSI(PA7)。
 
 本例程演示 Master 和 Slave 同时全双工收发。
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
	
	SPI_Cmd( SPI1, ENABLE );
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
	u8 j=0;
	u8 value;
	
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
	
#endif
	
	while(1)
	{
		while( ( i<18 ) || ( j<18 ))
		{
			if( i<18 )
			{
				if( SPI_I2S_GetFlagStatus( SPI1, SPI_I2S_FLAG_TXE ) != RESET )		
				{
					SPI_I2S_SendData( SPI1, TxData[i] );	
					i++;
				}
			}
			
			if( j<18 )
			{
				if( SPI_I2S_GetFlagStatus( SPI1, SPI_I2S_FLAG_RXNE ) != RESET ) 	
				{
					RxData[j] = SPI_I2S_ReceiveData( SPI1 );	
					j++;
				}
			}
		}

		value = memcmp( TxData, RxData, Size );
		
		if( value == 0 )
		{
			printf( "Same\r\n" );	
		}
		else
		{
			printf( "Different\r\n" );	
		}
		
		while(1);
	}
}

