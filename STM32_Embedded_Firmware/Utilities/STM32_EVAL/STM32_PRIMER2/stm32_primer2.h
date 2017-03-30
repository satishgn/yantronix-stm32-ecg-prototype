/*****************************************************************************
* File    : @file
* Company : Yantronix
* Author  : Satish Nair
* Mailto  : satishgn77@gmail.com
*****************************************************************************/
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32_PRIMER2_H
#define __STM32_PRIMER2_H

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
  
/** @addtogroup STM32_PRIMER2
  * @{
  */ 

/** @addtogroup STM32_PRIMER2_LOW_LEVEL
  * @{
  */ 
  
/** @defgroup STM32_PRIMER2_LOW_LEVEL_Exported_Types
  * @{
  */
/**
  * @}
  */ 

/** @defgroup STM32_PRIMER2_LOW_LEVEL_Exported_Constants
  * @{
  */ 
/** @addtogroup STM32_PRIMER2_LOW_LEVEL_LED
  * @{
  */
#define LEDn                             4

#define LED1_PIN                         GPIO_Pin_0
#define LED1_GPIO_PORT                   GPIOE
#define LED1_GPIO_CLK                    RCC_APB2Periph_GPIOE  
  
#define LED2_PIN                         GPIO_Pin_1
#define LED2_GPIO_PORT                   GPIOE
#define LED2_GPIO_CLK                    RCC_APB2Periph_GPIOE  

#define LED3_PIN                         0  
#define LED3_GPIO_PORT                   0
#define LED3_GPIO_CLK                    0  

#define LED4_PIN                         0
#define LED4_GPIO_PORT                   0
#define LED4_GPIO_CLK                    0

/**
  * @}
  */
  
/** @addtogroup STM32_PRIMER2_LOW_LEVEL_BUTTON
  * @{
  */  
#define BUTTONn                          8

/**
 * @brief Wakeup push-button
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
 * @brief Key push-button
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
#define RIGHT_BUTTON_PIN                 GPIO_Pin_3             //GPIO_Pin_5
#define RIGHT_BUTTON_GPIO_PORT           GPIOE
#define RIGHT_BUTTON_GPIO_CLK            RCC_APB2Periph_GPIOE
#define RIGHT_BUTTON_EXTI_LINE           EXTI_Line3             //EXTI_Line5
#define RIGHT_BUTTON_EXTI_PORT_SOURCE    RCC_APB2Periph_GPIOE
#define RIGHT_BUTTON_EXTI_PIN_SOURCE     GPIO_PinSource3        //GPIO_PinSource5
#define RIGHT_BUTTON_EXTI_IRQn           EXTI3_IRQn             //EXTI9_5_IRQn
/**
 * @brief Joystick Left push-button
 */    
#define LEFT_BUTTON_PIN                  GPIO_Pin_4             //GPIO_Pin_6
#define LEFT_BUTTON_GPIO_PORT            GPIOE
#define LEFT_BUTTON_GPIO_CLK             RCC_APB2Periph_GPIOE
#define LEFT_BUTTON_EXTI_LINE            EXTI_Line4             //EXTI_Line6
#define LEFT_BUTTON_EXTI_PORT_SOURCE     RCC_APB2Periph_GPIOE
#define LEFT_BUTTON_EXTI_PIN_SOURCE      GPIO_PinSource4        //GPIO_PinSource6
#define LEFT_BUTTON_EXTI_IRQn            EXTI4_IRQn             //EXTI9_5_IRQn  
/**
 * @brief Joystick Up push-button
 */
