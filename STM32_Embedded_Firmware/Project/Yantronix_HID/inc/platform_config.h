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

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Uncomment the line corresponding to the STMicroelectronics evaluation board
   used to run the example */
//#if !defined (USE_STM3210B_EVAL) &&  !defined (USE_STM3210E_EVAL) &&  !defined (USE_STM32_PRIMER2) &&  !defined (USE_STM32_H103) &&  !defined (USE_STM32_VL_DISCOVERY)
#if !defined (USE_STM32_PRIMER2)
 // #define USE_STM3210B_EVAL
 // #define USE_STM3210E_EVAL
 // #define USE_STM32_PRIMER2
 // #define USE_STM32_H103
 // #define USE_STM32_VL_DISCOVERY
#endif

/* Define the STM32F10x hardware depending on the used evaluation board */
#ifdef USE_STM3210B_EVAL

  #define USB_DISCONNECT                      GPIOD  
  #define USB_DISCONNECT_PIN                  GPIO_Pin_9
  #define RCC_APB2Periph_GPIO_DISCONNECT      RCC_APB2Periph_GPIOD

  #define RCC_APB2Periph_GPIO_KEY             RCC_APB2Periph_GPIOB
  #define RCC_APB2Periph_GPIO_TAMPER          RCC_APB2Periph_GPIOC
  #define RCC_APB2Periph_GPIO_IOAIN           RCC_APB2Periph_GPIOC
  #define RCC_APB2Periph_GPIO_LED             RCC_APB2Periph_GPIOC

  #define GPIO_KEY                            GPIOB
  #define GPIO_TAMPER                         GPIOC
  #define GPIO_IOAIN                          GPIOC
  #define GPIO_LED                            GPIOC

  #define GPIO_KEY_PIN                        GPIO_Pin_9 
  #define GPIO_TAMPER_PIN                     GPIO_Pin_13  
  #define GPIO_IOAIN_PIN                      GPIO_Pin_4   
  
  #define GPIO_LED1_PIN                       GPIO_Pin_6   
  #define GPIO_LED2_PIN                       GPIO_Pin_7    
  #define GPIO_LED3_PIN                       GPIO_Pin_8  
  #define GPIO_LED4_PIN                       GPIO_Pin_9   

  #define GPIO_KEY_PORTSOURCE                 GPIO_PortSourceGPIOB
  #define GPIO_KEY_PINSOURCE                  GPIO_PinSource9
  #define GPIO_KEY_EXTI_Line                  EXTI_Line9

  #define GPIO_TAMPER_PORTSOURCE              GPIO_PortSourceGPIOC
  #define GPIO_TAMPER_PINSOURCE               GPIO_PinSource13
  #define GPIO_TAMPER_EXTI_Line               EXTI_Line13

  #define ADC_AIN_CHANNEL                     ADC_Channel_14

#elif defined USE_STM3210E_EVAL

  #define USB_DISCONNECT                      GPIOB  
  #define USB_DISCONNECT_PIN                  GPIO_Pin_14
  #define RCC_APB2Periph_GPIO_DISCONNECT      RCC_APB2Periph_GPIOB

  #define RCC_APB2Periph_GPIO_KEY             RCC_APB2Periph_GPIOG
  #define RCC_APB2Periph_GPIO_TAMPER          RCC_APB2Periph_GPIOC
  #define RCC_APB2Periph_GPIO_IOAIN           RCC_APB2Periph_GPIOC
  #define RCC_APB2Periph_GPIO_LED             RCC_APB2Periph_GPIOF

  #define GPIO_KEY                            GPIOG
  #define GPIO_TAMPER                         GPIOC
  #define GPIO_IOAIN                          GPIOC
  #define GPIO_LED                            GPIOF

  #define GPIO_KEY_PIN                        GPIO_Pin_8   
  #define GPIO_TAMPER_PIN                     GPIO_Pin_13  
  #define GPIO_IOAIN_PIN                      GPIO_Pin_4   
  
  #define GPIO_LED1_PIN                       GPIO_Pin_6   
  #define GPIO_LED2_PIN                       GPIO_Pin_7   
  #define GPIO_LED3_PIN                       GPIO_Pin_8   
  #define GPIO_LED4_PIN                       GPIO_Pin_9    

  #define GPIO_KEY_PORTSOURCE                 GPIO_PortSourceGPIOG
  #define GPIO_KEY_PINSOURCE                  GPIO_PinSource8
  #define GPIO_KEY_EXTI_Line                  EXTI_Line8

  #define GPIO_TAMPER_PORTSOURCE              GPIO_PortSourceGPIOC
  #define GPIO_TAMPER_PINSOURCE               GPIO_PinSource13
  #define GPIO_TAMPER_EXTI_Line               EXTI_Line13

  #define ADC_AIN_CHANNEL                     ADC_Channel_14

