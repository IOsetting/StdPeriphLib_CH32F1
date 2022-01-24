/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2019/10/15
* Description        : Main program body.
*******************************************************************************/ 

/*
 *@Note
 轮询收发模式例程：
 Master：USART2_Tx(PA2)、USART2_Rx(PA3)。
 Slave：USART3_Tx(PB10)、USART3_Rx(PB11)。
 本例程演示 USART2 发，USART3 收。
 
 注：
     硬件连线：PA2 —— PB11
	           PA3 —— PB10

*/

#include "debug.h"


/* Global typedef */
typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;

/* Global define */
#define TxSize   (size(TxBuffer))
#define size(a)   (sizeof(a) / sizeof(*(a)))

/* Global Variable */ 
u8 TxBuffer[] = "Buffer Send from USART2 to USART3 by polling!";
u8 RxBuffer[TxSize]={0};
u8 TxCnt = 0, RxCnt = 0;  
TestStatus TransferStatus = FAILED;

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
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2|RCC_APB1Periph_USART3, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOB , ENABLE);	
	
  /* USART2 TX-->A.2   RX-->A.3 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
  /* USART3 TX-->B.10  RX-->B.11 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);		
	
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	
  USART_Init(USART2, &USART_InitStructure); 
  USART_Cmd(USART2, ENABLE);  
	
	USART_Init(USART3, &USART_InitStructure); 
  USART_Cmd(USART3, ENABLE);  
	
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
	printf("USART Polling TEST\r\n");
  USARTx_CFG();                                                 /* USART2 & USART3 INIT */  

	while(TxCnt<TxSize)
	{
	  USART_SendData(USART2, TxBuffer[TxCnt++]);
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
		{
			                                                          /* waiting for sending finish */
		}
		while(USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET)
		{
		                                                            /* waiting for receiving finish */
		}
		RxBuffer[RxCnt++] = (USART_ReceiveData(USART3));
	}
	
	TransferStatus=Buffercmp(TxBuffer,RxBuffer,TxSize);
	
	if(TransferStatus)
	{
	  printf("send success!\r\n");
		printf("TXBuffer: %s \r\n",TxBuffer);
		printf("RxBuffer: %s \r\n",RxBuffer);
	}
	else
	{
	  printf("send fail!\r\n");
		printf("TXBuffer: %s \r\n",TxBuffer);
		printf("RxBuffer: %s \r\n",RxBuffer);
	}
	while(1)
  {	
		
	}
}



