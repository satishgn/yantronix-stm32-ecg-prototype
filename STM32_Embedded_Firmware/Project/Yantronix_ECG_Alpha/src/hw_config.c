/*****************************************************************************
* File    : @file
* Company : Yantronix
* Author  : Satish Nair
* Mailto  : satishgn77@gmail.com
*****************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"

// const uint8_t Test_Image[] = { 
    // /* #include "bmp\\Test_Image_32x32.h" */
    // #include "Test_Image_128x64.h"
// };

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#ifdef USE_STM32_SGN101
  #define MESSAGE1   " STM32 High Density " 
  #define MESSAGE2   " Device running on  " 
  #define MESSAGE3   "    STM32-SGN101    " 
// #elif defined USE_STM32_XYZ
  // #define MESSAGE1   "--------------------" 
  // #define MESSAGE2   "--------------------" 
  // #define MESSAGE3   "--------------------"   
#endif

#define ADC1_DR_Address         ((uint32_t)0x4001244C)

#ifdef USE_DAC_SIGNALGEN 
#define DAC_DHR12R2_Address     ((uint32_t)0x40007414)
#endif

#define MAX_FILES   6

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ErrorStatus HSEStartUpStatus;

FRESULT res; //FatFs function common result code 
FATFS FATFS_Obj; //Work area (file system object) for logical drives 
FIL fil_obj; //file object
BYTE fileReadBuffer[20];
UINT bytesRead;
UINT bytesWritten;
FIL DATA_LOG_FIL_Obj; //data log file object
__IO uint16_t DATA_LOG_Read_Buff[DMA_DATA_BUFF_SIZE];
UINT dataLogBytesReadHT = 0;
UINT dataLogBytesReadTC = 0;
UINT dataLogBytesWritten = DMA_DATA_BUFF_SIZE;
uint16_t DATA_LOGGING_ENABLED = 0;
uint16_t MEM_DMA_TIM_ENABLED = 0;
#ifdef LCD_DEBUG_MESSAGE
uint8_t LCD_Display_Buff[20];
#endif
  
// __IO uint16_t ADCConvertedValue;  
uint8_t Data_Channel[DATA_CHANNELS];
uint8_t Data_Channel_Trace_Enabled[DATA_CHANNELS];
uint8_t Data_Channel_Filter_Enabled[DATA_CHANNELS];
__IO uint8_t Data_Channel_Index = 0;
uint16_t ADC_Sampling_Freq = ADC_SAMPLING_FREQ_125;
__IO uint16_t DATA_Buff[DMA_DATA_BUFF_SIZE];
#ifdef USE_ADC_OVERSAMPLING    
__IO uint16_t OVS_DATA_Buff[OVS_DMA_DATA_BUFF_SIZE];
#endif
uint16_t ADC_DMA_TIM_ENABLED = 1;

uint16_t traceXPos = 1;
uint16_t traceYPos = 1;
uint16_t traceWidth = 93;
uint16_t traceHeight = 62;  
uint16_t traceBackColor = LCD_COLOR_WHITE;
uint16_t traceColor = LCD_COLOR_BLACK;

__IO uint16_t prevData[DATA_CHANNELS];    //traceYPos + traceHeight/2;
__IO uint16_t traceOffset = 1;  //traceXPos;
  
uint16_t DETECT_DISPLAY_HR_ENABLED = 0;
uint16_t QRS_Upper_Threshold = 37;                      // there is a historysis to avoid noise
uint16_t QRS_Lower_Threshold = 37;
uint16_t QRS_Detect = 0;
uint16_t QRS_Interval = 0;
uint8_t Heart_Rate = 0;
  
const uint16_t HeartSymbol_SET_9x7_Table[] = {
    /* SHL/COM Output Scan Direction: $C0 Normal COM0->COM63 */
    0x60, 0xF0, 0xF8, 0x7C, 0x3E, 0x7C, 0xF8, 0xF0, 0x60
};

const uint16_t HeartSymbol_RESET_9x7_Table[] = {
    /* SHL/COM Output Scan Direction: $C0 Normal COM0->COM63 */
    0x60, 0x90, 0x88, 0x44, 0x22, 0x44, 0x88, 0x90, 0x60
};

#ifdef USE_DAC_SIGNALGEN 
uint16_t DAC_DMA_TIM_ENABLED = 1;
const uint16_t SineFreqTable[6] = {1, 5, 10, 20, 25, 50};
uint16_t SineFreqIndex = 0; //Default 1 Hz
const uint16_t Sine12bit[DAC_NPT] = { 2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056,
                                 3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 909, 
                                 599, 344, 155, 38, 0, 38, 155, 344, 599, 909, 1263, 1647 };
#endif  
#if defined (USE_DAC_VGND) || defined (USE_DAC_IA_REF)
__IO uint16_t DAC_Channel1_Data = 2048; //Virtual Ground Half Supply
#endif
                             
static __IO uint32_t TimingDelay = 0;
   
__IO uint8_t SD_CARD_DETECTED = 1;    //SD Card Not Present
__IO uint8_t USB_HOST_DETECTED = 0;   //USB Host Not Connected
__IO uint8_t REGULATOR_FAULT_DETECTED = 1;   //Voltage Regulator OK
                             
uint32_t LCD_BackLight_RTC_Counter = 0;
#ifdef USE_IWDG                              
uint32_t IWDG_ReloadCounter_RTC_Counter = 0;                             
#endif
uint32_t System_Shutdown_RTC_Counter = 0;                             
uint32_t LowPower_StopMode_RTC_Counter = 0;
   
__IO uint8_t STOPMODE_SYSTEM_SHUTDOWN = 0;
__IO uint8_t POWERKEY_SYSTEM_SHUTDOWN = 0;
                             
/* Extern variables ----------------------------------------------------------*/
extern uint16_t FILTER_ENABLED;
extern uint16_t DRAW_LCD_TRACE_ENABLED;
extern uint16_t filter_type;
extern uint16_t scale_factor;
extern uint16_t filter_offset;

extern __IO uint8_t LOW_POWER_STOP_MODE;

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

  /* Configure the EXTI lines */
  EXTI_Configuration();

  /* Configure the systick */    
  SysTick_Configuration();

  /* Initialize the LCD */
#ifdef USE_STM32_SGN101
  LCD_Init();  
// #elif defined USE_STM32_XYZ
  // LCD_Init();  
#endif

  /* Configure LCD Backlight - default no backlight */
  LCD_BackLightConfig();
  
  // LCD_SetBackLight(LCD_BRIGHTNESS_NIL);//BRIGHTNESS_NIL,BRIGHTNESS_LOW,BRIGHTNESS_MEDIUM,BRIGHTNESS_HIGH,BRIGHTNESS_FULL
  
  // LCD_Clear(LCD_COLOR_WHITE);
  // LCD_DisplayStringLine(LCD_LINE_0, MESSAGE1);
  // LCD_DisplayStringLine(LCD_LINE_1, MESSAGE2);
  // LCD_DisplayStringLine(LCD_LINE_2, MESSAGE3);
  
  // /* extern const uint8_t Test_Image[]; */
  // LCD_DisplayImage(Test_Image, 0, 0, 128, 64); /* 128x64 Image Size */
  
  // LCD_DrawRect(0, 0, LCD_WIDTH, LCD_HEIGHT, LCD_COLOR_BLACK);
  // LCD_DrawLine(0, LCD_HEIGHT/2, LCD_WIDTH, LCD_HEIGHT/2, LCD_COLOR_BLACK);
  // LCD_DrawLine(LCD_WIDTH/2, 0, LCD_WIDTH/2, LCD_HEIGHT, LCD_COLOR_BLACK);
  // LCD_DrawLine(0, 0, LCD_WIDTH/2, LCD_HEIGHT/2, LCD_COLOR_BLACK);
  // LCD_DrawLine(0, LCD_HEIGHT/2, LCD_WIDTH/2, 0, LCD_COLOR_BLACK);  
  // LCD_FillRect(5*LCD_WIDTH/8, LCD_HEIGHT/8, LCD_WIDTH/4, LCD_HEIGHT/4, LCD_COLOR_BLACK);
  // LCD_DrawCircle(3*LCD_WIDTH/4, 3*LCD_HEIGHT/4, LCD_HEIGHT/4-1, LCD_COLOR_BLACK);
  // LCD_FillCircle(3*LCD_WIDTH/4, 3*LCD_HEIGHT/4, LCD_HEIGHT/4-8, LCD_COLOR_BLACK);

  /* Clear the LCD */ 
  LCD_Clear(LCD_COLOR_WHITE);
 
  /* If HSE is not detected at program startup */
  /* if(HSEStartUpStatus == ERROR) */
  if(RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET)
  {
    /* Generate NMI exception */
    SCB->ICSR |= SCB_ICSR_NMIPENDSET;
  }

  PowerKeyIntExtOnOffConfig(ENABLE);
#ifdef REGULATOR_FAULT
  RegulatorFaultIntExtOnOffConfig(ENABLE);
#endif
#ifdef SD_CARD_DETECT
  SDCardDetectIntExtOnOffConfig(ENABLE);
#endif
#ifdef USB_HOST_DETECT
  USBHostDetectIntExtOnOffConfig(ENABLE);
#endif

#ifdef REGULATOR_FAULT
  /* Low Battery - Regulator fault detect */
  Regulator_Fault();
#endif

  if(!REGULATOR_FAULT_DETECTED)
  {
    LCD_Clear(LCD_COLOR_WHITE);

    LCD_DisplayStringLine(LCD_LINE_0, "====================");   
    LCD_DisplayStringLine(LCD_LINE_1, "   Low Battery!!!   ");    
    LCD_DisplayStringLine(LCD_LINE_2, "====================");   
  
    Delay(500);
    System_Shutdown();
  }
  else
  {
    Set_LCD_BackLight_Time();
  }

  /* Display the STM32 introduction */
  Menu_STM32Intro();

  /* Clear the LCD */ 
  LCD_Clear(LCD_COLOR_WHITE);

  /* Initialize the Calendar */
  Calendar_Init();

#ifdef USE_IWDG 
  /* Enable Independent watchdog */
  Enable_IWDG();
#endif

  /* Initialize the Low Power application */
  LowPower_Init();

  Set_LowPower_StopMode_Time();

  /* Initialize & mount SDcard filesystem */
  disk_initialize(0); //initialize the disk hardware
  f_mount(0, &FATFS_Obj); //register a work area

  /* Initialize the Menu */
  Menu_Init();

  /* Display the main menu icons */
  Menu_ShowMenuIcons();
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
#else
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
  
  /* Enable the RTC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  
#ifdef USE_SD_CARD_SDIO_INTERFACE
#if defined(STM32F10X_HD) || defined(STM32F10X_XL)  
  /* Enable the SDIO Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif
#endif

  /* PreemptionPriority = 0 & SubPriority = 2 has been reserved for SysTick Interrupt (SysTick_IRQn) */

#ifdef USE_USB_HID
#ifdef STM32F10X_CL
  /* Enable the USB Interrupts */
  NVIC_InitStructure.NVIC_IRQChannel = OTG_FS_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);   
#else
  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif /* STM32F10X_CL */
#endif

  /* Enable the RTC Alarm Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the ADC interrupt */
#if defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL) || defined (STM32F10X_HD_VL)
  NVIC_InitStructure.NVIC_IRQChannel = ADC1_IRQn;
#else
  NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
#endif
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);    

  /* Enable the DMA1 Channel1 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the DMA1 Channel7 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the EXTI0 Interrupt (POWER key) */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

#ifdef REGULATOR_FAULT
  /* Enable the EXTI15_10 Interrupt (REGULATOR_FAULT) */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif

  /* Enable the EXTI9_5 Interrupt (UP/DOWN key, SD_CARD_DETECT, USB_HOST_DETECT) */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the EXTI1 Interrupt (MENU key) */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the EXTI2 Interrupt (SEL key) */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);    
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
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_POWER | RCC_APB2Periph_GPIO_MENU | RCC_APB2Periph_GPIO_SEL | 
                         RCC_APB2Periph_GPIO_UP | RCC_APB2Periph_GPIO_DOWN, ENABLE);   
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_VBAT_AIN | RCC_APB2Periph_GPIO_ECG_AIN, ENABLE);  
#ifdef USE_DAC_SIGNALGEN 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_SIGNALGEN_AIN, ENABLE);  
#endif
#ifdef USE_DAC_VGND  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_VGND_AOUT, ENABLE);   
#endif
#ifdef USE_DAC_IA_REF  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_IA_REF_AOUT, ENABLE);   
#endif
#ifdef USE_DAC_SIGNALGEN 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_SIGNALGEN_AOUT, ENABLE);  
#endif
#ifdef USE_BUZZER
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_BUZZER, ENABLE);
#endif
#ifdef USE_LED  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_LED, ENABLE); 
#endif  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_SHUTDOWN, ENABLE);   
#ifdef USE_ANALOG_POWER_SWITCH
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_ANALOG_POWER, ENABLE);   
#endif
#ifdef SD_CARD_DETECT
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_SD_CARD_DETECT, ENABLE);   
#endif
#ifdef USB_HOST_DETECT
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_USB_HOST_DETECT, ENABLE);   
#endif
#ifdef REGULATOR_FAULT
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_REGULATOR_FAULT, ENABLE);   
#endif

#ifdef USE_USB_HID
#ifndef STM32F10X_CL 
  /* USB_DISCONNECT used as USB pull-up */
  GPIO_InitStructure.GPIO_Pin = USB_DISCONNECT_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(USB_DISCONNECT, &GPIO_InitStructure); 
#endif /* STM32F10X_CL */ 
#endif

  /* Configure the Power key as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_POWER;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIO_POWER, &GPIO_InitStructure);
 
  /* Configure the Menu key as IPD */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_MENU;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_Init(GPIO_MENU, &GPIO_InitStructure);
 
  /* Configure Sel Key as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_SEL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_Init(GPIO_SEL, &GPIO_InitStructure);
  
  /* Configure the Up key as IPD */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_Init(GPIO_UP, &GPIO_InitStructure);
  
  /* Configure the Down key as IPD */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_Init(GPIO_DOWN, &GPIO_InitStructure);
    
  /* Configure VBAT_AIN as analog input */
  GPIO_InitStructure.GPIO_Pin = GPIO_VBAT_AIN_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIO_VBAT_AIN, &GPIO_InitStructure);
 
  /* Configure ECG_AIN as analog input */ 
  GPIO_InitStructure.GPIO_Pin = GPIO_ECG_AIN_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIO_ECG_AIN, &GPIO_InitStructure);

#ifdef USE_DAC_SIGNALGEN 
  /* Configure SIGNALGEN_AIN as analog input */
  GPIO_InitStructure.GPIO_Pin = GPIO_SIGNALGEN_AIN_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIO_SIGNALGEN_AIN, &GPIO_InitStructure);
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

#ifdef USE_DAC_IA_REF  
  /* Once the DAC channel is enabled, the corresponding GPIO pin is automatically 
     connected to the DAC converter. In order to avoid parasitic consumption, 
     the GPIO pin should be configured in analog */ 
  GPIO_InitStructure.GPIO_Pin = GPIO_IA_REF_AOUT_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIO_IA_REF_AOUT, &GPIO_InitStructure);
#endif    
 
#ifdef USE_DAC_SIGNALGEN 
  /* Once the DAC channel is enabled, the corresponding GPIO pin is automatically 
     connected to the DAC converter. In order to avoid parasitic consumption, 
     the GPIO pin should be configured in analog */
  GPIO_InitStructure.GPIO_Pin = GPIO_SIGNALGEN_AOUT_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIO_SIGNALGEN_AOUT, &GPIO_InitStructure);
#endif  

#ifdef USE_BUZZER
  /* Configure the Buzzer IO as Output PP */
  GPIO_InitStructure.GPIO_Pin = GPIO_BUZZER_PIN ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIO_BUZZER, &GPIO_InitStructure);
  // /* Turn on Buzzer */
  // GPIO_ResetBits(GPIO_BUZZER, GPIO_BUZZER_PIN);
  /* Turn off Buzzer */
  GPIO_SetBits(GPIO_BUZZER, GPIO_BUZZER_PIN);
