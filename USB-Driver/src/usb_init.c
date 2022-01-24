/********************************** (C) COPYRIGHT *******************************
* File Name          : usb_init.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2019/10/15
* Description        : Initialization routines & global variables
*******************************************************************************/ 
#include "usb_lib.h"

uint8_t	EPindex;
DEVICE_INFO *pInformation;
DEVICE_PROP *pProperty;
uint16_t	SaveState ;
uint16_t  wInterrupt_Mask;
DEVICE_INFO	Device_Info;
USER_STANDARD_REQUESTS  *pUser_Standard_Requests;

/*******************************************************************************
* Function Name  : USB_Init
* Description    : USB system initialization
* Input          : None.
* Return         : None.
*******************************************************************************/
void USB_Init(void)
{
  pInformation = &Device_Info;
  pInformation->ControlState = 2;
  pProperty = &Device_Property;
  pUser_Standard_Requests = &User_Standard_Requests;
  pProperty->Init();
}





