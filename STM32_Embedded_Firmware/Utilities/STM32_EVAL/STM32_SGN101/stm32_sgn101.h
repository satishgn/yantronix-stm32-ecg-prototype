/*****************************************************************************
* File    : @file
* Company : Yantronix
* Author  : Satish Nair
* Mailto  : satishgn77@gmail.com
*****************************************************************************/
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32_SGN101_H
#define __STM32_SGN101_H

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
    
/** @addtogroup STM32_SGN101
  * @{
  */ 

/** @addtogroup STM32_SGN101_LOW_LEVEL
  * @{
  */
  
/** @defgroup STM32_SGN101_LOW_LEVEL_Exported_Types
  * @{
  */
/**
  * @}
  */


/** @defgroup STM32_SGN101_LOW_LEVEL_Exported_Constants
  * @{
  */ 
/** @addtogroup STM32_SGN101_LOW_LEVEL_LED
  * @{
  */
#define LEDn                             4
#define LED1_PIN                         GPIO_Pin_3  
#define LED1_GPIO_PORT                   GPIOA
#define LED1_GPIO_CLK                    RCC_APB2Periph_GPIOA  
  
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
  
/** @addtogroup STM32_SGN101_LOW_LEVEL_BUTTON
  * @{
  */  
#define BUTTONn                          8

/**
 * @brief Wakeup/Power push-button
 */
#define WAKEUP_BUTTON_PIN                GPIO_Pin_0
#define WAKEUP_BUTTON_GPIO_PORT          GPIOA
#define WAKEUP_BUTTON_GPIO_CLK           RCC_APB2Periph_GPIOA
#define WAKEUP_BUTTON_EXTI_LINE          EXTI_Line0
#define WAKEUP_BUTTON_EXTI_PORT_SOURCE   GPIO_PortSourceGPIOA
#define WAKEUP_BUTTON_EXTI_PIN_SOURCE    GPIO_PinSource0
#define WAKEUP_BUTTON_EXTI_IRQn          EXTI0_IRQn 
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
#define KEY_BUTTON_PIN                   GPIO_Pin_1
#define KEY_BUTTON_GPIO_PORT             GPIOA
#define KEY_BUTTON_GPIO_CLK              RCC_APB2Periph_GPIOA
#define KEY_BUTTON_EXTI_LINE             EXTI_Line1
#define KEY_BUTTON_EXTI_PORT_SOURCE      GPIO_PortSourceGPIOA
#define KEY_BUTTON_EXTI_PIN_SOURCE       GPIO_PinSource1
#define KEY_BUTTON_EXTI_IRQn             EXTI0_IRQn
/**
 * @brief Joystick Right push-button
 */
#define RIGHT_BUTTON_PIN                 GPIO_Pin_6
#define RIGHT_BUTTON_GPIO_PORT           GPIOB
#define RIGHT_BUTTON_GPIO_CLK            RCC_APB2Periph_GPIOB
#define RIGHT_BUTTON_EXTI_LINE           EXTI_Line6
#define RIGHT_BUTTON_EXTI_PORT_SOURCE    RCC_APB2Periph_GPIOB
#define RIGHT_BUTTON_EXTI_PIN_SOURCE     GPIO_PinSource6
#define RIGHT_BUTTON_EXTI_IRQn           EXTI9_5_IRQn
/**
 * @brief Joystick Left push-button
 */
#define LEFT_BUTTON_PIN                  GPIO_Pin_7
#define LEFT_BUTTON_GPIO_PORT            GPIOB
#define LEFT_BUTTON_GPIO_CLK             RCC_APB2Periph_GPIOB
#define LEFT_BUTTON_EXTI_LINE            EXTI_Line7
#define LEFT_BUTTON_EXTI_PORT_SOURCE     RCC_APB2Periph_GPIOB
#define LEFT_BUTTON_EXTI_PIN_SOURCE      GPIO_PinSource7
#define LEFT_BUTTON_EXTI_IRQn            EXTI9_5_IRQn  
/**
 * @brief Joystick Up push-button
 */
#define UP_BUTTON_PIN                    GPIO_Pin_7
#define UP_BUTTON_GPIO_PORT              GPIOB
#define UP_BUTTON_GPIO_CLK               RCC_APB2Periph_GPIOB
#define UP_BUTTON_EXTI_LINE              EXTI_Line7
#define UP_BUTTON_EXTI_PORT_SOURCE       RCC_APB2Periph_GPIOB
#define UP_BUTTON_EXTI_PIN_SOURCE        GPIO_PinSource7
#define UP_BUTTON_EXTI_IRQn              EXTI9_5_IRQn  
/**
 * @brief Joystick Down push-button
 */  
