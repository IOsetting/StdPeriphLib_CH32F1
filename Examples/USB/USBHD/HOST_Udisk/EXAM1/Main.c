/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2020/04/01
* Description        : Main program body.
*******************************************************************************/ 

/*
 *@Note
  U���ļ�ϵͳ���̣�
  USBHDM(PB6)��USBHDP(PB7)��
  ��������ʾ U���ļ�ϵͳ,���ֽ�Ϊ��λ��д�ļ��������ļ�������ɾ�����޸��ļ�
  ���ԣ��޸��ļ�����
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
       2�����CH103DiskReady���ɹ�,��ôǿ�н���ͷ��ʼ���� */
    while ( 1 )
    {  }
}
/*******************************************************************************
* Function Name  : USBHD_ClockCmd
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
	UINT8   s, c, i;
    UINT16  TotalCount;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Delay_Init();
	USART_Printf_Init(115200);
	printf("SystemClk:%d\r\n",SystemCoreClock);;
	
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
                  strcpy( (PCHAR)mCmdParam.Open.mPathName, "/C51/NEWFILE.C" );     //���ý�Ҫ�������ļ�·�����ļ���/C51/NEWFILE.C
                  s = CH103FileOpen( );                                      //���ļ�
                  if ( s == ERR_MISS_DIR || s == ERR_MISS_FILE ) {           //û���ҵ��ļ�
                       //�����ļ���ʾ
                    printf( "Find No File And Create\n" );
                    strcpy( (PCHAR)mCmdParam.Create.mPathName, "/NEWFILE.TXT" );          /* ���ļ���,�ڸ�Ŀ¼��,�����ļ��� */
                    s = CH103FileCreate( );                                        /* �½��ļ�����,����ļ��Ѿ���������ɾ�������½� */
                    mStopIfError( s );
                    printf( "ByteWrite\n" );
                    //ʵ��Ӧ���ж�д���ݳ��ȺͶ��建���������Ƿ������������ڻ�������������Ҫ���д��
                    i = sprintf( (PCHAR)buf,"Note: \xd\xa������������ֽ�Ϊ��λ����U���ļ���д,����ʾ���ܡ�\xd\xa");  /*��ʾ */
                    for(c=0; c<10; c++)
                    {
                        mCmdParam.ByteWrite.mByteCount = i;                          /* ָ������д����ֽ��� */
                        mCmdParam.ByteWrite.mByteBuffer = buf;                       /* ָ�򻺳��� */
                        s = CH103ByteWrite( );                                       /* ���ֽ�Ϊ��λ���ļ�д������ */
                        mStopIfError( s );
                        printf("�ɹ�д�� %02X��\n",(UINT16)c);
                    }
                    //��ʾ�޸��ļ�����
										printf( "Modify\n" );
										mCmdParam.Modify.mFileAttr = 0xff;   //�������: �µ��ļ�����,Ϊ0FFH���޸�
										mCmdParam.Modify.mFileTime = 0xffff;   //�������: �µ��ļ�ʱ��,Ϊ0FFFFH���޸�,ʹ���½��ļ�������Ĭ��ʱ��
										mCmdParam.Modify.mFileDate = MAKE_FILE_DATE( 2015, 5, 18 );  //�������: �µ��ļ�����: 2015.05.18
										mCmdParam.Modify.mFileSize = 0xffffffff;  // �������: �µ��ļ�����,���ֽ�Ϊ��λд�ļ�Ӧ���ɳ����ر��ļ�ʱ�Զ����³���,���Դ˴����޸�
										i = CH103FileModify( );   //�޸ĵ�ǰ�ļ�����Ϣ,�޸�����
										mStopIfError( i );
                    printf( "Close\n" );
                    mCmdParam.Close.mUpdateLen = 1;                                  /* �Զ������ļ�����,���ֽ�Ϊ��λд�ļ�,�����ó����ر��ļ��Ա��Զ������ļ����� */
                    i = CH103FileClose( );
                    mStopIfError( i );
                                     
                    /* ɾ��ĳ�ļ� */
