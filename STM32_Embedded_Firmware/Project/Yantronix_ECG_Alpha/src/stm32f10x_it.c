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

extern FRESULT res; //FatFs function common result code 
extern FIL DATA_LOG_FIL_Obj; //data log file object
extern __IO uint16_t DATA_LOG_Read_Buff[DMA_DATA_BUFF_SIZE];
extern UINT dataLogBytesReadHT;
extern UINT dataLogBytesReadTC;
extern UINT dataLogBytesWritten;
extern uint16_t DATA_LOGGING_ENABLED;
#ifdef LCD_DEBUG_MESSAGE
extern uint8_t LCD_Display_Buff[20];
#endif

uint16_t DRAW_LCD_TRACE_ENABLED = 1;

// extern __IO uint16_t ADCConvertedValue;
#ifdef USE_USB_HID
uint16_t Data_Report_Id = DATA_REPORT_ID;
#endif
extern uint8_t Data_Channel[DATA_CHANNELS];
extern __IO uint8_t Data_Channel_Index;
extern __IO uint16_t DATA_Buff[DMA_DATA_BUFF_SIZE];
#ifdef USE_ADC_OVERSAMPLING
extern __IO uint16_t OVS_DATA_Buff[OVS_DMA_DATA_BUFF_SIZE];
__IO uint32_t OVS_Data_Index = 0;
__IO uint32_t OVS_Data_Accumulated_Result = 0;
#endif
__IO uint16_t *pDataBuff;

#if defined (USE_DAC_VGND) || defined (USE_DAC_IA_REF)
extern __IO uint16_t DAC_Channel1_Data;
#endif

#ifdef USE_USB_HID
uint16_t Filter_Data_Report_Id = FILTER_DATA_REPORT_ID;
#endif
__IO uint16_t FILTER_DATA_Buff[DATA_BUFF_SIZE];
__IO uint16_t *pFilterDataBuff;
uint16_t FILTER_ENABLED = 0;

extern uint32_t LCD_BackLight_RTC_Counter;
#ifdef USE_IWDG 
extern uint32_t IWDG_ReloadCounter_RTC_Counter;
#endif
extern uint32_t LowPower_StopMode_RTC_Counter;

extern __IO uint8_t LOW_POWER_STOP_MODE;
extern __IO uint8_t STOPMODE_SYSTEM_SHUTDOWN;
extern __IO uint8_t POWERKEY_SYSTEM_SHUTDOWN;

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
  uint32_t tmp = LCD_COLUMN_0, index = 0;
  
  /* If HSE is not detected at program startup or HSE clock failed during program execution */
  if ((RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET) || (RCC_GetITStatus(RCC_IT_CSS) != RESET))
  {
    /* Clear the LCD */
    LCD_Clear(LCD_COLOR_WHITE);
    /* Set the LCD Text Color */
    LCD_SetTextColor(LCD_COLOR_TEXT_NORMAL);
  
    /* Display "No Clock Detected" message */
    LCD_DisplayStringLine(LCD_LINE_0, "No HSE Clock         ");
    LCD_DisplayStringLine(LCD_LINE_1, "Detected. STANDBY    ");
    LCD_DisplayStringLine(LCD_LINE_2, "mode in few seconds. ");

    LCD_DisplayStringLine(LCD_LINE_4, "If HSE Clock         ");
    LCD_DisplayStringLine(LCD_LINE_5, "recovers before the  ");
    LCD_DisplayStringLine(LCD_LINE_6, "time out, a System   ");
    LCD_DisplayStringLine(LCD_LINE_7, "Reset is generated.  ");

    /* Clear Clock Security System interrupt pending bit */
    RCC_ClearITPendingBit(RCC_IT_CSS);
  
    // #ifdef USE_LED    
    // GPIO_SetBits(GPIO_LED, GPIO_LED1_PIN);
    // GPIO_ResetBits(GPIO_LED, GPIO_LED2_PIN);
    // #endif
  
    /* Enable HSE */
    RCC_HSEConfig(RCC_HSE_ON);
    
    LCD_DrawRect(LCD_COLUMN_0-1, LCD_LINE_3*8, 122, 9, LCD_COLOR_BLACK);
    /* Set the LCD Text Color */
    LCD_SetTextColor(LCD_COLOR_TEXT_REVERSE);
    
    /* Wait till HSE is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET)
    {
      if (index == 0xFFFFF) //0x3FFFF
      {
        LCD_DisplayChar(LCD_LINE_3, tmp, ' '); //0x20
        tmp += LCD_FONT_WIDTH;
        index = 0;
      }
      index++;
      /* Enters the system in STANDBY mode */
      if (tmp > LCD_COLUMN_19)
      {
        /* Clear the LCD */
        LCD_Clear(LCD_COLOR_WHITE);
        /* Set the LCD Text Color */
        LCD_SetTextColor(LCD_COLOR_TEXT_NORMAL);   
      
        LCD_DisplayStringLine(LCD_LINE_6, " MCU in STANDBY Mode"); 
        LCD_DisplayStringLine(LCD_LINE_7, "To exit press Wakeup");
      
        /* Request to enter STANDBY mode */
        PWR_EnterSTANDBYMode();
      }
    }

    /* Generate a system reset */
    NVIC_SystemReset();
  }
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

