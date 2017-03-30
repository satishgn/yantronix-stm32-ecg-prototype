/*****************************************************************************
* File    : @file
* Company : Yantronix
* Author  : Satish Nair
* Mailto  : satishgn77@gmail.com
*****************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F10x_IT_H
#define __STM32F10x_IT_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

#ifdef USE_RTC
void RTC_IRQHandler(void);
void RTCAlarm_IRQHandler(void);
#endif

#ifdef USE_USB_HID
#ifndef STM32F10X_CL
void USB_LP_CAN1_RX0_IRQHandler(void);
#endif /* STM32F10X_CL */

#ifdef STM32F10X_CL
void OTG_FS_IRQHandler(void);
#endif /* STM32F10X_CL */
#endif

#ifdef USE_ADC_ANALOGWATCHDOG
void ADC1_2_IRQHandler(void);
#endif

#if defined (USE_ECG_MONITOR) || defined (USE_DAC_SIGNALGEN)
void DMA1_Channel1_IRQHandler(void);
#endif

#ifdef USE_POWER_BUTTON
#ifdef USE_STM32_SGN101
void EXTI0_IRQHandler(void);
#else /* USE_STM32_PRIMER2 or USE_STM32_SGN10X */
void EXTI9_5_IRQHandler(void);
#endif
#endif
		        
#endif /* __STM32F10x_IT_H */


