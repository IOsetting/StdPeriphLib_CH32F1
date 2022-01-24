/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32f10x_bkp.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2019/10/15
* Description        : This file provides all the BKP firmware functions.
*******************************************************************************/ 
#include "ch32f10x_bkp.h"
#include "ch32f10x_rcc.h"

/* BKP registers bit address in the alias region */
#define BKP_OFFSET        (BKP_BASE - PERIPH_BASE)

/* TPCTLR Register */

/* Alias word address of TPAL bit */
#define TPCTLR_OFFSET         (BKP_OFFSET + 0x30)
#define TPAL_BitNumber    0x01
#define TPCTLR_TPAL_BB        (PERIPH_BB_BASE + (TPCTLR_OFFSET * 32) + (TPAL_BitNumber * 4))

/* Alias word address of TPE bit */
#define TPE_BitNumber     0x00
#define TPCTLR_TPE_BB         (PERIPH_BB_BASE + (TPCTLR_OFFSET * 32) + (TPE_BitNumber * 4))

/* TPCSR Register */

/* Alias word address of TPIE bit */
#define TPCSR_OFFSET        (BKP_OFFSET + 0x34)
#define TPIE_BitNumber    0x02
#define TPCSR_TPIE_BB       (PERIPH_BB_BASE + (TPCSR_OFFSET * 32) + (TPIE_BitNumber * 4))

/* Alias word address of TIF bit */
#define TIF_BitNumber     0x09
#define TPCSR_TIF_BB        (PERIPH_BB_BASE + (TPCSR_OFFSET * 32) + (TIF_BitNumber * 4))

/* Alias word address of TEF bit */
#define TEF_BitNumber     0x08
#define TPCSR_TEF_BB        (PERIPH_BB_BASE + (TPCSR_OFFSET * 32) + (TEF_BitNumber * 4))

/* BKP registers bit mask */

/* OCTLR register bit mask */
#define OCTLR_CAL_MASK    ((uint16_t)0xFF80)
#define OCTLR_MASK        ((uint16_t)0xFC7F)

/*******************************************************************************
* Function Name  : BKP_DeInit
* Description    : Deinitializes the BKP peripheral registers to their default 
*      reset values.
* Input          : None
* Return         : None
*******************************************************************************/	
void BKP_DeInit(void)
{
  RCC_BackupResetCmd(ENABLE);
  RCC_BackupResetCmd(DISABLE);
}

/*******************************************************************************
* Function Name  : BKP_TamperPinLevelConfig
* Description    : Configures the Tamper Pin active level.
* Input          : BKP_TamperPinLevel: specifies the Tamper Pin active level.
*                    BKP_TamperPinLevel_High: Tamper pin active on high level.
*                    BKP_TamperPinLevel_Low: Tamper pin active on low level.
* Return         : None
*******************************************************************************/	
void BKP_TamperPinLevelConfig(uint16_t BKP_TamperPinLevel)
{
  *(__IO uint32_t *) TPCTLR_TPAL_BB = BKP_TamperPinLevel;
}

/*******************************************************************************
* Function Name  : BKP_TamperPinCmd
* Description    : Enables or disables the Tamper Pin activation.
* Input          : NewState: ENABLE or DISABLE.
* Return         : None
*******************************************************************************/	
void BKP_TamperPinCmd(FunctionalState NewState)
{
  *(__IO uint32_t *) TPCTLR_TPE_BB = (uint32_t)NewState;
}

/*******************************************************************************
* Function Name  : BKP_ITConfig
* Description    : Enables or disables the Tamper Pin Interrupt.
* Input          : NewState: ENABLE or DISABLE.
* Return         : None
*******************************************************************************/	
void BKP_ITConfig(FunctionalState NewState)
{
  *(__IO uint32_t *) TPCSR_TPIE_BB = (uint32_t)NewState;
}