#endif

#ifdef USE_LED
  /* Configure the LED1 & LED2 IOs as Output PP */
  GPIO_InitStructure.GPIO_Pin = GPIO_LED1_PIN | GPIO_LED2_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIO_LED, &GPIO_InitStructure);
  // /* Turn on LED2 */
  // GPIO_ResetBits(GPIO_LED, GPIO_LED2_PIN);
  /* Turn off LED2 */
  GPIO_SetBits(GPIO_LED, GPIO_LED2_PIN);
#endif
    
  /* Configure Shutdown Pin as output push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_SHUTDOWN_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIO_SHUTDOWN, &GPIO_InitStructure);

#ifdef USE_ANALOG_POWER_SWITCH
  /* Configure Analog Power Switch Pin as output push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_ANALOG_POWER_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIO_ANALOG_POWER, &GPIO_InitStructure);
#endif

#ifdef SD_CARD_DETECT
  /* Configure SD Card detect pin as IPU */
  GPIO_InitStructure.GPIO_Pin = GPIO_SD_CARD_DETECT_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIO_SD_CARD_DETECT, &GPIO_InitStructure);
#endif

#ifdef USB_HOST_DETECT
  /* Configure USB host detect pin as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_USB_HOST_DETECT_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIO_USB_HOST_DETECT, &GPIO_InitStructure);
#endif

#ifdef REGULATOR_FAULT
  /* Configure Regulator fault pin as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_REGULATOR_FAULT_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIO_REGULATOR_FAULT, &GPIO_InitStructure);
#endif
}

/*******************************************************************************
* Function Name : EXTI_Configuration.
* Description   : Configure the EXTI lines.
* Input         : None.
* Output        : None.
* Return value  : The direction value.
*******************************************************************************/
void EXTI_Configuration()
{
  EXTI_InitTypeDef EXTI_InitStructure;
  
  /* Connect Keys/Buttons to the corresponding EXTI lines */
  GPIO_EXTILineConfig(GPIO_POWER_PORTSOURCE, GPIO_POWER_PINSOURCE);
  GPIO_EXTILineConfig(GPIO_MENU_PORTSOURCE, GPIO_MENU_PINSOURCE);
  GPIO_EXTILineConfig(GPIO_SEL_PORTSOURCE, GPIO_SEL_PINSOURCE);
  GPIO_EXTILineConfig(GPIO_UP_PORTSOURCE, GPIO_UP_PINSOURCE);
  GPIO_EXTILineConfig(GPIO_DOWN_PORTSOURCE, GPIO_DOWN_PINSOURCE);
#ifdef SD_CARD_DETECT
  GPIO_EXTILineConfig(GPIO_SD_CARD_DETECT_PORTSOURCE, GPIO_SD_CARD_DETECT_PINSOURCE);
#endif
#ifdef USB_HOST_DETECT
  GPIO_EXTILineConfig(GPIO_USB_HOST_DETECT_PORTSOURCE, GPIO_USB_HOST_DETECT_PINSOURCE);
#endif
#ifdef REGULATOR_FAULT
  GPIO_EXTILineConfig(GPIO_REGULATOR_FAULT_PORTSOURCE, GPIO_REGULATOR_FAULT_PINSOURCE);
#endif
    
  /* Enable the EXTI Line17 Interrupt - wakeup from Stop mode via RTCAlarm interrupt */ 
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Line = EXTI_Line17;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure); 
  
  // IntExtOnOffConfig(DISABLE);
  // PowerKeyIntExtOnOffConfig(DISABLE);
// #ifdef REGULATOR_FAULT
  // RegulatorFaultIntExtOnOffConfig(DISABLE);
// #endif
// #ifdef SD_CARD_DETECT
  // SDCardDetectIntExtOnOffConfig(DISABLE);
// #endif
// #ifdef USB_HOST_DETECT
  // USBHostDetectIntExtOnOffConfig(DISABLE);
// #endif
}

/*******************************************************************************
* Function Name  : IntExtOnOffConfig
* Description    : Enables or disables EXTI for the menu navigation keys :
*                  EXTI lines corresponding to "MENU", "SEL", "UP" and "DOWN".
* Input          : NewState: New state of the navigation keys. This parameter
*                  can be: ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void IntExtOnOffConfig(FunctionalState NewState)
{
  EXTI_InitTypeDef EXTI_InitStructure;

  /* Initializes the EXTI_InitStructure */
  EXTI_StructInit(&EXTI_InitStructure);

  /* Disable the MENU, SEL, UP & DOWN EXTI lines */
  if(NewState == DISABLE)
  {
    EXTI_InitStructure.EXTI_Line = GPIO_MENU_EXTI_Line | GPIO_SEL_EXTI_Line | GPIO_UP_EXTI_Line | GPIO_DOWN_EXTI_Line;
    EXTI_InitStructure.EXTI_LineCmd = DISABLE;
    EXTI_Init(&EXTI_InitStructure);
  }
  /* Enable the MENU, SEL, UP & DOWN EXTI lines on rising edge */
  else
  {
    /* Clear the MENU, SEL, UP & DOWN EXTI lines interrupt pending bit */
    EXTI_ClearITPendingBit(GPIO_MENU_EXTI_Line | GPIO_SEL_EXTI_Line | GPIO_UP_EXTI_Line | GPIO_DOWN_EXTI_Line);
  
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Line = GPIO_MENU_EXTI_Line | GPIO_SEL_EXTI_Line | GPIO_UP_EXTI_Line | GPIO_DOWN_EXTI_Line;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
  }
}

/*******************************************************************************
* Function Name  : PowerKeyIntExtOnOffConfig
* Description    : Enable or disable the POWER key EXTI line
* Input          : NewState: This parameter can be ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void PowerKeyIntExtOnOffConfig(FunctionalState NewState)
{
  EXTI_InitTypeDef EXTI_InitStructure;

  /* Initializes the EXTI_InitStructure */
  EXTI_StructInit(&EXTI_InitStructure);

  /* Disable the POWER key EXTI line */
  if(NewState == DISABLE)
  {
    EXTI_InitStructure.EXTI_Line = GPIO_POWER_EXTI_Line;
    EXTI_InitStructure.EXTI_LineCmd = DISABLE;
    EXTI_Init(&EXTI_InitStructure);
  }
  /* Enable the Power key EXTI line on rising edge */
  else
  {
    /* Clear the POWER key EXTI line interrupt pending bit */
    EXTI_ClearITPendingBit(GPIO_POWER_EXTI_Line);

    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Line = GPIO_POWER_EXTI_Line;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
  }
}

#ifdef SD_CARD_DETECT
/*******************************************************************************
* Function Name  : SDCardDetectIntExtOnOffConfig
* Description    : Enable or disable the SD Card detect EXTI line
* Input          : NewState: This parameter can be ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void SDCardDetectIntExtOnOffConfig(FunctionalState NewState)
{
  EXTI_InitTypeDef EXTI_InitStructure;

  /* Initializes the EXTI_InitStructure */
  EXTI_StructInit(&EXTI_InitStructure);

  /* Disable the SD Card detect EXTI line */
  if(NewState == DISABLE)
  {
    EXTI_InitStructure.EXTI_Line = GPIO_SD_CARD_DETECT_EXTI_Line;
    EXTI_InitStructure.EXTI_LineCmd = DISABLE;
    EXTI_Init(&EXTI_InitStructure);
  }
  /* Enable the SD Card detect EXTI line on rising and falling edge */
  else
  {
    /* Clear the SD Card detect EXTI line interrupt pending bit */
    EXTI_ClearITPendingBit(GPIO_SD_CARD_DETECT_EXTI_Line);

    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Line = GPIO_SD_CARD_DETECT_EXTI_Line;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
  }
}

uint8_t SD_Card_Detect(void)
{
    Set_LCD_BackLight_Time();
    Set_LowPower_StopMode_Time();

    SD_CARD_DETECTED = GPIO_ReadInputDataBit(GPIO_SD_CARD_DETECT, GPIO_SD_CARD_DETECT_PIN);

    if(!SD_CARD_DETECTED)
    {
        // LCD_Clear(LCD_COLOR_WHITE);

        // LCD_DisplayStringLine(LCD_LINE_0, "====================");   
        // LCD_DisplayStringLine(LCD_LINE_1, " SD Card Inserted.. ");    
        // LCD_DisplayStringLine(LCD_LINE_2, "====================");   
    }
    else
    {
        // LCD_Clear(LCD_COLOR_WHITE);

        // LCD_DisplayStringLine(LCD_LINE_0, "====================");   
        // LCD_DisplayStringLine(LCD_LINE_1, " SD Card Removed... ");    
        // LCD_DisplayStringLine(LCD_LINE_2, "====================");   
    }

    return SD_CARD_DETECTED;
}
#endif

#ifdef USB_HOST_DETECT
/*******************************************************************************
* Function Name  : USBHostDetectIntExtOnOffConfig
* Description    : Enable or disable the USB Host detect EXTI line
* Input          : NewState: This parameter can be ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void USBHostDetectIntExtOnOffConfig(FunctionalState NewState)
{
  EXTI_InitTypeDef EXTI_InitStructure;

  /* Initializes the EXTI_InitStructure */
  EXTI_StructInit(&EXTI_InitStructure);

  /* Disable the USB Host detect EXTI line */
  if(NewState == DISABLE)
  {
    EXTI_InitStructure.EXTI_Line = GPIO_USB_HOST_DETECT_EXTI_Line;
    EXTI_InitStructure.EXTI_LineCmd = DISABLE;
    EXTI_Init(&EXTI_InitStructure);
  }
  /* Enable the USB Host detect EXTI line on rising and falling edge */
  else
  {
    /* Clear the USB Host detect EXTI line interrupt pending bit */
    EXTI_ClearITPendingBit(GPIO_USB_HOST_DETECT_EXTI_Line);

    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Line = GPIO_USB_HOST_DETECT_EXTI_Line;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
  }
}

uint8_t USB_Host_Detect(void)
{
    Set_LCD_BackLight_Time();
    Set_LowPower_StopMode_Time();

    USB_HOST_DETECTED = GPIO_ReadInputDataBit(GPIO_USB_HOST_DETECT, GPIO_USB_HOST_DETECT_PIN);

    if(!USB_HOST_DETECTED)
    {
        // LCD_Clear(LCD_COLOR_WHITE);

        // LCD_DisplayStringLine(LCD_LINE_0, "====================");   
        // LCD_DisplayStringLine(LCD_LINE_1, " USB Host Removed.. ");    
        // LCD_DisplayStringLine(LCD_LINE_2, "====================");   
    }
    else
    {
        // LCD_Clear(LCD_COLOR_WHITE);

        // LCD_DisplayStringLine(LCD_LINE_0, "====================");   
        // LCD_DisplayStringLine(LCD_LINE_1, " USB Host Found...  ");    
        // LCD_DisplayStringLine(LCD_LINE_2, "====================");       
    }

    return USB_HOST_DETECTED;
}
#endif

#ifdef REGULATOR_FAULT
/*******************************************************************************
* Function Name  : RegulatorFaultIntExtOnOffConfig
* Description    : Enable or disable the Regulator fault EXTI line
* Input          : NewState: This parameter can be ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void RegulatorFaultIntExtOnOffConfig(FunctionalState NewState)
{
  EXTI_InitTypeDef EXTI_InitStructure;

  /* Initializes the EXTI_InitStructure */
  EXTI_StructInit(&EXTI_InitStructure);

  /* Disable the Regulator fault EXTI line */
  if(NewState == DISABLE)
  {
    EXTI_InitStructure.EXTI_Line = GPIO_REGULATOR_FAULT_EXTI_Line;
    EXTI_InitStructure.EXTI_LineCmd = DISABLE;
    EXTI_Init(&EXTI_InitStructure);
  }
  /* Enable the Regulator fault EXTI line on falling edge */
  else
  {
    /* Clear the Regulator fault EXTI line interrupt pending bit */
    EXTI_ClearITPendingBit(GPIO_REGULATOR_FAULT_EXTI_Line);

    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Line = GPIO_REGULATOR_FAULT_EXTI_Line;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
  }
}

uint8_t Regulator_Fault(void)
{
    Set_LowPower_StopMode_Time();

    REGULATOR_FAULT_DETECTED = GPIO_ReadInputDataBit(GPIO_REGULATOR_FAULT, GPIO_REGULATOR_FAULT_PIN);

    if(!REGULATOR_FAULT_DETECTED)
    {
        LCD_SetBackLight(LCD_BRIGHTNESS_NIL);
    
        // LCD_Clear(LCD_COLOR_WHITE);

        // LCD_DisplayStringLine(LCD_LINE_0, "====================");   
        // LCD_DisplayStringLine(LCD_LINE_1, "   Low Battery!!!   ");    
        // LCD_DisplayStringLine(LCD_LINE_2, "====================");   
    }

    return REGULATOR_FAULT_DETECTED;
}
#endif

// /**
  // * @brief  Configures the RTC.
  // * @param  None
  // * @retval None
  // */
// void RTC_Configuration(void)
// {
  // /* Enable PWR and BKP clocks */
  // RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  // /* Allow access to BKP Domain */
  // PWR_BackupAccessCmd(ENABLE);

  // /* Reset Backup Domain */
  // BKP_DeInit();

  // /* Enable LSE */
  // RCC_LSEConfig(RCC_LSE_ON);
  // /* Wait till LSE is ready */
  // while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  // {}

  // /* Select LSE as RTC Clock Source */
  // RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  // /* Enable RTC Clock */
  // RCC_RTCCLKCmd(ENABLE);

  // /* Wait for RTC registers synchronization */
  // RTC_WaitForSynchro();

  // /* Wait until last write operation on RTC registers has finished */
  // RTC_WaitForLastTask();

  // /* Enable the RTC Second */
  // RTC_ITConfig(RTC_IT_SEC, ENABLE);

  // /* Wait until last write operation on RTC registers has finished */
  // RTC_WaitForLastTask();

  // /* Set RTC prescaler: set RTC period to 1sec */
  // RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

  // /* Wait until last write operation on RTC registers has finished */
  // RTC_WaitForLastTask();
// }

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

    // /* Enable the SysTick Counter */
    // SysTick->CTRL |= SysTick_CTRL_ENABLE;

    while(TimingDelay != 0);

    // /* Disable the SysTick Counter */
    // SysTick->CTRL &= ~SysTick_CTRL_ENABLE;

    // /* Clear the SysTick Counter */
    // SysTick->VAL = (uint32_t)0x0;
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

void ADC_Configuration(FunctionalState NewState)
{
    if(NewState == ENABLE)
    {
        ADC_InitTypeDef ADC_InitStructure;
     
        /* ADCCLK = PCLK2/4 */
        RCC_ADCCLKConfig(RCC_PCLK2_Div4); 
    
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

        // /* ADC1 regular channel configuration */ 
        // ADC_RegularChannelConfig(ADC1, VBAT_AIN_CHANNEL, 1, ADC_SampleTime_55Cycles5);
    
        // /* Enable ADC1 EOC interupt */
        // ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);

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
            
        // /* Start ADC1 Software Conversion */ 
        // ADC_SoftwareStartConvCmd(ADC1, ENABLE);        
    }
    else
    {
        /* Disable ADC1 */
        ADC_Cmd(ADC1, DISABLE);

        // /* Disable ADC1 EOC interupt */
        // ADC_ITConfig(ADC1, ADC_IT_EOC, DISABLE);

        /* Disable ADC1 clock */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);    
    }
}

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

        // /* Commented since ADC sampling rate is determined by TIM3 */
// #if defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL)
        // /* ADCCLK = PCLK2/2 */
        // RCC_ADCCLKConfig(RCC_PCLK2_Div2); 
// #else
        // /* ADCCLK = PCLK2/6 */
        // RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
