/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2019/10/15
* Description        : Main program body.
*******************************************************************************/ 

/*
 *@Note
 CAN时间触发通信模式：
 CAN_Tx(PB9),CAN_Rx(PB8)
 在Extended_Frame，1个32bit过滤器屏蔽位通信配置下，演示时间触发通信模式。

*/

#include "debug.h"

/* CAN Mode Definition */
#define TX_MODE   0
#define RX_MODE   1

/* CAN Communication Mode Selection */
#define CAN_MODE   TX_MODE
//#define CAN_MODE   RX_MODE

/* Global Variable */
u8 txbuf[8];
u8 tx;

/*******************************************************************************
* Function Name  : CAN_Mode_Init
* Description    : Initializes CAN communication test mode.
* Input          : tsjw；CAN synchronisation jump width.
*									 tbs2：CAN time quantum in bit segment 1.
*						  		 tbs1：CAN time quantum in bit segment 2.
*									 brp：Specifies the length of a time quantum.
*				  				 mode：Test mode.
*                    CAN_Mode_Normal.
*										 CAN_Mode_LoopBack.					
*										 CAN_Mode_Silent.		
*										 CAN_Mode_Silent_LoopBack.	
* Return         : None
*     			       Bps	=Fpclk1/((tpb1+1+tbs2+1+1)*brp)
*******************************************************************************/
void CAN_Mode_Init( u8 tsjw, u8 tbs2, u8 tbs1, u16 brp, u8 mode )
{
	GPIO_InitTypeDef GPIO_InitSturcture;
	CAN_InitTypeDef CAN_InitSturcture;
	CAN_FilterInitTypeDef CAN_FilterInitSturcture;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE ); 
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_CAN1, ENABLE );	
	
	GPIO_PinRemapConfig( GPIO_Remap1_CAN1, ENABLE);	
	
	GPIO_InitSturcture.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitSturcture.GPIO_Mode = GPIO_Mode_AF_PP;	
	GPIO_InitSturcture.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOB, &GPIO_InitSturcture);
	
	GPIO_InitSturcture.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitSturcture.GPIO_Mode = GPIO_Mode_IPU;	
	GPIO_Init( GPIOB, &GPIO_InitSturcture);

	CAN_InitSturcture.CAN_TTCM = ENABLE;	
	CAN_InitSturcture.CAN_ABOM = DISABLE;		
	CAN_InitSturcture.CAN_AWUM = DISABLE;		
	CAN_InitSturcture.CAN_NART = ENABLE;	
	CAN_InitSturcture.CAN_RFLM = DISABLE;		
	CAN_InitSturcture.CAN_TXFP = DISABLE;	
	CAN_InitSturcture.CAN_Mode = mode;
	CAN_InitSturcture.CAN_SJW = tsjw;		
	CAN_InitSturcture.CAN_BS1 = tbs1;		
	CAN_InitSturcture.CAN_BS2 = tbs2;		
	CAN_InitSturcture.CAN_Prescaler = brp;		
	CAN_Init( CAN1, &CAN_InitSturcture );
	
  /* identifier/mask mode, One 32-bit filter, ExtId: 0x12124567 */
	CAN_FilterInitSturcture.CAN_FilterNumber = 0;	
	CAN_FilterInitSturcture.CAN_FilterMode = CAN_FilterMode_IdMask;		
	CAN_FilterInitSturcture.CAN_FilterScale = CAN_FilterScale_32bit;	
	CAN_FilterInitSturcture.CAN_FilterIdHigh = 0x9092;	
	CAN_FilterInitSturcture.CAN_FilterIdLow = 0x2B3C;	
	CAN_FilterInitSturcture.CAN_FilterMaskIdHigh = 0xFFFF; 	
	CAN_FilterInitSturcture.CAN_FilterMaskIdLow = 0xFFFE;	
	CAN_FilterInitSturcture.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;		
	CAN_FilterInitSturcture.CAN_FilterActivation = ENABLE;	
	CAN_FilterInit( &CAN_FilterInitSturcture );
	CAN_TTComModeCmd( CAN1, ENABLE );	
	
#if (CAN_MODE == TX_MODE)		
	CAN_ClearITPendingBit( CAN1, CAN_IT_TME );	
	
	NVIC_InitStructure.NVIC_IRQChannel = USB_HP_CAN1_TX_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	CAN_ITConfig( CAN1, CAN_IT_TME, ENABLE );		
	
#elif (CAN_MODE == RX_MODE)		
  CAN_ClearITPendingBit( CAN1, CAN_IT_FMP0 );	
	
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	CAN_ITConfig( CAN1, CAN_IT_FMP0, ENABLE );	
	
