/********************************** (C) COPYRIGHT *******************************
* File Name          : usb_prop.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2019/10/15
* Description        : All processing related to Virtual Com Port Demo
*******************************************************************************/ 
#include "usb_lib.h"
#include "usb_conf.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "hw_config.h"

uint8_t Request = 0;

DEVICE Device_Table =
{
	EP_NUM,
	1
};

DEVICE_PROP Device_Property =
{
	USBD_init,
	USBD_Reset,
	USBD_Status_In,
	USBD_Status_Out,
	USBD_Data_Setup,
	USBD_NoData_Setup,
	USBD_Get_Interface_Setting,
	USBD_GetDeviceDescriptor,
	USBD_GetConfigDescriptor,
	USBD_GetStringDescriptor,
	0,
	0x08                                 
};

USER_STANDARD_REQUESTS User_Standard_Requests =
{
	USBD_GetConfiguration,
	USBD_SetConfiguration,
	USBD_GetInterface,
	USBD_SetInterface,
	USBD_GetStatus, 
	USBD_ClearFeature,
	USBD_SetEndPointFeature,
	USBD_SetDeviceFeature,
	USBD_SetDeviceAddress
};

ONE_DESCRIPTOR Device_Descriptor =
{
	(uint8_t*)USBD_DeviceDescriptor,
	USBD_SIZE_DEVICE_DESC
};

ONE_DESCRIPTOR Config_Descriptor =
{
	(uint8_t*)USBD_ConfigDescriptor,
	USBD_SIZE_CONFIG_DESC
};

ONE_DESCRIPTOR String_Descriptor[4] =
{
	{(uint8_t*)USBD_StringLangID, USBD_SIZE_STRING_LANGID},
	{(uint8_t*)USBD_StringVendor, USBD_SIZE_STRING_VENDOR},
	{(uint8_t*)USBD_StringProduct,USBD_SIZE_STRING_PRODUCT},
	{(uint8_t*)USBD_StringSerial, USBD_SIZE_STRING_SERIAL}
};


/*******************************************************************************
* Function Name  : USBD_SetConfiguration.
* Description    : Update the device state to configured.
* Input          : None.
* Return         : None.
*******************************************************************************/
void USBD_SetConfiguration(void)
{
  DEVICE_INFO *pInfo = &Device_Info;

  if (pInfo->Current_Configuration != 0)
  {
    bDeviceState = CONFIGURED;
  }
}

/*******************************************************************************
* Function Name  : USBD_SetDeviceAddress.
* Description    : Update the device state to addressed.
* Input          : None.
* Return         : None.
*******************************************************************************/
void USBD_SetDeviceAddress (void)
{
  bDeviceState = ADDRESSED;
}

/*******************************************************************************
* Function Name  : USBD_Status_In.
* Description    : USBD Status In Routine.
* Input          : None.
* Return         : None.
*******************************************************************************/
void USBD_Status_In(void)
{
 
}

/*******************************************************************************
* Function Name  : USBD_Status_Out
* Description    : USBD Status OUT Routine.
* Input          : None.
* Return         : None.
*******************************************************************************/
void USBD_Status_Out(void)
{
    
}

/*******************************************************************************
* Function Name  : USB_init.
* Description    : init routine.
* Input          : None.
* Return         : None.
*******************************************************************************/
void USBD_init(void)
{
  uint8_t	i;
    
  pInformation->Current_Configuration = 0;
  PowerOn();
  for (i=0;i<8;i++) _SetENDPOINT(i,_GetENDPOINT(i) & 0x7F7F & EPREG_MASK);//all clear
  _SetISTR((uint16_t)0x00FF);//all clear
  USB_SIL_Init();
  bDeviceState = UNCONNECTED;
}