// #endif
        
        /* Enable DMA1 clock */
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

        /* Enable ADC1 clock */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

        /* Enable TIM3 clock */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

        /* TIM3 Configuration */
        /* ADC Sampling Frequency = 72000000/7200/80 = 125Hz */
        /* ADC Sampling Frequency = 72000000/7200/40 = 250Hz */
        /* ADC Sampling Frequency = 72000000/7200/20 = 500Hz */
        /* ADC Sampling Frequency = 72000000/7200/10 = 1000Hz */
        /* TIM3_Prescaler: 7199 */
        /* TIM3_Autoreload: 79->125Hz, 39->250Hz, 19->500Hz, 9->1000Hz */
        /* ADC OverSampling Frequency(for 3 additional bits) = 72000000/45/200 = 64*125Hz = 8000Hz */
        /* ADC OverSampling Frequency(for 3 additional bits) = 72000000/45/100 = 64*250Hz = 16000Hz */
        /* ADC OverSampling Frequency(for 3 additional bits) = 72000000/45/50 = 64*500Hz = 32000Hz */
        /* ADC OverSampling Frequency(for 3 additional bits) = 72000000/45/25 = 64*1000Hz = 64000Hz */
        /* TIM3_Prescaler: 44 */
        /* TIM3_Autoreload: 199->8KHz, 99->16KHz, 49->32KHz, 24->64KHz */ 
#ifdef USE_ADC_OVERSAMPLING            
        TIM3_Prescaler = 0;
        TIM3_Autoreload = (SystemCoreClock / (ADC_OVERSAMPLING_FACTOR * ADC_Sampling_Freq)) - 1;        
#else
        TIM3_Prescaler = (SystemCoreClock / 10000) - 1;
        TIM3_Autoreload = (10000 / ADC_Sampling_Freq) - 1;      
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
        ADC_InitStructure.ADC_ScanConvMode = ENABLE;
        ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
        ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;
        ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
        ADC_InitStructure.ADC_NbrOfChannel = DATA_CHANNELS;
        ADC_Init(ADC1, &ADC_InitStructure);

        /* ADC1 regular channels configuration */ 
        for(Data_Channel_Index = 0; Data_Channel_Index < DATA_CHANNELS; Data_Channel_Index++)
        {
            ADC_RegularChannelConfig(ADC1, Data_Channel[Data_Channel_Index], Data_Channel_Index + 1, ADC_SampleTime_1Cycles5);
#ifdef USE_ADC_ANALOGWATCHDOG
            if(Data_Channel[Data_Channel_Index] == VBAT_AIN_CHANNEL)
            {
                /* Configure high and low analog watchdog thresholds */
                ADC_AnalogWatchdogThresholdsConfig(ADC1, ANALOGWATCHDOG_HIGH_THRESHOLD, ANALOGWATCHDOG_LOW_THRESHOLD);
                /* Configure VBAT_AIN_CHANNEL as the single analog watchdog guarded channel */
                ADC_AnalogWatchdogSingleChannelConfig(ADC1, VBAT_AIN_CHANNEL);
                /* Enable analog watchdog on one regular channel */
                ADC_AnalogWatchdogCmd(ADC1, ADC_AnalogWatchdog_SingleRegEnable);

                /* Enable ADC1 AWD interupt */
                ADC_ITConfig(ADC1, ADC_IT_AWD, ENABLE);
            }
#endif        
        }

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

/*******************************************************************************
* Function Name : MEM_DMA_TIM_Configuration.
* Description   : Configure the TIM and DMA for playing Data log file from SDCard
* Input         : NewState: new state.
* Output        : None.
* Return value  : The direction value.
*******************************************************************************/
void MEM_DMA_TIM_Configuration(FunctionalState NewState)
{
    if(NewState == ENABLE)
    {  
        DMA_InitTypeDef DMA_InitStructure;

        uint16_t TIM4_Prescaler = 0;
        uint16_t TIM4_Autoreload = 0;
    
        /* Enable DMA1 clock */
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

        /* Enable TIM4 clock */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

        /* TIM4 Configuration */
        /* TIM4 Update Frequency = 72000000/7200/80 = 125Hz */
        /* TIM4 Update Frequency = 72000000/7200/40 = 250Hz */
        /* TIM4 Update Frequency = 72000000/7200/20 = 500Hz */
        /* TIM4 Update Frequency = 72000000/7200/10 = 1000Hz */
        /* TIM4_Prescaler: 7199 */
        /* TIM4_Autoreload: 79->125Hz, 39->250Hz, 19->500Hz, 9->1000Hz */
        /* Time base configuration */
        // TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
        // TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
        // TIM_TimeBaseStructure.TIM_Prescaler = TIM4_Prescaler;       
        // TIM_TimeBaseStructure.TIM_Period = TIM4_Autoreload;          
        // TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;    
        // TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
        // TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 
        TIM4_Prescaler = (SystemCoreClock / 10000) - 1;
        TIM4_Autoreload = (10000 / (ADC_Sampling_Freq * DATA_CHANNELS)) - 1;
        TIM_PrescalerConfig(TIM4, TIM4_Prescaler, TIM_PSCReloadMode_Update);
        TIM_SetAutoreload(TIM4, TIM4_Autoreload);

        /* DMA1 channel7 configuration ---------------------------------------------*/
        DMA_DeInit(DMA1_Channel7);
        DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&DATA_LOG_Read_Buff[0];
        DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&DATA_Buff[0];
        DMA_InitStructure.DMA_BufferSize = DMA_DATA_BUFF_SIZE;
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
        DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
        DMA_InitStructure.DMA_Priority = DMA_Priority_High;
        DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
        DMA_Init(DMA1_Channel7, &DMA_InitStructure);

        /* Enable the DMA1 Channel7 Half Transfer & Transfer complete interrupt */
        DMA_ITConfig(DMA1_Channel7, DMA_IT_HT | DMA_IT_TC, ENABLE);

        /* Clear the DMA1 Channel7 Half Transfer & Transfer complete Flag */
        DMA_ClearFlag(DMA1_FLAG_HT7 | DMA1_FLAG_TC7);

        /* Enable DMA1 channel7 */
        DMA_Cmd(DMA1_Channel7, ENABLE);

        /* TIM4 Update DMA Request enable */
        TIM_DMACmd(TIM4, TIM_DMA_Update, ENABLE);

        /* TIM4 enable counter */
        TIM_Cmd(TIM4, ENABLE);
    }
    else
    {
        /* TIM4 disable counter */
        TIM_Cmd(TIM4, DISABLE);

        /* TIM4 Update DMA Request disable */
        TIM_DMACmd(TIM4, TIM_DMA_Update, DISABLE);

        /* Disable DMA1 channel7 */
        DMA_Cmd(DMA1_Channel7, DISABLE);

        /* Disable the DMA1 Channel7 Half Transfer & Transfer complete interrupt */
        DMA_ITConfig(DMA1_Channel7, DMA_IT_HT | DMA_IT_TC, DISABLE);

        /* Clear the DMA1 Channel7 Half Transfer & Transfer complete Flag */
        DMA_ClearFlag(DMA1_FLAG_HT7 | DMA1_FLAG_TC7);

        /* Disable TIM4 clock */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, DISABLE);

        // /* Disable DMA1 clock */
        // RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, DISABLE);  
    }
}

#if defined (USE_DAC_VGND) || defined (USE_DAC_IA_REF)
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
#ifdef USE_DAC_VGND      
        DAC_Init(VGND_AOUT_CHANNEL, &DAC_InitStructure);
#endif
#ifdef USE_DAC_IA_REF      
        DAC_Init(IA_REF_AOUT_CHANNEL, &DAC_InitStructure);
#endif
        DAC_SetChannel1Data(DAC_Align_12b_R, DAC_Channel1_Data);
        
        /* Enable DAC Channel1: Once the DAC channel1 is enabled, PA.04 is 
         automatically connected to the DAC converter. */    
#ifdef USE_DAC_VGND      
        DAC_Cmd(VGND_AOUT_CHANNEL, ENABLE);  
#endif
#ifdef USE_DAC_IA_REF      
        DAC_Cmd(IA_REF_AOUT_CHANNEL, ENABLE);  
#endif
    }
    else
    {
        /* Disable DAC Channel1 */
#ifdef USE_DAC_VGND      
        DAC_Cmd(VGND_AOUT_CHANNEL, DISABLE);  
#endif
#ifdef USE_DAC_IA_REF      
        DAC_Cmd(IA_REF_AOUT_CHANNEL, DISABLE);  
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

        uint16_t TIM7_Prescaler = 0;
        uint16_t TIM7_Autoreload = 0;
    
#if !defined STM32F10X_LD_VL && !defined STM32F10X_MD_VL
        /* DMA2 clock enable */
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
#else
        /* DMA1 clock enable */
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
#endif

        /* DAC Periph clock enable */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

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
    }
    else
    {
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

/*******************************************************************************
* Function Name  : System_Shutdown.
* Description    : Shutdown system.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void System_Shutdown(void)
{
    LCD_Clear(LCD_COLOR_WHITE);

    LCD_DisplayStringLine(LCD_LINE_0, "====================");   
    LCD_DisplayStringLine(LCD_LINE_1, "  Shutting down...  ");    
    LCD_DisplayStringLine(LCD_LINE_2, "====================");   

    Set_LCD_BackLight_Time();

    Delay(100);//Delay 1 sec

    /* Unmount SDcard filesystem before shutting down */
    f_mount(0, NULL);   //unregister work area

    // GPIO_WriteBit(GPIO_SHUTDOWN, GPIO_SHUTDOWN_PIN, Bit_SET);
    GPIO_SetBits(GPIO_SHUTDOWN, GPIO_SHUTDOWN_PIN);
}

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
    {
        // GPIO_WriteBit(GPIO_ANALOG_POWER, GPIO_ANALOG_POWER_PIN, Bit_SET);
        GPIO_SetBits(GPIO_ANALOG_POWER, GPIO_ANALOG_POWER_PIN);
    }
    else
    {
        // GPIO_WriteBit(GPIO_ANALOG_POWER, GPIO_ANALOG_POWER_PIN, Bit_RESET);
        GPIO_ResetBits(GPIO_ANALOG_POWER, GPIO_ANALOG_POWER_PIN);    
    }
}
#endif

#ifdef USE_IWDG 
void Enable_IWDG(void)
{
    /* IWDG timeout equal to 20 sec (the timeout may varies due to LSI frequency dispersion) */
    /* Enable write access to IWDG_PR and IWDG_RLR registers */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    /* IWDG counter clock: 40KHz(LSI) / 256 = 156.25 Hz */
    IWDG_SetPrescaler(IWDG_Prescaler_256);

    /* Set counter reload value to 3124 */
    IWDG_SetReload(3124);

    /* Reload IWDG counter */
    // IWDG_ReloadCounter();
    Set_IWDG_ReloadCounter_Time();  //Reload IWDG Counter every 10 secs

    /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
    IWDG_Enable();
}
#endif

#ifdef USE_USB_HID
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

#ifdef USE_USB_DFU
void DFU_Start(void)
{
    /* Clear LCD */
    LCD_Clear(LCD_COLOR_WHITE);        

    IntExtOnOffConfig(DISABLE);

    while(Menu_ReadKey() != NOKEY)
    {
    } 

    // LCD_DisplayStringLine(LCD_LINE_3, "        STM32       ");
    // LCD_DisplayStringLine(LCD_LINE_4, "  Firmware Upgrade  ");

    BKP_WriteBackupRegister(BKP_DR10, 0xFFFF);
#ifdef USE_USB_HID
    USB_Cable_Config(DISABLE);
#endif
    NVIC_SystemReset();
}
#endif

#ifdef USE_USB_MSD
void MSD_Start(void)
{
    /* Clear LCD */
    LCD_Clear(LCD_COLOR_WHITE);        

    IntExtOnOffConfig(DISABLE);

    while(Menu_ReadKey() != NOKEY)
    {
    } 

    // LCD_DisplayStringLine(LCD_LINE_3, "        STM32       ");
    // LCD_DisplayStringLine(LCD_LINE_4, "  USB Mass Storage  ");

    BKP_WriteBackupRegister(BKP_DR10, 0x5000);
#ifdef USE_USB_HID
    USB_Cable_Config(DISABLE);
#endif
    NVIC_SystemReset();
}
#endif

#ifdef USE_USB_VCP
void VCP_Start(void)
{
    /* Clear LCD */
    LCD_Clear(LCD_COLOR_WHITE);        

    IntExtOnOffConfig(DISABLE);

    while(Menu_ReadKey() != NOKEY)
    {
    } 

    // LCD_DisplayStringLine(LCD_LINE_3, "        STM32       ");
    // LCD_DisplayStringLine(LCD_LINE_4, "   USB Virtual Com  ");

    BKP_WriteBackupRegister(BKP_DR10, 0x9000);
#ifdef USE_USB_HID
    USB_Cable_Config(DISABLE);
#endif
    NVIC_SystemReset();
}
#endif

void Test_FatFs(void)
{
  /* Clear LCD */
  LCD_Clear(LCD_COLOR_WHITE);        

  // /* Display text */
  // LCD_DisplayStringLine(LCD_LINE_0, "====================");
  // LCD_DisplayStringLine(LCD_LINE_1, " SDCard FatFs-R0.08 ");
  // LCD_DisplayStringLine(LCD_LINE_2, "===================="); 

  // disk_initialize(0); //initialize the disk hardware
  // /* mount filesystem, store in the instance you just made */
  // f_mount(0, &FATFS_Obj); 

  /* open file with write permission */
  res = f_open(&fil_obj, "log.txt", FA_CREATE_ALWAYS | FA_WRITE | FA_READ); 
  if(res == FR_OK)
    LCD_DisplayStringLine(LCD_LINE_0, "Open log.txt:Success"); 
  else
    LCD_DisplayStringLine(LCD_LINE_0, "Open log.txt:Failed "); 
  
  // /* write text to file */
  // res = f_printf(&fil_obj, "%s\n%s\n%s\n", MESSAGE1, MESSAGE2, MESSAGE3); 
  // if(res != EOF)
    // LCD_DisplayStringLine(LCD_LINE_X, "       Success       "); 
  // else
    // LCD_DisplayStringLine(LCD_LINE_X, "       Failed        "); 

  /* write to file */
  res = f_write(&fil_obj, MESSAGE1, strlen(MESSAGE1), &bytesWritten);
  // res = f_sync(&fil_obj);
  res = f_write(&fil_obj, MESSAGE2, strlen(MESSAGE2), &bytesWritten);
  // res = f_sync(&fil_obj);
  res = f_write(&fil_obj, MESSAGE3, strlen(MESSAGE3), &bytesWritten);
  // res = f_sync(&fil_obj);
  if(res == FR_OK)
    LCD_DisplayStringLine(LCD_LINE_1, "File Logging:Success"); 
  else
    LCD_DisplayStringLine(LCD_LINE_1, "File Logging:Failed "); 

  /* move to top of the file */
  res = f_lseek(&fil_obj, 0);
  // if(res == FR_OK)
    // LCD_DisplayStringLine(LCD_LINE_X, "Seek from 0 :Success"); 
  // else
    // LCD_DisplayStringLine(LCD_LINE_X, "Seek from 0 :Failed "); 
  
  /* read file */
  res = f_read(&fil_obj, fileReadBuffer, sizeof(fileReadBuffer), &bytesRead);
  if(res == FR_OK)
    LCD_DisplayStringLine(LCD_LINE_2, fileReadBuffer); 
  else
    LCD_DisplayStringLine(LCD_LINE_2, "File Reading:Failed "); 

  res = f_read(&fil_obj, fileReadBuffer, sizeof(fileReadBuffer), &bytesRead);
  if(res == FR_OK)
    LCD_DisplayStringLine(LCD_LINE_3, fileReadBuffer); 
  else
    LCD_DisplayStringLine(LCD_LINE_3, "File Reading:Failed "); 

  res = f_read(&fil_obj, fileReadBuffer, sizeof(fileReadBuffer), &bytesRead);
  if(res == FR_OK)
    LCD_DisplayStringLine(LCD_LINE_4, fileReadBuffer); 
  else
    LCD_DisplayStringLine(LCD_LINE_4, "File Reading:Failed "); 
  
  /* close the file, writes cached content to disk */
  res = f_close(&fil_obj);
  if(res == FR_OK)
    LCD_DisplayStringLine(LCD_LINE_5, "File Closing:Success"); 
  else
    LCD_DisplayStringLine(LCD_LINE_5, "File Closing:Failed "); 
  
  res = f_unlink("stm32_logs");  
  if(res == FR_OK)
    LCD_DisplayStringLine(LCD_LINE_6, "Dir Deleting:Success"); 
  else
    LCD_DisplayStringLine(LCD_LINE_6, "Dir Deleting:Failed "); 

  res = f_mkdir("stm32_logs");  
  if(res == FR_OK)
    LCD_DisplayStringLine(LCD_LINE_7, "Dir Creating:Success"); 
  else
    LCD_DisplayStringLine(LCD_LINE_7, "Dir Creating:Failed "); 
  
  // /* unregister work area */
  // f_mount(0, NULL); 
}

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

