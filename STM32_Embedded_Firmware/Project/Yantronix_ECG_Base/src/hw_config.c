/*****************************************************************************
* File    : @file
* Company : Yantronix
* Author  : Satish Nair
* Mailto  : satishgn77@gmail.com
*****************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"
#if defined (USE_USB_HID) && defined (USE_DEBUG_REPORT)
#include <stdio.h>
#endif

#define ADC1_DR_Address         ((uint32_t)0x4001244C)

#ifdef USE_DAC_SIGNALGEN
#ifdef USE_STM32_PRIMER2
#define DAC_DHR12R1_Address     ((uint32_t)0x40007408)
#else
#define DAC_DHR12R2_Address     ((uint32_t)0x40007414)
#endif
#endif

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ErrorStatus HSEStartUpStatus;
  
__IO uint8_t PrevXferComplete = 1;  
// __IO uint16_t ADCConvertedValue;

#if defined (USE_ECG_MONITOR) || defined (USE_DAC_SIGNALGEN)
uint8_t ADC_Channel;
__IO uint16_t DATA_Buff[DMA_DATA_BUFF_SIZE];
__IO uint16_t *pDataBuff;
#ifdef USE_ADC_OVERSAMPLING    
__IO uint16_t OVS_DATA_Buff[OVS_DMA_DATA_BUFF_SIZE];
__IO uint16_t Data_Buff_Index = 0;
#endif
#ifdef USE_DSP_FILTER
uint16_t FILTER_ENABLED = 0;
__IO uint16_t FILTER_DATA_Buff[DATA_BUFF_SIZE];
__IO uint16_t *pFilterDataBuff;
#endif
#endif  
  
#ifdef USE_DAC_SIGNALGEN 
const uint16_t SineFreqTable[6] = {1, 5, 10, 20, 25, 50};
uint16_t SineFreqIndex = 0; //Default 1 Hz
const uint16_t Sine12bit[DAC_NPT] = { 2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056,
                                 3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 909, 
                                 599, 344, 155, 38, 0, 38, 155, 344, 599, 909, 1263, 1647 };
#endif  
                             
#ifdef USE_DAC_IAREF
__IO uint16_t DAC_IARef_Data = 2047; //IA Reference Voltage
#endif
                             
#ifdef USE_DAC_VGND
__IO uint16_t DAC_VGnd_Data = 2047; //Virtual Ground Voltage
#endif
    
#ifdef USE_ANALOG_MUX
uint8_t ECG_Lead = 0;
#endif

#ifdef USE_USB_HID
__IO uint16_t CONFIG_Buff[31];
__IO uint16_t Config_Report_Id = CONFIG_REPORT_ID;              
__IO uint16_t Data_Report_Id = DATA_REPORT_ID;
#ifdef USE_DSP_FILTER                             
__IO uint16_t Filter_Report_Id = FILTER_REPORT_ID;
#endif
#ifdef USE_DEBUG_REPORT
__IO uint8_t DEBUG_Buff[62];
__IO uint16_t Debug_Report_Id = DEBUG_REPORT_ID;
#endif                             
#endif
                             
static __IO uint32_t TimingDelay = 0;

/* Private function prototypes -----------------------------------------------*/
// static void RCC_Config(void);                            
                            
#ifdef USE_USB_HID                            
static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len);
#endif
                            
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
                            
/* Private functions ---------------------------------------------------------*/

// /*******************************************************************************
// * Function Name  : RCC_Config
// * Description    : Configures Main system clocks & power
// * Input          : None.
// * Return         : None.
// *******************************************************************************/
// static void RCC_Config(void)
// {    
  // /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/   
  // /* RCC system reset(for debug purpose) */
  // RCC_DeInit();

  // /* Enable HSE */
  // RCC_HSEConfig(RCC_HSE_ON);

  // /* Wait till HSE is ready */
  // HSEStartUpStatus = RCC_WaitForHSEStartUp();

  // if (HSEStartUpStatus == SUCCESS)
  // {
// #if !defined STM32F10X_LD_VL && !defined STM32F10X_MD_VL && !defined STM32F10X_HD_VL
    // /* Enable Prefetch Buffer */
    // FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    // /* Flash 2 wait state */
    // /* FLASH_SetLatency(FLASH_Latency_2); */
  
    // /* Flash 0 wait state */
    // FLASH_SetLatency(FLASH_Latency_0);
// #endif /* STM32F10X_LD_VL && STM32F10X_MD_VL */

    // /* HCLK = SYSCLK */
    // RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    // /* PCLK2 = HCLK */
    // RCC_PCLK2Config(RCC_HCLK_Div1); 

    // /* PCLK1 = HCLK/2 */
    // /* RCC_PCLK1Config(RCC_HCLK_Div2); */
 
    // /* PCLK1 = HCLK */
    // RCC_PCLK1Config(RCC_HCLK_Div1);
  
    // /* ADCCLK = PCLK2/8 */
    // /* RCC_ADCCLKConfig(RCC_PCLK2_Div8); */

// #if defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL) || defined (STM32F10X_HD_VL)
    // /* ADCCLK = PCLK2/2 */
    // RCC_ADCCLKConfig(RCC_PCLK2_Div2); 
// #else
    // /* ADCCLK = PCLK2/4 */
    // RCC_ADCCLKConfig(RCC_PCLK2_Div4); 
// #endif

// #ifdef STM32F10X_CL
    // /* Configure PLLs *********************************************************/
    // /* PLL2 configuration: PLL2CLK = (HSE / 5) * 8 = 40 MHz */
    // RCC_PREDIV2Config(RCC_PREDIV2_Div5);
    // RCC_PLL2Config(RCC_PLL2Mul_8);

    // /* Enable PLL2 */
    // RCC_PLL2Cmd(ENABLE);

    // /* Wait till PLL2 is ready */
    // while (RCC_GetFlagStatus(RCC_FLAG_PLL2RDY) == RESET)
    // {}

    // /* PLL configuration: PLLCLK = (PLL2 / 5) * 9 = 72 MHz */ 
    // /* RCC_PREDIV1Config(RCC_PREDIV1_Source_PLL2, RCC_PREDIV1_Div5); */
    // /* RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_9); */
    
    // /* PLL configuration: PLLCLK = (PLL2 / 10) * 6 = 24 MHz */ 
    // RCC_PREDIV1Config(RCC_PREDIV1_Source_PLL2, RCC_PREDIV1_Div10);
    // RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_6);    
// #elif defined STM32F10X_LD_VL || defined STM32F10X_MD_VL || defined STM32F10X_HD_VL 
    // /* PLLCLK = (8MHz/2) * 6 = 24 MHz */
    // RCC_PREDIV1Config(RCC_PREDIV1_Source_HSE, RCC_PREDIV1_Div2);
    // RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_6);
// #elif defined (USE_STM32_PRIMER2)
    // /* PLLCLK = 12MHz * 6 = 72 MHz */
    // /* RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_6); */
    
    // /* PLLCLK = 12MHz * 2 = 24 MHz */
    // RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_2);
// #else    
    // /* PLLCLK = 8MHz * 9 = 72 MHz */
    // /* RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9); */   
    
    // /* PLLCLK = 8MHz * 3 = 24 MHz */
    // RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_3);    
// #endif

    // /* Enable PLL */ 
    // RCC_PLLCmd(ENABLE);

    // /* Wait till PLL is ready */
    // while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    // {
    // }

    // /* Select PLL as system clock source */
    // RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    // /* Wait till PLL is used as system clock source */
    // while(RCC_GetSYSCLKSource() != 0x08)
    // {
    // }
  // }
  // else
  // { /* If HSE fails to start-up, the application will have wrong clock configuration.
       // User can add here some code to deal with this error */    

    // /* Go to infinite loop */
    // while (1)
    // {
    // }
  // }
// }
                            