#define UP_BUTTON_PIN                    GPIO_Pin_6             //GPIO_Pin_3
#define UP_BUTTON_GPIO_PORT              GPIOE
#define UP_BUTTON_GPIO_CLK               RCC_APB2Periph_GPIOE
#define UP_BUTTON_EXTI_LINE              EXTI_Line6             //EXTI_Line3
#define UP_BUTTON_EXTI_PORT_SOURCE       RCC_APB2Periph_GPIOE
#define UP_BUTTON_EXTI_PIN_SOURCE        GPIO_PinSource6        //GPIO_PinSource3
#define UP_BUTTON_EXTI_IRQn              EXTI9_5_IRQn           //EXTI3_IRQn  
/**
 * @brief Joystick Down push-button
 */   
#define DOWN_BUTTON_PIN                  GPIO_Pin_5             //GPIO_Pin_4
#define DOWN_BUTTON_GPIO_PORT            GPIOE
#define DOWN_BUTTON_GPIO_CLK             RCC_APB2Periph_GPIOE
#define DOWN_BUTTON_EXTI_LINE            EXTI_Line5             //EXTI_Line4
#define DOWN_BUTTON_EXTI_PORT_SOURCE     RCC_APB2Periph_GPIOE
#define DOWN_BUTTON_EXTI_PIN_SOURCE      GPIO_PinSource5        //GPIO_PinSource4
#define DOWN_BUTTON_EXTI_IRQn            EXTI9_5_IRQn           //EXTI4_IRQn  
/**
 * @brief Joystick Sel push-button
 */  
#define SEL_BUTTON_PIN                   GPIO_Pin_8
#define SEL_BUTTON_GPIO_PORT             GPIOA
#define SEL_BUTTON_GPIO_CLK              RCC_APB2Periph_GPIOA
#define SEL_BUTTON_EXTI_LINE             EXTI_Line8
#define SEL_BUTTON_EXTI_PORT_SOURCE      RCC_APB2Periph_GPIOA
#define SEL_BUTTON_EXTI_PIN_SOURCE       GPIO_PinSource8
#define SEL_BUTTON_EXTI_IRQn             EXTI9_5_IRQn          
/**
  * @}
  */ 

/** @addtogroup STM32_PRIMER2_LOW_LEVEL_COM
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
#define EVAL_COM2                        USART2
#define EVAL_COM2_CLK                    RCC_APB1Periph_USART2
#define EVAL_COM2_TX_PIN                 GPIO_Pin_2
#define EVAL_COM2_TX_GPIO_PORT           GPIOA
#define EVAL_COM2_TX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define EVAL_COM2_RX_PIN                 GPIO_Pin_3
#define EVAL_COM2_RX_GPIO_PORT           GPIOA
#define EVAL_COM2_RX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define EVAL_COM2_IRQn                   USART2_IRQn

/**
  * @}
  */ 

/** @addtogroup STM32_PRIMER2_LOW_LEVEL_SD_FLASH
  * @{
  */
/**
  * @brief  SD FLASH SDIO Interface
  */ 

// #define SD_DETECT_PIN                    0
// #define SD_DETECT_GPIO_PORT              0
// #define SD_DETECT_GPIO_CLK               0

#define SDIO_FIFO_ADDRESS                ((uint32_t)0x40018080)
/** 
  * @brief  SDIO Intialization Frequency (400KHz max)
  */
#define SDIO_INIT_CLK_DIV                ((uint8_t)0xB2)
/** 
  * @brief  SDIO Data Transfer Frequency (25MHz max) 
  */
#define SDIO_TRANSFER_CLK_DIV            ((uint8_t)0x1) 

/**
  * @}
  */ 
  
/**
  * @}
  */
  
/** @defgroup STM32_PRIMER2_LOW_LEVEL_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup STM32_PRIMER2_LOW_LEVEL_Exported_Functions
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
void SD_LowLevel_DMA_TxConfig(uint32_t *BufferSRC, uint32_t BufferSize);
void SD_LowLevel_DMA_RxConfig(uint32_t *BufferDST, uint32_t BufferSize);
uint32_t SD_DMAEndOfTransferStatus(void);

/**
  * @}
  */
#ifdef __cplusplus
}
#endif
  
#endif /* __STM32_PRIMER2_H */
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
  