// /*-----------------------------------------------------------------------*/
// /* Get current time                                                      */
// /*-----------------------------------------------------------------------*/
// DWORD get_fattime ()
// {
	// return	((2008UL-1980) << 25)	      // Year = 2008
			// | (4UL << 21)	      // Month = April
			// | (18L << 16)	      // Day = 18
			// | (16U << 11)	      // Hour = 16
			// | (30U << 5)	      // Min = 30
			// | (0U >> 1)	      // Sec = 0
			// ;
// }

#ifdef USE_DAC_SIGNALGEN 
void Start_SIGNALGEN_Data_Acquisition(void)
{
    char TraceMenu[5][6] = {"+Freq","-Freq","PAUSE","IIROF","EXIT "};
    int TraceMenuItems = 5, TraceMenuItemIndex = 0, MyKey = 0;

    uint8_t dataLogFileName[_MAX_LFN - 1];    //Year(4)+Month(2)+Day(2)+Hour(2)+Min(2)+Sec(2)+Ext(4)+NULL(1)

    /* ADC Configuration */
    Data_Channel[0] = VBAT_AIN_CHANNEL;
    Data_Channel_Trace_Enabled[0] = 0;
    Data_Channel_Filter_Enabled[0] = 0;
    prevData[0] = 0;

    Data_Channel[DATA_CHANNELS - 1] = SIGNALGEN_AIN_CHANNEL;
    Data_Channel_Trace_Enabled[DATA_CHANNELS - 1] = 1;
    Data_Channel_Filter_Enabled[DATA_CHANNELS - 1] = 1;
    prevData[DATA_CHANNELS - 1] = traceYPos + traceHeight/2;

    traceOffset = traceXPos;

    xv[0] = xv[1] = xv[2] = xv[3] = xv[4] = 0;
    yv[0] = yv[1] = yv[2] = yv[3] = yv[4] = 0;

/*
    xv_bp1[0] = xv_bp1[1] = xv_bp1[2] = 0;
    yv_bp1[0] = yv_bp1[1] = yv_bp1[2] = 0;
    xv_bp2[0] = xv_bp2[1] = xv_bp2[2] = 0;
    yv_bp2[0] = yv_bp2[1] = yv_bp2[2] = 0;
    xv_bp3[0] = xv_bp3[1] = xv_bp3[2] = 0;
    yv_bp3[0] = yv_bp3[1] = yv_bp3[2] = 0;
    xv_bp4[0] = xv_bp4[1] = xv_bp4[2] = 0;
    yv_bp4[0] = yv_bp4[1] = yv_bp4[2] = 0;
    xv_bp5[0] = xv_bp5[1] = xv_bp5[2] = 0;
    yv_bp5[0] = yv_bp5[1] = yv_bp5[2] = 0;
    xv_bp6[0] = xv_bp6[1] = xv_bp6[2] = 0;
    yv_bp6[0] = yv_bp6[1] = yv_bp6[2] = 0;
*/

    filter_type = 1;
    scale_factor = 1;
#ifdef USE_ADC_OVERSAMPLING
    filter_offset = 16384;
#else
    filter_offset = 2048;
#endif

    /* Feature Flags */
    DRAW_LCD_TRACE_ENABLED = 1;
    ADC_DMA_TIM_ENABLED = 0;
    DAC_DMA_TIM_ENABLED = 0;
    FILTER_ENABLED = 1;
    DATA_LOGGING_ENABLED = 0;

    DETECT_DISPLAY_HR_ENABLED = 0;

    /* Disable the JoyStick interrupts */
    IntExtOnOffConfig(DISABLE);
    
    /* Clear the LCD */
    LCD_Clear(LCD_COLOR_WHITE);

    if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
    {
        LCD_DisplayStringLine(LCD_LINE_5, "Please Configure    ");
        LCD_DisplayStringLine(LCD_LINE_6, "Time and Date       ");
        LCD_DisplayStringLine(LCD_LINE_7, "Exit: Press Any Key ");

        while (Menu_ReadKey() == NOKEY)
        {
        }

        /* Display Main Menu */
        Menu_ReturnMainMenu();
        // /* Clear the LCD */
        // LCD_Clear(LCD_COLOR_WHITE);
        // /* Display the menu */
        // Menu_DisplayMenu();
        /* Enable the JoyStick interrupts */
        IntExtOnOffConfig(ENABLE);
        return;
    }

    LCD_DrawRect(traceXPos-1, traceYPos-1, LCD_WIDTH, LCD_HEIGHT, LCD_COLOR_BLACK);
    LCD_DrawLine(traceXPos+traceWidth, traceYPos-1, traceXPos+traceWidth, LCD_HEIGHT, LCD_COLOR_BLACK);
    
    for(TraceMenuItemIndex = 0; TraceMenuItemIndex < TraceMenuItems; TraceMenuItemIndex++)
    {
        if(TraceMenuItemIndex == 0)
            LCD_SetTextColor(LCD_COLOR_TEXT_REVERSE);
        else
            LCD_SetTextColor(LCD_COLOR_TEXT_NORMAL);         
        LCD_DisplayStringLineColumn(TraceMenuItems-TraceMenuItemIndex-1, traceXPos+traceWidth+2, TraceMenu[TraceMenuItemIndex], 5);    
        if(TraceMenuItemIndex == 0)
        {
            LCD_DrawRect(traceXPos+traceWidth+1, (TraceMenuItems-TraceMenuItemIndex-1)*8, 32, 9, LCD_COLOR_BLACK);
        }
    }  
    LCD_DrawLine(traceXPos+traceWidth+1, traceYPos-1, LCD_WIDTH, traceYPos-1, LCD_COLOR_BLACK);

    LCD_SetTextColor(LCD_COLOR_TEXT_REVERSE);
    LCD_DisplayStringLineColumn(LCD_LINE_0, traceXPos+traceWidth+2, "   Hz", 5);                    
    if(SineFreqTable[SineFreqIndex]/10)
    {
        LCD_DisplayChar(LCD_LINE_0, LCD_COLUMN_16, ((SineFreqTable[SineFreqIndex]/10)+ 0x30));
        LCD_DisplayChar(LCD_LINE_0, LCD_COLUMN_17, ((SineFreqTable[SineFreqIndex]%10)+ 0x30));
    }
    else
    {
        LCD_DisplayChar(LCD_LINE_0, LCD_COLUMN_17, ((SineFreqTable[SineFreqIndex]%10)+ 0x30));
    }                
    LCD_DrawRect(traceXPos+traceWidth+1, LCD_LINE_0*8, 32, 9, LCD_COLOR_BLACK);

    /* create datetime file name */
    Create_Date_Time_Filename(dataLogFileName, ".dat");
    
    /* open file with write permission */
    res = f_open(&DATA_LOG_FIL_Obj, dataLogFileName, FA_CREATE_ALWAYS | FA_WRITE); 
    
    /*
    LCD_DisplayStringLineColumn(LCD_LINE_0, LCD_COLUMN_0, dataLogFileName, 20);
    if(res == FR_OK)
        LCD_DisplayStringLineColumn(LCD_LINE_1, LCD_COLUMN_0, "Open File : Success", 20); 
    else
        LCD_DisplayStringLineColumn(LCD_LINE_1, LCD_COLUMN_0, "Open File : Failed", 20);   
    */

    if(res == FR_OK)
        DATA_LOGGING_ENABLED = 1;

    if(DATA_LOGGING_ENABLED)
    {
        /* Write Data_Channel[DATA_CHANNELS] to log file */    
        res = f_write(&DATA_LOG_FIL_Obj, Data_Channel, DATA_CHANNELS, &bytesWritten);
        // res = f_sync(&DATA_LOG_FIL_Obj);
    }            
    
    /* Configure the ADC */
    ADC_DMA_TIM_Configuration(ENABLE);
    ADC_DMA_TIM_ENABLED = 1;

    /* Configure the DAC */
    DAC_DMA_TIM_Configuration(ENABLE);  
    DAC_DMA_TIM_ENABLED = 1;

    TraceMenuItemIndex = 0;
    while(1)
    {        
        /* Check which key is pressed */
        MyKey = Menu_ReadKey();

        /* If "MENU" pushbutton is pressed */
        if(MyKey == MENU)
        {
            break;
        }                  
    
        /* If "UP" pushbutton is pressed */
        if(MyKey == UP)
        {
            DRAW_LCD_TRACE_ENABLED = 0;
        
            LCD_SetTextColor(LCD_COLOR_TEXT_NORMAL);
            LCD_DisplayStringLineColumn(TraceMenuItems-TraceMenuItemIndex-1, traceXPos+traceWidth+2, TraceMenu[TraceMenuItemIndex], 5);    
            LCD_DrawRect(traceXPos+traceWidth+1, (TraceMenuItems-TraceMenuItemIndex-1)*8, 32, 9, LCD_COLOR_WHITE);
        
            if(TraceMenuItemIndex > 0)
            {
                TraceMenuItemIndex--;
            }
            else
            {
                TraceMenuItemIndex = (TraceMenuItems - 1);
            }

            LCD_SetTextColor(LCD_COLOR_TEXT_REVERSE);
            LCD_DisplayStringLineColumn(TraceMenuItems-TraceMenuItemIndex-1, traceXPos+traceWidth+2, TraceMenu[TraceMenuItemIndex], 5);    
            LCD_DrawRect(traceXPos+traceWidth+1, (TraceMenuItems-TraceMenuItemIndex-1)*8, 32, 9, LCD_COLOR_BLACK);
            LCD_DrawLine(traceXPos+traceWidth+1, traceYPos-1, LCD_WIDTH, traceYPos-1, LCD_COLOR_BLACK);
        
            DRAW_LCD_TRACE_ENABLED = 1;        
        }

        /* If "DOWN" pushbutton is pressed */
        if(MyKey == DOWN)
        {
            DRAW_LCD_TRACE_ENABLED = 0;
        
            LCD_SetTextColor(LCD_COLOR_TEXT_NORMAL);
            LCD_DisplayStringLineColumn(TraceMenuItems-TraceMenuItemIndex-1, traceXPos+traceWidth+2, TraceMenu[TraceMenuItemIndex], 5);    
            LCD_DrawRect(traceXPos+traceWidth+1, (TraceMenuItems-TraceMenuItemIndex-1)*8, 32, 9, LCD_COLOR_WHITE);

            if(TraceMenuItemIndex >= (TraceMenuItems - 1))
            {
                TraceMenuItemIndex = 0;
            }
            else
            {
                TraceMenuItemIndex++;
            }

            LCD_SetTextColor(LCD_COLOR_TEXT_REVERSE);
            LCD_DisplayStringLineColumn(TraceMenuItems-TraceMenuItemIndex-1, traceXPos+traceWidth+2, TraceMenu[TraceMenuItemIndex], 5);    
            LCD_DrawRect(traceXPos+traceWidth+1, (TraceMenuItems-TraceMenuItemIndex-1)*8, 32, 9, LCD_COLOR_BLACK);
            LCD_DrawLine(traceXPos+traceWidth+1, traceYPos-1, LCD_WIDTH, traceYPos-1, LCD_COLOR_BLACK);
        
            DRAW_LCD_TRACE_ENABLED = 1;        
        }

        /* If "SEL" pushbutton is pressed */
        if(MyKey == SEL)
        {
            DRAW_LCD_TRACE_ENABLED = 0;
        
            if(TraceMenuItemIndex == 0)
            {
                if(SineFreqIndex < 5)
                {
                    /* Stop DAC */
                    TIM_Cmd(TIM7, DISABLE);
                    DAC_Cmd(SIGNALGEN_AOUT_CHANNEL, DISABLE);
                    DAC_DMA_TIM_ENABLED = 0;
                
                    SineFreqIndex++;
                    TIM7->ARR = (1000 / SineFreqTable[SineFreqIndex]) - 1; 
                
                    LCD_SetTextColor(LCD_COLOR_TEXT_REVERSE);
                    LCD_DisplayStringLineColumn(LCD_LINE_0, traceXPos+traceWidth+2, "   Hz", 5);                    
                    if(SineFreqTable[SineFreqIndex]/10)
                    {
                        LCD_DisplayChar(LCD_LINE_0, LCD_COLUMN_16, ((SineFreqTable[SineFreqIndex]/10)+ 0x30));
                        LCD_DisplayChar(LCD_LINE_0, LCD_COLUMN_17, ((SineFreqTable[SineFreqIndex]%10)+ 0x30));
                    }
                    else
                    {
                        LCD_DisplayChar(LCD_LINE_0, LCD_COLUMN_17, ((SineFreqTable[SineFreqIndex]%10)+ 0x30));
                    }                
                    LCD_DrawRect(traceXPos+traceWidth+1, LCD_LINE_0*8, 32, 9, LCD_COLOR_BLACK);
                
                    /* Start DAC */                
                    DAC_Cmd(SIGNALGEN_AOUT_CHANNEL, ENABLE);
                    TIM_Cmd(TIM7, ENABLE);
                    DAC_DMA_TIM_ENABLED = 1;  
                }                
            }        
            else if(TraceMenuItemIndex == 1)
            {
                if(SineFreqIndex > 0)
                {
                    /* Stop DAC */
                    TIM_Cmd(TIM7, DISABLE);
                    DAC_Cmd(SIGNALGEN_AOUT_CHANNEL, DISABLE);
                    DAC_DMA_TIM_ENABLED = 0;
                
                    SineFreqIndex--;
                    TIM7->ARR = (1000 / SineFreqTable[SineFreqIndex]) - 1; 
                
                    LCD_SetTextColor(LCD_COLOR_TEXT_REVERSE);
                    LCD_DisplayStringLineColumn(LCD_LINE_0, traceXPos+traceWidth+2, "   Hz", 5);                    
                    if(SineFreqTable[SineFreqIndex]/10)
                    {
                        LCD_DisplayChar(LCD_LINE_0, LCD_COLUMN_16, ((SineFreqTable[SineFreqIndex]/10)+ 0x30));
                        LCD_DisplayChar(LCD_LINE_0, LCD_COLUMN_17, ((SineFreqTable[SineFreqIndex]%10)+ 0x30));
                    }
                    else
                    {
                        LCD_DisplayChar(LCD_LINE_0, LCD_COLUMN_17, ((SineFreqTable[SineFreqIndex]%10)+ 0x30));
                    }                
                    LCD_DrawRect(traceXPos+traceWidth+1, LCD_LINE_0*8, 32, 9, LCD_COLOR_BLACK);

                    /* Start DAC */                
                    DAC_Cmd(SIGNALGEN_AOUT_CHANNEL, ENABLE);
                    TIM_Cmd(TIM7, ENABLE);
                    DAC_DMA_TIM_ENABLED = 1;  
                }                
            }        
            else if(TraceMenuItemIndex == 2)
            {
                LCD_SetTextColor(LCD_COLOR_TEXT_REVERSE);            
                if(DAC_DMA_TIM_ENABLED || ADC_DMA_TIM_ENABLED)
                {                     
                    /* Stop DAC */
                    TIM_Cmd(TIM7, DISABLE);
                    DAC_Cmd(SIGNALGEN_AOUT_CHANNEL, DISABLE);

                    /* Stop ADC */
                    TIM_Cmd(TIM3, DISABLE);
                
                    strncpy(TraceMenu[TraceMenuItemIndex],"PLAY ",5);
                    LCD_DisplayStringLineColumn(TraceMenuItems-TraceMenuItemIndex-1, traceXPos+traceWidth+2, TraceMenu[TraceMenuItemIndex], 5);    

                    DAC_DMA_TIM_ENABLED = 0;
                    ADC_DMA_TIM_ENABLED = 0;                
                }
                else
                {
                    /* Start ADC */
                    TIM_Cmd(TIM3, ENABLE);
                
                    /* Start DAC */                
                    DAC_Cmd(SIGNALGEN_AOUT_CHANNEL, ENABLE);
                    TIM_Cmd(TIM7, ENABLE);
                                   
                    strncpy(TraceMenu[TraceMenuItemIndex],"PAUSE",5); 
                    LCD_DisplayStringLineColumn(TraceMenuItems-TraceMenuItemIndex-1, traceXPos+traceWidth+2, TraceMenu[TraceMenuItemIndex], 5);    
                
                    ADC_DMA_TIM_ENABLED = 1;
                    DAC_DMA_TIM_ENABLED = 1;                                    
                }
                LCD_DrawRect(traceXPos+traceWidth+1, (TraceMenuItems-TraceMenuItemIndex-1)*8, 32, 9, LCD_COLOR_BLACK);
            }
            else if(TraceMenuItemIndex == 3)
            {
                LCD_SetTextColor(LCD_COLOR_TEXT_REVERSE);            
                if(FILTER_ENABLED)
                {                
                    strncpy(TraceMenu[TraceMenuItemIndex],"IIRON",5);                
                    LCD_DisplayStringLineColumn(TraceMenuItems-TraceMenuItemIndex-1, traceXPos+traceWidth+2, TraceMenu[TraceMenuItemIndex], 5);    

                    FILTER_ENABLED = 0;                               
                }
                else
                {
                    strncpy(TraceMenu[TraceMenuItemIndex],"IIROF",5);  
                    LCD_DisplayStringLineColumn(TraceMenuItems-TraceMenuItemIndex-1, traceXPos+traceWidth+2, TraceMenu[TraceMenuItemIndex], 5);    

                    FILTER_ENABLED = 1;                  
                }
                LCD_DrawRect(traceXPos+traceWidth+1, (TraceMenuItems-TraceMenuItemIndex-1)*8, 32, 9, LCD_COLOR_BLACK);
            }
            else if(TraceMenuItemIndex == 4)
            {
                break;
            }       
        
            DRAW_LCD_TRACE_ENABLED = 1;        
        } 
    
        if (DATA_LOGGING_ENABLED && (res || dataLogBytesWritten < DMA_DATA_BUFF_SIZE))
        {                  
            /*
            LCD_DisplayStringLineColumn(LCD_LINE_0, LCD_COLUMN_0, dataLogFileName, 20);        
            LCD_DisplayStringLineColumn(LCD_LINE_1, LCD_COLUMN_0, "Error or Disk Full", 20); 
            */
            DATA_LOGGING_ENABLED = 0;        
        }
    }

    DAC_DMA_TIM_Configuration(DISABLE);  
    ADC_DMA_TIM_Configuration(DISABLE);
        
    /* close the file, writes cached content to disk */
    res = f_close(&DATA_LOG_FIL_Obj);
        
    /*
    LCD_DisplayStringLineColumn(LCD_LINE_0, LCD_COLUMN_0, dataLogFileName, 20);
    if(res == FR_OK)
        LCD_DisplayStringLineColumn(LCD_LINE_1, LCD_COLUMN_0, "Close File : Success", 20); 
    else
        LCD_DisplayStringLineColumn(LCD_LINE_1, LCD_COLUMN_0, "Close File : Failed", 20); 
    */

    /* Display Main Menu */
    Menu_ReturnMainMenu();
    // /* Clear the LCD */
    // LCD_Clear(LCD_COLOR_WHITE);
    // /* Display the menu */
    // Menu_DisplayMenu();
    /* Enable the JoyStick interrupts */
    IntExtOnOffConfig(ENABLE);
}
#endif