/*******************************************************************************
* Function Name  : Set_System
* Description    : Configures Main system clocks & power.
* Input          : None.
* Return         : None.
*******************************************************************************/
void Set_System(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f10x_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f10x.c file
     */     

  /* Setup the microcontroller system. Initialize the Embedded Flash Interface,  
     initialize the PLL and update the SystemFrequency variable. */
  //SystemInit();
  //RCC_Config();

  /* Configure the used GPIOs*/
  GPIO_Configuration();

  /* Configure NVIC */
  NVIC_Configuration();
  
#ifdef USE_USB_HID
  /* Configure USB Clock */
  Set_USBClock();
  
  /* Initialize USB system */
  USB_Init();
#endif

  /* Configure the systick */    
  SysTick_Configuration();
  
  /* If HSE is not detected at program startup */
  /* if(HSEStartUpStatus == ERROR) */
  if(RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET)
  {
    /* Generate NMI exception */
    SCB->ICSR |= SCB_ICSR_NMIPENDSET;
  }

#if defined (USE_DAC_IAREF) || defined (USE_DAC_VGND)
  DAC_Configuration(ENABLE);
#endif

#ifdef USE_RTC
  /* Configure RTC */
  RTC_Configuration();
#if defined (USE_USB_HID) && defined (USE_DEBUG_REPORT)
  Delay(200);//Delay 2 sec
  sprintf(DEBUG_Buff, "RTC Configured...\n");
  Device_Debug_Report();
#endif
#endif

#ifdef USE_POWER_BUTTON
  Delay(100);//Delay 1 sec
  POWER_EXTI_Configuration(ENABLE);
#endif

#ifdef USE_VBAT_MONITOR
  Start_Battery_Monitor();
#endif

#ifdef USE_ADC_ANALOGWATCHDOG
  ADC_AnalogWatchdog_Configuration(ENABLE);
#endif
}

#ifdef USE_USB_HID
/*******************************************************************************
* Function Name  : Set_USBClock
* Description    : Configures USB Clock input (48MHz).
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Set_USBClock(void)
{
#ifdef STM32F10X_CL
  /* Select USBCLK source */
  RCC_OTGFSCLKConfig(RCC_OTGFSCLKSource_PLLVCO_Div3);

  /* Enable the USB clock */ 
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_OTG_FS, ENABLE) ;
#else
  /* Select USBCLK source */
  RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
  
  /* Enable the USB clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);
#endif /* STM32F10X_CL */
}

/*******************************************************************************
* Function Name  : Enter_LowPowerMode.
* Description    : Power-off system clocks and power while entering suspend mode.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Enter_LowPowerMode(void)
{
  /* Set the device state to suspend */
  bDeviceState = SUSPENDED;
}

/*******************************************************************************
* Function Name  : Leave_LowPowerMode.
* Description    : Restores system clocks and power while exiting suspend mode.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Leave_LowPowerMode(void)
{
  DEVICE_INFO *pInfo = &Device_Info;
  
  /* Set the device state to the correct state */
  if (pInfo->Current_Configuration != 0)
  {
    /* Device configured */
    bDeviceState = CONFIGURED;
  }
  else 
  {
    bDeviceState = ATTACHED;
  }
}

/*******************************************************************************
* Function Name  : USB_Interrupts_Config.
* Description    : Configures the USB interrupts.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USB_Interrupts_Config(void)
{
  // NVIC_InitTypeDef NVIC_InitStructure; 
  
  // /* 2 bit for pre-emption priority, 2 bits for subpriority */
  // NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  
  
// #ifdef STM32F10X_CL
  // /* Enable the USB Interrupts */
  // NVIC_InitStructure.NVIC_IRQChannel = OTG_FS_IRQn;
  // NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  // NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  // NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  // NVIC_Init(&NVIC_InitStructure);   
// #else
  // NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
  // NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  // NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  // NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  // NVIC_Init(&NVIC_InitStructure);
// #endif /* STM32F10X_CL */
}

/*******************************************************************************
* Function Name  : USB_Cable_Config.
* Description    : Software Connection/Disconnection of USB Cable.
* Input          : NewState: new state.
* Output         : None.
* Return         : None
*******************************************************************************/
void USB_Cable_Config(FunctionalState NewState)
{ 
#ifdef STM32F10X_CL  
  if (NewState != DISABLE)
  {
    USB_DevConnect();
  }
  else
  {
    USB_DevDisconnect();
  }
#elif defined (USE_STM32_SGN10X)
  if (NewState != DISABLE)
  {
    GPIO_SetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);
  }
  else
  {
    GPIO_ResetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);
  }
#else /* USE_STM32_PRIMER2 or USE_STM32_SGN101 */
  if (NewState != DISABLE)
  {
    GPIO_ResetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);
  }
  else
  {
    GPIO_SetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);
  }
#endif /* STM32F10X_CL */
}

// /*******************************************************************************
// * Function Name  : Get_SerialNum.
// * Description    : Create the serial number string descriptor.
// * Input          : None.
// * Output         : None.
// * Return         : None.
// *******************************************************************************/
// void Get_SerialNum(void)
// {
  // uint32_t Device_Serial0, Device_Serial1, Device_Serial2;
  
  // Device_Serial0 = *(uint32_t*)(0x1FFFF7E8);
  // Device_Serial1 = *(uint32_t*)(0x1FFFF7EC);
  // Device_Serial2 = *(uint32_t*)(0x1FFFF7F0);
  
  // if(Device_Serial0 != 0)
  // {
     // CustomHID_StringSerial[2] = (uint8_t)(Device_Serial0 & 0x000000FF);  
     // CustomHID_StringSerial[4] = (uint8_t)((Device_Serial0 & 0x0000FF00) >> 8);
     // CustomHID_StringSerial[6] = (uint8_t)((Device_Serial0 & 0x00FF0000) >> 16);
     // CustomHID_StringSerial[8] = (uint8_t)((Device_Serial0 & 0xFF000000) >> 24);  
     
     // CustomHID_StringSerial[10] = (uint8_t)(Device_Serial1 & 0x000000FF);  
     // CustomHID_StringSerial[12] = (uint8_t)((Device_Serial1 & 0x0000FF00) >> 8);
     // CustomHID_StringSerial[14] = (uint8_t)((Device_Serial1 & 0x00FF0000) >> 16);
     // CustomHID_StringSerial[16] = (uint8_t)((Device_Serial1 & 0xFF000000) >> 24); 
     
     // CustomHID_StringSerial[18] = (uint8_t)(Device_Serial2 & 0x000000FF);  
     // CustomHID_StringSerial[20] = (uint8_t)((Device_Serial2 & 0x0000FF00) >> 8);
     // CustomHID_StringSerial[22] = (uint8_t)((Device_Serial2 & 0x00FF0000) >> 16);
     // CustomHID_StringSerial[24] = (uint8_t)((Device_Serial2 & 0xFF000000) >> 24); 
  // }   
// }

/*******************************************************************************
* Function Name  : Get_SerialNum.
* Description    : Create the serial number string descriptor.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Get_SerialNum(void)
{
  uint32_t Device_Serial0, Device_Serial1, Device_Serial2;

  Device_Serial0 = *(__IO uint32_t*)(0x1FFFF7E8);
  Device_Serial1 = *(__IO uint32_t*)(0x1FFFF7EC);
  Device_Serial2 = *(__IO uint32_t*)(0x1FFFF7F0);

  Device_Serial0 += Device_Serial2;

  if (Device_Serial0 != 0)
  {
    IntToUnicode (Device_Serial0, &CustomHID_StringSerial[2] , 8);
    IntToUnicode (Device_Serial1, &CustomHID_StringSerial[18], 4);
  }
}

/*******************************************************************************
* Function Name  : HexToChar.
* Description    : Convert Hex 32Bits value into char.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len)
{
  uint8_t idx = 0;
  
  for( idx = 0 ; idx < len ; idx ++)
  {
    if( ((value >> 28)) < 0xA )
    {
      pbuf[ 2* idx] = (value >> 28) + '0';
    }
    else
    {
      pbuf[2* idx] = (value >> 28) + 'A' - 10; 
    }
    
    value = value << 4;
    
    pbuf[ 2* idx + 1] = 0;
  }
}

#ifdef STM32F10X_CL
/*******************************************************************************
* Function Name  : USB_OTG_BSP_uDelay.
* Description    : provide delay (usec).
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USB_OTG_BSP_uDelay (const uint32_t usec)
{
  RCC_ClocksTypeDef  RCC_Clocks;  

  /* Configure HCLK clock as SysTick clock source */
  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
  
  RCC_GetClocksFreq(&RCC_Clocks);
  
  SysTick_Config(usec * (RCC_Clocks.HCLK_Frequency / 1000000));  
  
  SysTick->CTRL  &= ~SysTick_CTRL_TICKINT_Msk ;
  
  while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
}
#endif
#endif

