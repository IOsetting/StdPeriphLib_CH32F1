/********************************** (C) COPYRIGHT *******************************
* File Name          : usb_type.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2019/10/15
* Description        : This file contains all the functions prototypes for the  
*                      USB types firmware library.
*******************************************************************************/ 
#ifndef __USB_TYPE_H
#define __USB_TYPE_H
#include "debug.h"
#include "usb_conf.h"

#ifndef NULL
#define NULL ((void *)0)
#endif

typedef enum
{
  FALSE = 0, TRUE  = !FALSE
}
bool;

#endif /* __USB_TYPE_H */