/**
  * @brief  This function handles RTC global interrupt request.
  * @param  None
  * @retval None
  */
void RTC_IRQHandler(void)
{
#ifdef USE_IWDG 
  if (IWDG_ReloadCounter_RTC_Counter == RTC_GetCounter())
  {
    Set_IWDG_ReloadCounter_Time();
  }
#endif

  if (LCD_BackLight_RTC_Counter == RTC_GetCounter())
  {
    LCD_BackLight_RTC_Counter = 0;
  
    LCD_SetBackLight(LCD_BRIGHTNESS_NIL);
  }

  if (LowPower_StopMode_RTC_Counter == RTC_GetCounter())
  {  
    LowPower_StopMode_RTC_Counter = 0;
  
    LOW_POWER_STOP_MODE = 1;
  }

  /* If counter is equal to 86339: one day was elapsed */
  if ((RTC_GetCounter() / 3600 == 23) && (((RTC_GetCounter() % 3600) / 60) == 59) &&
      (((RTC_GetCounter() % 3600) % 60) == 59)) /* 23*3600 + 59*60 + 59 = 86339 */
  /* if (RTC_GetCounter() == 0x00015180) // Reset RTC Counter when Time is 23:59:59 */
  {
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
  
    /* Reset counter value */
    RTC_SetCounter(0x0);
  
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
  
    /* Increment the date */
    Calendar_DateUpdate();
  }

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

  LowPower_Set_STOPModeStatus();

  STOPMODE_SYSTEM_SHUTDOWN = 1;

  /* Clear the EXTI Line 17/ */
  EXTI_ClearITPendingBit(EXTI_Line17);
}

#ifdef USE_SD_CARD_SDIO_INTERFACE
#if defined(STM32F10X_HD) || defined(STM32F10X_XL) 
/*******************************************************************************
* Function Name  : SDIO_IRQHandler
* Description    : This function handles SDIO global interrupt request.
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SDIO_IRQHandler(void)
{ 
  /* Process All SDIO Interrupt Sources */
  SD_ProcessIRQSrc();
  
}
#endif /* STM32F10X_HD | STM32F10X_XL*/
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

/*******************************************************************************
* Function Name  : ADC1_IRQHandler/ADC1_2_IRQHandler
* Description    : This function handles ADC1 and ADC2 global interrupts requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#if defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL) || defined (STM32F10X_HD_VL)
void ADC1_IRQHandler(void)
#else
void ADC1_2_IRQHandler(void)
#endif
{
  // if(ADC_GetITStatus(ADC1, ADC_IT_EOC) != RESET)
  // {
    // /* Get adc channel converted value */
    // ADCConvertedValue = ADC_GetConversionValue(ADC1);
  
    // /* Clear ADC1 EOC pending interrupt bit */
    // ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
  // }

#ifdef USE_ADC_ANALOGWATCHDOG
  if(ADC_GetITStatus(ADC1, ADC_IT_AWD) != RESET)
  {
    // #ifdef USE_LED
    // /* Toggle LED2 */
    // GPIO_WriteBit(GPIO_LED, GPIO_LED2_PIN, (BitAction)((1-GPIO_ReadOutputDataBit(GPIO_LED, GPIO_LED2_PIN))));
    // #endif

    /* Clear ADC1 AWD pending interrupt bit */
    ADC_ClearITPendingBit(ADC1, ADC_IT_AWD);
  }