#endif		
}

#if (CAN_MODE == TX_MODE)	
/*******************************************************************************
* Function Name  : CAN_Send_Msg
* Description    : CAN Transmit function.
* Input          : msg：Transmit data buffer.
*									 len：Data length.
* Return         : 0：Send successful.
*									 1：Send failed.
*******************************************************************************/
u8 CAN_Send_Msg( u8 *msg, u8 len )
{
	u8 mbox;
	u16 i = 0;
	
	CanTxMsg CanTxStructure;
	
	CanTxStructure.ExtId = 0x12124567;		
	CanTxStructure.IDE = CAN_Id_Extended;		
	CanTxStructure.RTR = CAN_RTR_Data;	
	CanTxStructure.DLC = len;
	
	for( i=0; i<len; i++ )
	{
		CanTxStructure.Data[i] = msg[i];	
	}
	
	mbox = CAN_Transmit( CAN1, &CanTxStructure);
	i = 0;
	
	while( ( CAN_TransmitStatus( CAN1, mbox ) != CAN_TxStatus_Ok ) && (i < 0xFFF) ) 
	{
		i++;
	}
	
	if( i == 0xFFF )
	{
		return 1;
	}
	else 
	{
		return 0;
	}
}

/*******************************************************************************
* Function Name  : USB_HP_CAN1_TX_IRQHandler
* Description    : This function handles CAN transmit mailboxe Handler.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_HP_CAN1_TX_IRQHandler(void)
{	
  u8 i;
	
	if( CAN_GetITStatus( CAN1, CAN_IT_TME ) != RESET )
	{
		printf( "Send Success\r\n" );
		printf( "Send Data:\r\n" );
		
		for(i=0; i<8; i++)
		{
			printf( "%02x\r\n", txbuf[i] );				
		}
		
		printf( "TDTR:%08x\r\n", CAN1->sTxMailBox[0].TXMDTR );		
	}
	
	CAN_ClearITPendingBit( CAN1, CAN_IT_TME );	
}

#elif (CAN_MODE == RX_MODE)
/*******************************************************************************
* Function Name  : CAN_Receive_Msg
* Description    : CAN Receive function.
* Input          : buf：Receive data buffer.
* Output         : None
* Return         : CanRxStructure.DLC：Receive data length.
*******************************************************************************/
u8 CAN_Receive_Msg( u8 *buf )
{
	u8 i;
	
	CanRxMsg CanRxStructure;
	
	if( CAN_MessagePending( CAN1, CAN_FIFO0 ) == 0)	
	{
		return 0;
	}
	
	CAN_Receive( CAN1, CAN_FIFO0, &CanRxStructure );	
	
	for( i=0; i<8; i++ )
	{
		buf[i] = CanRxStructure.Data[i];
	}
	
	return CanRxStructure.DLC;
	
}

/*******************************************************************************
* Function Name  : USB_LP_CAN1_RX0_IRQHandler
* Description    : This function handles CAN receive FIFO0 Handler.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	u8 rx,i;
	u8 rxbuf[8];
	
	if( CAN_GetITStatus( CAN1, CAN_IT_FMP0 ) != RESET )
	{
		rx = CAN_Receive_Msg( rxbuf );	
		
		if( rx )	
		{
			printf( "Receive Data:\r\n" );
			
			for(i=0; i<8; i++)
			{
				 printf( "%02x\r\n", rxbuf[i] );				
			}
		}
		
		printf( "RDTR:%08x\r\n", CAN1->sFIFOMailBox[0].RXMDTR );		
	}
	
	CAN_ClearITPendingBit( CAN1, CAN_IT_FMP0 );	
}

#endif	

/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Return         : None
*******************************************************************************/
int main(void)
{
	u8 i=0;
	u8 cnt=1;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	Delay_Init();
	USART_Printf_Init(115200);
	printf("SystemClk:%d\r\n",SystemCoreClock);

/* Bps = 500Kbps */	
	CAN_Mode_Init( CAN_SJW_1tq, CAN_BS2_5tq, CAN_BS1_6tq, 12, CAN_Mode_Normal );
	
#if (CAN_MODE == TX_MODE)		
  printf("TX Mode\r\n");
	
#elif (CAN_MODE == RX_MODE)	
  printf("RX Mode\r\n");
	
#endif	
	
	while(1)
	{
#if (CAN_MODE == TX_MODE)	
		for( i=0; i<8; i++ )
		{
			txbuf[i] = cnt+i;	
		}
		tx = CAN_Send_Msg( txbuf, 8);	
	
		Delay_Ms(1000);
		cnt++;
		
#endif		
	}
}