/*******************************************************************************
* Function Name  : USBD_Reset
* Description    : Virtual_Com_Port Mouse reset routine
* Input          : None.
* Return         : None.
*******************************************************************************/
void USBD_Reset(void)
{
  pInformation->Current_Configuration = 0;
  pInformation->Current_Feature = USBD_ConfigDescriptor[7];
  pInformation->Current_Interface = 0;

  SetBTABLE(BTABLE_ADDRESS);

  SetEPType(ENDP0, EP_CONTROL);
  SetEPTxStatus(ENDP0, EP_TX_STALL);
  SetEPRxAddr(ENDP0, ENDP0_RXADDR);
  SetEPTxAddr(ENDP0, ENDP0_TXADDR);
  Clear_Status_Out(ENDP0);
  SetEPRxCount(ENDP0, Device_Property.MaxPacketSize);
  SetEPRxValid(ENDP0);
  _ClearDTOG_RX(ENDP0);
  _ClearDTOG_TX(ENDP0);

  SetEPType(ENDP1, EP_BULK);
  SetEPTxAddr(ENDP1, ENDP1_TXADDR);
  SetEPTxStatus(ENDP1, EP_TX_NAK);
  SetEPRxStatus(ENDP1, EP_RX_DIS);
  _ClearDTOG_RX(ENDP1);
  _ClearDTOG_TX(ENDP1);

  SetEPType(ENDP2, EP_BULK);
  SetEPTxAddr(ENDP2, ENDP2_TXADDR);
  SetEPRxAddr(ENDP2, ENDP2_RXADDR);   
  SetEPRxCount(ENDP2, USBD_DATA_SIZE);
  SetEPRxStatus(ENDP2, EP_RX_VALID);
  SetEPTxStatus(ENDP2, EP_TX_NAK);
  _ClearDTOG_RX(ENDP2);
  _ClearDTOG_TX(ENDP2);

  SetEPType(ENDP3, EP_INTERRUPT);
  SetEPRxAddr(ENDP3, ENDP3_RXADDR);
  SetEPRxCount(ENDP3, USBD_DATA_SIZE);
  SetEPRxStatus(ENDP3, EP_RX_VALID);
  SetEPTxStatus(ENDP3, EP_TX_DIS);
  _ClearDTOG_RX(ENDP3);
  _ClearDTOG_TX(ENDP3);
   
  SetDeviceAddress(0);
  
  bDeviceState = ATTACHED;
}

/*******************************************************************************
* Function Name  : USBD_GetDeviceDescriptor.
* Description    : Gets the device descriptor.
* Input          : Length.
* Return         : The address of the device descriptor.
*******************************************************************************/
uint8_t *USBD_GetDeviceDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Device_Descriptor);
}

/*******************************************************************************
* Function Name  : USBD_GetConfigDescriptor.
* Description    : get the configuration descriptor.
* Input          : Length.
* Return         : The address of the configuration descriptor.
*******************************************************************************/
uint8_t *USBD_GetConfigDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Config_Descriptor);
}

/*******************************************************************************
* Function Name  : USBD_GetStringDescriptor
* Description    : Gets the string descriptors according to the needed index
* Input          : Length.
* Return         : The address of the string descriptors.
*******************************************************************************/
uint8_t *USBD_GetStringDescriptor(uint16_t Length)
{
  uint8_t wValue0 = pInformation->USBwValue0;
	
  if (wValue0 > 4)
  {
    return NULL;
  }
  else
  {
    return Standard_GetDescriptorData(Length, &String_Descriptor[wValue0]);
  }
}

/*******************************************************************************
* Function Name  : USBD_Get_Interface_Setting.
* Description    : test the interface and the alternate setting according to the
*                  supported one.
* Input1         : Interface: interface number.
*                  AlternateSetting: Alternate Setting number.
* Return         : The address of the string descriptors.
*******************************************************************************/
RESULT USBD_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting)
{
  if (AlternateSetting > 0)
  {
    return USB_UNSUPPORT;
  }
  else if (Interface > 1)
  {
    return USB_UNSUPPORT;
  }
	
  return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : USBD_Data_Setup
* Description    : handle the data class specific requests
* Input          : Request Nb.
* Return         : USB_UNSUPPORT or USB_SUCCESS.
*******************************************************************************/
RESULT USBD_Data_Setup(uint8_t RequestNo)
{
  
  return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : USBD_NoData_Setup.
* Description    : handle the no data class specific requests.
* Input          : Request Nb.
* Return         : USB_UNSUPPORT or USB_SUCCESS.
*******************************************************************************/
RESULT USBD_NoData_Setup(uint8_t RequestNo)
{                                          
  return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : Virtual_Com_Port_GetLineCoding.
* Description    : send the linecoding structure to the PC host.
* Input          : Length.
* Return         : Linecoding structure base address.
*******************************************************************************/
uint8_t *Virtual_Com_Port_GetLineCoding(uint16_t Length)
{
  return 0;
}

/*******************************************************************************
* Function Name  : Virtual_Com_Port_SetLineCoding.
* Description    : Set the linecoding structure fields.
* Input          : Length.
* Return         : Linecoding structure base address.
*******************************************************************************/
uint8_t *Virtual_Com_Port_SetLineCoding(uint16_t Length)
{
  return 0;
}