#endif
}

/*******************************************************************************
* Function Name  : DMA1_Channel1_IRQHandler
* Description    : This function handles DMA1 Channel 1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel1_IRQHandler(void)
{
    if(DMA_GetFlagStatus(DMA1_FLAG_HT1)) //half transfer
    {
#ifdef USE_ADC_OVERSAMPLING
        for(Data_Channel_Index = 0; Data_Channel_Index < DATA_CHANNELS; Data_Channel_Index++)
        {            
            OVS_Data_Accumulated_Result = 0;
            for( OVS_Data_Index = Data_Channel_Index; OVS_Data_Index < OVS_DATA_BUFF_SIZE ; OVS_Data_Index = OVS_Data_Index + DATA_CHANNELS)
                OVS_Data_Accumulated_Result += OVS_DATA_Buff[OVS_Data_Index];
            
            DATA_Buff[Data_Channel_Index] = (uint16_t)(OVS_Data_Accumulated_Result >> ADC_ADDITIONAL_BITS);
            
#ifdef USE_DAC_IA_REF  
            if(Data_Channel[Data_Channel_Index] == ECG_AIN_CHANNEL)
            {
                /* Control Instrumentation Amplifier's(IA) output level */
                if(DATA_Buff[Data_Channel_Index] > OVS_DATA_UPPER_THRESHOLD)
                    DAC_Channel1_Data -= 1;
                if(DATA_Buff[Data_Channel_Index] < OVS_DATA_LOWER_THRESHOLD)
                    DAC_Channel1_Data += 1;
                DAC_SetChannel1Data(DAC_Align_12b_R, DAC_Channel1_Data);
            }
#endif        
        }
