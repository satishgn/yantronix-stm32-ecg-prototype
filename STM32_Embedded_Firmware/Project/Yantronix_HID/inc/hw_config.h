/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
* File Name          : hw_config.h
* Author             : MCD Application Team
* Version            : V3.3.0
* Date               : 21-March-2011
* Description        : Hardware Configuration & Setup
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "platform_config.h"
#include "ff.h"
#include "stm32_eval.h"
#ifdef USE_STM32100B_EVAL
 #include "stm32100b_eval_lcd.h"
#elif defined USE_STM3210B_EVAL
 #include "stm3210b_eval_lcd.h"
#elif defined USE_STM3210E_EVAL
 #include "stm3210e_eval_lcd.h"
#elif defined USE_STM3210C_EVAL
 #include "stm3210c_eval_lcd.h"
#elif defined USE_STM32_PRIMER2
 #include "stm32_primer2_lcd.h"
#elif defined USE_STM32_H103
 #include "stm32_st7565_lcd.h"
#elif defined USE_STM32_VL_DISCOVERY
 #include "stm32_st7565_lcd.h"
#endif
#include "menu.h"
#include "calendar.h"
#include "lowpower.h"
#include "dsp.h"
// #ifdef USE_USB_HID
#include "usb_lib.h"
#include "usb_conf.h"
#include "usb_istr.h"
#include "usb_pwr.h"
#include "usb_prop.h"
#include "usb_desc.h"
// #endif

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#ifndef NULL
#define NULL ((void *)0)
#endif

/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define ADC_SAMPLING_FREQ_125           125
#define ADC_SAMPLING_FREQ_250           250
#define ADC_SAMPLING_FREQ_500           500
#define ADC_SAMPLING_FREQ_1000          1000

#define DATA_CHANNELS                   1

/* ADC usage without OverSampling */
#define DATA_BUFF_SIZE                  DATA_CHANNELS
#define DMA_DATA_BUFF_SIZE              2 * DATA_BUFF_SIZE

/* ADC usage with OverSampling */
#ifdef USE_ADC_OVERSAMPLING
#define ADC_ADDITIONAL_BITS             3
#define ADC_OVERSAMPLING_FACTOR         64  //pow(4, ADC_ADDITIONAL_BITS)
#define OVS_DATA_BUFF_SIZE              DATA_CHANNELS * ADC_OVERSAMPLING_FACTOR      
#define OVS_DMA_DATA_BUFF_SIZE          2 * OVS_DATA_BUFF_SIZE
#ifdef USE_DAC_IA_REF  
#define OVS_DATA_UPPER_THRESHOLD        24576   //(pow(2, 15) / 4) * 3
#define OVS_DATA_LOWER_THRESHOLD        8192    //(pow(2, 15) / 4)
#endif
#endif

/* ADC usage for Battery monitoring */
#define ADC_VDDA_VOLTAGE_MV             2880
#define LOW_BATTERY_VOLTAGE_MV          3750
#define HIGH_BATTERY_VOLTAGE_MV         4200

#ifdef USE_ADC_ANALOGWATCHDOG
#define ANALOGWATCHDOG_HIGH_THRESHOLD   (uint16_t)((HIGH_BATTERY_VOLTAGE_MV * 4096)/(2 * ADC_VDDA_VOLTAGE_MV))
#define ANALOGWATCHDOG_LOW_THRESHOLD    (uint16_t)((LOW_BATTERY_VOLTAGE_MV * 4096)/(2 * ADC_VDDA_VOLTAGE_MV))
#endif

/* DAC use: No of sample points */
#ifdef USE_DAC_SIGNALGEN 
#define DAC_NPT                         32  //128
#endif

/* QRS/HR usage */
// #define QRS_THRESHOLD_LCD_DISPLAY 
#ifdef USE_LED  
#define QRS_DETECT_LED_OUT
#endif
#ifdef USE_BUZZER
#define QRS_DETECT_BUZZER_OUT
#endif

// #ifdef USE_USB_HID
/* USB HID INPUT REPORT IDs */
#define DATA_REPORT_ID                  2   //0x02
#define FILTER_DATA_REPORT_ID           10  //0x0A
/* USB HID OUTPUT REPORT IDs */
#define SHUTDOWN_REPORT_ID              20  //0x14
#define LED_REPORT_ID                   21  //0x15
// #endif

/* Exported functions ------------------------------------------------------- */
void Set_System(void);
#ifdef USE_USB_HID
void Set_USBClock(void);
void Enter_LowPowerMode(void);
void Leave_LowPowerMode(void);
void USB_Interrupts_Config(void);
void USB_Cable_Config(FunctionalState NewState);
#endif
void NVIC_Configuration(void);
void GPIO_Configuration(void);
void EXTI_Configuration(void);
void IntExtOnOffConfig(FunctionalState NewState);
// void RTC_Configuration(void);
void SysTick_Configuration(void);
void Delay(__IO uint32_t nCount);
// uint32_t DelayJoyStick(__IO uint32_t nTime);
void Decrement_TimingDelay(void);

void ADC_Configuration(FunctionalState NewState);
void ADC_DMA_TIM_Configuration(FunctionalState NewState);
void MEM_DMA_TIM_Configuration(FunctionalState NewState);

#if defined (USE_DAC_VGND) || defined (USE_DAC_IA_REF)
void DAC_Configuration(FunctionalState NewState);
#endif  
#ifdef USE_DAC_SIGNALGEN 
void DAC_DMA_TIM_Configuration(FunctionalState NewState);
#endif  

void Shutdown_System(void);
#ifdef USE_ANALOG_POWER_SWITCH
void Analog_Power(FunctionalState NewState);
#endif
void EnterAutoShutdownMode_RTCAlarm(uint32_t AlarmTime);
#ifdef USE_IWDG 
void Enable_IWDG(void);
#endif

#ifdef USE_USB_HID
void Get_SerialNum(void);
#endif

#ifdef USE_USB_DFU
void DFU_Start(void);
#endif
#ifdef USE_USB_MSD
void MSD_Start(void);
#endif
#ifdef USE_USB_VCP
void VCP_Start(void);
#endif

void Test_FatFs(void);

#ifdef USE_DAC_SIGNALGEN 
void Start_SIGNALGEN_Data_Acquisition(void);
#endif
void Start_ECG_Data_Acquisition(void);
void Play_Log_File(uint8_t *logFile);
void Browse_Log_Files(void);
void Delete_All_Log_Files(void);
void Draw_LCD_Trace(uint16_t *dataBuffer);
void Detect_Display_HR(uint16_t dataSample);
void Start_Battery_Monitor(void);

#endif  /*__HW_CONFIG_H*/

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
