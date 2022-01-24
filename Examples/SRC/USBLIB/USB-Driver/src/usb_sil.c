/********************************** (C) COPYRIGHT *******************************
* File Name          : usb_sil.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2019/10/15
* Description        : Simplified Interface Layer for Global Initialization and 
*			           Endpoint  Rea/Write operations.
*******************************************************************************/ 
#include "usb_lib.h"


/*******************************************************************************
* Function Name  : USB_SIL_Init
* Description    : Initialize the USB Device IP and the Endpoint 0.
* Input          : None.
* Return         : Status.
*******************************************************************************/
uint32_t USB_SIL_Init(void)
{
  _SetISTR(0);
  wInterrupt_Mask = IMR_MSK;
  _SetCNTR(wInterrupt_Mask);
	
  return 0;
}

/*******************************************************************************
* Function Name  : USB_SIL_Write
* Description    : Write a buffer of data to a selected endpoint.
* Input          : bEpAddr: The address of the non control endpoint.
*                  pBufferPointer: The pointer to the buffer of data to be written
*                    to the endpoint.
*                  wBufferSize: Number of data to be written (in bytes).
* Return         : Status.
*******************************************************************************/
uint32_t USB_SIL_Write(uint8_t bEpAddr, uint8_t* pBufferPointer, uint32_t wBufferSize)
{
  UserToPMABufferCopy(pBufferPointer, GetEPTxAddr(bEpAddr & 0x7F), wBufferSize);
  SetEPTxCount((bEpAddr & 0x7F), wBufferSize);
  
  return 0;
}

/*******************************************************************************
* Function Name  : USB_SIL_Read
* Description    : Write a buffer of data to a selected endpoint.
* Input          : bEpAddr: The address of the non control endpoint.
*                  pBufferPointer: The pointer to which will be saved the 
*      received data buffer.
* Return         : Number of received data (in Bytes).
*******************************************************************************/
uint32_t USB_SIL_Read(uint8_t bEpAddr, uint8_t* pBufferPointer)
{
  uint32_t DataLength = 0;

  DataLength = GetEPRxCount(bEpAddr & 0x7F); 
  PMAToUserBufferCopy(pBufferPointer, GetEPRxAddr(bEpAddr & 0x7F), DataLength);

  return DataLength;
}