#endif
        
        pDataBuff = &DATA_Buff[0];

        if(DATA_LOGGING_ENABLED)
        {
            /* write to log file */
            res = f_write(&DATA_LOG_FIL_Obj, pDataBuff, DMA_DATA_BUFF_SIZE, &dataLogBytesWritten);
            // res = f_sync(&DATA_LOG_FIL_Obj);
        }            
    
        if(FILTER_ENABLED)
        {
            pFilterDataBuff = &FILTER_DATA_Buff[0];
            iir_mk_filter(pFilterDataBuff, pDataBuff);

#ifdef USE_USB_HID
            /* Write the descriptor through the endpoint */
            /* USB_SIL_Write(EP1_IN, (uint8_t*) Send_Buffer, 2); */  //Use this for STM32F10X_CL devices(mandatory).        
            /* OR */
            /* Use the memory interface function to write to the selected endpoint */
            /* CustomHIDUserToPMABufferCopy(&Filter_Data_Report_Id, pFilterDataBuff, GetEPTxAddr(EP1_IN & 0x7F), DMA_DATA_BUFF_SIZE + 2); */
            /* Update the data length in the control register */
            /* SetEPTxCount((EP1_IN & 0x7F), DMA_DATA_BUFF_SIZE + 2); */
            /* OR */
            CustomHIDUserToPMABufferCopy(&Filter_Data_Report_Id, pFilterDataBuff, ENDP1_TXADDR, DMA_DATA_BUFF_SIZE + 2);      
            SetEPTxCount(ENDP1, DMA_DATA_BUFF_SIZE + 2);

#ifndef STM32F10X_CL        
            SetEPTxValid(ENDP1);
#endif /* STM32F10X_CL */
#endif
        
            if(DRAW_LCD_TRACE_ENABLED)
                Draw_LCD_Trace(pFilterDataBuff);            
        }
        else
        { 
#ifdef USE_USB_HID
            /* Write the descriptor through the endpoint */
            /* USB_SIL_Write(EP1_IN, (uint8_t*) Send_Buffer, 2); */  //Use this for STM32F10X_CL devices(mandatory).        
            /* OR */
            /* Use the memory interface function to write to the selected endpoint */
            /* CustomHIDUserToPMABufferCopy(&Data_Report_Id, pDataBuff, GetEPTxAddr(EP1_IN & 0x7F), DMA_DATA_BUFF_SIZE + 2); */
            /* Update the data length in the control register */
            /* SetEPTxCount((EP1_IN & 0x7F), DMA_DATA_BUFF_SIZE + 2); */
            /* OR */
            CustomHIDUserToPMABufferCopy(&Data_Report_Id, pDataBuff, ENDP1_TXADDR, DMA_DATA_BUFF_SIZE + 2);      
            SetEPTxCount(ENDP1, DMA_DATA_BUFF_SIZE + 2);

#ifndef STM32F10X_CL
            SetEPTxValid(ENDP1);
#endif /* STM32F10X_CL */
#endif
        
            if(DRAW_LCD_TRACE_ENABLED)
                Draw_LCD_Trace(pDataBuff);
        }
    
        DMA_ClearFlag(DMA1_FLAG_HT1);
    }
    else if(DMA_GetFlagStatus(DMA1_FLAG_TC1)) //transfer complete
    {
#ifdef USE_ADC_OVERSAMPLING
        for(Data_Channel_Index = 0; Data_Channel_Index < DATA_CHANNELS; Data_Channel_Index++)
        {            
            OVS_Data_Accumulated_Result = 0;
            for( OVS_Data_Index = Data_Channel_Index + OVS_DATA_BUFF_SIZE; OVS_Data_Index < OVS_DMA_DATA_BUFF_SIZE ; OVS_Data_Index = OVS_Data_Index + DATA_CHANNELS)
                OVS_Data_Accumulated_Result += OVS_DATA_Buff[OVS_Data_Index];

            DATA_Buff[Data_Channel_Index + DATA_BUFF_SIZE] = (uint16_t)(OVS_Data_Accumulated_Result >> ADC_ADDITIONAL_BITS);
            
#ifdef USE_DAC_IA_REF  
            if(Data_Channel[Data_Channel_Index] == ECG_AIN_CHANNEL)
            {
                /* Control Instrumentation Amplifier's(IA) output level */
                if(DATA_Buff[Data_Channel_Index + DATA_BUFF_SIZE] > OVS_DATA_UPPER_THRESHOLD)
                    DAC_Channel1_Data -= 1;
                if(DATA_Buff[Data_Channel_Index + DATA_BUFF_SIZE] < OVS_DATA_LOWER_THRESHOLD)
                    DAC_Channel1_Data += 1;
                DAC_SetChannel1Data(DAC_Align_12b_R, DAC_Channel1_Data);
            }
#endif        
        }
#endif
            
        pDataBuff = &DATA_Buff[DATA_BUFF_SIZE];
    
        if(DATA_LOGGING_ENABLED)
        {
            /* write to log file */
            res = f_write(&DATA_LOG_FIL_Obj, pDataBuff, DMA_DATA_BUFF_SIZE, &dataLogBytesWritten);
            // res = f_sync(&DATA_LOG_FIL_Obj);
        }            
    
        if(FILTER_ENABLED)
        {
            pFilterDataBuff = &FILTER_DATA_Buff[0];
            iir_mk_filter(pFilterDataBuff, pDataBuff);        

#ifdef USE_USB_HID
            /* Write the descriptor through the endpoint */
            /* USB_SIL_Write(EP1_IN, (uint8_t*) Send_Buffer, 2); */  //Use this for STM32F10X_CL devices(mandatory).        
            /* OR */
            /* Use the memory interface function to write to the selected endpoint */
            /* CustomHIDUserToPMABufferCopy(&Filter_Data_Report_Id, pFilterDataBuff, GetEPTxAddr(EP1_IN & 0x7F), DMA_DATA_BUFF_SIZE + 2); */
            /* Update the data length in the control register */
            /* SetEPTxCount((EP1_IN & 0x7F), DMA_DATA_BUFF_SIZE + 2); */
            /* OR */
            CustomHIDUserToPMABufferCopy(&Filter_Data_Report_Id, pFilterDataBuff, ENDP1_TXADDR, DMA_DATA_BUFF_SIZE + 2);      
            SetEPTxCount(ENDP1, DMA_DATA_BUFF_SIZE + 2);

#ifndef STM32F10X_CL        
            SetEPTxValid(ENDP1);
#endif /* STM32F10X_CL */
#endif
        
            if(DRAW_LCD_TRACE_ENABLED)
                Draw_LCD_Trace(pFilterDataBuff);            
        }
        else
        {
#ifdef USE_USB_HID
            /* Write the descriptor through the endpoint */
            /* USB_SIL_Write(EP1_IN, (uint8_t*) Send_Buffer, 2); */  //Use this for STM32F10X_CL devices(mandatory).        
            /* OR */
            /* Use the memory interface function to write to the selected endpoint */
            /* CustomHIDUserToPMABufferCopy(&Data_Report_Id, pDataBuff, GetEPTxAddr(EP1_IN & 0x7F), DMA_DATA_BUFF_SIZE + 2); */
            /* Update the data length in the control register */
            /* SetEPTxCount((EP1_IN & 0x7F), DMA_DATA_BUFF_SIZE + 2); */
            /* OR */
            CustomHIDUserToPMABufferCopy(&Data_Report_Id, pDataBuff, ENDP1_TXADDR, DMA_DATA_BUFF_SIZE + 2);      
            SetEPTxCount(ENDP1, DMA_DATA_BUFF_SIZE + 2);

#ifndef STM32F10X_CL
            SetEPTxValid(ENDP1);
#endif /* STM32F10X_CL */
#endif
        
            if(DRAW_LCD_TRACE_ENABLED)
                Draw_LCD_Trace(pDataBuff);
        }
    
        DMA_ClearFlag(DMA1_FLAG_TC1);
    }
    //DMA_ClearFlag(DMA1_FLAG_GL1);
}