/**
  * @brief  Configure the nested vectored interrupt controller.
  * @param  None
  * @retval : None
  */
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Set the Vector Table base location at 0xC000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0xC000);

  /* Configure the NVIC Preemption Priority Bits */
  /* 4 bits for pre-emption priority(0-15 PreemptionPriority) and 0 bits for subpriority(0 SubPriority) */
  /* NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); */

  /* Configure the Priority Group to 2 bits */
  /* 2 bits for pre-emption priority(0-3 PreemptionPriority) and 2 bits for subpriority(0-3 SubPriority) */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
#ifdef USE_RTC
  /* Enable the RTC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the RTC Alarm Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif

  /* PreemptionPriority = 0 & SubPriority = 2 has been reserved for SysTick Interrupt (SysTick_IRQn) */

#if defined (USE_ECG_MONITOR) || defined (USE_DAC_SIGNALGEN)
  /* Enable the DMA1 Channel1 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif  

#ifdef USE_USB_HID
#ifdef STM32F10X_CL
  /* Enable the USB Interrupts */
  NVIC_InitStructure.NVIC_IRQChannel = OTG_FS_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);   
#else
  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif /* STM32F10X_CL */
#endif

#ifdef USE_POWER_BUTTON
#ifdef USE_STM32_SGN101
  /* Enable the EXTI0 Interrupt (POWER button) */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#else /* USE_STM32_PRIMER2 or USE_STM32_SGN10X */
  /* Enable the EXTI9_5 Interrupt (POWER button) */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif  
#endif

#ifdef USE_ADC_ANALOGWATCHDOG
  /* Enable the ADC interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);    
#endif
}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); 
#ifdef USE_USB_HID
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_DISCONNECT, ENABLE);
#endif
#ifdef USE_VBAT_MONITOR
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_VBAT_AIN, ENABLE); 
#endif
#ifdef USE_ECG_MONITOR
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_IA_AIN, ENABLE); 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_ECG_AIN, ENABLE); 
#endif
#ifdef USE_DAC_SIGNALGEN 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_SIGNALGEN_AIN, ENABLE);  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_SIGNALGEN_AOUT, ENABLE);  
#endif
#ifdef USE_DAC_IAREF  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_IAREF_AOUT, ENABLE);   
#endif
#ifdef USE_DAC_VGND  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_VGND_AOUT, ENABLE);   
#endif
#ifdef USE_ANALOG_MUX
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_MUX_123, ENABLE);
#endif
#ifdef USE_LED  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_LED, ENABLE); 
#endif  
#ifdef USE_SYSTEM_SHUTDOWN
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_SHUTDOWN, ENABLE);   
#endif
#ifdef USE_ANALOG_POWER_SWITCH
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_ANALOG_POWER, ENABLE);   
#endif
#ifdef USE_POWER_BUTTON
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_POWER, ENABLE);  
#endif

#ifdef USE_USB_HID
#ifndef STM32F10X_CL 
  /* USB_DISCONNECT used as USB pull-up */
  GPIO_InitStructure.GPIO_Pin = USB_DISCONNECT_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
#ifdef USE_STM32_SGN10X
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
#else /* USE_STM32_PRIMER2 or USE_STM32_SGN101 */  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
#endif  
  GPIO_Init(USB_DISCONNECT, &GPIO_InitStructure); 
#endif /* STM32F10X_CL */ 
#endif
     
#ifdef USE_VBAT_MONITOR     
  /* Configure VBAT_AIN as analog input */
  GPIO_InitStructure.GPIO_Pin = GPIO_VBAT_AIN_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIO_VBAT_AIN, &GPIO_InitStructure);
#endif  
 
#ifdef USE_ECG_MONITOR 
  /* Configure IA_AIN as analog input */ 
  GPIO_InitStructure.GPIO_Pin = GPIO_IA_AIN_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIO_IA_AIN, &GPIO_InitStructure);
  
  /* Configure ECG_AIN as analog input */ 
  GPIO_InitStructure.GPIO_Pin = GPIO_ECG_AIN_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIO_ECG_AIN, &GPIO_InitStructure);
#endif  

#ifdef USE_DAC_SIGNALGEN 
  /* Configure SIGNALGEN_AIN as analog input */
  GPIO_InitStructure.GPIO_Pin = GPIO_SIGNALGEN_AIN_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIO_SIGNALGEN_AIN, &GPIO_InitStructure);
  
  /* Once the DAC channel is enabled, the corresponding GPIO pin is automatically 
     connected to the DAC converter. In order to avoid parasitic consumption, 
     the GPIO pin should be configured in analog */
  GPIO_InitStructure.GPIO_Pin = GPIO_SIGNALGEN_AOUT_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIO_SIGNALGEN_AOUT, &GPIO_InitStructure);  
#endif  

#ifdef USE_DAC_IAREF  
  /* Once the DAC channel is enabled, the corresponding GPIO pin is automatically 
     connected to the DAC converter. In order to avoid parasitic consumption, 
     the GPIO pin should be configured in analog */ 
  GPIO_InitStructure.GPIO_Pin = GPIO_IAREF_AOUT_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIO_IAREF_AOUT, &GPIO_InitStructure);
#endif

#ifdef USE_DAC_VGND  
  /* Once the DAC channel is enabled, the corresponding GPIO pin is automatically 
     connected to the DAC converter. In order to avoid parasitic consumption, 
     the GPIO pin should be configured in analog */ 
  GPIO_InitStructure.GPIO_Pin = GPIO_VGND_AOUT_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIO_VGND_AOUT, &GPIO_InitStructure);
#endif
 
#ifdef USE_ANALOG_MUX 
  /* Configure MUX-3 Control Pins as output push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_MUX_3_A_PIN | GPIO_MUX_3_B_PIN | GPIO_MUX_3_C_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIO_MUX_123, &GPIO_InitStructure);

  /* Configure MUX-1,2 Control Pins as output push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_MUX_12_A_PIN | GPIO_MUX_12_B_PIN | GPIO_MUX_12_C_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIO_MUX_123, &GPIO_InitStructure);

  /* Configure MUX-1,2,3 INH Pins as output push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_MUX_123_INH_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIO_MUX_123, &GPIO_InitStructure);
#endif

#ifdef USE_LED
  /* Configure the Led Pin as Output PP */
  GPIO_InitStructure.GPIO_Pin = GPIO_LED_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIO_LED, &GPIO_InitStructure);
#ifdef USE_STM32_SGN10X
  GPIO_SetBits(GPIO_LED, GPIO_LED_PIN); //Led Off
#else /* USE_STM32_PRIMER2 or USE_STM32_SGN101 */
  GPIO_ResetBits(GPIO_LED, GPIO_LED_PIN); //Led Off
#endif  
#endif
    
#ifdef USE_SYSTEM_SHUTDOWN    
  /* Configure Shutdown Pin as output push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_SHUTDOWN_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIO_SHUTDOWN, &GPIO_InitStructure);
#endif  
  
#ifdef USE_ANALOG_POWER_SWITCH
  /* Configure Analog Power Switch Pin as output push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_ANALOG_POWER_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIO_ANALOG_POWER, &GPIO_InitStructure);
#endif

#ifdef USE_POWER_BUTTON
  /* Configure the Power key as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_POWER_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIO_POWER, &GPIO_InitStructure);
  
  /* Connect Power Button to the corresponding EXTI line */
  GPIO_EXTILineConfig(GPIO_POWER_PORTSOURCE, GPIO_POWER_PINSOURCE);