/*******************************************************************************
* Function Name  : BKP_RTCOutputConfig
* Description    : Select the RTC output source to output on the Tamper pin.
* Input          : BKP_RTCOutputSource: specifies the RTC output source.
*                    BKP_RTCOutputSource_None: no RTC output on the Tamper pin.
*                    BKP_RTCOutputSource_CalibClock: output the RTC clock with 
*      frequency divided by 64 on the Tamper pin.
*                    BKP_RTCOutputSource_Alarm: output the RTC Alarm pulse signal 
*      on the Tamper pin.
*                    BKP_RTCOutputSource_Second: output the RTC Second pulse 
*      signal on the Tamper pin. 
* Return         : None
*******************************************************************************/	
void BKP_RTCOutputConfig(uint16_t BKP_RTCOutputSource)
{
  uint16_t tmpreg = 0;

  tmpreg = BKP->OCTLR;
  tmpreg &= OCTLR_MASK;
  tmpreg |= BKP_RTCOutputSource;
  BKP->OCTLR = tmpreg;
}

/*******************************************************************************
* Function Name  : BKP_SetRTCCalibrationValue
* Description    : Sets RTC Clock Calibration value.
* Input          : CalibrationValue: specifies the RTC Clock Calibration value.
*                    This parameter must be a number between 0 and 0x1F.
* Return         : None
*******************************************************************************/
void BKP_SetRTCCalibrationValue(uint8_t CalibrationValue)
{
  uint16_t tmpreg = 0;

  tmpreg = BKP->OCTLR;
  tmpreg &= OCTLR_CAL_MASK;
  tmpreg |= CalibrationValue;
  BKP->OCTLR = tmpreg;
}

/*******************************************************************************
* Function Name  : BKP_WriteBackupRegister
* Description    : Writes user data to the specified Data Backup Register.
* Input          : BKP_DR: specifies the Data Backup Register.
*                  Data: data to write.
* Return         : None
*******************************************************************************/
void BKP_WriteBackupRegister(uint16_t BKP_DR, uint16_t Data)
{
  __IO uint32_t tmp = 0;

  tmp = (uint32_t)BKP_BASE; 
  tmp += BKP_DR;
  *(__IO uint32_t *) tmp = Data;
}

/*******************************************************************************
* Function Name  : BKP_ReadBackupRegister
* Description    : Reads data from the specified Data Backup Register.
* Input          : BKP_DR: specifies the Data Backup Register.
*                    This parameter can be BKP_DRx where x:[1, 42].
* Return         : None
*******************************************************************************/
uint16_t BKP_ReadBackupRegister(uint16_t BKP_DR)
{
  __IO uint32_t tmp = 0;

  tmp = (uint32_t)BKP_BASE; 
  tmp += BKP_DR;

  return (*(__IO uint16_t *) tmp);
}

/*******************************************************************************
* Function Name  : BKP_GetFlagStatus
* Description    : Checks whether the Tamper Pin Event flag is set or not.
* Input          : None
* Return         : FlagStatus: SET or RESET.
*******************************************************************************/
FlagStatus BKP_GetFlagStatus(void)
{
  return (FlagStatus)(*(__IO uint32_t *) TPCSR_TEF_BB);
}

/*******************************************************************************
* Function Name  : BKP_ClearFlag
* Description    : Clears Tamper Pin Event pending flag.
* Input          : None
* Return         : None
*******************************************************************************/
void BKP_ClearFlag(void)
{
  BKP->TPCSR |= BKP_CTE;
}

/*******************************************************************************
* Function Name  : BKP_GetITStatus
* Description    : Checks whether the Tamper Pin Interrupt has occurred or not.
* Input          : None
* Return         : ITStatus: SET or RESET.
*******************************************************************************/
ITStatus BKP_GetITStatus(void)
{
  return (ITStatus)(*(__IO uint32_t *) TPCSR_TIF_BB);
}

/*******************************************************************************
* Function Name  : BKP_ClearITPendingBit
* Description    : Clears Tamper Pin Interrupt pending bit.
* Input          : None
* Return         : None
*******************************************************************************/
void BKP_ClearITPendingBit(void)
{
  BKP->TPCSR |= BKP_CTI;
}