/*******************************************************************************
* Function Name  : DMA1_Channel7_IRQHandler
* Description    : This function handles DMA1 Channel 7 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel7_IRQHandler(void)
{
    if(DMA_GetFlagStatus(DMA1_FLAG_HT7)) //half transfer
    {
        if (res || dataLogBytesReadHT == 0)
        {
            MEM_DMA_TIM_Configuration(DISABLE);
        }
        else if(FILTER_ENABLED)
        {
            pDataBuff = &DATA_Buff[0];  
            pFilterDataBuff = &FILTER_DATA_Buff[0];
            iir_mk_filter(pFilterDataBuff, pDataBuff);

#ifdef USE_USB_HID
            /* Write the descriptor through the endpoint */
            /* USB_SIL_Write(EP1_IN, (uint8_t*) Send_Buffer, 2); */  //Use this for STM32F10X_CL devices(mandatory).        
            /* OR */
            /* Use the memory interface function to write to the selected endpoint */
            /* CustomHIDUserToPMABufferCopy(&Filter_Data_Report_Id, pFilterDataBuff, GetEPTxAddr(EP1_IN & 0x7F), DMA_DATA_BUFF_SIZE + 2); */
            /* Update the data length in the control register */
            /* SetEPTxCount((EP1_IN & 0x7F), DMA_DATA_BUFF_SIZE + 2); */
            /* OR */
            CustomHIDUserToPMABufferCopy(&Filter_Data_Report_Id, pFilterDataBuff, ENDP1_TXADDR, DMA_DATA_BUFF_SIZE + 2);      
            SetEPTxCount(ENDP1, DMA_DATA_BUFF_SIZE + 2);

#ifndef STM32F10X_CL        
            SetEPTxValid(ENDP1);