void Start_ECG_Data_Acquisition(void)
{
    char TraceMenu[4][6] = {"PAUSE","IIROF","MULx2","EXIT "};
    int TraceMenuItems = 4, TraceMenuItemIndex = 0, MyKey = 0;

    uint8_t dataLogFileName[_MAX_LFN - 1];    //Year(4)+Month(2)+Day(2)+Hour(2)+Min(2)+Sec(2)+Ext(4)+NULL(1)

    /* ADC Configuration */
    Data_Channel[0] = VBAT_AIN_CHANNEL;
    Data_Channel_Trace_Enabled[0] = 0;
    Data_Channel_Filter_Enabled[0] = 0;
    prevData[0] = 0;

    Data_Channel[DATA_CHANNELS - 1] = ECG_AIN_CHANNEL;
    Data_Channel_Trace_Enabled[DATA_CHANNELS - 1] = 1;
    Data_Channel_Filter_Enabled[DATA_CHANNELS - 1] = 1;
    prevData[DATA_CHANNELS - 1] = traceYPos + traceHeight/2;

    traceOffset = traceXPos;

#if defined (USE_DAC_VGND) || defined (USE_DAC_IA_REF)
    DAC_Channel1_Data = 2048; //Virtual Ground Half Supply
#endif

    xv[0] = xv[1] = xv[2] = xv[3] = xv[4] = 0;
    yv[0] = yv[1] = yv[2] = yv[3] = yv[4] = 0;

    xv_qrs[0] = xv_qrs[1] = xv_qrs[2] = xv_qrs[3] = xv_qrs[4] = 0;
    yv_qrs[0] = yv_qrs[1] = yv_qrs[2] = yv_qrs[3] = yv_qrs[4] = 0;

/*
    xv_bp1[0] = xv_bp1[1] = xv_bp1[2] = 0;
    yv_bp1[0] = yv_bp1[1] = yv_bp1[2] = 0;
    xv_bp2[0] = xv_bp2[1] = xv_bp2[2] = 0;
    yv_bp2[0] = yv_bp2[1] = yv_bp2[2] = 0;
    xv_bp3[0] = xv_bp3[1] = xv_bp3[2] = 0;
    yv_bp3[0] = yv_bp3[1] = yv_bp3[2] = 0;
    xv_bp4[0] = xv_bp4[1] = xv_bp4[2] = 0;
    yv_bp4[0] = yv_bp4[1] = yv_bp4[2] = 0;
    xv_bp5[0] = xv_bp5[1] = xv_bp5[2] = 0;
    yv_bp5[0] = yv_bp5[1] = yv_bp5[2] = 0;
    xv_bp6[0] = xv_bp6[1] = xv_bp6[2] = 0;
    yv_bp6[0] = yv_bp6[1] = yv_bp6[2] = 0;
*/

#ifdef USE_DAC_IA_REF  
    filter_type = 2;
    scale_factor = 100;
#else
    filter_type = 1;
    scale_factor = 1;
#endif
#ifdef USE_ADC_OVERSAMPLING
    filter_offset = 16384;
#else
    filter_offset = 2048;
#endif

    /* Feature Flags */
    DRAW_LCD_TRACE_ENABLED = 1;
    ADC_DMA_TIM_ENABLED = 0;
    FILTER_ENABLED = 1;
    DATA_LOGGING_ENABLED = 0;
    
    DETECT_DISPLAY_HR_ENABLED = 1;
    QRS_Detect = 0;
    QRS_Interval = 0;
    Heart_Rate = 0;

    /* Disable the JoyStick interrupts */
    IntExtOnOffConfig(DISABLE);

    /* Clear the LCD */
    LCD_Clear(LCD_COLOR_WHITE);

    if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
    {
        LCD_DisplayStringLine(LCD_LINE_5, "Please Configure    ");
        LCD_DisplayStringLine(LCD_LINE_6, "Time and Date       ");
        LCD_DisplayStringLine(LCD_LINE_7, "Exit: Press Any Key ");

        while (Menu_ReadKey() == NOKEY)
        {
        }

        /* Display Main Menu */
        Menu_ReturnMainMenu();
        // /* Clear the LCD */
        // LCD_Clear(LCD_COLOR_WHITE);
        // /* Display the menu */
        // Menu_DisplayMenu();
        /* Enable the JoyStick interrupts */
        IntExtOnOffConfig(ENABLE);
        return;
    }

    LCD_DrawRect(traceXPos-1, traceYPos-1, LCD_WIDTH, LCD_HEIGHT, LCD_COLOR_BLACK);
    LCD_DrawLine(traceXPos+traceWidth, traceYPos-1, traceXPos+traceWidth, LCD_HEIGHT, LCD_COLOR_BLACK);
    
    for(TraceMenuItemIndex = 0; TraceMenuItemIndex < TraceMenuItems; TraceMenuItemIndex++)
    {
        if(TraceMenuItemIndex == 0)
            LCD_SetTextColor(LCD_COLOR_TEXT_REVERSE);
        else
            LCD_SetTextColor(LCD_COLOR_TEXT_NORMAL);         
        LCD_DisplayStringLineColumn(TraceMenuItems-TraceMenuItemIndex-1, traceXPos+traceWidth+2, TraceMenu[TraceMenuItemIndex], 5);    
        if(TraceMenuItemIndex == 0)
        {
            LCD_DrawRect(traceXPos+traceWidth+1, (TraceMenuItems-TraceMenuItemIndex-1)*8, 32, 9, LCD_COLOR_BLACK);
        }
    }  
    LCD_DrawLine(traceXPos+traceWidth+1, traceYPos-1, LCD_WIDTH, traceYPos-1, LCD_COLOR_BLACK);

    /* create datetime file name */
    Create_Date_Time_Filename(dataLogFileName, ".dat");
    
    /* open file with write permission */
    res = f_open(&DATA_LOG_FIL_Obj, dataLogFileName, FA_CREATE_ALWAYS | FA_WRITE); 
    
    /*
    LCD_DisplayStringLineColumn(LCD_LINE_0, LCD_COLUMN_0, dataLogFileName, 20);
    if(res == FR_OK)
        LCD_DisplayStringLineColumn(LCD_LINE_1, LCD_COLUMN_0, "Open File : Success", 20); 
    else
        LCD_DisplayStringLineColumn(LCD_LINE_1, LCD_COLUMN_0, "Open File : Failed", 20);   
    */

    if(res == FR_OK)
        DATA_LOGGING_ENABLED = 1;

    if(DATA_LOGGING_ENABLED)
    {
        /* Write Data_Channel[DATA_CHANNELS] to log file */    
        res = f_write(&DATA_LOG_FIL_Obj, Data_Channel, DATA_CHANNELS, &bytesWritten);
        // res = f_sync(&DATA_LOG_FIL_Obj);
    }            
    
    /* Configure the ADC */
    ADC_DMA_TIM_Configuration(ENABLE);
    ADC_DMA_TIM_ENABLED = 1;

#ifdef USE_ANALOG_POWER_SWITCH
    Analog_Power(ENABLE);
#endif
    
#if defined (USE_DAC_VGND) || defined (USE_DAC_IA_REF)
    DAC_Configuration(ENABLE);
#endif

    TraceMenuItemIndex = 0;
    while(1)
    {        
        /* Check which key is pressed */
        MyKey = Menu_ReadKey();

        /* If "MENU" pushbutton is pressed */
        if(MyKey == MENU)
        {
            break;
        }                  
    
        /* If "UP" pushbutton is pressed */
        if(MyKey == UP)
        {
            DRAW_LCD_TRACE_ENABLED = 0;
        
            LCD_SetTextColor(LCD_COLOR_TEXT_NORMAL);
            LCD_DisplayStringLineColumn(TraceMenuItems-TraceMenuItemIndex-1, traceXPos+traceWidth+2, TraceMenu[TraceMenuItemIndex], 5);    
            LCD_DrawRect(traceXPos+traceWidth+1, (TraceMenuItems-TraceMenuItemIndex-1)*8, 32, 9, LCD_COLOR_WHITE);

            if(TraceMenuItemIndex > 0)
            {
                TraceMenuItemIndex--;
            }
            else
            {
                TraceMenuItemIndex = (TraceMenuItems - 1);
            }

            LCD_SetTextColor(LCD_COLOR_TEXT_REVERSE);
            LCD_DisplayStringLineColumn(TraceMenuItems-TraceMenuItemIndex-1, traceXPos+traceWidth+2, TraceMenu[TraceMenuItemIndex], 5);    
            LCD_DrawRect(traceXPos+traceWidth+1, (TraceMenuItems-TraceMenuItemIndex-1)*8, 32, 9, LCD_COLOR_BLACK);
            LCD_DrawLine(traceXPos+traceWidth+1, traceYPos-1, LCD_WIDTH, traceYPos-1, LCD_COLOR_BLACK);
        
            DRAW_LCD_TRACE_ENABLED = 1;        
        }

        /* If "DOWN" pushbutton is pressed */
        if(MyKey == DOWN)
        {
            DRAW_LCD_TRACE_ENABLED = 0;
        
            LCD_SetTextColor(LCD_COLOR_TEXT_NORMAL);
            LCD_DisplayStringLineColumn(TraceMenuItems-TraceMenuItemIndex-1, traceXPos+traceWidth+2, TraceMenu[TraceMenuItemIndex], 5);    
            LCD_DrawRect(traceXPos+traceWidth+1, (TraceMenuItems-TraceMenuItemIndex-1)*8, 32, 9, LCD_COLOR_WHITE);

            if(TraceMenuItemIndex >= (TraceMenuItems - 1))
            {
                TraceMenuItemIndex = 0;
            }
            else
            {
                TraceMenuItemIndex++;
            }

            LCD_SetTextColor(LCD_COLOR_TEXT_REVERSE);
            LCD_DisplayStringLineColumn(TraceMenuItems-TraceMenuItemIndex-1, traceXPos+traceWidth+2, TraceMenu[TraceMenuItemIndex], 5);    
            LCD_DrawRect(traceXPos+traceWidth+1, (TraceMenuItems-TraceMenuItemIndex-1)*8, 32, 9, LCD_COLOR_BLACK);
            LCD_DrawLine(traceXPos+traceWidth+1, traceYPos-1, LCD_WIDTH, traceYPos-1, LCD_COLOR_BLACK);
        
            DRAW_LCD_TRACE_ENABLED = 1;        
        }

        /* If "SEL" pushbutton is pressed */
        if(MyKey == SEL)
        {
            DRAW_LCD_TRACE_ENABLED = 0;
        
            if(TraceMenuItemIndex == 0)
            {            
                LCD_SetTextColor(LCD_COLOR_TEXT_REVERSE);            
                if(ADC_DMA_TIM_ENABLED)
                {                     
                    /* Stop ADC */
                    TIM_Cmd(TIM3, DISABLE);
                
                    strncpy(TraceMenu[TraceMenuItemIndex],"PLAY ",5);
                    LCD_DisplayStringLineColumn(TraceMenuItems-TraceMenuItemIndex-1, traceXPos+traceWidth+2, TraceMenu[TraceMenuItemIndex], 5);    

                    ADC_DMA_TIM_ENABLED = 0;                
                }
                else
                {
                    /* Start ADC */
                    TIM_Cmd(TIM3, ENABLE);
                
                    strncpy(TraceMenu[TraceMenuItemIndex],"PAUSE",5); 
                    LCD_DisplayStringLineColumn(TraceMenuItems-TraceMenuItemIndex-1, traceXPos+traceWidth+2, TraceMenu[TraceMenuItemIndex], 5);    
                
                    ADC_DMA_TIM_ENABLED = 1;
                }
                LCD_DrawRect(traceXPos+traceWidth+1, (TraceMenuItems-TraceMenuItemIndex-1)*8, 32, 9, LCD_COLOR_BLACK);
            }
            else if(TraceMenuItemIndex == 1)
            {
                LCD_SetTextColor(LCD_COLOR_TEXT_REVERSE);            
                if(FILTER_ENABLED)
                {                
                    strncpy(TraceMenu[TraceMenuItemIndex],"IIRON",5);                
                    LCD_DisplayStringLineColumn(TraceMenuItems-TraceMenuItemIndex-1, traceXPos+traceWidth+2, TraceMenu[TraceMenuItemIndex], 5);    

                    FILTER_ENABLED = 0;                               
                }
                else
                {
                    strncpy(TraceMenu[TraceMenuItemIndex],"IIROF",5);  
                    LCD_DisplayStringLineColumn(TraceMenuItems-TraceMenuItemIndex-1, traceXPos+traceWidth+2, TraceMenu[TraceMenuItemIndex], 5);    

                    FILTER_ENABLED = 1;                  
                }
                LCD_DrawRect(traceXPos+traceWidth+1, (TraceMenuItems-TraceMenuItemIndex-1)*8, 32, 9, LCD_COLOR_BLACK);
            }
            else if(TraceMenuItemIndex == 2)
            {
                LCD_SetTextColor(LCD_COLOR_TEXT_REVERSE);            
                if((scale_factor == 1) || (scale_factor == 100))
                {                
                    strncpy(TraceMenu[TraceMenuItemIndex],"MULx1",5);                
                    LCD_DisplayStringLineColumn(TraceMenuItems-TraceMenuItemIndex-1, traceXPos+traceWidth+2, TraceMenu[TraceMenuItemIndex], 5);    

                    scale_factor = scale_factor * 2;   
                }
                else
                {
                    strncpy(TraceMenu[TraceMenuItemIndex],"MULx2",5);  
                    LCD_DisplayStringLineColumn(TraceMenuItems-TraceMenuItemIndex-1, traceXPos+traceWidth+2, TraceMenu[TraceMenuItemIndex], 5);    

                    scale_factor = scale_factor / 2;                  
                }
                LCD_DrawRect(traceXPos+traceWidth+1, (TraceMenuItems-TraceMenuItemIndex-1)*8, 32, 9, LCD_COLOR_BLACK);
            }
            else if(TraceMenuItemIndex == 3)
            {
                break;
            }            
        
            DRAW_LCD_TRACE_ENABLED = 1;        
        } 
    
        if (DATA_LOGGING_ENABLED && (res || dataLogBytesWritten < DMA_DATA_BUFF_SIZE))
        {                  
            /*
            LCD_DisplayStringLineColumn(LCD_LINE_0, LCD_COLUMN_0, dataLogFileName, 20);        
            LCD_DisplayStringLineColumn(LCD_LINE_1, LCD_COLUMN_0, "Error or Disk Full", 20); 
            */
            DATA_LOGGING_ENABLED = 0;        
        }
    }
  
#if defined (USE_DAC_VGND) || defined (USE_DAC_IA_REF)
    DAC_Configuration(DISABLE);
#endif

#ifdef USE_ANALOG_POWER_SWITCH
    Analog_Power(DISABLE);
#endif

    ADC_DMA_TIM_Configuration(DISABLE);

    /* close the file, writes cached content to disk */
    res = f_close(&DATA_LOG_FIL_Obj);
        
    /*
    LCD_DisplayStringLineColumn(LCD_LINE_0, LCD_COLUMN_0, dataLogFileName, 20);
    if(res == FR_OK)
        LCD_DisplayStringLineColumn(LCD_LINE_1, LCD_COLUMN_0, "Close File : Success", 20); 
    else
        LCD_DisplayStringLineColumn(LCD_LINE_1, LCD_COLUMN_0, "Close File : Failed", 20); 
    */

#ifdef QRS_DETECT_LED_OUT     
    /* Turn off LED2 */
    GPIO_SetBits(GPIO_LED, GPIO_LED2_PIN);
#endif
#ifdef QRS_DETECT_BUZZER_OUT     
    /* Turn off Buzzer */
    GPIO_SetBits(GPIO_BUZZER, GPIO_BUZZER_PIN);
#endif

    /* Display Main Menu */
    Menu_ReturnMainMenu();
    // /* Clear the LCD */
    // LCD_Clear(LCD_COLOR_WHITE);
    // /* Display the menu */
    // Menu_DisplayMenu();
    /* Enable the JoyStick interrupts */
    IntExtOnOffConfig(ENABLE);
}

