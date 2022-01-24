/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2019/10/15
* Description        : Main program body.
*******************************************************************************/ 

/*
 *@Note
  U���ļ�ö�����̣�
  USBHDM(PB6)��USBHDP(PB7)��
  ��������ʾ U���ļ�ϵͳ��ö�ٸ�Ŀ¼����ָ��Ŀ�µ��ļ���
  ע���ļ�ϵͳ֧�� FAT12/FAT16/FAT32��

*/

#include "debug.h"
#include "string.h"
#include "CH32F103UFI.H" 
/* Global define */


/* Global Variable */    
__align(4) UINT8  RxBuffer[ MAX_PACKET_SIZE ] ;      // IN, must even address
__align(4) UINT8  TxBuffer[ MAX_PACKET_SIZE ] ;      // OUT, must even address
__align(4) UINT8  Com_Buffer[ 128 ];                

UINT8  buf[100];                                  //���ȿ��Ը���Ӧ���Լ�ָ��

/* ������״̬,�����������ʾ������벢ͣ�� */
void mStopIfError( UINT8 iError )
{
    if ( iError == ERR_SUCCESS )
    {
        return;    /* �����ɹ� */
    }
    printf( "Error: %02X\n", (UINT16)iError );  /* ��ʾ���� */
    /* ���������,Ӧ�÷����������Լ�CH103DiskStatus״̬,�������CH103DiskReady��ѯ��ǰU���Ƿ�����,���U���ѶϿ���ô�����µȴ�U�̲����ٲ���,
       ��������Ĵ�����:
       1������һ��CH103DiskReady,�ɹ����������,����Open,Read/Write��
       2�����CH103DiskReady���ɹ�,��ôǿ�н���ͷ��ʼ����(�ȴ�U�����ӣ�CH103DiskReady��) */
    while ( 1 )
    {  }
}
/*******************************************************************************
* Function Name  : Set_USBConfig
* Description    : Set USB clock.
* Input          : None
* Return         : None
*******************************************************************************/
void USBHD_ClockCmd(UINT32 RCC_USBCLKSource,FunctionalState NewState)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, NewState);
	EXTEN->EXTEN_CTR |= EXTEN_USBHD_IO_EN;
	RCC_USBCLKConfig(RCC_USBCLKSource);             //USBclk=PLLclk/1.5=48Mhz			 
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_USBHD,NewState);  
}

/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Return         : None
*******************************************************************************/
int main()
{	
	UINT8   s, i;
    PUINT8  pCodeStr;
    UINT16  j;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Delay_Init();
	USART_Printf_Init(115200);
	printf("SystemClk:%d\r\n",SystemCoreClock);
	
	printf("USBHD   HOST Test\r\n");
	USBHD_ClockCmd(RCC_USBCLKSource_PLLCLK_1Div5,ENABLE); 
    pHOST_RX_RAM_Addr = RxBuffer;
    pHOST_TX_RAM_Addr = TxBuffer;
	USB_HostInit();
	CH103LibInit();
	printf( "Wait Device In\n" );
	
	while(1)
	{
		s = ERR_SUCCESS;
		
		if ( R8_USB_INT_FG & RB_UIF_DETECT )
		{  
			R8_USB_INT_FG = RB_UIF_DETECT ; 

			s = AnalyzeRootHub( );   
			if ( s == ERR_USB_CONNECT ) 
			{
				printf( "New Device In\r\n" );		
				FoundNewDev = 1;
			}
			if( s == ERR_USB_DISCON )
			{
				printf( "Device Out\r\n" );					
			}
		}
		
		if ( FoundNewDev || s == ERR_USB_CONNECT ) 
		{ 
			FoundNewDev = 0;
			Delay_Ms( 200 ); 
			s = InitRootDevice( Com_Buffer );  
			 if ( s == ERR_SUCCESS )
            {
                // U�̲������̣�USB���߸�λ��U�����ӡ���ȡ�豸������������USB��ַ����ѡ�Ļ�ȡ������������֮�󵽴�˴�����CH103�ӳ���������ɺ�������
                CH103DiskStatus = DISK_USB_ADDR;
                for ( i = 0; i != 10; i ++ )
                {
                    printf( "Wait DiskReady\n" );
                    s = CH103DiskReady( );                                 //�ȴ�U��׼����
                    if ( s == ERR_SUCCESS )
                    {
                        break;
                    }
                    else
                    {
                        printf("%02x\n",(UINT16)s);
                    }
                    Delay_Ms( 50 );
                }
			   if ( CH103DiskStatus >= DISK_MOUNTED )
                {                    
                /* ���ļ� */
                    printf( "Open\n" );
                    strcpy( (PCHAR)mCmdParam.Open.mPathName, "/C51/CH103HFT.C" );         //����Ҫ�������ļ�����·��
                    s = CH103FileOpen( );                                          //���ļ�
                    if ( s == ERR_MISS_DIR )
                    {
                        printf("�����ڸ��ļ������г���Ŀ¼�����ļ�\n");
                        pCodeStr = (PUINT8)"/*";
                    }
                    else
                    {
                        pCodeStr = (PUINT8)"/C51/*";    //�г�\C51��Ŀ¼�µĵ��ļ�
                    }
                    
                    printf( "List file %s\n", pCodeStr );
                    for ( j = 0; j < 10000; j ++ )                                 //�޶�10000���ļ�,ʵ����û������
                    {
                        strcpy( (PCHAR)mCmdParam.Open.mPathName, (PCCHAR)pCodeStr );              //�����ļ���,*Ϊͨ���,�����������ļ�������Ŀ¼
                        i = strlen( (PCHAR)mCmdParam.Open.mPathName );
                        mCmdParam.Open.mPathName[ i ] = 0xFF;                      //�����ַ������Ƚ��������滻Ϊ���������,��0��254,�����0xFF��255��˵�����������CH103vFileSize������
                        CH103vFileSize = j;                                        //ָ������/ö�ٵ����
                        i = CH103FileOpen( );                                      //���ļ�,����ļ����к���ͨ���*,��Ϊ�����ļ�������
                        /* CH103FileEnum �� CH103FileOpen ��Ψһ�����ǵ����߷���ERR_FOUND_NAMEʱ��ô��Ӧ��ǰ�߷���ERR_SUCCESS */
                        if ( i == ERR_MISS_FILE )
                        {
                            break;    //��Ҳ��������ƥ����ļ�,�Ѿ�û��ƥ����ļ���
                        }
                        if ( i == ERR_FOUND_NAME )                                 //��������ͨ�����ƥ����ļ���,�ļ�����������·�������������
                        {
                            printf( "  match file %04d#: %s\n", (unsigned int)j, mCmdParam.Open.mPathName );  /* ��ʾ��ź���������ƥ���ļ���������Ŀ¼�� */
                            continue;                                                /* ����������һ��ƥ����ļ���,�´�����ʱ��Ż��1 */
                        }
                        else                                                       //����
                        {
                            mStopIfError( i );
                            break;
                        }
                    }
                    i = CH103FileClose( );                                          //�ر��ļ�
                    printf( "U����ʾ���\n" );                     
                }
                else
                {
                    printf( "U��û��׼���� ERR =%02X\n", (UINT16)s );
                }
                  
                
			}
		}
		Delay_Ms( 100 );  // ģ�ⵥƬ����������
        SetUsbSpeed( 1 );  // Ĭ��Ϊȫ��
	}
}