#endif
}

#ifdef USE_POWER_BUTTON
/*******************************************************************************
* Function Name  : POWER_EXTI_Configuration
* Description    : Enable or disable the POWER button EXTI line
* Input          : NewState: This parameter can be ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void POWER_EXTI_Configuration(FunctionalState NewState)
{
  EXTI_InitTypeDef EXTI_InitStructure;

  /* Initializes the EXTI_InitStructure */
  EXTI_StructInit(&EXTI_InitStructure);

  /* Disable the POWER button EXTI line */
  if(NewState == DISABLE)
  {
    EXTI_InitStructure.EXTI_Line = GPIO_POWER_EXTI_Line;
    EXTI_InitStructure.EXTI_LineCmd = DISABLE;
    EXTI_Init(&EXTI_InitStructure);
  }
  /* Enable the Power button EXTI line on rising edge */
  else
  {
    /* Clear the POWER button EXTI line interrupt pending bit */
    EXTI_ClearITPendingBit(GPIO_POWER_EXTI_Line);

    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Line = GPIO_POWER_EXTI_Line;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
  }
}
#endif

#ifdef USE_RTC
/**
  * @brief  Configures the RTC.
  * @param  None
  * @retval None
  */
void RTC_Configuration(void)
{
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Reset Backup Domain */
  BKP_DeInit();

  /* Enable LSE */
  RCC_LSEConfig(RCC_LSE_ON);
  /* Wait till LSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {}

  /* Select LSE as RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  /* Enable RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC registers synchronization */
  RTC_WaitForSynchro();

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Enable the RTC Second */
  RTC_ITConfig(RTC_IT_SEC, ENABLE);

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Set RTC prescaler: set RTC period to 1sec */
  RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
}
#endif

/**
  * @brief  Configure a SysTick Base time to 10 ms.
  * @param  None
  * @retval None
  */
void SysTick_Configuration(void)
{
  /* Setup SysTick Timer for 10 msec interrupts  */
  if (SysTick_Config(SystemCoreClock / 100))    //1000 for 1 msec
  {
    /* Capture error */
    while (1);
  }

  // /* Configure the SysTick handler priority */
  // NVIC_SetPriority(SysTick_IRQn, 0x0);
  /* Set SysTick interrupt vector Preemption Priority to 0 & Subpriority to 2 */
  NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0,2));
}

/**
  * @brief  Inserts a delay time.
  * @param  nCount: specifies the delay time length (time base 10 ms).
  * @retval None
  */
void Delay(__IO uint32_t nCount)
{
    TimingDelay = nCount;

    /* Enable the SysTick Counter */
    SysTick->CTRL |= SysTick_CTRL_ENABLE;

    while(TimingDelay != 0);

    /* Disable the SysTick Counter */
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE;

    /* Clear the SysTick Counter */
    SysTick->VAL = (uint32_t)0x0;
}

// /**
  // * @brief  Inserts a delay time while no joystick MENU, UP, DOWN and SEL are pressed.
  // * @param  nTount: specifies the delay time length (time base 10 ms).
  // * @retval Pressed Key. This value can be: NOKEY, MENU, UP, DOWN and SEL.
  // */
// uint32_t DelayJoyStick(__IO uint32_t nTime)
// {
  // __IO uint32_t keystate = 0;

  // TimingDelay = nTime;

  // /* Enable the SysTick Counter */
  // SysTick->CTRL |= SysTick_CTRL_ENABLE;

  // while ((TimingDelay != 0) && (keystate != MENU) && (keystate != UP) && (keystate != DOWN) && (keystate != SEL))
  // {
    // keystate = Menu_ReadKey();
  // }

  // /* Disable the SysTick Counter */
  // SysTick->CTRL &= ~SysTick_CTRL_ENABLE;

  // /* Clear the SysTick Counter */
  // SysTick->VAL = (uint32_t)0x0;

  // return keystate;
// }

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void Decrement_TimingDelay(void)
{
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}

#ifdef USE_USB_HID
void Device_Config_Report(void)
{
    CONFIG_Buff[0] = ADC_VREF_MV;
    CONFIG_Buff[1] = ADC_RESOLUTION_BITS;
    CONFIG_Buff[2] = ADC_SAMPLING_FREQ;
#ifndef USE_ANALOG_MUX
    CONFIG_Buff[3] = 0;
#endif

    if (PrevXferComplete)
    {
        CustomHIDUserToPMABufferCopy(&Config_Report_Id, CONFIG_Buff, ENDP1_TXADDR, 10);
        SetEPTxCount(ENDP1, 10);
        #ifndef STM32F10X_CL
        SetEPTxValid(ENDP1);
        #endif /* STM32F10X_CL */
        PrevXferComplete = 0;
    }
}

#ifdef USE_DEBUG_REPORT
void Device_Debug_Report(void)
{
    if (PrevXferComplete)
    {
        CustomHIDUserToPMABufferCopy(&Debug_Report_Id, DEBUG_Buff, ENDP1_TXADDR, 64);
        SetEPTxCount(ENDP1, 64);
        #ifndef STM32F10X_CL
        SetEPTxValid(ENDP1);
        #endif /* STM32F10X_CL */
        PrevXferComplete = 0;
    }
}
#endif
#endif

#ifdef USE_VBAT_MONITOR
void ADC_Configuration(FunctionalState NewState)
{
    if(NewState == ENABLE)
    {
        ADC_InitTypeDef ADC_InitStructure;
     
#if defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL) || defined (STM32F10X_HD_VL)
        /* ADCCLK = PCLK2/2 */
        RCC_ADCCLKConfig(RCC_PCLK2_Div2);
#else
        /* ADCCLK = PCLK2/4 */
        RCC_ADCCLKConfig(RCC_PCLK2_Div4);
#endif
    
        /* Enable ADC1 clock */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
     
        /* ADC1 configuration */
        ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
        ADC_InitStructure.ADC_ScanConvMode = DISABLE;
        ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
        ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
        ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
        ADC_InitStructure.ADC_NbrOfChannel = 1;
        ADC_Init(ADC1, &ADC_InitStructure);

        /* ADC1 regular channel configuration */ 
        ADC_RegularChannelConfig(ADC1, VBAT_AIN_CHANNEL, 1, ADC_SampleTime_1Cycles5);
    
        /* Enable ADC1 */
        ADC_Cmd(ADC1, ENABLE);

        /* Enable ADC1 reset calibaration register */   
        ADC_ResetCalibration(ADC1);
        
        /* Check the end of ADC1 reset calibration register */
        while(ADC_GetResetCalibrationStatus(ADC1));

        /* Start ADC1 calibaration */
        ADC_StartCalibration(ADC1);

        /* Check the end of ADC1 calibration */
        while(ADC_GetCalibrationStatus(ADC1));
    }
    else
    {
        /* Disable ADC1 */
        ADC_Cmd(ADC1, DISABLE);

        /* Disable ADC1 clock */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);    
    }
}
#endif

