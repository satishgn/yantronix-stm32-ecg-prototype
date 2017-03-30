/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
* File Name          : usb_endp.c
* Author             : MCD Application Team
* Version            : V3.3.0
* Date               : 21-March-2011
* Description        : Endpoint routines
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint8_t Receive_Buffer[64];
extern __IO uint8_t PrevXferComplete;

/* External variables */
#if defined (USE_ECG_MONITOR) || defined (USE_DAC_SIGNALGEN)
extern uint8_t ADC_Channel;
#ifdef USE_DSP_FILTER
extern uint16_t FILTER_ENABLED;
#endif
#endif

#ifdef USE_ANALOG_MUX
extern uint8_t ECG_Lead;
#endif

#ifdef USE_DAC_IAREF
extern __IO uint16_t DAC_IARef_Data;
#endif

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : EP1_OUT_Callback.
* Description    : EP1 OUT Callback Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP1_OUT_Callback(void)
{
  BitAction Bit_State;

  /* Read recieved data (64 bytes) */
  /* USB_SIL_Read(EP1_OUT, Receive_Buffer); */ //Use this for STM32F10X_CL devices(mandatory)
  /* OR */
  /* Use the memory interface function to write to the selected endpoint */
  /* PMAToUserBufferCopy(Receive_Buffer, GetEPRxAddr(EP1_OUT & 0x7F), GetEPRxCount(EP1_OUT & 0x7F)); */
  /* OR */
  PMAToUserBufferCopy(Receive_Buffer, ENDP1_RXADDR, 64);
  
  if (Receive_Buffer[1] == 0)
  {
    Bit_State = Bit_RESET;
  }
  else 
  {
    Bit_State = Bit_SET;
  }

  switch (Receive_Buffer[0])
  {
    case SHUTDOWN_CMD_REPORT_ID: /* Shutdown System -> 20 */
        #ifdef USE_SYSTEM_SHUTDOWN
        GPIO_WriteBit(GPIO_SHUTDOWN, GPIO_SHUTDOWN_PIN, Bit_State);
        // System_Shutdown();
        #endif  
        break;

    case LED_CMD_REPORT_ID: /* Led -> 21 */
        #ifdef USE_LED     
        #ifdef USE_STM32_SGN10X
        GPIO_WriteBit(GPIO_LED, GPIO_LED_PIN, Bit_State);
        #else /* USE_STM32_PRIMER2 or USE_STM32_SGN101 */
        GPIO_WriteBit(GPIO_LED, GPIO_LED_PIN, !Bit_State);
        #endif
        // if(Bit_State == Bit_SET)
        // {
            // Led_State(ENABLE);
        // }
        // else
        // {
            // Led_State(DISABLE);
        // }
        #endif
        break;

    case DFU_CMD_REPORT_ID:
        #ifdef USE_USB_DFU
        DFU_Start();
        #endif
        break;

    case DAC_CMD_REPORT_ID:
        /* To Do */
        break;

    case ADC_CMD_REPORT_ID:
        /* To Do */
        break;
    
    case MUX_CMD_REPORT_ID:
        #ifdef USE_ANALOG_MUX
        GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_123_INH_PIN, Bit_SET);
        ECG_Lead = Receive_Buffer[2];
        ECG_Lead_Select(ECG_Lead);    
        GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_123_INH_PIN, Bit_RESET);
        #endif
        #ifdef USE_DAC_IAREF
        DAC_IARef_Data = 2047;
        DAC_SetChannel2Data(DAC_Align_12b_R, DAC_IARef_Data);
        #endif
        break;

    case ECG_CMD_REPORT_ID:
        #ifdef USE_ECG_MONITOR
        if(Bit_State == Bit_SET)
        {        
            #ifdef USE_ANALOG_POWER_SWITCH
            Analog_Power(ENABLE);
            #endif
                
            #ifdef USE_ANALOG_MUX
            ECG_Lead = Receive_Buffer[2];
            ECG_Lead_Select(ECG_Lead);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_123_INH_PIN, Bit_RESET);
            #endif        
        
            ADC_Channel = IA_AIN_CHANNEL;//ECG_AIN_CHANNEL;         
            ADC_DMA_Reset_Data();

            Device_Config_Report();
        
            #ifdef USE_DAC_IAREF
            DAC_IARef_Data = 2047;
            DAC_SetChannel2Data(DAC_Align_12b_R, DAC_IARef_Data);
            #endif
        
            ADC_DMA_TIM_Configuration(ENABLE);
        }
        else
        {      
            ADC_DMA_TIM_Configuration(DISABLE);
        
            #ifdef USE_ANALOG_MUX
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_123_INH_PIN, Bit_SET);
            #endif
        
            #ifdef USE_ANALOG_POWER_SWITCH
            Analog_Power(DISABLE);
            #endif
        } 
        #endif
        break;
    
    case TEST_CMD_REPORT_ID:
        #ifdef USE_DAC_SIGNALGEN 
        if(Bit_State == Bit_SET)
        {        
            ADC_Channel = SIGNALGEN_AIN_CHANNEL;         
            ADC_DMA_Reset_Data();

            Device_Config_Report();        
        
            ADC_DMA_TIM_Configuration(ENABLE);
            DAC_DMA_TIM_Configuration(ENABLE);  
        }
        else
        {
            DAC_DMA_TIM_Configuration(DISABLE); 
            ADC_DMA_TIM_Configuration(DISABLE);        
        }
        #endif
        break;
      
    case FILTER_CMD_REPORT_ID:
        #ifdef USE_DSP_FILTER
        if(Bit_State == Bit_SET)
        {
            FILTER_ENABLED = 1;
        }
        else
        {
            FILTER_ENABLED = 0;        
        }
        #endif
        break;
  }
 
#ifndef STM32F10X_CL   
  SetEPRxStatus(ENDP1, EP_RX_VALID);
#endif /* STM32F10X_CL */
}

/*******************************************************************************
* Function Name  : EP1_IN_Callback.
* Description    : EP1 IN Callback Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP1_IN_Callback(void)
{
  PrevXferComplete = 1;
}
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