#define DOWN_BUTTON_PIN                  GPIO_Pin_6
#define DOWN_BUTTON_GPIO_PORT            GPIOB
#define DOWN_BUTTON_GPIO_CLK             RCC_APB2Periph_GPIOB
#define DOWN_BUTTON_EXTI_LINE            EXTI_Line6
#define DOWN_BUTTON_EXTI_PORT_SOURCE     RCC_APB2Periph_GPIOB
#define DOWN_BUTTON_EXTI_PIN_SOURCE      GPIO_PinSource6
#define DOWN_BUTTON_EXTI_IRQn            EXTI9_5_IRQn  
/**
 * @brief Joystick Sel push-button
 */
#define SEL_BUTTON_PIN                   GPIO_Pin_2
#define SEL_BUTTON_GPIO_PORT             GPIOA
#define SEL_BUTTON_GPIO_CLK              RCC_APB2Periph_GPIOA
#define SEL_BUTTON_EXTI_LINE             EXTI_Line2
#define SEL_BUTTON_EXTI_PORT_SOURCE      RCC_APB2Periph_GPIOA
#define SEL_BUTTON_EXTI_PIN_SOURCE       GPIO_PinSource2
#define SEL_BUTTON_EXTI_IRQn             EXTI2_IRQn          
/**
  * @}
  */ 

/** @addtogroup STM32_SGN101_LOW_LEVEL_COM
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

/** @addtogroup STM32_SGN101_LOW_LEVEL_SD_SPI
  * @{
  */
/**
  * @brief  SD SPI Interface pins
  */

#define SD_SPI                           SPI1
#define SD_SPI_CLK                       RCC_APB2Periph_SPI1
#define SD_SPI_SCK_PIN                   GPIO_Pin_3                  /* (SPI1 Remap)*/
#define SD_SPI_SCK_GPIO_PORT             GPIOB                       /* (SPI1 Remap) */
#define SD_SPI_SCK_GPIO_CLK              RCC_APB2Periph_GPIOB        /* (SPI1 Remap) */
#define SD_SPI_MISO_PIN                  GPIO_Pin_4                  /* (SPI1 Remap) */
#define SD_SPI_MISO_GPIO_PORT            GPIOB                       /* (SPI1 Remap) */
#define SD_SPI_MISO_GPIO_CLK             RCC_APB2Periph_GPIOB        /* (SPI1 Remap) */
#define SD_SPI_MOSI_PIN                  GPIO_Pin_5                  /* (SPI1 Remap) */
#define SD_SPI_MOSI_GPIO_PORT            GPIOB                       /* (SPI1 Remap) */
#define SD_SPI_MOSI_GPIO_CLK             RCC_APB2Periph_GPIOB        /* (SPI1 Remap) */
#define SD_CS_PIN                        GPIO_Pin_2
#define SD_CS_GPIO_PORT                  GPIOD
#define SD_CS_GPIO_CLK                   RCC_APB2Periph_GPIOD
// #define SD_DETECT_PIN                    0
// #define SD_DETECT_GPIO_PORT              0
// #define SD_DETECT_GPIO_CLK               0

/**
  * @}
  */
    
/**
  * @}
  */ 
  
/** @defgroup STM32_SGN101_LOW_LEVEL_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup STM32_SGN101_LOW_LEVEL_Exported_Functions
  * @{
  */ 
void STM_EVAL_LEDInit(Led_TypeDef Led);
void STM_EVAL_LEDOn(Led_TypeDef Led);
void STM_EVAL_LEDOff(Led_TypeDef Led);
void STM_EVAL_LEDToggle(Led_TypeDef Led);
void STM_EVAL_PBInit(Button_TypeDef Button, ButtonMode_TypeDef Button_Mode);
uint32_t STM_EVAL_PBGetState(Button_TypeDef Button);
void STM_EVAL_COMInit(COM_TypeDef COM, USART_InitTypeDef* USART_InitStruct);
void SD_LowLevel_DeInit(void);
void SD_LowLevel_Init(void); 
 
/**
  * @}
  */ 
    
#ifdef __cplusplus
}
#endif
  
#endif /* __STM32_SGN101_H */
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

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
