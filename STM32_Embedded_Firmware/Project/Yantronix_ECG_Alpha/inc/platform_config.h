/*****************************************************************************
* File    : @file
* Company : Yantronix
* Author  : Satish Nair
* Mailto  : satishgn77@gmail.com
*****************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PLATFORM_CONFIG_H
#define __PLATFORM_CONFIG_H

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Uncomment the line corresponding to the STMicroelectronics evaluation board
   used to run the example */
//#if !defined (USE_STM3210B_EVAL) &&  !defined (USE_STM3210E_EVAL) &&  !defined (USE_STM32_PRIMER2) &&  !defined (USE_STM32_H103) &&  !defined (USE_STM32_VL_DISCOVERY)
#if !defined (USE_STM32_SGN101)
 #define USE_STM32_SGN101
#endif

/* Define the STM32F10x hardware depending on the used evaluation board */
#ifdef USE_STM32_SGN101
  
  // /* Add any of the following in Project->Properties->GCC compiler->Defines */
  // #define USE_SD_CARD_SPI_INTERFACE
  // #define USE_SD_CARD_SDIO_INTERFACE
 
  #define SD_CARD_DETECT
  #define USB_HOST_DETECT
  #define REGULATOR_FAULT
  // #define USE_LED
  // #define USE_BUZZER
  #define USE_ADC_OVERSAMPLING
  // #define USE_ADC_ANALOGWATCHDOG
  #define USE_DAC_VGND
  // #define USE_DAC_IA_REF
  #define USE_DAC_SIGNALGEN  
  // #define USE_IWDG 
  #define USE_ANALOG_POWER_SWITCH
  #define USE_USB_HID
  #define USE_USB_MSD
  #define USE_USB_VCP
  #define USE_USB_DFU
  
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
  #define RCC_APB2Periph_GPIO_BUZZER          0  //Optional (Via External Header Pin)
#endif
#ifdef USE_LED  
  #define RCC_APB2Periph_GPIO_LED             0  //Optional (Via External Header Pin)
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
  #define GPIO_BUZZER                         0  //Optional (Via External Header Pin)
#endif  
#ifdef USE_LED  
  #define GPIO_LED                            0  //Optional (Via External Header Pin)
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
  #define GPIO_BUZZER_PIN                     0    //Optional (Via External Header Pin)
#endif
#ifdef USE_LED  
  #define GPIO_LED1_PIN                       0    //Optional (Via External Header Pin)    
  #define GPIO_LED2_PIN                       0   //Optional (Via External Header Pin)
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

#ifdef USE_USB_HID  
  #define RCC_APB2Periph_GPIO_DISCONNECT      RCC_APB2Periph_GPIOB
  #define USB_DISCONNECT                      GPIOB  
  #define USB_DISCONNECT_PIN                  GPIO_Pin_8
#endif

  /* System shutdown port and pin */
  #define RCC_APB2Periph_GPIO_SHUTDOWN        RCC_APB2Periph_GPIOC
  #define GPIO_SHUTDOWN                       GPIOC
  #define GPIO_SHUTDOWN_PIN                   GPIO_Pin_13 

#ifdef USE_ANALOG_POWER_SWITCH
  /* Analog power enable port and pin */
  #define RCC_APB2Periph_GPIO_ANALOG_POWER    RCC_APB2Periph_GPIOB
  #define GPIO_ANALOG_POWER                   GPIOB
  #define GPIO_ANALOG_POWER_PIN               GPIO_Pin_2   
#endif  

#ifdef SD_CARD_DETECT
  /* SD Card detect port and pin */
  #define RCC_APB2Periph_GPIO_SD_CARD_DETECT  RCC_APB2Periph_GPIOA
  #define GPIO_SD_CARD_DETECT                 GPIOA
  #define GPIO_SD_CARD_DETECT_PIN             GPIO_Pin_8
  #define GPIO_SD_CARD_DETECT_PORTSOURCE      GPIO_PortSourceGPIOA
  #define GPIO_SD_CARD_DETECT_PINSOURCE       GPIO_PinSource8
  #define GPIO_SD_CARD_DETECT_EXTI_Line       EXTI_Line8    
#endif  

#ifdef USB_HOST_DETECT
  /* USB Host detect port and pin (USB_P) */
  #define RCC_APB2Periph_GPIO_USB_HOST_DETECT RCC_APB2Periph_GPIOB
  #define GPIO_USB_HOST_DETECT                GPIOB
  #define GPIO_USB_HOST_DETECT_PIN            GPIO_Pin_9   
  #define GPIO_USB_HOST_DETECT_PORTSOURCE     GPIO_PortSourceGPIOB
  #define GPIO_USB_HOST_DETECT_PINSOURCE      GPIO_PinSource9
  #define GPIO_USB_HOST_DETECT_EXTI_Line      EXTI_Line9  
#endif  

#ifdef REGULATOR_FAULT
  /* Regulator fault port and pin (!FAULT) */
  #define RCC_APB2Periph_GPIO_REGULATOR_FAULT RCC_APB2Periph_GPIOA
  #define GPIO_REGULATOR_FAULT                GPIOA
  #define GPIO_REGULATOR_FAULT_PIN            GPIO_Pin_15
  #define GPIO_REGULATOR_FAULT_PORTSOURCE     GPIO_PortSourceGPIOA
  #define GPIO_REGULATOR_FAULT_PINSOURCE      GPIO_PinSource15
  #define GPIO_REGULATOR_FAULT_EXTI_Line      EXTI_Line15    
#endif  

//#elif defined USE_STM32_XYZ
//
#endif /* USE_STM32_SGN101 */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __PLATFORM_CONFIG_H */



