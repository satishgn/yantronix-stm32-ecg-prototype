/*****************************************************************************
* File    : @file
* Company : Yantronix
* Author  : Satish Nair
* Mailto  : satishgn77@gmail.com
*****************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "platform_config.h"
#include "dsp.h"
#include "usb_lib.h"
#include "usb_conf.h"
#include "usb_istr.h"
#include "usb_pwr.h"
#include "usb_prop.h"
#include "usb_desc.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#ifndef NULL
#define NULL ((void *)0)
#endif

/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#if defined (USE_VBAT_MONITOR) && defined (STM32F10X_HD)
// #define USE_ADC_ANALOGWATCHDOG
#endif
// #define USE_DEBUG_REPORT
// #define USE_RTC
// #define USE_DSP_FILTER

// #define USE_ADC_RESOLUTION_12_BITS
#define USE_ADC_RESOLUTION_15_BITS
// #define USE_ADC_RESOLUTION_16_BITS

#ifdef USE_ADC_RESOLUTION_12_BITS
#define ADC_RESOLUTION_BITS             12
#define ADC_DATA_MAX_VALUE              4095    //pow(2, ADC_RESOLUTION_BITS) - 1
#define ADC_SAMPLING_FREQ               500     /* 125/250/500/625/1000/1250/2000 */
#elif defined (USE_ADC_RESOLUTION_15_BITS)
#define ADC_RESOLUTION_BITS             15
#define ADC_DATA_MAX_VALUE              32767   //pow(2, ADC_RESOLUTION_BITS) - 1
#define ADC_SAMPLING_FREQ               500     /* 125/250/450/500/625/750/1000/1125/1250/1500 */
#define USE_ADC_OVERSAMPLING
#define ADC_ADDITIONAL_BITS             3
#define ADC_OVERSAMPLING_FACTOR         64      //pow(4, ADC_ADDITIONAL_BITS)
#elif defined (USE_ADC_RESOLUTION_16_BITS)
#define ADC_RESOLUTION_BITS             16
#define ADC_DATA_MAX_VALUE              65535   //pow(2, ADC_RESOLUTION_BITS) - 1
#define ADC_SAMPLING_FREQ               450     /* 125/250/450/625/750/1125/1250 */
#define USE_ADC_OVERSAMPLING
#define ADC_ADDITIONAL_BITS             4
#define ADC_OVERSAMPLING_FACTOR         256     //pow(4, ADC_ADDITIONAL_BITS)
#endif

/* ADC usage without OverSampling */
#define DATA_BUFF_SIZE                  31
#define DMA_DATA_BUFF_SIZE              2 * DATA_BUFF_SIZE

/* ADC usage with OverSampling */
#ifdef USE_ADC_OVERSAMPLING
#define OVS_DATA_BUFF_SIZE              ADC_OVERSAMPLING_FACTOR      
#define OVS_DMA_DATA_BUFF_SIZE          2 * OVS_DATA_BUFF_SIZE
#endif

/* ADC VREF Voltage */
#ifdef USE_STM32_PRIMER2
#define ADC_VREF_MV                     2880
#elif defined (USE_STM32_SGN101)
#define ADC_VREF_MV                     3087
#elif defined (USE_STM32_SGN10X)
#define ADC_VREF_MV                     3300
#endif

#define LOW_BATTERY_VOLTAGE_MV          3000
#define HIGH_BATTERY_VOLTAGE_MV         5500

#ifdef USE_ADC_ANALOGWATCHDOG
#define ANALOGWATCHDOG_HIGH_THRESHOLD   (uint16_t)((HIGH_BATTERY_VOLTAGE_MV * 4096)/(2 * ADC_VDDA_VOLTAGE_MV))
#define ANALOGWATCHDOG_LOW_THRESHOLD    (uint16_t)((LOW_BATTERY_VOLTAGE_MV * 4096)/(2 * ADC_VDDA_VOLTAGE_MV))
#endif

/* DAC use: No of sample points */
#ifdef USE_DAC_SIGNALGEN 
#define DAC_NPT                         32  //128
#endif

#ifdef USE_DAC_IAREF
#define IA_AIN_DATA_HIGH                24576
#define IA_AIN_DATA_LOW                 8192
#endif

// /* System Shutdown Time */
// #define SYSTEM_SHUTDOWN_TIME            180 //3 min    

/* USB HID INPUT REPORT IDs */
#define DEBUG_REPORT_ID                 1
#define CONFIG_REPORT_ID                2
#define DATA_REPORT_ID                  3
#define FILTER_REPORT_ID                4
/* USB HID OUTPUT REPORT IDs */
#define SHUTDOWN_CMD_REPORT_ID          20
#define LED_CMD_REPORT_ID               21
#define DFU_CMD_REPORT_ID               22
#define DAC_CMD_REPORT_ID               23
#define ADC_CMD_REPORT_ID               24
#define MUX_CMD_REPORT_ID               25
#define ECG_CMD_REPORT_ID               26                   
#define TEST_CMD_REPORT_ID              27       
#define FILTER_CMD_REPORT_ID            28

/* Exported functions ------------------------------------------------------- */
void Set_System(void);
#ifdef USE_USB_HID
void Set_USBClock(void);
void Enter_LowPowerMode(void);
void Leave_LowPowerMode(void);
void USB_Interrupts_Config(void);
void USB_Cable_Config(FunctionalState NewState);
void Get_SerialNum(void);
#endif
void NVIC_Configuration(void);
void GPIO_Configuration(void);

#ifdef USE_POWER_BUTTON
void POWER_EXTI_Configuration(FunctionalState NewState);
#endif

#ifdef USE_RTC
void RTC_Configuration(void);
#endif

void SysTick_Configuration(void);
void Delay(__IO uint32_t nCount);
// uint32_t DelayJoyStick(__IO uint32_t nTime);
void Decrement_TimingDelay(void);

#ifdef USE_USB_HID
void Device_Config_Report(void);
#ifdef USE_DEBUG_REPORT
void Device_Debug_Report(void);
#endif
#endif

#ifdef USE_VBAT_MONITOR
void ADC_Configuration(FunctionalState NewState);
#endif

#ifdef USE_ADC_ANALOGWATCHDOG
void ADC_AnalogWatchdog_Configuration(FunctionalState NewState);
#endif

#if defined (USE_ECG_MONITOR) || defined (USE_DAC_SIGNALGEN)
void ADC_DMA_TIM_Configuration(FunctionalState NewState);
void ADC_DMA_Reset_Data(void);
void ADC_DMA_Process_Data(void);
#endif

#if defined (USE_DAC_IAREF) || defined (USE_DAC_VGND)
void DAC_Configuration(FunctionalState NewState);
#endif  

#ifdef USE_DAC_SIGNALGEN 
void DAC_DMA_TIM_Configuration(FunctionalState NewState);
#endif  

#ifdef USE_VBAT_MONITOR
void Start_Battery_Monitor(void);
#endif

#ifdef USE_ANALOG_MUX
void ECG_Lead_Select(uint8_t ECG_Lead);
#endif

#ifdef USE_LED
void Led_State(FunctionalState NewState);
#endif

#ifdef USE_SYSTEM_SHUTDOWN
void System_Shutdown(void);
#endif  

#ifdef USE_ANALOG_POWER_SWITCH
void Analog_Power(FunctionalState NewState);
#endif

#ifdef USE_USB_DFU
void DFU_Start(void);
#endif

#endif  /*__HW_CONFIG_H*/