#ifdef USE_ADC_ANALOGWATCHDOG
void ADC_AnalogWatchdog_Configuration(FunctionalState NewState)
{
    if(NewState == ENABLE)
    {
        ADC_InitTypeDef ADC_InitStructure;
     
#if defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL) || defined (STM32F10X_HD_VL)
        /* ADCCLK = PCLK2/2 */
        RCC_ADCCLKConfig(RCC_PCLK2_Div2);
#else
        /* ADCCLK = PCLK2/4 */
        RCC_ADCCLKConfig(RCC_PCLK2_Div4);
#endif
    
        /* Enable ADC2 clock */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
     
        /* ADC2 configuration */
        ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
        ADC_InitStructure.ADC_ScanConvMode = DISABLE;
        ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
        ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
        ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
        ADC_InitStructure.ADC_NbrOfChannel = 1;
        ADC_Init(ADC2, &ADC_InitStructure);

        /* ADC2 regular channel configuration */ 
        ADC_RegularChannelConfig(ADC2, VBAT_AIN_CHANNEL, 1, ADC_SampleTime_13Cycles5);

        /* Configure high and low analog watchdog thresholds */
        ADC_AnalogWatchdogThresholdsConfig(ADC2, ANALOGWATCHDOG_HIGH_THRESHOLD, ANALOGWATCHDOG_LOW_THRESHOLD);
        
        /* Configure VBAT_AIN_CHANNEL as the single analog watchdog guarded channel */
        ADC_AnalogWatchdogSingleChannelConfig(ADC2, VBAT_AIN_CHANNEL);
        
        /* Enable analog watchdog on one regular channel */
        ADC_AnalogWatchdogCmd(ADC2, ADC_AnalogWatchdog_SingleRegEnable);

        /* Enable AWD interrupt */
        ADC_ITConfig(ADC2, ADC_IT_AWD, ENABLE);

        /* Enable ADC2 */
        ADC_Cmd(ADC2, ENABLE);

        /* Enable ADC2 reset calibration register */   
        ADC_ResetCalibration(ADC2);
        
        /* Check the end of ADC2 reset calibration register */
        while(ADC_GetResetCalibrationStatus(ADC2));

        /* Start ADC2 calibration */
        ADC_StartCalibration(ADC2);
        
        /* Check the end of ADC2 calibration */
        while(ADC_GetCalibrationStatus(ADC2));

        /* Start ADC2 Software Conversion */ 
        ADC_SoftwareStartConvCmd(ADC2, ENABLE);
    }
    else
    {
        /* Disable ADC2 */
        ADC_Cmd(ADC2, DISABLE);

        /* Disable AWD interrupt */
        ADC_ITConfig(ADC2, ADC_IT_AWD, DISABLE);

        /* Disable ADC2 clock */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, DISABLE);    
    }
}
#endif

#if defined (USE_ECG_MONITOR) || defined (USE_DAC_SIGNALGEN)
/*******************************************************************************
* Function Name : ADC_DMA_TIM_Configuration.
* Description   : Configure the ADC and DMA.
* Input         : NewState: new state.
* Output        : None.
* Return value  : The direction value.
*******************************************************************************/
void ADC_DMA_TIM_Configuration(FunctionalState NewState)
{
    if(NewState == ENABLE)
    {
        ADC_InitTypeDef ADC_InitStructure;
        DMA_InitTypeDef DMA_InitStructure;

        uint16_t TIM3_Prescaler = 0;
        uint16_t TIM3_Autoreload = 0;

        /* Commented since ADC sampling rate is determined by TIM3 */
// #if defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL) || defined (STM32F10X_HD_VL)
        // /* ADCCLK = PCLK2/2 */
        // RCC_ADCCLKConfig(RCC_PCLK2_Div2);
// #else
        // /* ADCCLK = PCLK2/4 */
        // RCC_ADCCLKConfig(RCC_PCLK2_Div4);
// #endif
        
        /* Enable DMA1 clock */
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

        /* Enable ADC1 clock */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

        /* Enable TIM3 clock */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

        /* TIM3 Configuration */
        /* ADC OverSampling Frequency(for 3 additional bits) = 72000000/1/9000 = 64*125Hz  = 8000Hz */
        /* ADC OverSampling Frequency(for 3 additional bits) = 72000000/1/4500 = 64*250Hz  = 16000Hz */
        /* ADC OverSampling Frequency(for 3 additional bits) = 72000000/1/2500 = 64*450Hz  = 28800Hz */
        /* ADC OverSampling Frequency(for 3 additional bits) = 72000000/1/2250 = 64*500Hz  = 32000Hz */
        /* ADC OverSampling Frequency(for 3 additional bits) = 72000000/1/1800 = 64*625Hz  = 40000Hz */
        /* ADC OverSampling Frequency(for 3 additional bits) = 72000000/1/1500 = 64*750Hz  = 48000Hz */
        /* ADC OverSampling Frequency(for 3 additional bits) = 72000000/1/1125 = 64*1000Hz = 64000Hz */
        /* ADC OverSampling Frequency(for 3 additional bits) = 72000000/1/1000 = 64*1125Hz = 72000Hz */
        /* ADC OverSampling Frequency(for 3 additional bits) = 72000000/1/900  = 64*1250Hz = 80000Hz */
        /* ADC OverSampling Frequency(for 3 additional bits) = 72000000/1/750  = 64*1500Hz = 96000Hz */
        /* TIM3_Prescaler: 0 */
        /* TIM3_Autoreload: 8999->8KHz, 4499->16KHz, 2499->28.8KHz, 2249->32KHz, 1799->40KHz,
                            1499->48KHz, 1124->64KHz, 999->72KHz, 899->80KHz, 749->96KHz */
        /* ADC OverSampling Frequency(for 4 additional bits) = 72000000/1/2250 = 256*125Hz  = 32000Hz */
        /* ADC OverSampling Frequency(for 4 additional bits) = 72000000/1/1125 = 256*250Hz  = 64000Hz */
        /* ADC OverSampling Frequency(for 4 additional bits) = 72000000/1/625  = 256*450Hz  = 115200Hz */
        /* ADC OverSampling Frequency(for 4 additional bits) = 72000000/1/450  = 256*625Hz  = 160000Hz */
        /* ADC OverSampling Frequency(for 4 additional bits) = 72000000/1/375  = 256*750Hz  = 192000Hz */
        /* ADC OverSampling Frequency(for 4 additional bits) = 72000000/1/250  = 256*1125Hz = 288000Hz */
        /* ADC OverSampling Frequency(for 4 additional bits) = 72000000/1/225  = 256*1250Hz = 320000Hz */
        /* TIM3_Prescaler: 0 */
        /* TIM3_Autoreload: 2249->32KHz, 1124->64KHz, 624->115.2KHz, 449->060KHz, 374->192KHz,
                            249->288KHz, 224->320KHz */
        /* ADC Sampling Frequency = 72000000/7200/80 = 125Hz */
        /* ADC Sampling Frequency = 72000000/7200/40 = 250Hz */
        /* ADC Sampling Frequency = 72000000/7200/20 = 500Hz */
        /* ADC Sampling Frequency = 72000000/7200/16 = 625Hz */
        /* ADC Sampling Frequency = 72000000/7200/10 = 1000Hz */
        /* ADC Sampling Frequency = 72000000/7200/8  = 1250Hz */
        /* ADC Sampling Frequency = 72000000/7200/5  = 2000Hz */
        /* TIM3_Prescaler: 7199 */
        /* TIM3_Autoreload: 79->125Hz, 39->250Hz, 19->500Hz, 15->625Hz, 9->1KHz, 7->1.25KHz, 4->2KHz */        
#ifdef USE_ADC_OVERSAMPLING            
        TIM3_Prescaler = 0;
        TIM3_Autoreload = (SystemCoreClock / (ADC_OVERSAMPLING_FACTOR * ADC_SAMPLING_FREQ)) - 1;        
#else
        TIM3_Prescaler = (SystemCoreClock / 10000) - 1;
        TIM3_Autoreload = (10000 / ADC_SAMPLING_FREQ) - 1;      
#endif            
        TIM_PrescalerConfig(TIM3, TIM3_Prescaler, TIM_PSCReloadMode_Update);
        TIM_SetAutoreload(TIM3, TIM3_Autoreload);

        /* TIM3 TRGO selection */
        TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);

        /* DMA1 channel1 configuration ---------------------------------------------*/
        DMA_DeInit(DMA1_Channel1);
        DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
#ifdef USE_ADC_OVERSAMPLING
        DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&OVS_DATA_Buff[0];
        DMA_InitStructure.DMA_BufferSize = OVS_DMA_DATA_BUFF_SIZE;
#else
        DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&DATA_Buff[0];
        DMA_InitStructure.DMA_BufferSize = DMA_DATA_BUFF_SIZE;  
#endif
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
        DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
        DMA_InitStructure.DMA_Priority = DMA_Priority_High;
        DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
        DMA_Init(DMA1_Channel1, &DMA_InitStructure);

        /* Enable the DMA1 Channel1 Half Transfer & Transfer complete interrupt */
        DMA_ITConfig(DMA1_Channel1, DMA_IT_HT | DMA_IT_TC, ENABLE);

        /* Clear the DMA1 Channel1 Half Transfer & Transfer complete Flag */
        DMA_ClearFlag(DMA1_FLAG_HT1 | DMA1_FLAG_TC1);

        /* Enable DMA1 channel1 */
        DMA_Cmd(DMA1_Channel1, ENABLE);

        /* ADC1 configuration ------------------------------------------------------*/
        ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
        ADC_InitStructure.ADC_ScanConvMode = DISABLE;
        ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
        ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;
        ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
        ADC_InitStructure.ADC_NbrOfChannel = 1;
        ADC_Init(ADC1, &ADC_InitStructure);

        /* ADC1 regular channels configuration */ 
        ADC_RegularChannelConfig(ADC1, ADC_Channel, 1, ADC_SampleTime_1Cycles5);
        
        /* Enable ADC1 DMA */
        ADC_DMACmd(ADC1, ENABLE);

        /* Enable ADC1 external trigger */
        ADC_ExternalTrigConvCmd(ADC1, ENABLE);

        /* Enable ADC1 */
        ADC_Cmd(ADC1, ENABLE);

        /* Enable ADC1 reset calibaration register */   
        ADC_ResetCalibration(ADC1);
        
        /* Check the end of ADC1 reset calibration register */
        while(ADC_GetResetCalibrationStatus(ADC1));

        /* Start ADC1 calibaration */
        ADC_StartCalibration(ADC1);

        /* Check the end of ADC1 calibration */
        while(ADC_GetCalibrationStatus(ADC1));  

        /* TIM3 enable counter */
        TIM_Cmd(TIM3, ENABLE);
    }
    else
    {
        /* TIM3 disable counter */
        TIM_Cmd(TIM3, DISABLE);

        /* Disable ADC1 */
        ADC_Cmd(ADC1, DISABLE);

        /* Disable ADC1 external trigger */
        ADC_ExternalTrigConvCmd(ADC1, DISABLE);

        /* Disable ADC1 DMA */
        ADC_DMACmd(ADC1, DISABLE);

        /* Disable DMA1 channel1 */
        DMA_Cmd(DMA1_Channel1, DISABLE);

        /* Disable the DMA1 Channel1 Half Transfer & Transfer complete interrupt */
        DMA_ITConfig(DMA1_Channel1, DMA_IT_HT | DMA_IT_TC, DISABLE);
    
        /* Clear the DMA1 Channel1 Half Transfer & Transfer complete Flag */
        DMA_ClearFlag(DMA1_FLAG_HT1 | DMA1_FLAG_TC1);    

        /* Disable TIM3 clock */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, DISABLE);

        /* Disable ADC1 clock */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);

        /* Disable DMA1 clock */
        // RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, DISABLE);    
    }
}