#elif defined USE_STM32_PRIMER2

  // /* Add any of the following in Project->Properties->GCC compiler->Defines */
  // #define USE_SD_CARD_SPI_INTERFACE
  // #define USE_SD_CARD_SDIO_INTERFACE
  
  #define USE_LED
  // #define USE_BUZZER
  #define USE_ADC_OVERSAMPLING
  // #define USE_ADC_ANALOGWATCHDOG
  // #define USE_DAC_VGND
  #define USE_DAC_IA_REF
  // #define USE_DAC_SIGNALGEN  
  #define USE_IWDG 
  #define USE_ANALOG_POWER_SWITCH
  #define USE_USB_HID
  #define USE_USB_MSD
  #define USE_USB_VCP
  #define USE_USB_DFU
  
#ifdef USE_USB_HID  
  #define USB_DISCONNECT                      GPIOD  
  #define USB_DISCONNECT_PIN                  GPIO_Pin_3
  #define RCC_APB2Periph_GPIO_DISCONNECT      RCC_APB2Periph_GPIOD
#endif

  #define RCC_APB2Periph_GPIO_POWER           RCC_APB2Periph_GPIOE
  #define RCC_APB2Periph_GPIO_MENU            RCC_APB2Periph_GPIOE
  #define RCC_APB2Periph_GPIO_SEL             RCC_APB2Periph_GPIOA
  #define RCC_APB2Periph_GPIO_UP              RCC_APB2Periph_GPIOE
  #define RCC_APB2Periph_GPIO_DOWN            RCC_APB2Periph_GPIOE
  #define RCC_APB2Periph_GPIO_VBAT_AIN        RCC_APB2Periph_GPIOB  
  #define RCC_APB2Periph_GPIO_ECG_AIN         RCC_APB2Periph_GPIOA
#ifdef USE_DAC_SIGNALGEN 
  #define RCC_APB2Periph_GPIO_SIGNALGEN_AIN   RCC_APB2Periph_GPIOA
#endif  
#ifdef USE_DAC_VGND  
  #define RCC_APB2Periph_GPIO_VGND_AOUT       RCC_APB2Periph_GPIOA  
#endif  
#ifdef USE_DAC_IA_REF  
  #define RCC_APB2Periph_GPIO_IA_REF_AOUT     RCC_APB2Periph_GPIOA  
#endif  
#ifdef USE_DAC_SIGNALGEN 
  #define RCC_APB2Periph_GPIO_SIGNALGEN_AOUT  RCC_APB2Periph_GPIOA  
#endif 
#ifdef USE_BUZZER
  #define RCC_APB2Periph_GPIO_BUZZER          0
#endif
#ifdef USE_LED  
  #define RCC_APB2Periph_GPIO_LED             RCC_APB2Periph_GPIOE
#endif  

  #define GPIO_POWER                          GPIOE 
  #define GPIO_MENU                           GPIOE  
  #define GPIO_SEL                            GPIOA  
  #define GPIO_UP                             GPIOE  
  #define GPIO_DOWN                           GPIOE
  #define GPIO_VBAT_AIN                       GPIOB
  #define GPIO_ECG_AIN                        GPIOA
#ifdef USE_DAC_SIGNALGEN 
  #define GPIO_SIGNALGEN_AIN                  GPIOA
#endif  
#ifdef USE_DAC_VGND  
  #define GPIO_VGND_AOUT                      GPIOA
#endif    
#ifdef USE_DAC_IA_REF  
  #define GPIO_IA_REF_AOUT                    GPIOA
