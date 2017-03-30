/*****************************************************************************
* File    : @file
* Company : Yantronix
* Author  : Satish Nair
* Mailto  : satishgn77@gmail.com
*****************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "hw_config.h"

/** @addtogroup StdPeriph_Examples
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//__IO uint8_t Send_Buffer[2];

// extern __IO uint16_t ADCConvertedValue;

#if defined (USE_ECG_MONITOR) || defined (USE_DAC_SIGNALGEN)
extern __IO uint16_t DATA_Buff[DMA_DATA_BUFF_SIZE];
extern __IO uint16_t *pDataBuff;
#ifdef USE_ADC_OVERSAMPLING
extern __IO uint16_t OVS_DATA_Buff[OVS_DMA_DATA_BUFF_SIZE];
extern __IO uint16_t Data_Buff_Index;
__IO uint16_t OVS_Data_Buff_Index = 0;
__IO uint32_t OVS_Data_Accumulated_Result = 0;
#endif
#endif

#ifdef USE_DAC_IAREF
extern __IO uint16_t DAC_IARef_Data;
#endif

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval : None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval : None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval : None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval : None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval : None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval : None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval : None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval : None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval : None
  */
void SysTick_Handler(void)
{
    /* Decrement the TimingDelay variable */
    Decrement_TimingDelay();
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval : None
  */
/*void PPP_IRQHandler(void)
{
}*/

#ifdef USE_RTC
/**
  * @brief  This function handles RTC global interrupt request.
  * @param  None
  * @retval None
  */
void RTC_IRQHandler(void)
{
    /* Clear the RTC Second Interrupt pending bit */
    RTC_ClearITPendingBit(RTC_IT_SEC);
}

/**
  * @brief  This function handles RTC Alarm interrupt request.
  * @param  None
  * @retval None
  */
void RTCAlarm_IRQHandler(void)
{
    /* Clear the Alarm Pending Bit */
    RTC_ClearITPendingBit(RTC_IT_ALR);

    // /* Clear the EXTI Line 17/ */
    // EXTI_ClearITPendingBit(EXTI_Line17);
}
#endif

#ifdef USE_USB_HID
#ifndef STM32F10X_CL
/*******************************************************************************
* Function Name  : USB_LP_CAN1_RX0_IRQHandler
* Description    : This function handles USB Low Priority or CAN RX0 interrupts
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_LP_CAN1_RX0_IRQHandler(void)
{
  USB_Istr();
}
#endif /* STM32F10X_CL */

#ifdef STM32F10X_CL
/*******************************************************************************
* Function Name  : OTG_FS_IRQHandler
* Description    : This function handles USB-On-The-Go FS global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void OTG_FS_IRQHandler(void)
{
  STM32_PCD_OTG_ISR_Handler(); 
}
#endif /* STM32F10X_CL */
#endif

#ifdef USE_ADC_ANALOGWATCHDOG
/*******************************************************************************
* Function Name  : ADC1_2_IRQHandler
* Description    : This function handles ADC1 and ADC2 global interrupts requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC1_2_IRQHandler(void)
{
  if(ADC_GetITStatus(ADC2, ADC_IT_AWD) != RESET)
  {
    #ifdef USE_LED
    /* Toggle LED */
    GPIO_WriteBit(GPIO_LED, GPIO_LED_PIN, (BitAction)((1-GPIO_ReadOutputDataBit(GPIO_LED, GPIO_LED_PIN))));
    #endif

    /* Clear ADC2 AWD pending interrupt bit */
    ADC_ClearITPendingBit(ADC2, ADC_IT_AWD);
  }
}
#endif