#endif /* STM32F10X_CL */
#endif
        
            if(DRAW_LCD_TRACE_ENABLED)
                Draw_LCD_Trace(pFilterDataBuff); 

            /* Read a chunk of file */
            res = f_read(&DATA_LOG_FIL_Obj, &DATA_LOG_Read_Buff[0], DMA_DATA_BUFF_SIZE, &dataLogBytesReadHT);
        }
        else
        {
            pDataBuff = &DATA_Buff[0];
        
#ifdef USE_USB_HID
            /* Write the descriptor through the endpoint */
            /* USB_SIL_Write(EP1_IN, (uint8_t*) Send_Buffer, 2); */  //Use this for STM32F10X_CL devices(mandatory).        
            /* OR */
            /* Use the memory interface function to write to the selected endpoint */
            /* CustomHIDUserToPMABufferCopy(&Data_Report_Id, pDataBuff, GetEPTxAddr(EP1_IN & 0x7F), DMA_DATA_BUFF_SIZE + 2); */
            /* Update the data length in the control register */
            /* SetEPTxCount((EP1_IN & 0x7F), DMA_DATA_BUFF_SIZE + 2); */
            /* OR */
            CustomHIDUserToPMABufferCopy(&Data_Report_Id, pDataBuff, ENDP1_TXADDR, DMA_DATA_BUFF_SIZE + 2);      
            SetEPTxCount(ENDP1, DMA_DATA_BUFF_SIZE + 2);

#ifndef STM32F10X_CL
            SetEPTxValid(ENDP1);
#endif /* STM32F10X_CL */
#endif
        
            if(DRAW_LCD_TRACE_ENABLED)
                Draw_LCD_Trace(pDataBuff);
            
            /* Read a chunk of file */
            res = f_read(&DATA_LOG_FIL_Obj, &DATA_LOG_Read_Buff[0], DMA_DATA_BUFF_SIZE, &dataLogBytesReadHT);
        }

        /* Clear DMA1 Channel7 half transfer flag */ 
        DMA_ClearFlag(DMA1_FLAG_HT7);
    }
    else if(DMA_GetFlagStatus(DMA1_FLAG_TC7)) //transfer complete
    {
        if (res || dataLogBytesReadTC == 0)
        {
            MEM_DMA_TIM_Configuration(DISABLE);
        }
        else if(FILTER_ENABLED)
        {
            pDataBuff = &DATA_Buff[DATA_BUFF_SIZE];
            pFilterDataBuff = &FILTER_DATA_Buff[0];
            iir_mk_filter(pFilterDataBuff, pDataBuff);

#ifdef USE_USB_HID
            /* Write the descriptor through the endpoint */
            /* USB_SIL_Write(EP1_IN, (uint8_t*) Send_Buffer, 2); */  //Use this for STM32F10X_CL devices(mandatory).        
            /* OR */
            /* Use the memory interface function to write to the selected endpoint */
            /* CustomHIDUserToPMABufferCopy(&Filter_Data_Report_Id, pFilterDataBuff, GetEPTxAddr(EP1_IN & 0x7F), DMA_DATA_BUFF_SIZE + 2); */
            /* Update the data length in the control register */
            /* SetEPTxCount((EP1_IN & 0x7F), DMA_DATA_BUFF_SIZE + 2); */
            /* OR */
            CustomHIDUserToPMABufferCopy(&Filter_Data_Report_Id, pFilterDataBuff, ENDP1_TXADDR, DMA_DATA_BUFF_SIZE + 2);      
            SetEPTxCount(ENDP1, DMA_DATA_BUFF_SIZE + 2);

#ifndef STM32F10X_CL        
            SetEPTxValid(ENDP1);
#endif /* STM32F10X_CL */
#endif
        
            if(DRAW_LCD_TRACE_ENABLED)
                Draw_LCD_Trace(pFilterDataBuff); 

            /* Read a chunk of file */
            res = f_read(&DATA_LOG_FIL_Obj, &DATA_LOG_Read_Buff[DATA_BUFF_SIZE], DMA_DATA_BUFF_SIZE, &dataLogBytesReadTC);
        }
        else
        {
            pDataBuff = &DATA_Buff[DATA_BUFF_SIZE];        
        
#ifdef USE_USB_HID
            /* Write the descriptor through the endpoint */
            /* USB_SIL_Write(EP1_IN, (uint8_t*) Send_Buffer, 2); */  //Use this for STM32F10X_CL devices(mandatory).        
            /* OR */
            /* Use the memory interface function to write to the selected endpoint */
            /* CustomHIDUserToPMABufferCopy(&Data_Report_Id, pDataBuff, GetEPTxAddr(EP1_IN & 0x7F), DMA_DATA_BUFF_SIZE + 2); */
            /* Update the data length in the control register */
            /* SetEPTxCount((EP1_IN & 0x7F), DMA_DATA_BUFF_SIZE + 2); */
            /* OR */
            CustomHIDUserToPMABufferCopy(&Data_Report_Id, pDataBuff, ENDP1_TXADDR, DMA_DATA_BUFF_SIZE + 2);      
            SetEPTxCount(ENDP1, DMA_DATA_BUFF_SIZE + 2);
        
#ifndef STM32F10X_CL
            SetEPTxValid(ENDP1);
#endif /* STM32F10X_CL */
#endif
        
            if(DRAW_LCD_TRACE_ENABLED)
                Draw_LCD_Trace(pDataBuff);
            
            /* Read a chunk of file */
            res = f_read(&DATA_LOG_FIL_Obj, &DATA_LOG_Read_Buff[DATA_BUFF_SIZE], DMA_DATA_BUFF_SIZE, &dataLogBytesReadTC);
        }

        /* Clear DMA1 Channel7 Transfer complete flag */
        DMA_ClearFlag(DMA1_FLAG_TC7);
    }
    //DMA_ClearFlag(DMA1_FLAG_GL7);
}

