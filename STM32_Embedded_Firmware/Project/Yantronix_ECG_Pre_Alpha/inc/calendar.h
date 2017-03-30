/*****************************************************************************
* File    : @file
* Company : Yantronix
* Author  : Satish Nair
* Mailto  : satishgn77@gmail.com
*****************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CALENDAR_H
#define __CALENDAR_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/** @addtogroup STM32_BOARD_APP
  * @{
  */

/** @addtogroup CALENDAR
  * @{
  */

/** @defgroup CALENDAR_Exported_Types
  * @{
  */
/**
  * @}
  */

/** @defgroup CALENDAR_Exported_Constants
  * @{
  */
/**
  * @}
  */

/** @defgroup CALENDAR_Exported_Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup CALENDAR_Exported_Functions
  * @{
  */
uint8_t Calendar_ReadDigit(uint8_t ColBegin, uint8_t CountBegin, uint8_t ValueMax, uint8_t ValueMin);
void Calendar_Init(void);
uint32_t Calendar_TimeRegulate(void);
void Calendar_TimeAdjust(void);
void Calendar_TimeShow(void);
void Calendar_TimeDisplay(uint32_t TimeVar);
void Calendar_DateRegulate(void);
void Calendar_DateAdjust(void);
void Calendar_DateDisplay(uint16_t nYear, uint8_t nMonth, uint8_t nDay);
void Calendar_DateShow(void);
void Calendar_DateUpdate(void);
uint32_t Calendar_AlarmRegulate(void);
void Calendar_AlarmAdjust(void);
void Calendar_AlarmPreAdjust(void);
void Calendar_AlarmDisplay(uint32_t AlarmVar);
void Calendar_AlarmShow(void);
void Create_Date_Time_Filename(uint8_t* Filename, uint8_t* Filetype);

#ifdef __cplusplus
}
#endif

#endif /* __CALENDAR_H */
/**
  * @}
  */
  
/**
  * @}
  */

/**
  * @}
  */