void Play_Log_File(uint8_t *logFile)
{
    char TraceMenu[5][6] = {"PAUSE","IIROF","MULx2","DEL  ","EXIT "};
    int TraceMenuItems = 5, TraceMenuItemIndex = 0, MyKey = 0, ReStart = 0;
  
    xv[0] = xv[1] = xv[2] = xv[3] = xv[4] = 0;
    yv[0] = yv[1] = yv[2] = yv[3] = yv[4] = 0;

    xv_qrs[0] = xv_qrs[1] = xv_qrs[2] = xv_qrs[3] = xv_qrs[4] = 0;
    yv_qrs[0] = yv_qrs[1] = yv_qrs[2] = yv_qrs[3] = yv_qrs[4] = 0;

/*
    xv_bp1[0] = xv_bp1[1] = xv_bp1[2] = 0;
    yv_bp1[0] = yv_bp1[1] = yv_bp1[2] = 0;
    xv_bp2[0] = xv_bp2[1] = xv_bp2[2] = 0;
    yv_bp2[0] = yv_bp2[1] = yv_bp2[2] = 0;
    xv_bp3[0] = xv_bp3[1] = xv_bp3[2] = 0;
    yv_bp3[0] = yv_bp3[1] = yv_bp3[2] = 0;
    xv_bp4[0] = xv_bp4[1] = xv_bp4[2] = 0;
    yv_bp4[0] = yv_bp4[1] = yv_bp4[2] = 0;
    xv_bp5[0] = xv_bp5[1] = xv_bp5[2] = 0;
    yv_bp5[0] = yv_bp5[1] = yv_bp5[2] = 0;
    xv_bp6[0] = xv_bp6[1] = xv_bp6[2] = 0;
    yv_bp6[0] = yv_bp6[1] = yv_bp6[2] = 0;
*/

    /* Clear the LCD */
    LCD_Clear(LCD_COLOR_WHITE);

    LCD_DrawRect(traceXPos-1, traceYPos-1, LCD_WIDTH, LCD_HEIGHT, LCD_COLOR_BLACK);
    LCD_DrawLine(traceXPos+traceWidth, traceYPos-1, traceXPos+traceWidth, LCD_HEIGHT, LCD_COLOR_BLACK);
    
    for(TraceMenuItemIndex = 0; TraceMenuItemIndex < TraceMenuItems; TraceMenuItemIndex++)
    {
        if(TraceMenuItemIndex == 0)
            LCD_SetTextColor(LCD_COLOR_TEXT_REVERSE);
        else
            LCD_SetTextColor(LCD_COLOR_TEXT_NORMAL);         
        LCD_DisplayStringLineColumn(TraceMenuItems-TraceMenuItemIndex-1, traceXPos+traceWidth+2, TraceMenu[TraceMenuItemIndex], 5);    
        if(TraceMenuItemIndex == 0)
        {
            LCD_DrawRect(traceXPos+traceWidth+1, (TraceMenuItems-TraceMenuItemIndex-1)*8, 32, 9, LCD_COLOR_BLACK);
        }
    }  
    LCD_DrawLine(traceXPos+traceWidth+1, traceYPos-1, LCD_WIDTH, traceYPos-1, LCD_COLOR_BLACK);

    /* open file with read permission */
    res = f_open(&DATA_LOG_FIL_Obj, logFile, FA_OPEN_EXISTING | FA_READ); 
    
    /*
    LCD_DisplayStringLineColumn(LCD_LINE_0, LCD_COLUMN_0, logFile, 20);
    if(res == FR_OK)
        LCD_DisplayStringLineColumn(LCD_LINE_1, LCD_COLUMN_0, "Open File : Success", 20); 
    else
        LCD_DisplayStringLineColumn(LCD_LINE_1, LCD_COLUMN_0, "Open File : Failed", 20);   
    */

    /* Read Data_Channel[DATA_CHANNELS] from file */
    res = f_read(&DATA_LOG_FIL_Obj, Data_Channel, DATA_CHANNELS, &bytesRead);

    if(Data_Channel[0] == VBAT_AIN_CHANNEL)
    {
        Data_Channel_Trace_Enabled[0] = 0;
        Data_Channel_Filter_Enabled[0] = 0;
        prevData[0] = 0;
    }

    if(Data_Channel[DATA_CHANNELS - 1] != VBAT_AIN_CHANNEL)
    {
        Data_Channel_Trace_Enabled[DATA_CHANNELS - 1] = 1;
        Data_Channel_Filter_Enabled[DATA_CHANNELS - 1] = 1;
        prevData[DATA_CHANNELS - 1] = traceYPos + traceHeight/2;
    }

    traceOffset = traceXPos;

    if(Data_Channel[DATA_CHANNELS - 1] == ECG_AIN_CHANNEL)
    {
#ifdef USE_DAC_IA_REF  
        filter_type = 2;
        scale_factor = 100;
#else
        filter_type = 1;
        scale_factor = 1;
#endif

    }
    else
    {
        filter_type = 1;
        scale_factor = 1;    
    }

#ifdef USE_ADC_OVERSAMPLING
    filter_offset = 16384;
#else
    filter_offset = 2048;
#endif

    /* Feature Flags */
    DRAW_LCD_TRACE_ENABLED = 1;
    MEM_DMA_TIM_ENABLED = 0;
    FILTER_ENABLED = 1;

    if(Data_Channel[DATA_CHANNELS - 1] == ECG_AIN_CHANNEL)
    {
        DETECT_DISPLAY_HR_ENABLED = 1;
        QRS_Detect = 0;
        QRS_Interval = 0;
        Heart_Rate = 0;
    }

    /* Read a chunk of file */
    res = f_read(&DATA_LOG_FIL_Obj, &DATA_LOG_Read_Buff[0], DMA_DATA_BUFF_SIZE, &dataLogBytesReadHT);
    
    /* Read a chunk of file */
    res = f_read(&DATA_LOG_FIL_Obj, &DATA_LOG_Read_Buff[DATA_BUFF_SIZE], DMA_DATA_BUFF_SIZE, &dataLogBytesReadTC);
        
    MEM_DMA_TIM_Configuration(ENABLE);
    MEM_DMA_TIM_ENABLED = 1;
   
    TraceMenuItemIndex = 0;
    while(1)
    {        
        /* Check which key is pressed */
        MyKey = Menu_ReadKey();

        /* If "MENU" pushbutton is pressed */
        if(MyKey == MENU)
        {
            break;
        }                  
    
        /* If "UP" pushbutton is pressed */
        if(MyKey == UP)
        {
            DRAW_LCD_TRACE_ENABLED = 0;
        
            LCD_SetTextColor(LCD_COLOR_TEXT_NORMAL);
            LCD_DisplayStringLineColumn(TraceMenuItems-TraceMenuItemIndex-1, traceXPos+traceWidth+2, TraceMenu[TraceMenuItemIndex], 5);    
            LCD_DrawRect(traceXPos+traceWidth+1, (TraceMenuItems-TraceMenuItemIndex-1)*8, 32, 9, LCD_COLOR_WHITE);

            if(TraceMenuItemIndex > 0)
            {
                TraceMenuItemIndex--;
            }
            else
            {
                TraceMenuItemIndex = (TraceMenuItems - 1);
            }

            LCD_SetTextColor(LCD_COLOR_TEXT_REVERSE);
            LCD_DisplayStringLineColumn(TraceMenuItems-TraceMenuItemIndex-1, traceXPos+traceWidth+2, TraceMenu[TraceMenuItemIndex], 5);    
            LCD_DrawRect(traceXPos+traceWidth+1, (TraceMenuItems-TraceMenuItemIndex-1)*8, 32, 9, LCD_COLOR_BLACK);
            LCD_DrawLine(traceXPos+traceWidth+1, traceYPos-1, LCD_WIDTH, traceYPos-1, LCD_COLOR_BLACK);
        
            DRAW_LCD_TRACE_ENABLED = 1;        
        }

        /* If "DOWN" pushbutton is pressed */
        if(MyKey == DOWN)
        {
            DRAW_LCD_TRACE_ENABLED = 0;
        
            LCD_SetTextColor(LCD_COLOR_TEXT_NORMAL);
            LCD_DisplayStringLineColumn(TraceMenuItems-TraceMenuItemIndex-1, traceXPos+traceWidth+2, TraceMenu[TraceMenuItemIndex], 5);    
            LCD_DrawRect(traceXPos+traceWidth+1, (TraceMenuItems-TraceMenuItemIndex-1)*8, 32, 9, LCD_COLOR_WHITE);

            if(TraceMenuItemIndex >= (TraceMenuItems - 1))
            {
                TraceMenuItemIndex = 0;
            }
            else
            {
                TraceMenuItemIndex++;
            }

            LCD_SetTextColor(LCD_COLOR_TEXT_REVERSE);
            LCD_DisplayStringLineColumn(TraceMenuItems-TraceMenuItemIndex-1, traceXPos+traceWidth+2, TraceMenu[TraceMenuItemIndex], 5);    
            LCD_DrawRect(traceXPos+traceWidth+1, (TraceMenuItems-TraceMenuItemIndex-1)*8, 32, 9, LCD_COLOR_BLACK);
            LCD_DrawLine(traceXPos+traceWidth+1, traceYPos-1, LCD_WIDTH, traceYPos-1, LCD_COLOR_BLACK);
        
            DRAW_LCD_TRACE_ENABLED = 1;        
        }
    
        /* If "SEL" pushbutton is pressed */
        if(MyKey == SEL)
        {
            DRAW_LCD_TRACE_ENABLED = 0;
        
            if(TraceMenuItemIndex == 0)
            {
                LCD_SetTextColor(LCD_COLOR_TEXT_REVERSE);            
                if(ReStart)
                {
                    MEM_DMA_TIM_Configuration(ENABLE);
                
                    strncpy(TraceMenu[TraceMenuItemIndex],"PAUSE",5);  
                    LCD_DisplayStringLineColumn(TraceMenuItems-TraceMenuItemIndex-1, traceXPos+traceWidth+2, TraceMenu[TraceMenuItemIndex], 5);    

                    MEM_DMA_TIM_ENABLED = 1;
                    ReStart = 0;                
                }
                else if(MEM_DMA_TIM_ENABLED)
                {
                    /* TIM4 disable counter */
                    TIM_Cmd(TIM4, DISABLE);
                
                    strncpy(TraceMenu[TraceMenuItemIndex],"PLAY ",5);
                    LCD_DisplayStringLineColumn(TraceMenuItems-TraceMenuItemIndex-1, traceXPos+traceWidth+2, TraceMenu[TraceMenuItemIndex], 5);    

                    MEM_DMA_TIM_ENABLED = 0;
                }
                else
                {
                    /* TIM4 enable counter */
                    TIM_Cmd(TIM4, ENABLE);
                
                    strncpy(TraceMenu[TraceMenuItemIndex],"PAUSE",5);  
                    LCD_DisplayStringLineColumn(TraceMenuItems-TraceMenuItemIndex-1, traceXPos+traceWidth+2, TraceMenu[TraceMenuItemIndex], 5);    

                    MEM_DMA_TIM_ENABLED = 1;
                }
                LCD_DrawRect(traceXPos+traceWidth+1, (TraceMenuItems-TraceMenuItemIndex-1)*8, 32, 9, LCD_COLOR_BLACK);
            }
            else if(TraceMenuItemIndex == 1)
            {
                LCD_SetTextColor(LCD_COLOR_TEXT_REVERSE);            
                if(FILTER_ENABLED)
                {                
                    strncpy(TraceMenu[TraceMenuItemIndex],"IIRON",5);                
                    LCD_DisplayStringLineColumn(TraceMenuItems-TraceMenuItemIndex-1, traceXPos+traceWidth+2, TraceMenu[TraceMenuItemIndex], 5);    

                    FILTER_ENABLED = 0;   
                }
                else
                {
                    strncpy(TraceMenu[TraceMenuItemIndex],"IIROF",5);  
                    LCD_DisplayStringLineColumn(TraceMenuItems-TraceMenuItemIndex-1, traceXPos+traceWidth+2, TraceMenu[TraceMenuItemIndex], 5);    

                    FILTER_ENABLED = 1;                  
                }
                LCD_DrawRect(traceXPos+traceWidth+1, (TraceMenuItems-TraceMenuItemIndex-1)*8, 32, 9, LCD_COLOR_BLACK);
            }
            else if(TraceMenuItemIndex == 2)
            {
                LCD_SetTextColor(LCD_COLOR_TEXT_REVERSE);            
                if((scale_factor == 1) || (scale_factor == 100))
                {                
                    strncpy(TraceMenu[TraceMenuItemIndex],"MULx1",5);                
                    LCD_DisplayStringLineColumn(TraceMenuItems-TraceMenuItemIndex-1, traceXPos+traceWidth+2, TraceMenu[TraceMenuItemIndex], 5);    

                    scale_factor = scale_factor * 2;   
                }
                else
                {
                    strncpy(TraceMenu[TraceMenuItemIndex],"MULx2",5);  
                    LCD_DisplayStringLineColumn(TraceMenuItems-TraceMenuItemIndex-1, traceXPos+traceWidth+2, TraceMenu[TraceMenuItemIndex], 5);    

                    scale_factor = scale_factor / 2;                  
                }
                LCD_DrawRect(traceXPos+traceWidth+1, (TraceMenuItems-TraceMenuItemIndex-1)*8, 32, 9, LCD_COLOR_BLACK);
            }
            else if(TraceMenuItemIndex == 3)
            {
                MEM_DMA_TIM_Configuration(DISABLE);

                /* close the file, writes cached content to disk */
                f_close(&DATA_LOG_FIL_Obj);
            
                /* delete the file */
                res = f_unlink(logFile);
            
                strncpy(logFile,"--------deleted---------",_MAX_LFN - 2);
            
                return;
            }
            else if(TraceMenuItemIndex == 4)
            {
                break;
            }            
        
            DRAW_LCD_TRACE_ENABLED = 1;        
        } 

        if(MEM_DMA_TIM_ENABLED && (res || dataLogBytesReadHT==0 || dataLogBytesReadTC==0))
        {
            MEM_DMA_TIM_Configuration(DISABLE);
        
            strncpy(TraceMenu[0],"PLAY ",5);
            if(TraceMenuItemIndex == 0)
                LCD_SetTextColor(LCD_COLOR_TEXT_REVERSE);
            else
                LCD_SetTextColor(LCD_COLOR_TEXT_NORMAL);         
            LCD_DisplayStringLineColumn(LCD_LINE_3, traceXPos+traceWidth+2, TraceMenu[0], 5);    
            if(TraceMenuItemIndex == 0)
            {
                LCD_DrawRect(traceXPos+traceWidth+1, LCD_LINE_3*8, 32, 9, LCD_COLOR_BLACK);            
            }
               
            f_lseek(&DATA_LOG_FIL_Obj, 0); 
        
            /* Read Data_Channel[DATA_CHANNELS] from file */
            res = f_read(&DATA_LOG_FIL_Obj, Data_Channel, DATA_CHANNELS, &bytesRead);

            if(Data_Channel[0] == VBAT_AIN_CHANNEL)
            {
                Data_Channel_Trace_Enabled[0] = 0;
                Data_Channel_Filter_Enabled[0] = 0;
                prevData[0] = 0;
            }

            if(Data_Channel[DATA_CHANNELS - 1] != VBAT_AIN_CHANNEL)
            {
                Data_Channel_Trace_Enabled[DATA_CHANNELS - 1] = 1;
                Data_Channel_Filter_Enabled[DATA_CHANNELS - 1] = 1;
                prevData[DATA_CHANNELS - 1] = traceYPos + traceHeight/2;
            }

            traceOffset = traceXPos;

            /* Read a chunk of file */
            f_read(&DATA_LOG_FIL_Obj, &DATA_LOG_Read_Buff[0], DMA_DATA_BUFF_SIZE, &dataLogBytesReadHT);
            /* Read a chunk of file */
            f_read(&DATA_LOG_FIL_Obj, &DATA_LOG_Read_Buff[DATA_BUFF_SIZE], DMA_DATA_BUFF_SIZE, &dataLogBytesReadTC);        

            MEM_DMA_TIM_ENABLED = 0;  
            ReStart = 1;        
        }   
    }

    MEM_DMA_TIM_Configuration(DISABLE);

    /* close the file, writes cached content to disk */
    res = f_close(&DATA_LOG_FIL_Obj);

    /*
    LCD_DisplayStringLineColumn(LCD_LINE_0, LCD_COLUMN_0, logFile, 20);
    if(res == FR_OK)
        LCD_DisplayStringLineColumn(LCD_LINE_1, LCD_COLUMN_0, "Close File : Success", 20); 
    else
        LCD_DisplayStringLineColumn(LCD_LINE_1, LCD_COLUMN_0, "Close File : Failed", 20); 
    */

#ifdef QRS_DETECT_LED_OUT     
    /* Turn off LED2 */
    GPIO_SetBits(GPIO_LED, GPIO_LED2_PIN);
#endif
#ifdef QRS_DETECT_BUZZER_OUT     
    /* Turn off Buzzer */
    GPIO_SetBits(GPIO_BUZZER, GPIO_BUZZER_PIN);
#endif
}

