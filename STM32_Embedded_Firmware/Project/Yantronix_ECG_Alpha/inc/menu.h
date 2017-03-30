/*****************************************************************************
* File    : @file
* Company : Yantronix
* Author  : Satish Nair
* Mailto  : satishgn77@gmail.com
*****************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MENU_H
#define __MENU_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/** @addtogroup STM32_BOARD_APP
  * @{
  */

/** @addtogroup MENU
  * @{
  */

/** @defgroup MENU_Exported_Types
  * @{
  */
/**
  * @}
  */

/** @defgroup MENU_Exported_Constants
  * @{
  */
#define  MAX_MENU_LEVELS 4
#define  NOKEY  0
#define  MENU   1
#define  SEL    2
#define  UP     3
#define  DOWN   4


/**
  * @}
  */

/** @defgroup MENU_Exported_Macros
  * @{
  */
#define countof(a) (sizeof(a) / sizeof(*(a)))
/**
  * @}
  */

/** @defgroup MENU_Exported_Functions
  * @{
  */
void Menu_Init(void);
void Menu_DisplayMenu(void);
void Menu_SelFunc(void);
void Menu_UpFunc(void);
void Menu_DownFunc(void);
void Menu_ReturnFunc(void);
void Menu_ReturnMainMenu(void);
uint8_t Menu_ReadKey(void);
void Menu_IdleFunc(void);
void Menu_DisplayIcons(void);
void Menu_ShowMenuIcons(void);
void Menu_STM32Intro(void);
void Menu_HelpFunc(void);
void Menu_AboutFunc(void);

#ifdef __cplusplus
}
#endif

#endif /* __MENU_H */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */


