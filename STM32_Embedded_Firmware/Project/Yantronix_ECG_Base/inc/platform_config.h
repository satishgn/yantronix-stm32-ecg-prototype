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
/* Uncomment the line corresponding to the board used to run the firmware */
#if !defined (USE_STM32_PRIMER2) &&  !defined (USE_STM32_SGN101) &&  !defined (USE_STM32_SGN10X)
 // #define USE_STM32_PRIMER2 
 // #define USE_STM32_SGN101
 // #define USE_STM32_SGN10X
#endif

/* Define the STM32F10x hardware depending on the used evaluation board */
#ifdef USE_STM32_PRIMER2

  #define USE_LED
  #define USE_POWER_BUTTON
  #define USE_SYSTEM_SHUTDOWN
  #define USE_DAC_SIGNALGEN  
  #define USE_USB_HID
  #define USE_USB_DFU
  // #define USE_VBAT_MONITOR
  
#ifdef USE_VBAT_MONITOR
  #define RCC_APB2Periph_GPIO_VBAT_AIN          RCC_APB2Periph_GPIOB  
  #define GPIO_VBAT_AIN                         GPIOB
  #define GPIO_VBAT_AIN_PIN                     GPIO_Pin_1
  #define VBAT_AIN_CHANNEL                      ADC_Channel_9  
#endif  
  
#ifdef USE_DAC_SIGNALGEN 
  #define RCC_APB2Periph_GPIO_SIGNALGEN_AIN     RCC_APB2Periph_GPIOA
  #define GPIO_SIGNALGEN_AIN                    GPIOA
  #define GPIO_SIGNALGEN_AIN_PIN                GPIO_Pin_1
  #define SIGNALGEN_AIN_CHANNEL                 ADC_Channel_1
  
  #define RCC_APB2Periph_GPIO_SIGNALGEN_AOUT    RCC_APB2Periph_GPIOA
  #define GPIO_SIGNALGEN_AOUT                   GPIOA
  #define GPIO_SIGNALGEN_AOUT_PIN               GPIO_Pin_4
  #define SIGNALGEN_AOUT_CHANNEL                DAC_Channel_1  
#endif

#ifdef USE_USB_HID  
  #define RCC_APB2Periph_GPIO_DISCONNECT        RCC_APB2Periph_GPIOD
  #define USB_DISCONNECT                        GPIOD  
  #define USB_DISCONNECT_PIN                    GPIO_Pin_3
#endif

#ifdef USE_LED  
  #define RCC_APB2Periph_GPIO_LED               RCC_APB2Periph_GPIOB
  #define GPIO_LED                              GPIOB
  #define GPIO_LED_PIN                          GPIO_Pin_8    
#endif  

#ifdef USE_SYSTEM_SHUTDOWN
  #define RCC_APB2Periph_GPIO_SHUTDOWN          RCC_APB2Periph_GPIOC
  #define GPIO_SHUTDOWN                         GPIOC
  #define GPIO_SHUTDOWN_PIN                     GPIO_Pin_13 
#endif  

#ifdef USE_POWER_BUTTON
  #define RCC_APB2Periph_GPIO_POWER             RCC_APB2Periph_GPIOA
  #define GPIO_POWER                            GPIOA
  #define GPIO_POWER_PIN                        GPIO_Pin_8   
  #define GPIO_POWER_PORTSOURCE                 GPIO_PortSourceGPIOA
  #define GPIO_POWER_PINSOURCE                  GPIO_PinSource8
  #define GPIO_POWER_EXTI_Line                  EXTI_Line8
#endif

#elif defined (USE_STM32_SGN101)

  #define USE_LED
  #define USE_POWER_BUTTON
  #define USE_SYSTEM_SHUTDOWN
  #define USE_ANALOG_POWER_SWITCH
  #define USE_DAC_IAREF
  #define USE_DAC_VGND
  // #define USE_DAC_SIGNALGEN  
  #define USE_USB_HID
  #define USE_USB_DFU
  // #define USE_VBAT_MONITOR
  #define USE_ECG_MONITOR
      
#ifdef USE_VBAT_MONITOR
  #define RCC_APB2Periph_GPIO_VBAT_AIN          RCC_APB2Periph_GPIOB  
  #define GPIO_VBAT_AIN                         GPIOB
  #define GPIO_VBAT_AIN_PIN                     GPIO_Pin_0
  #define VBAT_AIN_CHANNEL                      ADC_Channel_8  
#endif  
  
#ifdef USE_ECG_MONITOR  
  #define RCC_APB2Periph_GPIO_IA_AIN            RCC_APB2Periph_GPIOA
  #define GPIO_IA_AIN                           GPIOA
  #define GPIO_IA_AIN_PIN                       GPIO_Pin_1
  #define IA_AIN_CHANNEL                        ADC_Channel_1

  #define RCC_APB2Periph_GPIO_ECG_AIN           RCC_APB2Periph_GPIOB
  #define GPIO_ECG_AIN                          GPIOB
  #define GPIO_ECG_AIN_PIN                      GPIO_Pin_1
  #define ECG_AIN_CHANNEL                       ADC_Channel_9