void Browse_Log_Files(void)
{
    FRESULT res;
    FILINFO fno;
    DIR dir;
    char *fn;
#if _USE_LFN
    char logfiles[MAX_FILES][_MAX_LFN - 1];
#endif
    int TotalFiles = 0, TotalPages = 0, FileIndex = 0, PageIndex = 0;
    int FileItemIndex = 0, SelectedFileItemIndex = 0, MaxFileItemIndex = MAX_FILES - 1;
    int FetchPreviousPage = 0, FetchNextPage = 1, MyKey = 0;
#if _USE_LFN
    static char lfn[_MAX_LFN * (/*_DF1S*/0 ? 2 : 1) + 1];
    fno.lfname = lfn;
    fno.lfsize = sizeof(lfn);
#endif

    IntExtOnOffConfig(DISABLE);

    res = f_opendir(&dir, "/");
    if (res == FR_OK) {  
        for(;;)
        {
            res = f_readdir(&dir, &fno);
            if (res != FR_OK || fno.fname[0] == 0)
            {
                f_readdir(&dir, 0);
                break;
            }
#if _USE_LFN
            fn = *fno.lfname ? fno.lfname : fno.fname;
            /* memcpy(logfiles[0],fn,_MAX_LFN - 1); */
            strncpy(logfiles[0],fn,_MAX_LFN - 1);
            if((logfiles[0][_MAX_LFN - 6] != '.')
            || (logfiles[0][_MAX_LFN - 5] != 'd')
            || (logfiles[0][_MAX_LFN - 4] != 'a')
            || (logfiles[0][_MAX_LFN - 3] != 't')
            || (logfiles[0][_MAX_LFN - 2] != '\0')) continue;
#else
            fn = fno.fname;
#endif        
            TotalFiles++;
        }
    
        for(;;)
        {
            /* Clear the LCD */
            LCD_Clear(LCD_COLOR_WHITE);       
            LCD_SetTextColor(LCD_COLOR_TEXT_NORMAL);
        
            if(!TotalFiles)
            {
                LCD_DisplayStringLine(LCD_LINE_3, "    No Data Files   ");
                LCD_DisplayStringLine(LCD_LINE_4, " To exit press a key");
                
                while (Menu_ReadKey() == NOKEY)
                {
                }
                
                Menu_ReturnMainMenu();
                // /* Clear the LCD */
                // LCD_Clear(LCD_COLOR_WHITE);
                // /* Display the menu */
                // Menu_DisplayMenu();
                /* Enable the JoyStick interrupts */
                IntExtOnOffConfig(ENABLE);
                return;            
            }
        
            LCD_DisplayStringLine(LCD_LINE_0, "  Play Signal File  ");
                              
            if(FetchPreviousPage)
            {            
                FileItemIndex = FileIndex;           
                while(FileItemIndex)
                {
                    f_readdir(&dir, &fno);
#if _USE_LFN
                    fn = *fno.lfname ? fno.lfname : fno.fname;
                    /* memcpy(logfiles[0],fn,_MAX_LFN - 1); */
                    strncpy(logfiles[0],fn,_MAX_LFN - 1);
                    if((logfiles[0][_MAX_LFN - 6] != '.')
                    || (logfiles[0][_MAX_LFN - 5] != 'd')
                    || (logfiles[0][_MAX_LFN - 4] != 'a')
                    || (logfiles[0][_MAX_LFN - 3] != 't')
                    || (logfiles[0][_MAX_LFN - 2] != '\0')) continue;
#else
                    fn = fno.fname;
#endif        
                    FileItemIndex--;
                }
                FetchNextPage = 1;            
            }
   
            FileItemIndex = 0;
        
            while(1)
            {            
                if(FetchNextPage)
                {
                    res = f_readdir(&dir, &fno);
                    if (res != FR_OK || fno.fname[0] == 0)
                    {
                        MaxFileItemIndex = FileItemIndex - 1;
                        FileItemIndex = SelectedFileItemIndex;                    
                        break;
                    }
                    /* if (fno.fname[0] == '.') continue; */
#if _USE_LFN
                    fn = *fno.lfname ? fno.lfname : fno.fname;
                    /* memcpy(logfiles[FileItemIndex],fn,_MAX_LFN - 1); */
                    strncpy(logfiles[FileItemIndex],fn,_MAX_LFN - 1);
                    if((logfiles[FileItemIndex][_MAX_LFN - 6] != '.')
                    || (logfiles[FileItemIndex][_MAX_LFN - 5] != 'd')
                    || (logfiles[FileItemIndex][_MAX_LFN - 4] != 'a')
                    || (logfiles[FileItemIndex][_MAX_LFN - 3] != 't')
                    || (logfiles[FileItemIndex][_MAX_LFN - 2] != '\0')) continue;
#else
                    fn = fno.fname;
                    /* memcpy(logfiles[FileItemIndex],fn,13); */
                    strncpy(logfiles[FileItemIndex],fn,13);
#endif   
                    FileIndex++;            
                }
 
                if(FileItemIndex == SelectedFileItemIndex)
                    LCD_SetTextColor(LCD_COLOR_TEXT_REVERSE);
                else
                    LCD_SetTextColor(LCD_COLOR_TEXT_NORMAL);               
                LCD_DisplayStringLine(6-FileItemIndex, logfiles[FileItemIndex]);                
                if(FileItemIndex == SelectedFileItemIndex)
                {
                    LCD_DrawRect(LCD_COLUMN_0-1, (6-FileItemIndex)*8, 121, 9, LCD_COLOR_BLACK);
                }
                
                FileItemIndex++;            
            
                if(FileItemIndex > MaxFileItemIndex)
                {
                    FileItemIndex = SelectedFileItemIndex;
                    break;
                }
            }
            
            TotalPages = ((TotalFiles + MAX_FILES - 1) / MAX_FILES);
            PageIndex = ((FileIndex + MAX_FILES - 1) / MAX_FILES);
            LCD_SetTextColor(LCD_COLOR_TEXT_NORMAL);
            LCD_DisplayStringLine(LCD_LINE_7, "Files:000 Page:00/00");      
            LCD_DisplayChar(LCD_LINE_7, LCD_COLUMN_6, ((TotalFiles / 100) + 0x30));
            LCD_DisplayChar(LCD_LINE_7, LCD_COLUMN_7, (((TotalFiles % 100) / 10) + 0x30));
            LCD_DisplayChar(LCD_LINE_7, LCD_COLUMN_8, (((TotalFiles % 100) % 10) + 0x30));
            LCD_DisplayChar(LCD_LINE_7, LCD_COLUMN_15, ((PageIndex / 10) + 0x30));
            LCD_DisplayChar(LCD_LINE_7, LCD_COLUMN_16, ((PageIndex % 10) + 0x30));
            LCD_DisplayChar(LCD_LINE_7, LCD_COLUMN_18, ((TotalPages / 10) + 0x30));
            LCD_DisplayChar(LCD_LINE_7, LCD_COLUMN_19, ((TotalPages % 10) + 0x30));

            while(1)
            {
                /* Check which key is pressed */
                MyKey = Menu_ReadKey();

                /* If "MENU" pushbutton is pressed */
                if(MyKey == MENU)
                {
                    Menu_ReturnMainMenu();
                    // /* Clear the LCD */
                    // LCD_Clear(LCD_COLOR_WHITE);
                    // /* Display the menu */
                    // Menu_DisplayMenu();
                    /* Enable the JoyStick interrupts */
                    IntExtOnOffConfig(ENABLE);
                    return;
                }                  
            
                /* If "UP" pushbutton is pressed */
                if(MyKey == UP)
                {
                    LCD_SetTextColor(LCD_COLOR_TEXT_NORMAL);
                    LCD_DisplayStringLine(6-FileItemIndex, logfiles[FileItemIndex]);    
                    LCD_DrawRect(LCD_COLUMN_0-1, (6-FileItemIndex)*8, 121, 9, LCD_COLOR_WHITE);
                
                    if(FileItemIndex > 0)
                    {
                        FileItemIndex--;
                    }
                    else
                    {
                        FetchPreviousPage = 1;
                        FetchNextPage = 0;
                        f_readdir(&dir, 0);
                        if(PageIndex > 1)
                        {                         
                            FileIndex = (PageIndex - 2) * MAX_FILES;
                            MaxFileItemIndex = MAX_FILES - 1;
                        }
                        else
                        {  
                            FileIndex = (TotalPages - 1) * MAX_FILES;
                            MaxFileItemIndex = TotalFiles - FileIndex - 1;
                        }
                        SelectedFileItemIndex = MaxFileItemIndex;
                        break;
                    }
      
                    LCD_SetTextColor(LCD_COLOR_TEXT_REVERSE);
                    LCD_DisplayStringLine(6-FileItemIndex, logfiles[FileItemIndex]);    
                    LCD_DrawRect(LCD_COLUMN_0-1, (6-FileItemIndex)*8, 121, 9, LCD_COLOR_BLACK);               
                }

                /* If "DOWN" pushbutton is pressed */
                if(MyKey == DOWN)
                {
                    LCD_SetTextColor(LCD_COLOR_TEXT_NORMAL);
                    LCD_DisplayStringLine(6-FileItemIndex, logfiles[FileItemIndex]);    
                    LCD_DrawRect(LCD_COLUMN_0-1, (6-FileItemIndex)*8, 121, 9, LCD_COLOR_WHITE);

                    if(FileItemIndex >= MaxFileItemIndex)
                    {
                        FetchPreviousPage = 0;
                        FetchNextPage = 1;
                        MaxFileItemIndex = MAX_FILES - 1;
                        SelectedFileItemIndex = 0;
                        if(PageIndex == TotalPages)
                        {
                            f_readdir(&dir, 0);
                            FileIndex = 0;
                        }
                        break;                    
                    }
                    else
                    {
                        FileItemIndex++;
                    }
      
                    LCD_SetTextColor(LCD_COLOR_TEXT_REVERSE);
                    LCD_DisplayStringLine(6-FileItemIndex, logfiles[FileItemIndex]);    
                    LCD_DrawRect(LCD_COLUMN_0-1, (6-FileItemIndex)*8, 121, 9, LCD_COLOR_BLACK);               
                }
            
                /* If "SEL" pushbutton is pressed */
                if(MyKey == SEL)
                {
                    if((logfiles[FileItemIndex][_MAX_LFN - 6] == '.')
                    && (logfiles[FileItemIndex][_MAX_LFN - 5] == 'd')
                    && (logfiles[FileItemIndex][_MAX_LFN - 4] == 'a')
                    && (logfiles[FileItemIndex][_MAX_LFN - 3] == 't')
                    && (logfiles[FileItemIndex][_MAX_LFN - 2] == '\0'))
                    {
                        Play_Log_File(logfiles[FileItemIndex]);
                    }
                    if((logfiles[FileItemIndex][_MAX_LFN - 6] != '.')
                    || (logfiles[FileItemIndex][_MAX_LFN - 5] != 'd')
                    || (logfiles[FileItemIndex][_MAX_LFN - 4] != 'a')
                    || (logfiles[FileItemIndex][_MAX_LFN - 3] != 't')
                    || (logfiles[FileItemIndex][_MAX_LFN - 2] != '\0'))
                    {
                        TotalFiles--;
                        FileIndex--;
                        TotalPages = ((TotalFiles + MAX_FILES - 1) / MAX_FILES);
                        PageIndex = ((FileIndex + MAX_FILES - 1) / MAX_FILES);
                    
                        FetchPreviousPage = 1;
                        FetchNextPage = 0;
                        f_readdir(&dir, 0);
                        FileIndex = (PageIndex - 1) * MAX_FILES;
                        MaxFileItemIndex = TotalFiles - FileIndex - 1;
                        SelectedFileItemIndex = FileItemIndex;
                        if(PageIndex == TotalPages)
                        {
                            if((SelectedFileItemIndex > MaxFileItemIndex) || (MaxFileItemIndex == (MAX_FILES - 1)))
                            {
                                FileIndex = 0;
                                SelectedFileItemIndex = 0;
                            }
                        }
                        MaxFileItemIndex = MAX_FILES - 1;
                    }
                    else
                    {
                        FetchPreviousPage = 0;
                        FetchNextPage = 0;
                        SelectedFileItemIndex = FileItemIndex;
                    }
                    break;                    
                }   
            }
        }
    }
    else
    {
        /* Clear the LCD */
        LCD_Clear(LCD_COLOR_WHITE);       
        LCD_SetTextColor(LCD_COLOR_TEXT_NORMAL);
        LCD_DisplayStringLine(LCD_LINE_6, "SDCard Not Found.   ");    
        LCD_DisplayStringLine(LCD_LINE_7, "Press a key to Exit.");
    
        while(Menu_ReadKey() == NOKEY)
        {
        }

        LCD_Clear(LCD_COLOR_WHITE);
        Menu_DisplayMenu();
        IntExtOnOffConfig(ENABLE);
    }
}