/*******************************************************************************
* Function Name  : EXTI0_IRQHandler
* Description    : This function handles External line 0 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI0_IRQHandler(void)
{
  if(EXTI_GetITStatus(GPIO_POWER_EXTI_Line) != RESET)
  {  
    // LCD_DisplayStringLine(Line0, "POWER Pressed");

    POWERKEY_SYSTEM_SHUTDOWN = 1;
  
    /* Clear the EXTI line pending bit */
    EXTI_ClearITPendingBit(GPIO_POWER_EXTI_Line);
  }
}

/*******************************************************************************
* Function Name  : EXTI1_IRQHandler
* Description    : This function handles External line 1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI1_IRQHandler(void)
{
  if(EXTI_GetITStatus(GPIO_MENU_EXTI_Line) != RESET)
  {  
    // LCD_DisplayStringLine(Line0, "MENU Pressed");
  
    if(!LOW_POWER_STOP_MODE)
        Menu_ReturnMainMenu();
  
    /* Clear the EXTI line pending bit */
    EXTI_ClearITPendingBit(GPIO_MENU_EXTI_Line);
  }
}

/*******************************************************************************
* Function Name  : EXTI2_IRQHandler
* Description    : This function handles External line 2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI2_IRQHandler(void)
{
  if(EXTI_GetITStatus(GPIO_SEL_EXTI_Line) != RESET)
  {  
    // LCD_DisplayStringLine(Line0, "SEL Pressed");
  
    if(!LOW_POWER_STOP_MODE)
        Menu_SelFunc();
      
    /* Clear the EXTI line pending bit */
    EXTI_ClearITPendingBit(GPIO_SEL_EXTI_Line);
  }
}

/*******************************************************************************
* Function Name  : EXTI9_5_IRQHandler
* Description    : This function handles External lines 5 to 9 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI9_5_IRQHandler(void)
{
  if(EXTI_GetITStatus(GPIO_UP_EXTI_Line) != RESET)
  {  
    // LCD_DisplayStringLine(Line0, "UP Pressed");

    if(!LOW_POWER_STOP_MODE)
        Menu_UpFunc();
  
    /* Clear the EXTI line pending bit */
    EXTI_ClearITPendingBit(GPIO_UP_EXTI_Line);
  }

  if(EXTI_GetITStatus(GPIO_DOWN_EXTI_Line) != RESET)
  {  
    // LCD_DisplayStringLine(Line0, "DOWN Pressed");
  
    if(!LOW_POWER_STOP_MODE)
        Menu_DownFunc();
  
    /* Clear the EXTI line pending bit */
    EXTI_ClearITPendingBit(GPIO_DOWN_EXTI_Line);
  }

#ifdef SD_CARD_DETECT
  if(EXTI_GetITStatus(GPIO_SD_CARD_DETECT_EXTI_Line) != RESET)
  {  
    // LCD_DisplayStringLine(Line0, "SD Card Detect");
  
    SD_Card_Detect();
  
    /* Clear the EXTI line pending bit */
    EXTI_ClearITPendingBit(GPIO_SD_CARD_DETECT_EXTI_Line);
  }
#endif

#ifdef USB_HOST_DETECT
  if(EXTI_GetITStatus(GPIO_USB_HOST_DETECT_EXTI_Line) != RESET)
  {  
    // LCD_DisplayStringLine(Line0, "USB Host Detect");
  
    USB_Host_Detect();
  
    /* Clear the EXTI line pending bit */
    EXTI_ClearITPendingBit(GPIO_USB_HOST_DETECT_EXTI_Line);
  }
#endif
}

/*******************************************************************************
* Function Name  : EXTI15_10_IRQHandler
* Description    : This function handles External lines 10 to 15 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI15_10_IRQHandler(void)
{
#ifdef REGULATOR_FAULT
  if(EXTI_GetITStatus(GPIO_REGULATOR_FAULT_EXTI_Line) != RESET)
  {  
    // LCD_DisplayStringLine(Line0, "Regulator Fault");
  
    Regulator_Fault();
  
    /* Clear the EXTI line pending bit */
    EXTI_ClearITPendingBit(GPIO_REGULATOR_FAULT_EXTI_Line);
  }
#endif
}

/**
  * @}
  */

/**
  * @}
  */