#endif  
  
#ifdef USE_DAC_SIGNALGEN 
  #define RCC_APB2Periph_GPIO_SIGNALGEN_AIN     RCC_APB2Periph_GPIOA
  #define GPIO_SIGNALGEN_AIN                    GPIOA
  #define GPIO_SIGNALGEN_AIN_PIN                GPIO_Pin_6
  #define SIGNALGEN_AIN_CHANNEL                 ADC_Channel_6
  
  #define RCC_APB2Periph_GPIO_SIGNALGEN_AOUT    RCC_APB2Periph_GPIOA
  #define GPIO_SIGNALGEN_AOUT                   GPIOA
  #define GPIO_SIGNALGEN_AOUT_PIN               GPIO_Pin_5
  #define SIGNALGEN_AOUT_CHANNEL                DAC_Channel_2  
#endif

#ifdef USE_DAC_IAREF
  #define RCC_APB2Periph_GPIO_IAREF_AOUT        RCC_APB2Periph_GPIOA
  #define GPIO_IAREF_AOUT                       GPIOA
  #define GPIO_IAREF_AOUT_PIN                   GPIO_Pin_5
  #define IAREF_AOUT_CHANNEL                    DAC_Channel_2  
#endif

#ifdef USE_DAC_VGND  
  #define RCC_APB2Periph_GPIO_VGND_AOUT         RCC_APB2Periph_GPIOA
  #define GPIO_VGND_AOUT                        GPIOA
  #define GPIO_VGND_AOUT_PIN                    GPIO_Pin_4
  #define VGND_AOUT_CHANNEL                     DAC_Channel_1  
#endif

#ifdef USE_USB_HID  
  #define RCC_APB2Periph_GPIO_DISCONNECT        RCC_APB2Periph_GPIOB
  #define USB_DISCONNECT                        GPIOB  
  #define USB_DISCONNECT_PIN                    GPIO_Pin_8
#endif

#ifdef USE_LED  
  #define RCC_APB2Periph_GPIO_LED               RCC_APB2Periph_GPIOA
  #define GPIO_LED                              GPIOA
  #define GPIO_LED_PIN                          GPIO_Pin_3    
#endif  

#ifdef USE_SYSTEM_SHUTDOWN
  #define RCC_APB2Periph_GPIO_SHUTDOWN          RCC_APB2Periph_GPIOC
  #define GPIO_SHUTDOWN                         GPIOC
  #define GPIO_SHUTDOWN_PIN                     GPIO_Pin_13 
#endif  

#ifdef USE_ANALOG_POWER_SWITCH
  #define RCC_APB2Periph_GPIO_ANALOG_POWER      RCC_APB2Periph_GPIOB
  #define GPIO_ANALOG_POWER                     GPIOB
  #define GPIO_ANALOG_POWER_PIN                 GPIO_Pin_2   
#endif  

#ifdef USE_POWER_BUTTON
  #define RCC_APB2Periph_GPIO_POWER             RCC_APB2Periph_GPIOA
  #define GPIO_POWER                            GPIOA 
  #define GPIO_POWER_PIN                        GPIO_Pin_0   
  #define GPIO_POWER_PORTSOURCE                 GPIO_PortSourceGPIOA
  #define GPIO_POWER_PINSOURCE                  GPIO_PinSource0
  #define GPIO_POWER_EXTI_Line                  EXTI_Line0
#endif

#elif defined (USE_STM32_SGN10X)

  #define USE_LED
  // #define USE_POWER_BUTTON
  // #define USE_SYSTEM_SHUTDOWN
  // #define USE_ANALOG_POWER_SWITCH
  #define USE_ANALOG_MUX
  #define USE_DAC_IAREF
  #define USE_DAC_VGND
  // #define USE_DAC_SIGNALGEN  
  #define USE_USB_HID
  #define USE_USB_DFU
  // #define USE_VBAT_MONITOR
  #define USE_ECG_MONITOR
      
#ifdef USE_VBAT_MONITOR
  #define RCC_APB2Periph_GPIO_VBAT_AIN          RCC_APB2Periph_GPIOB  
  #define GPIO_VBAT_AIN                         GPIOB
  #define GPIO_VBAT_AIN_PIN                     GPIO_Pin_1
  #define VBAT_AIN_CHANNEL                      ADC_Channel_9  
#endif  
  