void ADC_DMA_Reset_Data(void)
{
    uint16_t i = 0;

#ifdef USE_ADC_OVERSAMPLING    
    Data_Buff_Index = 0;

    for(i = 0; i < OVS_DMA_DATA_BUFF_SIZE; i++)
        OVS_DATA_Buff[i] = 0;
#endif

    pDataBuff = 0;

    for(i = 0; i < DMA_DATA_BUFF_SIZE; i++)
        DATA_Buff[i] = 0;

#ifdef USE_DSP_FILTER
    pFilterDataBuff = 0;
    
    for(i = 0; i < DATA_BUFF_SIZE; i++)
        FILTER_DATA_Buff[i] = 0; 

    xv[0] = xv[1] = xv[2] = xv[3] = xv[4] = 0;
    yv[0] = yv[1] = yv[2] = yv[3] = yv[4] = 0;
#endif    
}

void ADC_DMA_Process_Data(void)
{
    if(pDataBuff)
    {
#ifdef USE_DSP_FILTER
        if(FILTER_ENABLED)
        {
            pFilterDataBuff = &FILTER_DATA_Buff[0];
            iir_mk_filter(pFilterDataBuff, pDataBuff, DATA_BUFF_SIZE);
        
#ifdef USE_USB_HID
            if (PrevXferComplete)
            {
                /* Write the descriptor through the endpoint */
                /* USB_SIL_Write(EP1_IN, (uint8_t*) Send_Buffer, 2); */  //Use this for STM32F10X_CL devices(mandatory).        
                /* OR */
                /* Use the memory interface function to write to the selected endpoint */
                /* CustomHIDUserToPMABufferCopy(&Filter_Report_Id, pFilterDataBuff, GetEPTxAddr(EP1_IN & 0x7F), DMA_DATA_BUFF_SIZE + 2); */
                /* Update the data length in the control register */
                /* SetEPTxCount((EP1_IN & 0x7F), DMA_DATA_BUFF_SIZE + 2); */
                /* OR */
                CustomHIDUserToPMABufferCopy(&Filter_Report_Id, pFilterDataBuff, ENDP1_TXADDR, DMA_DATA_BUFF_SIZE + 2);      
                SetEPTxCount(ENDP1, DMA_DATA_BUFF_SIZE + 2);
#ifndef STM32F10X_CL        
                SetEPTxValid(ENDP1);
#endif /* STM32F10X_CL */
                PrevXferComplete = 0;
            }            
#endif  /* USE_USB_HID */
        }
        else
        {
#endif  /* USE_DSP_FILTER */   
#ifdef USE_USB_HID
            if (PrevXferComplete)
            {        
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
                PrevXferComplete = 0;
            }            
#endif  /* USE_USB_HID */
#ifdef USE_DSP_FILTER        
        }
#endif  /* USE_DSP_FILTER */   
        pDataBuff = 0;
    }
}
#endif

#if defined (USE_DAC_IAREF) || defined (USE_DAC_VGND)
void DAC_Configuration(FunctionalState NewState)
{
    if(NewState == ENABLE)
    {
        DAC_InitTypeDef DAC_InitStructure;
    
        /* DAC Periph clock enable */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

        DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
        DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
        DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;   //DAC_OutputBuffer_Disable;
#ifdef USE_DAC_IAREF
        DAC_Init(IAREF_AOUT_CHANNEL, &DAC_InitStructure);
        DAC_SetChannel2Data(DAC_Align_12b_R, DAC_IARef_Data);        
        /* Enable DAC Channel2: Once the DAC channel2 is enabled, PA.05 is 
         automatically connected to the DAC converter. */    
        DAC_Cmd(IAREF_AOUT_CHANNEL, ENABLE);  
#endif
#ifdef USE_DAC_VGND    
        DAC_Init(VGND_AOUT_CHANNEL, &DAC_InitStructure);
        DAC_SetChannel1Data(DAC_Align_12b_R, DAC_VGnd_Data);        
        /* Enable DAC Channel1: Once the DAC channel1 is enabled, PA.04 is 
         automatically connected to the DAC converter. */    
        DAC_Cmd(VGND_AOUT_CHANNEL, ENABLE);
#endif        
    }
    else
    {
#ifdef USE_DAC_IAREF    
        /* Disable DAC Channel2 */
        DAC_Cmd(IAREF_AOUT_CHANNEL, DISABLE);  
#endif
#ifdef USE_DAC_VGND
        /* Disable DAC Channel1 */
        DAC_Cmd(VGND_AOUT_CHANNEL, DISABLE);  
#endif    
        /* DAC Periph clock disable */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, DISABLE);    
    }
}
#endif

#ifdef USE_DAC_SIGNALGEN
/*******************************************************************************
* Function Name : DAC_DMA_TIM_Configuration.
* Description   : Configure the DAC to generate Sine wave.
* Input         : NewState: new state.
* Output        : None.
* Return value  : The direction value.
*******************************************************************************/
void DAC_DMA_TIM_Configuration(FunctionalState NewState)
{
    if(NewState == ENABLE)
    {
        DAC_InitTypeDef DAC_InitStructure;
        DMA_InitTypeDef DMA_InitStructure;

#ifdef USE_STM32_PRIMER2
        uint16_t TIM6_Prescaler = 0;
        uint16_t TIM6_Autoreload = 0;
#else /* USE_STM32_SGN101 or USE_STM32_SGN10X */    
        uint16_t TIM7_Prescaler = 0;
        uint16_t TIM7_Autoreload = 0;
#endif    
    
#if !defined STM32F10X_LD_VL && !defined STM32F10X_MD_VL
        /* DMA2 clock enable */
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
#else
        /* DMA1 clock enable */
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
#endif

        /* DAC Periph clock enable */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

#ifdef USE_STM32_PRIMER2
        /* TIM6 Periph clock enable */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

        /* TIM6 Configuration */
        /* Sine Wave Frequency/Sample Rate = 72000000/3750/150/128 = 1 Hz */
        /* TIM6_Prescaler: 3749 */
        /* TIM6_Autoreload: 149->1Hz, 0->150Hz */
        TIM6_Prescaler = (SystemCoreClock / DAC_NPT / 1000) - 1;
        TIM6_Autoreload = (1000 / SineFreqTable[SineFreqIndex]) - 1;        
        TIM_PrescalerConfig(TIM6, TIM6_Prescaler, TIM_PSCReloadMode_Update);
        TIM_SetAutoreload(TIM6, TIM6_Autoreload);

        /* TIM6 TRGO selection */
        TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);

#if !defined STM32F10X_LD_VL && !defined STM32F10X_MD_VL
        /* DMA2 channel3 configuration */
        DMA_DeInit(DMA2_Channel3);
#else
        /* DMA1 channel3 configuration */
        DMA_DeInit(DMA1_Channel3);
#endif
        DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12R1_Address;
        DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&Sine12bit;
        DMA_InitStructure.DMA_BufferSize = DAC_NPT;
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
        DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
        DMA_InitStructure.DMA_Priority = DMA_Priority_High;
        DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
#if !defined STM32F10X_LD_VL && !defined STM32F10X_MD_VL
        DMA_Init(DMA2_Channel3, &DMA_InitStructure);
#else
        DMA_Init(DMA1_Channel3, &DMA_InitStructure);
#endif

#if !defined STM32F10X_LD_VL && !defined STM32F10X_MD_VL
        /* Enable DMA2 Channel3 */
        DMA_Cmd(DMA2_Channel3, ENABLE);
#else
        /* Enable DMA1 Channel3 */
        DMA_Cmd(DMA1_Channel3, ENABLE);
#endif

        /* DAC channel1 Configuration */
        DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
        DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
        DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable; //DAC_OutputBuffer_Disable;
        DAC_Init(SIGNALGEN_AOUT_CHANNEL, &DAC_InitStructure);

        /* Enable DAC Channel1: Once the DAC channel1 is enabled, PA.04 is 
        automatically connected to the DAC converter. */
        DAC_Cmd(SIGNALGEN_AOUT_CHANNEL, ENABLE);

        /* Enable DMA for DAC Channel1 */
        DAC_DMACmd(SIGNALGEN_AOUT_CHANNEL, ENABLE);

        /* TIM6 enable counter */
        TIM_Cmd(TIM6, ENABLE);
#else /* USE_STM32_SGN101 or USE_STM32_SGN10X */
        /* TIM7 Periph clock enable */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);

        /* TIM7 Configuration */
        /* Sine Wave Frequency/Sample Rate = 72000000/3750/150/128 = 1 Hz */
        /* TIM7_Prescaler: 3749 */
        /* TIM7_Autoreload: 149->1Hz, 0->150Hz */
        TIM7_Prescaler = (SystemCoreClock / DAC_NPT / 1000) - 1;
        TIM7_Autoreload = (1000 / SineFreqTable[SineFreqIndex]) - 1;        
        TIM_PrescalerConfig(TIM7, TIM7_Prescaler, TIM_PSCReloadMode_Update);
        TIM_SetAutoreload(TIM7, TIM7_Autoreload);

        /* TIM7 TRGO selection */
        TIM_SelectOutputTrigger(TIM7, TIM_TRGOSource_Update);

#if !defined STM32F10X_LD_VL && !defined STM32F10X_MD_VL
        /* DMA2 channel4 configuration */
        DMA_DeInit(DMA2_Channel4);
#else
        /* DMA1 channel4 configuration */
        DMA_DeInit(DMA1_Channel4);
#endif
        DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12R2_Address;
        DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&Sine12bit;
        DMA_InitStructure.DMA_BufferSize = DAC_NPT;
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
        DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
        DMA_InitStructure.DMA_Priority = DMA_Priority_High;
        DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
#if !defined STM32F10X_LD_VL && !defined STM32F10X_MD_VL
        DMA_Init(DMA2_Channel4, &DMA_InitStructure);
#else
        DMA_Init(DMA1_Channel4, &DMA_InitStructure);
#endif

#if !defined STM32F10X_LD_VL && !defined STM32F10X_MD_VL
        /* Enable DMA2 Channel4 */
        DMA_Cmd(DMA2_Channel4, ENABLE);
#else
        /* Enable DMA1 Channel4 */
        DMA_Cmd(DMA1_Channel4, ENABLE);
#endif

        /* DAC channel2 Configuration */
        DAC_InitStructure.DAC_Trigger = DAC_Trigger_T7_TRGO;
        DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
        DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable; //DAC_OutputBuffer_Disable;
        DAC_Init(SIGNALGEN_AOUT_CHANNEL, &DAC_InitStructure);

        /* Enable DAC Channel2: Once the DAC channel2 is enabled, PA.05 is 
        automatically connected to the DAC converter. */
        DAC_Cmd(SIGNALGEN_AOUT_CHANNEL, ENABLE);

        /* Enable DMA for DAC Channel2 */
        DAC_DMACmd(SIGNALGEN_AOUT_CHANNEL, ENABLE);

        /* TIM7 enable counter */
        TIM_Cmd(TIM7, ENABLE);
#endif /* USE_STM32_PRIMER2 */        
    }
    else
    {
#ifdef USE_STM32_PRIMER2
        /* TIM6 disable counter */
        TIM_Cmd(TIM6, DISABLE);

        /* Disable DMA for DAC Channel1 */
        DAC_DMACmd(SIGNALGEN_AOUT_CHANNEL, DISABLE);

        /* Disable DAC Channel1 */
        DAC_Cmd(SIGNALGEN_AOUT_CHANNEL, DISABLE);

#if !defined STM32F10X_LD_VL && !defined STM32F10X_MD_VL
        /* Disable DMA2 Channel3 */
        DMA_Cmd(DMA2_Channel3, DISABLE);
#else
        /* Disable DMA1 Channel3 */
        DMA_Cmd(DMA1_Channel3, DISABLE);
#endif

        /* TIM6 Periph clock disable */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, DISABLE);
#else /* USE_STM32_SGN101 or USE_STM32_SGN10X */    
        /* TIM7 disable counter */
        TIM_Cmd(TIM7, DISABLE);

        /* Disable DMA for DAC Channel2 */
        DAC_DMACmd(SIGNALGEN_AOUT_CHANNEL, DISABLE);

        /* Disable DAC Channel2 */
        DAC_Cmd(SIGNALGEN_AOUT_CHANNEL, DISABLE);

#if !defined STM32F10X_LD_VL && !defined STM32F10X_MD_VL
        /* Disable DMA2 Channel4 */
        DMA_Cmd(DMA2_Channel4, DISABLE);
#else
        /* Disable DMA1 Channel4 */
        DMA_Cmd(DMA1_Channel4, DISABLE);
#endif

        /* TIM7 Periph clock disable */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, DISABLE);
#endif /* USE_STM32_PRIMER2 */        

        /* DAC Periph clock disable */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, DISABLE);