#endif    
#ifdef USE_DAC_SIGNALGEN 
  #define GPIO_SIGNALGEN_AOUT                 GPIOA
#endif  
#ifdef USE_BUZZER
  #define GPIO_BUZZER                         0
#endif  
#ifdef USE_LED  
  #define GPIO_LED                            GPIOE
#endif

  #define GPIO_Pin_POWER                      GPIO_Pin_3   
  #define GPIO_Pin_MENU                       GPIO_Pin_4 
  #define GPIO_Pin_SEL                        GPIO_Pin_8  
  #define GPIO_Pin_UP                         GPIO_Pin_6  
  #define GPIO_Pin_DOWN                       GPIO_Pin_5 
  #define GPIO_VBAT_AIN_PIN                   GPIO_Pin_1
  #define GPIO_ECG_AIN_PIN                    GPIO_Pin_1
#ifdef USE_DAC_SIGNALGEN 
  #define GPIO_SIGNALGEN_AIN_PIN              GPIO_Pin_1
#endif  
#ifdef USE_DAC_VGND  
  #define GPIO_VGND_AOUT_PIN                  GPIO_Pin_4
#endif      
#ifdef USE_DAC_IA_REF  
  #define GPIO_IA_REF_AOUT_PIN                GPIO_Pin_4
#endif      
#ifdef USE_DAC_SIGNALGEN 
  #define GPIO_SIGNALGEN_AOUT_PIN             GPIO_Pin_4
#endif  
#ifdef USE_BUZZER
  #define GPIO_BUZZER_PIN                     0
#endif
#ifdef USE_LED  
  #define GPIO_LED1_PIN                       GPIO_Pin_0  
  #define GPIO_LED2_PIN                       GPIO_Pin_1
#endif

  #define GPIO_POWER_PORTSOURCE               GPIO_PortSourceGPIOE
  #define GPIO_POWER_PINSOURCE                GPIO_PinSource3
  #define GPIO_POWER_EXTI_Line                EXTI_Line3

  #define GPIO_MENU_PORTSOURCE                GPIO_PortSourceGPIOE
  #define GPIO_MENU_PINSOURCE                 GPIO_PinSource4
  #define GPIO_MENU_EXTI_Line                 EXTI_Line4

  #define GPIO_SEL_PORTSOURCE                 GPIO_PortSourceGPIOA
  #define GPIO_SEL_PINSOURCE                  GPIO_PinSource8
  #define GPIO_SEL_EXTI_Line                  EXTI_Line8

  #define GPIO_UP_PORTSOURCE                  GPIO_PortSourceGPIOE
  #define GPIO_UP_PINSOURCE                   GPIO_PinSource6
  #define GPIO_UP_EXTI_Line                   EXTI_Line6

  #define GPIO_DOWN_PORTSOURCE                GPIO_PortSourceGPIOE
  #define GPIO_DOWN_PINSOURCE                 GPIO_PinSource5
  #define GPIO_DOWN_EXTI_Line                 EXTI_Line5

  #define VBAT_AIN_CHANNEL                    ADC_Channel_9
  #define ECG_AIN_CHANNEL                     ADC_Channel_1
#ifdef USE_DAC_SIGNALGEN 
  #define SIGNALGEN_AIN_CHANNEL               ADC_Channel_1
#endif  
#ifdef USE_DAC_VGND  
  #define VGND_AOUT_CHANNEL                   DAC_Channel_1
#endif      
#ifdef USE_DAC_IA_REF  
  #define IA_REF_AOUT_CHANNEL                 DAC_Channel_1
#endif      
#ifdef USE_DAC_SIGNALGEN 
  #define SIGNALGEN_AOUT_CHANNEL              DAC_Channel_1
#endif  
  
  /* System shutdown port and pin */
  #define GPIO_SHUTDOWN                       GPIOC
  #define RCC_APB2Periph_GPIO_SHUTDOWN        RCC_APB2Periph_GPIOC
  #define GPIO_SHUTDOWN_PIN                   GPIO_Pin_13 

#ifdef USE_ANALOG_POWER_SWITCH
  /* Analog power enable port and pin */
  #define GPIO_ANALOG_POWER                   GPIOB
  #define RCC_APB2Periph_GPIO_ANALOG_POWER    RCC_APB2Periph_GPIOB
  #define GPIO_ANALOG_POWER_PIN               GPIO_Pin_6   
#endif  
 
#elif defined USE_STM32_H103

  #define USB_DISCONNECT                      GPIOC  
  #define USB_DISCONNECT_PIN                  GPIO_Pin_11
  #define RCC_APB2Periph_GPIO_DISCONNECT      RCC_APB2Periph_GPIOC

  // #define RCC_APB2Periph_GPIO_LEFT            0
  // #define RCC_APB2Periph_GPIO_RIGHT           0
  #define RCC_APB2Periph_GPIO_DOWN            RCC_APB2Periph_GPIOC
  #define RCC_APB2Periph_GPIO_UP              RCC_APB2Periph_GPIOC
  #define RCC_APB2Periph_GPIO_SEL             RCC_APB2Periph_GPIOC
  #define RCC_APB2Periph_GPIO_IOAIN           RCC_APB2Periph_GPIOC
  // #define RCC_APB2Periph_GPIO_IOAOUT          RCC_APB2Periph_GPIOA  
  #define RCC_APB2Periph_GPIO_LED             RCC_APB2Periph_GPIOC

  // #define GPIO_LEFT                           0
  // #define GPIO_RIGHT                          0  
  #define GPIO_DOWN                           GPIOC
  #define GPIO_UP                             GPIOC  
  #define GPIO_SEL                            GPIOC  
  #define GPIO_IOAIN                          GPIOC
  // #define GPIO_IOAOUT                         GPIOA
  #define GPIO_LED                            GPIOC

  // #define GPIO_Pin_LEFT                       0
  // #define GPIO_Pin_RIGHT                      0   
  #define GPIO_Pin_DOWN                       GPIO_Pin_8
  #define GPIO_Pin_UP                         GPIO_Pin_6   
  #define GPIO_Pin_SEL                        GPIO_Pin_7   
  #define GPIO_IOAIN1_PIN                     GPIO_Pin_5    //ADC Channel 15
  // #define GPIO_IOAIN2_PIN                     GPIO_Pin_4    //ADC Channel 14 
  // #define GPIO_IOAOUT1_PIN                    GPIO_Pin_4
  
  #define GPIO_LED1_PIN                       0   
  #define GPIO_LED2_PIN                       0    
  #define GPIO_LED3_PIN                       0  
  #define GPIO_LED4_PIN                       0    

  // #define GPIO_LEFT_PORTSOURCE                0
  // #define GPIO_LEFT_PINSOURCE                 0
  // #define GPIO_LEFT_EXTI_Line                 0

  // #define GPIO_RIGHT_PORTSOURCE               0
  // #define GPIO_RIGHT_PINSOURCE                0
  // #define GPIO_RIGHT_EXTI_Line                0

  #define GPIO_DOWN_PORTSOURCE                GPIO_PortSourceGPIOC
  #define GPIO_DOWN_PINSOURCE                 GPIO_PinSource8
  #define GPIO_DOWN_EXTI_Line                 EXTI_Line8

  #define GPIO_UP_PORTSOURCE                  GPIO_PortSourceGPIOC
  #define GPIO_UP_PINSOURCE                   GPIO_PinSource6
  #define GPIO_UP_EXTI_Line                   EXTI_Line6

  #define GPIO_SEL_PORTSOURCE                 GPIO_PortSourceGPIOC
  #define GPIO_SEL_PINSOURCE                  GPIO_PinSource7
  #define GPIO_SEL_EXTI_Line                  EXTI_Line7

  #define ADC_AIN1_CHANNEL                    ADC_Channel_15 //ADC_Channel_14
  // #define ADC_AIN2_CHANNEL                    ADC_Channel_14 //ADC_Channel_15

  //Shutdown port and pin
  #define GPIO_SHUTDOWN                       GPIOC
  #define RCC_APB2Periph_GPIO_SHUTDOWN        RCC_APB2Periph_GPIOC
  #define GPIO_SHUTDOWN_PIN                   GPIO_Pin_13  
  
#elif defined USE_STM32_VL_DISCOVERY

  // /* Add any of the following in Project->Properties->GCC compiler->Defines */
  // #define USE_SD_CARD_SPI_INTERFACE
  // #define USE_SD_CARD_SDIO_INTERFACE
  
  #define USE_LED
  #define USE_BUZZER
  #define USE_ADC_OVERSAMPLING
  // #define USE_ADC_ANALOGWATCHDOG
  // #define USE_DAC_VGND
  // #define USE_DAC_IA_REF
  #define USE_DAC_SIGNALGEN  
  #define USE_IWDG 
  // #define USE_ANALOG_POWER_SWITCH
  // #define USE_USB_HID
  // #define USE_USB_MSD
  // #define USE_USB_VCP
  // #define USE_USB_DFU
  
#ifdef USE_USB_HID  
  #define USB_DISCONNECT                      GPIOB  
  #define USB_DISCONNECT_PIN                  GPIO_Pin_8
  #define RCC_APB2Periph_GPIO_DISCONNECT      RCC_APB2Periph_GPIOB
#endif

  #define RCC_APB2Periph_GPIO_POWER           RCC_APB2Periph_GPIOA
  #define RCC_APB2Periph_GPIO_MENU            RCC_APB2Periph_GPIOA
  #define RCC_APB2Periph_GPIO_SEL             RCC_APB2Periph_GPIOA
  #define RCC_APB2Periph_GPIO_UP              RCC_APB2Periph_GPIOB
  #define RCC_APB2Periph_GPIO_DOWN            RCC_APB2Periph_GPIOB
  #define RCC_APB2Periph_GPIO_VBAT_AIN        RCC_APB2Periph_GPIOB  
  #define RCC_APB2Periph_GPIO_ECG_AIN         RCC_APB2Periph_GPIOB
#ifdef USE_DAC_SIGNALGEN 
  #define RCC_APB2Periph_GPIO_SIGNALGEN_AIN   RCC_APB2Periph_GPIOA
#endif  
#ifdef USE_DAC_VGND  
  #define RCC_APB2Periph_GPIO_VGND_AOUT       RCC_APB2Periph_GPIOA  
#endif  
#ifdef USE_DAC_IA_REF  
  #define RCC_APB2Periph_GPIO_IA_REF_AOUT     RCC_APB2Periph_GPIOA  
#endif  
#ifdef USE_DAC_SIGNALGEN 
  #define RCC_APB2Periph_GPIO_SIGNALGEN_AOUT  RCC_APB2Periph_GPIOA  
#endif 
#ifdef USE_BUZZER
  #define RCC_APB2Periph_GPIO_BUZZER          RCC_APB2Periph_GPIOA  //Optional (Via External Header Pin)
#endif
#ifdef USE_LED  
  #define RCC_APB2Periph_GPIO_LED             RCC_APB2Periph_GPIOC  //Optional (Via External Header Pin)
#endif  

  #define GPIO_POWER                          GPIOA 
  #define GPIO_MENU                           GPIOA  
  #define GPIO_SEL                            GPIOA  
  #define GPIO_UP                             GPIOB  
  #define GPIO_DOWN                           GPIOB
  #define GPIO_VBAT_AIN                       GPIOB
  #define GPIO_ECG_AIN                        GPIOB
#ifdef USE_DAC_SIGNALGEN 
  #define GPIO_SIGNALGEN_AIN                  GPIOA
#endif  
#ifdef USE_DAC_VGND  
  #define GPIO_VGND_AOUT                      GPIOA
#endif    
#ifdef USE_DAC_IA_REF  
  #define GPIO_IA_REF_AOUT                    GPIOA
#endif    
#ifdef USE_DAC_SIGNALGEN 
  #define GPIO_SIGNALGEN_AOUT                 GPIOA
#endif  
#ifdef USE_BUZZER
  #define GPIO_BUZZER                         GPIOA  //Optional (Via External Header Pin)
#endif  
#ifdef USE_LED  
  #define GPIO_LED                            GPIOC  //Optional (Via External Header Pin)