#ifdef USE_ECG_MONITOR
  #define RCC_APB2Periph_GPIO_IA_AIN            RCC_APB2Periph_GPIOA
  #define GPIO_IA_AIN                           GPIOA
  #define GPIO_IA_AIN_PIN                       GPIO_Pin_1
  #define IA_AIN_CHANNEL                        ADC_Channel_1

  #define RCC_APB2Periph_GPIO_ECG_AIN           RCC_APB2Periph_GPIOB
  #define GPIO_ECG_AIN                          GPIOB
  #define GPIO_ECG_AIN_PIN                      GPIO_Pin_0
  #define ECG_AIN_CHANNEL                       ADC_Channel_8
#endif  
  
#ifdef USE_DAC_SIGNALGEN 
  #define RCC_APB2Periph_GPIO_SIGNALGEN_AIN     RCC_APB2Periph_GPIOA
  #define GPIO_SIGNALGEN_AIN                    GPIOA
  #define GPIO_SIGNALGEN_AIN_PIN                GPIO_Pin_6
  #define SIGNALGEN_AIN_CHANNEL                 ADC_Channel_6
  
  #define RCC_APB2Periph_GPIO_SIGNALGEN_AOUT    RCC_APB2Periph_GPIOA
  #define GPIO_SIGNALGEN_AOUT                   GPIOA
  #define GPIO_SIGNALGEN_AOUT_PIN               GPIO_Pin_5
  #define SIGNALGEN_AOUT_CHANNEL                DAC_Channel_2  
#endif

#ifdef USE_DAC_IAREF
  #define RCC_APB2Periph_GPIO_IAREF_AOUT        RCC_APB2Periph_GPIOA
  #define GPIO_IAREF_AOUT                       GPIOA
  #define GPIO_IAREF_AOUT_PIN                   GPIO_Pin_5
  #define IAREF_AOUT_CHANNEL                    DAC_Channel_2  
#endif

#ifdef USE_DAC_VGND  
  #define RCC_APB2Periph_GPIO_VGND_AOUT         RCC_APB2Periph_GPIOA
  #define GPIO_VGND_AOUT                        GPIOA
  #define GPIO_VGND_AOUT_PIN                    GPIO_Pin_4
  #define VGND_AOUT_CHANNEL                     DAC_Channel_1  
#endif

#ifdef USE_USB_HID  
  #define RCC_APB2Periph_GPIO_DISCONNECT        RCC_APB2Periph_GPIOB
  #define USB_DISCONNECT                        GPIOB  
  #define USB_DISCONNECT_PIN                    GPIO_Pin_2
#endif

#ifdef USE_ANALOG_MUX
  #define RCC_APB2Periph_GPIO_MUX_123           RCC_APB2Periph_GPIOC
  #define GPIO_MUX_123                          GPIOC
  #define GPIO_MUX_3_A_PIN                      GPIO_Pin_0 
  #define GPIO_MUX_3_B_PIN                      GPIO_Pin_1 
  #define GPIO_MUX_3_C_PIN                      GPIO_Pin_2
  #define GPIO_MUX_12_A_PIN                     GPIO_Pin_3 
  #define GPIO_MUX_12_B_PIN                     GPIO_Pin_4 
  #define GPIO_MUX_12_C_PIN                     GPIO_Pin_5
  #define GPIO_MUX_123_INH_PIN                  GPIO_Pin_6 
#endif

#ifdef USE_LED  
  #define RCC_APB2Periph_GPIO_LED               RCC_APB2Periph_GPIOB
  #define GPIO_LED                              GPIOB
  #define GPIO_LED_PIN                          GPIO_Pin_9    
#endif  

#ifdef USE_SYSTEM_SHUTDOWN
  #define RCC_APB2Periph_GPIO_SHUTDOWN          RCC_APB2Periph_GPIOC
  #define GPIO_SHUTDOWN                         GPIOC
  #define GPIO_SHUTDOWN_PIN                     GPIO_Pin_13 
#endif  

#ifdef USE_ANALOG_POWER_SWITCH
  #define RCC_APB2Periph_GPIO_ANALOG_POWER      RCC_APB2Periph_GPIOC
  #define GPIO_ANALOG_POWER                     GPIOC
  #define GPIO_ANALOG_POWER_PIN                 GPIO_Pin_13   
#endif  

#ifdef USE_POWER_BUTTON
  #define RCC_APB2Periph_GPIO_POWER             RCC_APB2Periph_GPIOC
  #define GPIO_POWER                            GPIOC
  #define GPIO_POWER_PIN                        GPIO_Pin_7   
  #define GPIO_POWER_PORTSOURCE                 GPIO_PortSourceGPIOC
  #define GPIO_POWER_PINSOURCE                  GPIO_PinSource7
  #define GPIO_POWER_EXTI_Line                  EXTI_Line7
#endif

#endif /* USE_STM32_PRIMER2 */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __PLATFORM_CONFIG_H */