//                    printf( "Erase\n" );
//                    strcpy( (PCHAR)mCmdParam.Create.mPathName, "/OLD" );  //����ɾ�����ļ���,�ڸ�Ŀ¼��
//                    i = CH103FileErase( );  //ɾ���ļ����ر�
//                    if ( i != ERR_SUCCESS ) printf( "Error: %02X\n", (UINT16)i );  //��ʾ����
                  }
                  else
                  {	
	                  //���ϣ������������ӵ�ԭ�ļ���β��,�����ƶ��ļ�ָ��
					  ///////////һ��д���ļ�/////////////////////////////////////////
					    printf( "ByteWrite\n" );
						mCmdParam.ByteLocate.mByteOffset = 0xffffffff;  //�Ƶ��ļ���β��
						CH103ByteLocate( );
						 //ʵ��Ӧ���ж�д���ݳ��ȺͶ��建���������Ƿ������������ڻ�������������Ҫ���д��
						i = sprintf( (PCHAR)buf,"Note: \xd\xa������������ֽ�Ϊ��λ����U���ļ���д,����ʾ���ܡ�\xd\xa");  /*��ʾ */
						for(c=0; c<10; c++)
						{
                        mCmdParam.ByteWrite.mByteCount = i;                          /* ָ������д����ֽ��� */
                        mCmdParam.ByteWrite.mByteBuffer = buf;                       /* ָ�򻺳��� */
                        s = CH103ByteWrite( );                                       /* ���ֽ�Ϊ��λ���ļ�д������ */
                        mStopIfError( s );
                        printf("�ɹ�д�� %02X��\n",(UINT16)c);
						}
																			
						///////////������ȡ�ļ�ǰN�ֽ�/////////////////////////////////////////
						TotalCount = 100;                                      //����׼����ȡ�ܳ���100�ֽ�
						printf( "������ǰ%d���ַ���:\n",TotalCount );
						while ( TotalCount ) {                                 //����ļ��Ƚϴ�,һ�ζ�����,�����ٵ���CH103ByteRead������ȡ,�ļ�ָ���Զ�����ƶ�
                           if ( TotalCount > (MAX_PATH_LEN-1) ) c = MAX_PATH_LEN-1;/* ʣ�����ݽ϶�,���Ƶ��ζ�д�ĳ��Ȳ��ܳ��� sizeof( mCmdParam.Other.mBuffer ) */
                           else c = TotalCount;                                 /* ���ʣ����ֽ��� */
                           mCmdParam.ByteRead.mByteCount = c;                   /* ���������ʮ�ֽ����� */
                           mCmdParam.ByteRead.mByteBuffer= &buf[0];
                           s = CH103ByteRead( );                                /* ���ֽ�Ϊ��λ��ȡ���ݿ�,���ζ�д�ĳ��Ȳ��ܳ���MAX_BYTE_IO,�ڶ��ε���ʱ���Ÿղŵ����� */
                           TotalCount -= mCmdParam.ByteRead.mByteCount;         /* ����,��ȥ��ǰʵ���Ѿ��������ַ��� */
                           for ( i=0; i!=mCmdParam.ByteRead.mByteCount; i++ ) printf( "%c", mCmdParam.ByteRead.mByteBuffer[i] );  /* ��ʾ�������ַ� */
                           if ( mCmdParam.ByteRead.mByteCount < c ) {           /* ʵ�ʶ������ַ�������Ҫ��������ַ���,˵���Ѿ����ļ��Ľ�β */
                                printf( "\n" );
                                printf( "�ļ��Ѿ�����\n" );
                                break;
                           }
                      }
					///////////������ָ��λ�ö�ȡ�ļ�N�ֽ�/////////////////////////////////////////
						
							printf( "Close\n" );
							i = CH103FileClose( );                                    /* �ر��ļ� */
							mStopIfError( i );
                  }
                }
			}
		}
		Delay_Ms( 100 );  // ģ�ⵥƬ����������
        SetUsbSpeed( 1 );  // Ĭ��Ϊȫ��
	}
}