void Delete_All_Log_Files(void)
{
    FRESULT res;
    FILINFO fno;
    DIR dir;
    char *fn;
#if _USE_LFN
    char logfile[_MAX_LFN - 1];
#endif
#if _USE_LFN
    static char lfn[_MAX_LFN * (/*_DF1S*/0 ? 2 : 1) + 1];
    fno.lfname = lfn;
    fno.lfsize = sizeof(lfn);
#endif

    uint32_t pressedkey = 0;

    LCD_Clear(LCD_COLOR_WHITE);
    IntExtOnOffConfig(DISABLE);

    res = f_opendir(&dir, "/");
    if (res == FR_OK) {
        LCD_DisplayStringLine(LCD_LINE_6, "Press OK to continue");
        LCD_DisplayStringLine(LCD_LINE_7, "else any key to Exit");

        while (1)
        {
            pressedkey = Menu_ReadKey();
            if (pressedkey == SEL)
            {
                break;
            }
            else if (pressedkey != NOKEY)
            {
                LCD_Clear(LCD_COLOR_WHITE);
                Menu_DisplayMenu();
                IntExtOnOffConfig(ENABLE);     
                return;
            }
        }
    
        LCD_ClearLine(LCD_LINE_6);
        LCD_DisplayStringLine(LCD_LINE_7, "Deleting Log Files..");

        for(;;)
        {
            res = f_readdir(&dir, &fno);
            if (res != FR_OK || fno.fname[0] == 0) break;
            /* if (fno.fname[0] == '.') continue; */
#if _USE_LFN
            fn = *fno.lfname ? fno.lfname : fno.fname;
            strncpy(logfile,fn,_MAX_LFN - 1);
            if((logfile[_MAX_LFN - 6] != '.')
            || (logfile[_MAX_LFN - 5] != 'd')
            || (logfile[_MAX_LFN - 4] != 'a')
            || (logfile[_MAX_LFN - 3] != 't')
            || (logfile[_MAX_LFN - 2] != '\0')) continue;            
#else
            fn = fno.fname;
            strncpy(logfile,fn,13);
#endif                        
            f_unlink(logfile);
        }
    
        LCD_DisplayStringLine(LCD_LINE_6, " Log Files Deleted  ");
        LCD_DisplayStringLine(LCD_LINE_7, "Press a key to Exit.");
    }
    else
    {
        LCD_DisplayStringLine(LCD_LINE_6, "SDCard Not Found.   ");    
        LCD_DisplayStringLine(LCD_LINE_7, "Press a key to Exit.");
    }

    while(Menu_ReadKey() == NOKEY)
    {
    }

    LCD_Clear(LCD_COLOR_WHITE);
    Menu_DisplayMenu();
    IntExtOnOffConfig(ENABLE);
}

void Draw_LCD_Trace(uint16_t *dataBuffer)
{
    uint16_t *pWork = dataBuffer;
    uint16_t channelData = 0;

    if(traceOffset >= (traceXPos + traceWidth))
        traceOffset = traceXPos;

    /* Erase the area */
    LCD_DrawLine(traceOffset, traceYPos, traceOffset, traceHeight, traceBackColor);
    
    for(Data_Channel_Index = 0; Data_Channel_Index < DATA_CHANNELS; Data_Channel_Index++, pWork++)
    {
        if(!Data_Channel_Trace_Enabled[Data_Channel_Index])
            continue;

        channelData = *pWork;
        
        if((Data_Channel[Data_Channel_Index] == ECG_AIN_CHANNEL) && DETECT_DISPLAY_HR_ENABLED)
            Detect_Display_HR(channelData);

#ifdef USE_ADC_OVERSAMPLING    
        channelData = channelData >> 9; //6+ADC_ADDITIONAL_BITS
#else
        channelData = channelData >> 6;    
#endif
    
        if(channelData > traceHeight)
            channelData = traceHeight;
        else if(channelData < traceYPos)
            channelData = traceYPos;
                
        /* Draw data */
        LCD_DrawLine(traceOffset, prevData[Data_Channel_Index], traceOffset, channelData, traceColor);
        
        prevData[Data_Channel_Index] = channelData;
    }
    traceOffset++;            
}

void LCD_ToggleHeartSymbol(uint8_t Line, uint8_t Column)
{
    uint8_t c = 0;
    static uint8_t toggleStatus = 0;

    LCD_SendLCDCmd(0xB0 | Line);
    LCD_SendLCDCmd(0x10 | (Column >> 4));
    LCD_SendLCDCmd(Column & 0x0F);  

    toggleStatus ^= 1;

    for(c = 0; c < 9; c++)
    {
        if(toggleStatus != RESET)
            LCD_SendLCDData(HeartSymbol_SET_9x7_Table[c]);
        else
            LCD_SendLCDData(HeartSymbol_RESET_9x7_Table[c]);
    }
}

void Detect_Display_HR(uint16_t dataSample)
{
    uint16_t channelData = qrs_filter(dataSample);

#ifdef USE_ADC_OVERSAMPLING    
    channelData = channelData >> 9; //6+ADC_ADDITIONAL_BITS
#else
    channelData = channelData >> 6;    
#endif
    
    if((channelData > traceHeight) || (channelData < traceYPos))
    {
        QRS_Detect = 0;
        QRS_Interval = 0;
        Heart_Rate = 0;
 
        LCD_SetTextColor(LCD_COLOR_TEXT_NORMAL);
        LCD_DisplayStringLineColumn(LCD_LINE_1, traceXPos+traceWidth+2, "     ", 5);   
        return;
    }

    if( !QRS_Detect )   // detect QRS wave
    {
#ifdef QRS_DETECT_LED_OUT     
        /* Turn off LED2 */
        GPIO_SetBits(GPIO_LED, GPIO_LED2_PIN);
#endif
#ifdef QRS_DETECT_BUZZER_OUT     
        /* Turn off Buzzer */
        GPIO_SetBits(GPIO_BUZZER, GPIO_BUZZER_PIN);
#endif
    
        ++QRS_Interval;
        if( channelData >= QRS_Upper_Threshold && QRS_Interval > 50 )   // HR must < 150/min  [(ADC_Sampling_Freq * 60)/150 = 50]
        {
#ifdef QRS_DETECT_LED_OUT     
            /* Turn on LED2 */
            GPIO_ResetBits(GPIO_LED, GPIO_LED2_PIN);
#endif
#ifdef QRS_DETECT_BUZZER_OUT     
            /* Turn on Buzzer */
            GPIO_ResetBits(GPIO_BUZZER, GPIO_BUZZER_PIN);
#endif
        
            LCD_ToggleHeartSymbol(LCD_LINE_1, traceXPos+traceWidth+2);

            Heart_Rate = 7500 / QRS_Interval;   // 1 minute has (ADC_Sampling_Freq * 60) = 7500 samples
            QRS_Detect = 1;
    
            LCD_SetTextColor(LCD_COLOR_TEXT_NORMAL);
            LCD_DisplayStringLineColumn(LCD_LINE_1, LCD_COLUMN_17, "   ", 3);                    
            if (Heart_Rate / 100)
            {
                LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_17, ((Heart_Rate / 100) + 0x30));
                LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_18, (((Heart_Rate % 100) / 10) + 0x30));
                LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_19, (((Heart_Rate % 100) % 10) + 0x30));
            }
            else if (Heart_Rate / 10)
            {
                LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_18, ((Heart_Rate / 10) + 0x30));
                LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_19, ((Heart_Rate % 10) + 0x30));
            }
            else
            {
                LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_19, ((Heart_Rate % 10) + 0x30));
            }        
        }
    }

    if( QRS_Detect )                                           // make sure the QRS wave return below threshold
    {
        if( channelData < QRS_Lower_Threshold )
        {
            QRS_Detect = 0;
            QRS_Interval = 0;            
        }
    }

#ifdef QRS_THRESHOLD_LCD_DISPLAY 
    LCD_DrawLine(traceOffset, QRS_Upper_Threshold, traceOffset, QRS_Upper_Threshold, traceColor);
    LCD_DrawLine(traceOffset, QRS_Lower_Threshold, traceOffset, QRS_Lower_Threshold, traceColor);
#endif
}

void Start_Battery_Monitor(void)
{
    uint16_t VBat = 0;
    uint16_t i = 0;
    uint32_t ADCConvertedValue = 0;

    LCD_Clear(LCD_COLOR_WHITE);
    IntExtOnOffConfig(DISABLE);
    LCD_SetTextColor(LCD_COLOR_TEXT_REVERSE);
    LCD_DisplayStringLine(LCD_LINE_0, "   Battery Voltage  ");    
    LCD_SetTextColor(LCD_COLOR_TEXT_NORMAL);
    LCD_DisplayStringLine(LCD_LINE_4, "            mV      ");    

    ADC_Configuration(ENABLE);

    /* ADC1 regular channel configuration */ 
    ADC_RegularChannelConfig(ADC1, VBAT_AIN_CHANNEL, 1, ADC_SampleTime_1Cycles5);

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
            VBat = (ADCConvertedValue * ADC_VDDA_VOLTAGE_MV) / 32768;
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
    VBat = (ADCConvertedValue * ADC_VDDA_VOLTAGE_MV) / 4096;
    VBat *= 2; //Divider bridge  VBAT <-> 1M -<--|-->- 1M <-> GND,        
#endif

    LCD_DisplayChar(LCD_LINE_4, LCD_COLUMN_7, ((VBat / 1000) + 0x30));
    LCD_DisplayChar(LCD_LINE_4, LCD_COLUMN_8, (((VBat % 1000) / 100) + 0x30));
    LCD_DisplayChar(LCD_LINE_4, LCD_COLUMN_9, ((((VBat % 1000) % 100) / 10) + 0x30));
    LCD_DisplayChar(LCD_LINE_4, LCD_COLUMN_10, ((((VBat % 1000) % 100) % 10) + 0x30));

    /* Display Battery Voltage Graphics here */

    ADC_Configuration(DISABLE);

    while (Menu_ReadKey() == NOKEY)
    {
    }

    LCD_Clear(LCD_COLOR_WHITE);
    Menu_DisplayMenu();
    IntExtOnOffConfig(ENABLE);
}

void Set_LCD_BackLight_Time(void)
{
#ifdef REGULATOR_FAULT
    /* Low Battery - Regulator fault detect */
    Regulator_Fault();
#endif

    if(!REGULATOR_FAULT_DETECTED)
        LCD_SetBackLight(LCD_BRIGHTNESS_NIL);
    else
        LCD_SetBackLight(LCD_BRIGHTNESS_HIGH);

    LCD_BackLight_RTC_Counter = RTC_GetCounter() + LCD_BACKLIGHT_TIME;

    if (LCD_BackLight_RTC_Counter / 86399 != 0)
    {
        LCD_BackLight_RTC_Counter = LCD_BACKLIGHT_TIME;
    }
}

#ifdef USE_IWDG 
void Set_IWDG_ReloadCounter_Time(void)
{
    IWDG_ReloadCounter();

    IWDG_ReloadCounter_RTC_Counter = RTC_GetCounter() + IWDG_RELOADCOUNTER_TIME;

    if (IWDG_ReloadCounter_RTC_Counter / 86399 != 0)
    {
        IWDG_ReloadCounter_RTC_Counter = IWDG_RELOADCOUNTER_TIME;
    }
}
#endif

void Set_LowPower_StopMode_Time(void)
{
    LOW_POWER_STOP_MODE = 0;

    LowPower_StopMode_RTC_Counter = RTC_GetCounter() + LOWPOWER_STOPMODE_TIME;

    if (LowPower_StopMode_RTC_Counter / 86399 != 0)
    {
        LowPower_StopMode_RTC_Counter = LOWPOWER_STOPMODE_TIME;
    }
}

/**
  * @brief  Enters MCU in STOP mode. The wake-up from STOP mode is performed by 
  *         an external interrupt or RTC Alarm.
  * @param  None
  * @retval None
  */
void LowPower_EnterSTOPMode_EXTI_RTCAlarm(void)
{
    STOPMODE_SYSTEM_SHUTDOWN = 0;

    System_Shutdown_RTC_Counter = RTC_GetCounter() + SYSTEM_SHUTDOWN_TIME;

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    if (System_Shutdown_RTC_Counter / 86399 != 0)
    {
        System_Shutdown_RTC_Counter = SYSTEM_SHUTDOWN_TIME;
    }

    /* Set RTC Alarm register with the new value */
    RTC_SetAlarm(System_Shutdown_RTC_Counter);

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask(); 

#ifdef USE_USB_HID
    USB_Cable_Config(DISABLE);
#endif

    // LCD_DisplayStringLineColumn(LCD_LINE_0, LCD_COLUMN_18, "LP", 2);

    /* Clear the RTC Alarm flag */
    RTC_ClearFlag(RTC_FLAG_ALR);

    /* Request to enter STOP mode with regulator in low power */
    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);

    /* Configures system clock after wake-up from STOP: enable HSE, PLL and select PLL
    as system clock source (HSE and PLL are disabled in STOP mode) */
    LowPower_SYSCLKConfig_STOP();

    // LCD_DisplayStringLineColumn(LCD_LINE_0, LCD_COLUMN_18, "  ", 2);
    
#ifdef USE_USB_HID
    USB_Cable_Config(ENABLE);
#endif    

    if (STOPMODE_SYSTEM_SHUTDOWN)
    {
        System_Shutdown();
    }
}


