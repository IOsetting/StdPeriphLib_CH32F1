/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2019/10/15
* Description        : Main program body.
*******************************************************************************/ 

/*
 *@Note
 单线半双工模式，Master/Slave 模式收发例程：
 Master：USART2_Tx(PA2)
 Slave：USART3_Tx(PB10)
 
 本例程演示 UART2 和 USART3 单线半双工模式数据收发。
 注：
     硬件连线：PA2 —— PB10

*/

#include "debug.h"


/* Global typedef */
typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;

/* Global define */
#define TxSize1   (size(TxBuffer1))
#define TxSize2   (size(TxBuffer2))
#define size(a)   (sizeof(a) / sizeof(*(a)))

/* Global Variable */ 
u8 TxBuffer1[] = "*Buffer1 Send from USART2 to USART3 using HalfDuplex Mode!";     /* Send by UART2 */
u8 TxBuffer2[] = "#Buffer2 Send from USART3 to USART2 using HalfDuplex Mode!";     /* Send by UART3 */
u8 RxBuffer1[TxSize1]={0};                                                         /* USART2 Using */
u8 RxBuffer2[TxSize2]={0};                                                         /* USART3 Using  */

u8 TxCnt1 = 0, RxCnt1 = 0;  
u8 TxCnt2 = 0, RxCnt2 = 0;

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
* Function Name  : USARTx_CFG
* Description    : Initializes the USART2 & USART3 peripheral.  
* Input          : None                                 
* Return         : None
*******************************************************************************/
void USARTx_CFG(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2|RCC_APB1Periph_USART3, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOB , ENABLE);	
	
  /* USART2 TX-->A.2   RX-->A.3 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;                /* Only Configure TX Pin */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* USART3 TX-->B.10  RX-->B.11 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;               /* Only Configure TX Pin */
  GPIO_Init(GPIOB, &GPIO_InitStructure);
			
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	
  USART_Init(USART2, &USART_InitStructure);   
	USART_Init(USART3, &USART_InitStructure); 
	USART_Cmd(USART2, ENABLE); 
	USART_Cmd(USART3, ENABLE);  
	
  USART_HalfDuplexCmd(USART2, ENABLE);
  USART_HalfDuplexCmd(USART3, ENABLE);
	
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
	USART_Printf_Init(115200);                                     /* printf using USART1 */
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf("USART HalfDuplex TEST\r\n");
  USARTx_CFG();                                                 /* USART2 & USART3 INIT */  
  
	while(TxCnt2<TxSize2)                                         /* USART3--->USART2 */
	{
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET) /* waiting for sending finish */
		{	  
		}
		USART_SendData(USART3, TxBuffer2[TxCnt2++]);
		
		while(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET)
    {
    }
    RxBuffer1[RxCnt1++] = USART_ReceiveData(USART2);
	}

	while(TxCnt1<TxSize1)                                         /* USART2--->USART3 */     
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET) /* waiting for sending finish */
		{	  
		}  
		USART_SendData(USART2, TxBuffer1[TxCnt1++]); 
		
		while(USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET)
    {
    }
    RxBuffer2[RxCnt2++] = USART_ReceiveData(USART3);
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