// #if !defined STM32F10X_LD_VL && !defined STM32F10X_MD_VL
        // /* DMA2 clock disable */
        // RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, DISABLE);
// #else
        // /* DMA1 clock disable */
        // RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, DISABLE);
// #endif
    }
}
#endif

#ifdef USE_VBAT_MONITOR
void Start_Battery_Monitor(void)
{
    uint16_t VBat = 0;
    uint16_t i = 0;
    uint32_t ADCConvertedValue = 0;

    ADC_Configuration(ENABLE);

#ifdef USE_ADC_OVERSAMPLING    
    for(;;)
    {  
        /* Start the conversion */
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);
        /* Wait until conversion completion */
        while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
        /* Convert the conversion values to battery voltage */
        ADCConvertedValue += ADC_GetConversionValue(ADC1);
        if(i == ADC_OVERSAMPLING_FACTOR)
        {
            ADCConvertedValue = (uint16_t)(ADCConvertedValue >> ADC_ADDITIONAL_BITS);
            VBat = (ADCConvertedValue * ADC_VREF_MV) / ADC_DATA_MAX_VALUE;
            VBat *= 2; //Divider bridge  VBAT <-> 1M -<--|-->- 1M <-> GND,        
            break;
        }
        i++;
    }
#else
    /* Start the conversion */
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    /* Wait until conversion completion */
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
    /* Convert the conversion values to battery voltage */
    ADCConvertedValue = ADC_GetConversionValue(ADC1);
    VBat = (ADCConvertedValue * ADC_VREF_MV) / ADC_DATA_MAX_VALUE;
    VBat *= 2; //Divider bridge  VBAT <-> 1M -<--|-->- 1M <-> GND,        
#endif

    /* Display Battery Voltage (VBat) in mV */
#if defined (USE_USB_HID) && defined (USE_DEBUG_REPORT)
    Delay(200);//Delay 2 sec
    sprintf(DEBUG_Buff, "Battery Voltage: %d mV\n", VBat);
    Device_Debug_Report();
#endif

    ADC_Configuration(DISABLE);
}
#endif

#ifdef USE_ANALOG_MUX
void ECG_Lead_Select(uint8_t ECG_Lead)
{
    CONFIG_Buff[3] = ECG_Lead;

    if(ECG_Lead == 0)
    {
        ECG_Lead = 1;
    }

    #ifdef USE_USB_HID
    Data_Report_Id = (uint16_t)(DATA_REPORT_ID + (ECG_Lead << 8));
    #ifdef USE_DSP_FILTER 
    Filter_Report_Id = (uint16_t)(FILTER_REPORT_ID + (ECG_Lead << 8));
    #endif
    #endif

    switch(ECG_Lead)
    {
        case 1:
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_A_PIN, Bit_RESET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_B_PIN, Bit_RESET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_C_PIN, Bit_RESET);
        
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_A_PIN, Bit_SET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_B_PIN, Bit_SET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_C_PIN, Bit_SET);         
            break;
        
        case 2:
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_A_PIN, Bit_SET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_B_PIN, Bit_RESET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_C_PIN, Bit_RESET); 

            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_A_PIN, Bit_SET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_B_PIN, Bit_SET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_C_PIN, Bit_SET);         
            break;
        
        case 3:
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_A_PIN, Bit_RESET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_B_PIN, Bit_SET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_C_PIN, Bit_RESET);

            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_A_PIN, Bit_SET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_B_PIN, Bit_SET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_C_PIN, Bit_SET);         
            break;
        
        case 4:
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_A_PIN, Bit_SET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_B_PIN, Bit_SET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_C_PIN, Bit_RESET);

            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_A_PIN, Bit_SET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_B_PIN, Bit_SET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_C_PIN, Bit_SET);        
            break;
        
        case 5:
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_A_PIN, Bit_RESET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_B_PIN, Bit_RESET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_C_PIN, Bit_SET);  

            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_A_PIN, Bit_SET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_B_PIN, Bit_SET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_C_PIN, Bit_SET);         
            break;
        
        case 6:
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_A_PIN, Bit_SET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_B_PIN, Bit_RESET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_C_PIN, Bit_SET); 

            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_A_PIN, Bit_SET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_B_PIN, Bit_SET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_C_PIN, Bit_SET);         
            break;
        
        case 7:
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_A_PIN, Bit_RESET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_B_PIN, Bit_SET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_C_PIN, Bit_SET);            

            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_A_PIN, Bit_RESET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_B_PIN, Bit_RESET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_C_PIN, Bit_RESET);                    
            break;
        
        case 8:
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_A_PIN, Bit_RESET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_B_PIN, Bit_SET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_C_PIN, Bit_SET);            

            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_A_PIN, Bit_SET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_B_PIN, Bit_RESET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_C_PIN, Bit_RESET);                                
            break;
        
        case 9:
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_A_PIN, Bit_RESET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_B_PIN, Bit_SET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_C_PIN, Bit_SET);            

            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_A_PIN, Bit_RESET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_B_PIN, Bit_SET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_C_PIN, Bit_RESET);                                
            break;
        
        case 10:
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_A_PIN, Bit_RESET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_B_PIN, Bit_SET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_C_PIN, Bit_SET);            

            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_A_PIN, Bit_SET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_B_PIN, Bit_SET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_C_PIN, Bit_RESET);                                
            break;
        
        case 11:
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_A_PIN, Bit_RESET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_B_PIN, Bit_SET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_C_PIN, Bit_SET);            

            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_A_PIN, Bit_RESET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_B_PIN, Bit_RESET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_C_PIN, Bit_SET);                                
            break;
        
        case 12:
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_A_PIN, Bit_RESET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_B_PIN, Bit_SET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_C_PIN, Bit_SET);            

            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_A_PIN, Bit_SET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_B_PIN, Bit_RESET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_C_PIN, Bit_SET);                                
            break;
        
        default:
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_A_PIN, Bit_SET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_B_PIN, Bit_SET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_12_C_PIN, Bit_SET);

            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_A_PIN, Bit_SET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_B_PIN, Bit_SET);
            GPIO_WriteBit(GPIO_MUX_123, GPIO_MUX_3_C_PIN, Bit_SET);         
            break;
    }
}
#endif

#ifdef USE_LED
/*******************************************************************************
* Function Name : Led State.
* Description   : Led On/Off state.
* Input         : NewState: new state.
* Output        : None.
* Return value  : None.
*******************************************************************************/
void Led_State(FunctionalState NewState)
{
    if(NewState == ENABLE)
        GPIO_SetBits(GPIO_LED, GPIO_LED_PIN);
    else
        GPIO_ResetBits(GPIO_LED, GPIO_LED_PIN);    
}
#endif

#ifdef USE_SYSTEM_SHUTDOWN
/*******************************************************************************
* Function Name  : System_Shutdown.
* Description    : Shutdown system.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void System_Shutdown(void)
{
    Delay(100);//Delay 1 sec
    GPIO_SetBits(GPIO_SHUTDOWN, GPIO_SHUTDOWN_PIN);
}
#endif

#ifdef USE_ANALOG_POWER_SWITCH
/*******************************************************************************
* Function Name : Analog_Power.
* Description   : Enable/Disable power for analog circuits(IA, OpAmps etc.).
* Input         : NewState: new state.
* Output        : None.
* Return value  : None.
*******************************************************************************/
void Analog_Power(FunctionalState NewState)
{
    if(NewState == ENABLE)
        GPIO_SetBits(GPIO_ANALOG_POWER, GPIO_ANALOG_POWER_PIN);
    else
        GPIO_ResetBits(GPIO_ANALOG_POWER, GPIO_ANALOG_POWER_PIN);    
}
#endif

#ifdef USE_USB_DFU
void DFU_Start(void)
{
    BKP_WriteBackupRegister(BKP_DR10, 0xFFFF);
#ifdef USE_USB_HID
    USB_Cable_Config(DISABLE);
#endif
    NVIC_SystemReset();
}
#endif

/**
  * @brief  Retargets the C library printf function to the USART or USB.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  //Do nothing for now
  return ch;
}