#if defined (USE_ECG_MONITOR) || defined (USE_DAC_SIGNALGEN)
/*******************************************************************************
* Function Name  : DMA1_Channel1_IRQHandler
* Description    : This function handles DMA1 Channel 1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel1_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_IT_HT1)) //half transfer
    {
#ifdef USE_ADC_OVERSAMPLING
        OVS_Data_Accumulated_Result = 0;

        for(OVS_Data_Buff_Index = 0; OVS_Data_Buff_Index < OVS_DATA_BUFF_SIZE ; OVS_Data_Buff_Index++)
            OVS_Data_Accumulated_Result += OVS_DATA_Buff[OVS_Data_Buff_Index];
        
        DATA_Buff[Data_Buff_Index] = (uint16_t)(OVS_Data_Accumulated_Result >> ADC_ADDITIONAL_BITS);

#ifdef USE_DAC_IAREF
        /* Control Instrumentation Amplifier's(IA) output level */
        if(DATA_Buff[Data_Buff_Index] > IA_AIN_DATA_HIGH)
        {
            DAC_IARef_Data -= 1;
            DAC_SetChannel2Data(DAC_Align_12b_R, DAC_IARef_Data);
        }
        else if(DATA_Buff[Data_Buff_Index] < IA_AIN_DATA_LOW)
        {
            DAC_IARef_Data += 1;
            DAC_SetChannel2Data(DAC_Align_12b_R, DAC_IARef_Data);
        }
#endif
        
        Data_Buff_Index += 1;
        
        if(Data_Buff_Index == DATA_BUFF_SIZE)
        {
            pDataBuff = &DATA_Buff[0];
        }
#else
        pDataBuff = &DATA_Buff[0];        
#endif
    
        DMA_ClearITPendingBit(DMA1_IT_HT1);
    }
    else if(DMA_GetITStatus(DMA1_IT_TC1)) //transfer complete
    {
#ifdef USE_ADC_OVERSAMPLING
        OVS_Data_Accumulated_Result = 0;

        for(OVS_Data_Buff_Index = OVS_DATA_BUFF_SIZE; OVS_Data_Buff_Index < OVS_DMA_DATA_BUFF_SIZE ; OVS_Data_Buff_Index++)
            OVS_Data_Accumulated_Result += OVS_DATA_Buff[OVS_Data_Buff_Index];

        DATA_Buff[Data_Buff_Index] = (uint16_t)(OVS_Data_Accumulated_Result >> ADC_ADDITIONAL_BITS);

#ifdef USE_DAC_IAREF
        /* Control Instrumentation Amplifier's(IA) output level */
        if(DATA_Buff[Data_Buff_Index] > IA_AIN_DATA_HIGH)
        {
            DAC_IARef_Data -= 1;
            DAC_SetChannel2Data(DAC_Align_12b_R, DAC_IARef_Data);
        }
        else if(DATA_Buff[Data_Buff_Index] < IA_AIN_DATA_LOW)
        {
            DAC_IARef_Data += 1;
            DAC_SetChannel2Data(DAC_Align_12b_R, DAC_IARef_Data);
        }    
#endif
    
        Data_Buff_Index += 1;
        
        if(Data_Buff_Index == DMA_DATA_BUFF_SIZE)
        {
            pDataBuff = &DATA_Buff[DATA_BUFF_SIZE];
            Data_Buff_Index = 0;
        }
#else
        pDataBuff = &DATA_Buff[DATA_BUFF_SIZE];
#endif
    
        DMA_ClearITPendingBit(DMA1_IT_TC1);
    }
    //DMA_ClearITPendingBit(DMA1_IT_GL1);
}
#endif

#ifdef USE_POWER_BUTTON
#ifdef USE_STM32_SGN101
/*******************************************************************************
* Function Name  : EXTI0_IRQHandler
* Description    : This function handles External line 0 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI0_IRQHandler(void)
#else /* USE_STM32_PRIMER2 or USE_STM32_SGN10X */
/*******************************************************************************
* Function Name  : EXTI9_5_IRQHandler
* Description    : This function handles External lines 5 to 9 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI9_5_IRQHandler(void)
#endif
{
  if(EXTI_GetITStatus(GPIO_POWER_EXTI_Line) != RESET)
  {  
    /* POWER Button Pressed */

    #ifdef USE_SYSTEM_SHUTDOWN
    System_Shutdown();
    #endif  
  
    /* Clear the EXTI line pending bit */
    EXTI_ClearITPendingBit(GPIO_POWER_EXTI_Line);
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */


