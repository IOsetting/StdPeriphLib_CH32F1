/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2019/10/15
* Description        : Main program body.
*******************************************************************************/ 

/*
 *@Note
 同步模式例程：
 Master：USART1_CK(PA8)、USART1_Tx(PA9)、USART1_Rx(PA10)。
 Slave：SPI1_SCK(PA5)、SPI1_MISO(PA6)、SPI1_MOSI(PA7)。
 本例程演示使用 USART1 作主机和 SPI1作从机，全双工收发数据。
 
 注：使用 USART2 作为打印调试口，注意修改 debug.h 中 DEBUG。
 硬件连线：PA8  —— PA5
	       PA9  —— PA7
	       PA10 —— PA6

*/

#include "debug.h"


/* Global typedef */
typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;

/* Global define */
#define TxSize1   (size(TxBuffer1))
#define TxSize2   (size(TxBuffer2))
#define size(a)   (sizeof(a) / sizeof(*(a)))

/* Global Variable */ 
u8 TxBuffer1[] = "*Buffer1 Send from USART1 to SPI1 using SynchromousMode!";     /* Send by UART1 */
u8 TxBuffer2[] = "#Buffer2 Send from SPI1 to USART1 using SynchromousMode!";     /* Send by SPI1  */
u8 RxBuffer1[TxSize1]={0};                                                       /* USART1 Using  */
u8 RxBuffer2[TxSize2]={0};                                                       /* SPI1   Using  */

u8 TxCnt1 = 0, RxCnt1 = 0;  
u8 TxCnt2 = 0, RxCnt2 = 0;
u8 Tempdata=0x00;

TestStatus TransferStatus1 = FAILED;
TestStatus TransferStatus2 = FAILED;


/*******************************************************************************
* Function Name  : Buffercmp
* Description    : Compares two buffers  
* Input          : Buf1,Buf2:buffers to be compared
*                  BufferLength: buffer's length
* Return         : PASSED: Buf1 identical to Buf2
*                  FAILED: Buf1 differs from Buf2
*******************************************************************************/
TestStatus Buffercmp(uint8_t* Buf1, uint8_t* Buf2, uint16_t BufLength)
{
  while(BufLength--)
  {
    if(*Buf1 != *Buf2)
    {
      return FAILED;
    }
    Buf1++;
    Buf2++;
  }
  return PASSED;  
}

/*******************************************************************************
* Function Name  : USART1_ReCFG
* Description    : ReInitializes the USART1 peripheral.  
* Input          : None                                 
* Return         : None
*******************************************************************************/
void USART1_ReCFG(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStructure; 
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1 , ENABLE);	
  /* USART1  Ck-->A.8   TX-->A.9   RX-->A.10 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 ; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	USART_ClockInitStructure.USART_Clock = USART_Clock_Enable;      
  USART_ClockInitStructure.USART_CPOL = USART_CPOL_High;           /* Clock is active High */ 
  USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;          /* Data is captured on the second edge */ 
  USART_ClockInitStructure.USART_LastBit = USART_LastBit_Enable;   /* The clock pulse of the last data bit is output to the SCLK pin */
  USART_ClockInit(USART1, &USART_ClockInitStructure);
	
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	
  USART_Init(USART1, &USART_InitStructure); 	
	USART_Cmd(USART1, ENABLE); 
}

/*******************************************************************************
* Function Name  : SPI1_INIT
* Description    : Initializes the SPI1 to be Slave Mode.  
* Input          : None                                 
* Return         : None
*******************************************************************************/
void SPI1_INIT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);
  SPI_StructInit(&SPI_InitStructure);
	SPI_I2S_DeInit(SPI1);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;               /* SPI1 MISO-->PA.6 */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;  /* SPI1 SCK-->PA.5 MOSI-->PA.7 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB;

  SPI_Init(SPI1, &SPI_InitStructure);
  SPI_Cmd(SPI1, ENABLE);
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
	USART_Printf_Init(115200);                                         /* printf using USART2 */
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf("USART SynchromousMode TEST\r\n");
  USART1_ReCFG();                                                   /* USART1 ReInitializes */  
  SPI1_INIT();
	
	while(TxCnt1<TxSize1)                                             /* USART1--->SPI1 */
	{
		USART_SendData(USART1, TxBuffer1[TxCnt1++]);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)     /* waiting for sending finish */
		{	  		
		}
		while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
    {
    }
		RxBuffer2[RxCnt2++] = SPI_I2S_ReceiveData(SPI1);    
	}
	USART_ReceiveData(USART1);                                        /* Clear the USART1 Data Register */
	while(TxCnt2<TxSize2)                                             /* SPI1--->USART1 */     
	{
		while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)== RESET)    /* waiting for sending finish */
    {
    }
		SPI_I2S_SendData(SPI1, TxBuffer2[TxCnt2++]);
		USART_SendData(USART1, Tempdata);                               /* Send Tempdata for SCK */
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)     
		{	 			
		} 	 
    while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
    {
    }
		RxBuffer1[RxCnt1++] = USART_ReceiveData(USART1);
	}

	TransferStatus1=Buffercmp(TxBuffer1,RxBuffer2,TxSize1);
	TransferStatus2=Buffercmp(TxBuffer2,RxBuffer1,TxSize2);
	if(TransferStatus1&&TransferStatus2)
	{
	  printf("\r\nSend Success!\r\n");
	}
	else
	{
	  printf("\r\nSend Fail!\r\n");
	}
	printf("TxBuffer1---->RxBuffer2     TxBuffer2---->RxBuffer1\r\n");
	printf("TxBuffer1:%s\r\n",TxBuffer1);
	printf("RxBuffer1:%s\r\n",RxBuffer1);
  printf("TxBuffer2:%s\r\n",TxBuffer2);
	printf("RxBuffer2:%s\r\n",RxBuffer2);
	
	while(1)
  {	
	}
}

