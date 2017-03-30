/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
* File Name          : platform_config.h
* Author             : MCD Application Team
* Version            : V3.3.0
* Date               : 21-March-2011
* Description        : Evaluation board specific configuration file.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PLATFORM_CONFIG_H
#define __PLATFORM_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Uncomment the line corresponding to the STMicroelectronics evaluation board
   used to run the example */
#if !defined (USE_STM3210B_EVAL) &&  !defined (USE_STM3210E_EVAL) && !defined (USE_STM3210C_EVAL) &&  !defined (USE_STM32_PRIMER2) &&  !defined (USE_STM32_H103) &&  !defined (USE_STM32_SGN101)
 // #define USE_STM3210B_EVAL
 // #define USE_STM3210E_EVAL
 // #define USE_STM3210C_EVAL
 // #define USE_STM32_PRIMER2 
 // #define USE_STM32_H103
 // #define USE_STM32_SGN101
#endif
/* Define the STM32F10x hardware depending on the used evaluation board */
#ifdef USE_STM3210B_EVAL

  #define USB_DISCONNECT                    GPIOD  
  #define USB_DISCONNECT_PIN                GPIO_Pin_9
  #define RCC_APB2Periph_GPIO_DISCONNECT    RCC_APB2Periph_GPIOD
  
  #define USE_LED

#elif defined (USE_STM3210E_EVAL)

  #define USB_DISCONNECT                    GPIOB  
  #define USB_DISCONNECT_PIN                GPIO_Pin_14
  #define RCC_APB2Periph_GPIO_DISCONNECT    RCC_APB2Periph_GPIOB
  
  #define USE_LED

#elif defined (USE_STM3210C_EVAL)

  #define USB_DISCONNECT                    0  
  #define USB_DISCONNECT_PIN                0
  #define RCC_APB2Periph_GPIO_DISCONNECT    0
  
  #define USE_LED  

#elif defined (USE_STM32_PRIMER2)

  #define USB_DISCONNECT                    GPIOD  
  #define USB_DISCONNECT_PIN                GPIO_Pin_3
  #define RCC_APB2Periph_GPIO_DISCONNECT    RCC_APB2Periph_GPIOD
  
  #define USE_LED

#elif defined (USE_STM32_H103)

  #define USB_DISCONNECT                    GPIOC  
  #define USB_DISCONNECT_PIN                GPIO_Pin_11
  #define RCC_APB2Periph_GPIO_DISCONNECT    RCC_APB2Periph_GPIOC

  #define USE_LED

#elif defined (USE_STM32_SGN101)

  #define USB_DISCONNECT                    GPIOB  
  #define USB_DISCONNECT_PIN                GPIO_Pin_8
  #define RCC_APB2Periph_GPIO_DISCONNECT    RCC_APB2Periph_GPIOB
  
  // #define USE_LED

#endif /* USE_STM3210B_EVAL */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __PLATFORM_CONFIG_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