#endif

  #define GPIO_Pin_POWER                      GPIO_Pin_0   
  #define GPIO_Pin_MENU                       GPIO_Pin_1 
  #define GPIO_Pin_SEL                        GPIO_Pin_2  
  #define GPIO_Pin_UP                         GPIO_Pin_7  
  #define GPIO_Pin_DOWN                       GPIO_Pin_6 
  #define GPIO_VBAT_AIN_PIN                   GPIO_Pin_0
  #define GPIO_ECG_AIN_PIN                    GPIO_Pin_1
#ifdef USE_DAC_SIGNALGEN 
  #define GPIO_SIGNALGEN_AIN_PIN              GPIO_Pin_6
#endif  
#ifdef USE_DAC_VGND  
  #define GPIO_VGND_AOUT_PIN                  GPIO_Pin_4
#endif      
#ifdef USE_DAC_IA_REF  
  #define GPIO_IA_REF_AOUT_PIN                GPIO_Pin_4
#endif      
#ifdef USE_DAC_SIGNALGEN 
  #define GPIO_SIGNALGEN_AOUT_PIN             GPIO_Pin_5
#endif  
#ifdef USE_BUZZER
  #define GPIO_BUZZER_PIN                     GPIO_Pin_8    //Optional (Via External Header Pin)
#endif
#ifdef USE_LED  
  #define GPIO_LED1_PIN                       GPIO_Pin_9    //Optional (Via External Header Pin)    
  #define GPIO_LED2_PIN                       GPIO_Pin_10   //Optional (Via External Header Pin)
#endif

  #define GPIO_POWER_PORTSOURCE               GPIO_PortSourceGPIOA
  #define GPIO_POWER_PINSOURCE                GPIO_PinSource0
  #define GPIO_POWER_EXTI_Line                EXTI_Line0

  #define GPIO_MENU_PORTSOURCE                GPIO_PortSourceGPIOA
  #define GPIO_MENU_PINSOURCE                 GPIO_PinSource1
  #define GPIO_MENU_EXTI_Line                 EXTI_Line1

  #define GPIO_SEL_PORTSOURCE                 GPIO_PortSourceGPIOA
  #define GPIO_SEL_PINSOURCE                  GPIO_PinSource2
  #define GPIO_SEL_EXTI_Line                  EXTI_Line2

  #define GPIO_UP_PORTSOURCE                  GPIO_PortSourceGPIOB
  #define GPIO_UP_PINSOURCE                   GPIO_PinSource7
  #define GPIO_UP_EXTI_Line                   EXTI_Line7

  #define GPIO_DOWN_PORTSOURCE                GPIO_PortSourceGPIOB
  #define GPIO_DOWN_PINSOURCE                 GPIO_PinSource6
  #define GPIO_DOWN_EXTI_Line                 EXTI_Line6

  #define VBAT_AIN_CHANNEL                    ADC_Channel_8
  #define ECG_AIN_CHANNEL                     ADC_Channel_9
#ifdef USE_DAC_SIGNALGEN 
  #define SIGNALGEN_AIN_CHANNEL               ADC_Channel_6
#endif  
#ifdef USE_DAC_VGND  
  #define VGND_AOUT_CHANNEL                   DAC_Channel_1
#endif      
#ifdef USE_DAC_IA_REF  
  #define IA_REF_AOUT_CHANNEL                 DAC_Channel_1
#endif      
#ifdef USE_DAC_SIGNALGEN 
  #define SIGNALGEN_AOUT_CHANNEL              DAC_Channel_2
#endif  
  
  /* System shutdown port and pin */
  #define GPIO_SHUTDOWN                       GPIOC
  #define RCC_APB2Periph_GPIO_SHUTDOWN        RCC_APB2Periph_GPIOC
  #define GPIO_SHUTDOWN_PIN                   GPIO_Pin_13 

#ifdef USE_ANALOG_POWER_SWITCH
  /* Analog power enable port and pin */
  #define GPIO_ANALOG_POWER                   GPIOB
  #define RCC_APB2Periph_GPIO_ANALOG_POWER    RCC_APB2Periph_GPIOB
  #define GPIO_ANALOG_POWER_PIN               GPIO_Pin_2   
#endif  

#endif /* USE_STM3210B_EVAL */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __PLATFORM_CONFIG_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

