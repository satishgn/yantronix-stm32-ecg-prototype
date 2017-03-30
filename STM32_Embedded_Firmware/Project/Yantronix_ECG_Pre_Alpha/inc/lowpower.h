/*****************************************************************************
* File    : @file
* Company : Yantronix
* Author  : Satish Nair
* Mailto  : satishgn77@gmail.com
*****************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LOWPOWER_H
#define __LOWPOWER_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/** @addtogroup STM32_BOARD_APP
  * @{
  */

/** @addtogroup LOWPOWER
  * @{
  */

/** @defgroup LOWPOWER_Exported_Types
  * @{
  */
/**
  * @}
  */

/** @defgroup LOWPOWER_Exported_Constants
  * @{
  */
/**
  * @}
  */

/** @defgroup LOWPOWER_Exported_Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup LOWPOWER_Exported_Functions
  * @{
  */
void LowPower_Init(void);
void LowPower_SYSCLKConfig_STOP(void);
void LowPower_EnterSTOPMode_EXTI(void);
void LowPower_EnterSTOPMode_RTCAlarm(void);
void LowPower_EnterSTANDBYMode_WAKEUP(void);
void LowPower_EnterSTANDBYMode_RTCAlarm(void);
void LowPower_Set_STOPModeStatus(void);
void LowPower_SaveGPIOConfig(void);
void LowPower_RestoreGPIOConfig(void);

#ifdef __cplusplus
}
#endif

#endif /* __LOWPOWER_H */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */


