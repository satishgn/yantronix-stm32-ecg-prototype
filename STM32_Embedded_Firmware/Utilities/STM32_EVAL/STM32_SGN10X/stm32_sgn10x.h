/*****************************************************************************
* File    : @file
* Company : Yantronix
* Author  : Satish Nair
* Mailto  : satishgn77@gmail.com
*****************************************************************************/
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32_SGN10X_H
#define __STM32_SGN10X_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32_eval.h"

/** @addtogroup Utilities
  * @{
  */

/** @addtogroup STM32_EVAL
  * @{
  */
    
/** @addtogroup STM32_SGN10X
  * @{
  */ 

/** @addtogroup STM32_SGN10X_LOW_LEVEL
  * @{
  */
  
/** @defgroup STM32_SGN10X_LOW_LEVEL_Exported_Types
  * @{
  */
/**
  * @}
  */


/** @defgroup STM32_SGN10X_LOW_LEVEL_Exported_Constants
  * @{
  */ 
/** @addtogroup STM32_SGN10X_LOW_LEVEL_LED
  * @{
  */
#define LEDn                             4
#define LED1_PIN                         GPIO_Pin_9  
#define LED1_GPIO_PORT                   GPIOB
#define LED1_GPIO_CLK                    RCC_APB2Periph_GPIOB  
  
#define LED2_PIN                         0
#define LED2_GPIO_PORT                   0
#define LED2_GPIO_CLK                    0  

#define LED3_PIN                         0
#define LED3_GPIO_PORT                   0
#define LED3_GPIO_CLK                    0  
  
#define LED4_PIN                         0
#define LED4_GPIO_PORT                   0
#define LED4_GPIO_CLK                    0  

/**
  * @}
  */ 
  
/** @addtogroup STM32_SGN10X_LOW_LEVEL_BUTTON
  * @{
  */  
#define BUTTONn                          8

/**
 * @brief Wakeup/Power push-button
 */
#define WAKEUP_BUTTON_PIN                0
#define WAKEUP_BUTTON_GPIO_PORT          0
#define WAKEUP_BUTTON_GPIO_CLK           0
#define WAKEUP_BUTTON_EXTI_LINE          0
#define WAKEUP_BUTTON_EXTI_PORT_SOURCE   0
#define WAKEUP_BUTTON_EXTI_PIN_SOURCE    0
#define WAKEUP_BUTTON_EXTI_IRQn          0 
/**
 * @brief Tamper push-button
 */
#define TAMPER_BUTTON_PIN                0
#define TAMPER_BUTTON_GPIO_PORT          0
#define TAMPER_BUTTON_GPIO_CLK           0
#define TAMPER_BUTTON_EXTI_LINE          0
#define TAMPER_BUTTON_EXTI_PORT_SOURCE   0
#define TAMPER_BUTTON_EXTI_PIN_SOURCE    0
#define TAMPER_BUTTON_EXTI_IRQn          0 
/**
 * @brief Key/Menu push-button
 */
#define KEY_BUTTON_PIN                   0
#define KEY_BUTTON_GPIO_PORT             0
#define KEY_BUTTON_GPIO_CLK              0
#define KEY_BUTTON_EXTI_LINE             0
#define KEY_BUTTON_EXTI_PORT_SOURCE      0
#define KEY_BUTTON_EXTI_PIN_SOURCE       0
#define KEY_BUTTON_EXTI_IRQn             0
/**
 * @brief Joystick Right push-button
 */
#define RIGHT_BUTTON_PIN                 0
#define RIGHT_BUTTON_GPIO_PORT           0
#define RIGHT_BUTTON_GPIO_CLK            0
#define RIGHT_BUTTON_EXTI_LINE           0
#define RIGHT_BUTTON_EXTI_PORT_SOURCE    0
#define RIGHT_BUTTON_EXTI_PIN_SOURCE     0
#define RIGHT_BUTTON_EXTI_IRQn           0
/**
 * @brief Joystick Left push-button
 */
#define LEFT_BUTTON_PIN                  0
#define LEFT_BUTTON_GPIO_PORT            0
#define LEFT_BUTTON_GPIO_CLK             0
#define LEFT_BUTTON_EXTI_LINE            0
#define LEFT_BUTTON_EXTI_PORT_SOURCE     0
#define LEFT_BUTTON_EXTI_PIN_SOURCE      0
#define LEFT_BUTTON_EXTI_IRQn            0  
/**
 * @brief Joystick Up push-button
 */
#define UP_BUTTON_PIN                    0
#define UP_BUTTON_GPIO_PORT              0
#define UP_BUTTON_GPIO_CLK               0
#define UP_BUTTON_EXTI_LINE              0
#define UP_BUTTON_EXTI_PORT_SOURCE       0
#define UP_BUTTON_EXTI_PIN_SOURCE        0
#define UP_BUTTON_EXTI_IRQn              0  
/**
 * @brief Joystick Down push-button
 */  
#define DOWN_BUTTON_PIN                  0
#define DOWN_BUTTON_GPIO_PORT            0
#define DOWN_BUTTON_GPIO_CLK             0
#define DOWN_BUTTON_EXTI_LINE            0
#define DOWN_BUTTON_EXTI_PORT_SOURCE     0
#define DOWN_BUTTON_EXTI_PIN_SOURCE      0
#define DOWN_BUTTON_EXTI_IRQn            0  
/**
 * @brief Joystick Sel push-button
 */
#define SEL_BUTTON_PIN                   0
#define SEL_BUTTON_GPIO_PORT             0
#define SEL_BUTTON_GPIO_CLK              0
#define SEL_BUTTON_EXTI_LINE             0
#define SEL_BUTTON_EXTI_PORT_SOURCE      0
#define SEL_BUTTON_EXTI_PIN_SOURCE       0
#define SEL_BUTTON_EXTI_IRQn             0          
/**
  * @}
  */ 

/** @addtogroup STM32_SGN10X_LOW_LEVEL_COM
  * @{
  */
#define COMn                             2

/**
 * @brief Definition for COM port1, connected to USART1
 */ 
#define EVAL_COM1                        USART1
#define EVAL_COM1_CLK                    RCC_APB2Periph_USART1
#define EVAL_COM1_TX_PIN                 GPIO_Pin_9
#define EVAL_COM1_TX_GPIO_PORT           GPIOA
#define EVAL_COM1_TX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define EVAL_COM1_RX_PIN                 GPIO_Pin_10
#define EVAL_COM1_RX_GPIO_PORT           GPIOA
#define EVAL_COM1_RX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define EVAL_COM1_IRQn                   USART1_IRQn

/**
 * @brief Definition for COM port2, connected to USART2
 */ 
#define EVAL_COM2                        0
#define EVAL_COM2_CLK                    0
#define EVAL_COM2_TX_PIN                 0
#define EVAL_COM2_TX_GPIO_PORT           0
#define EVAL_COM2_TX_GPIO_CLK            0
#define EVAL_COM2_RX_PIN                 0
#define EVAL_COM2_RX_GPIO_PORT           0
#define EVAL_COM2_RX_GPIO_CLK            0
#define EVAL_COM2_IRQn                   0

/**
  * @}
  */
  
/** @defgroup STM32_SGN10X_LOW_LEVEL_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup STM32_SGN10X_LOW_LEVEL_Exported_Functions
  * @{
  */ 
void STM_EVAL_LEDInit(Led_TypeDef Led);
void STM_EVAL_LEDOn(Led_TypeDef Led);
void STM_EVAL_LEDOff(Led_TypeDef Led);
void STM_EVAL_LEDToggle(Led_TypeDef Led);
void STM_EVAL_PBInit(Button_TypeDef Button, ButtonMode_TypeDef Button_Mode);
uint32_t STM_EVAL_PBGetState(Button_TypeDef Button);
void STM_EVAL_COMInit(COM_TypeDef COM, USART_InitTypeDef* USART_InitStruct);
 
/**
  * @}
  */ 
    
#ifdef __cplusplus
}
#endif
  
#endif /* __STM32_SGN10X_H */
/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */  

/**
  * @}
  */    


