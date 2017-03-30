/******************** (C) COPYRIGHT 2009 STMicroelectronics ********************
* File Name          : lowpower.c
* Author             : MCD Application Team
* Version            : V2.0.0
* Date               : 04/27/2009
* Description        : This file includes the low power driver for the 
*                      STM3210B-EVAL demonstration.
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t STOPModeStatus = 0;
static uint32_t GPIOA_CRL = 0, GPIOA_CRH = 0, GPIOB_CRL = 0, GPIOB_CRH = 0,
           GPIOC_CRL = 0, GPIOC_CRH = 0, GPIOD_CRL = 0, GPIOD_CRH = 0,
           GPIOE_CRL = 0, GPIOE_CRH = 0;
           
__IO uint8_t LOW_POWER_STOP_MODE = 0;            
           
/* Private function prototypes -----------------------------------------------*/
static void GPIO_SaveConfig(void);
static void GPIO_RestoreConfig(void);

/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : LowPower_Init
* Description    : Initializes Low Power application. 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LowPower_Init(void)
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Enable WakeUp pin */
  PWR_WakeUpPinCmd(ENABLE);

  /* Enable Clock Security System(CSS) */
  RCC_ClockSecuritySystemCmd(ENABLE);
}

/*******************************************************************************
* Function Name  : SYSCLKConfig_STOP
* Description    : Configures system clock after wake-up from STOP: enable HSE, PLL
*                  and select PLL as system clock source.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SYSCLKConfig_STOP(void)
{
  ErrorStatus HSEStartUpStatus;

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {
    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
}

/*******************************************************************************
* Function Name  : EnterSTOPMode_EXTI
* Description    : Enters MCU in STOP mode. The wake-up from STOP mode is 
*                  performed by an external interrupt.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EnterSTOPMode_EXTI(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  
  STOPModeStatus = 0;

  /* Clear the LCD */
  LCD_Clear(White);
  
  /* Set the LCD Back Color */
  LCD_SetBackColor(Blue);

  /* Set the LCD Text Color */
  LCD_SetTextColor(White);

  /* Disable the JoyStick interrupts */
  IntExtOnOffConfig(DISABLE);

  /* Configure the MENU EXTI Line */
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Line = GPIO_MENU_EXTI_Line;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable the EXTI1 Interrupt (MENU key) */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  LCD_DisplayStringLine(Line6, "  MCU in STOP Mode  "); 
  LCD_DisplayStringLine(Line7, "   To exit press    ");
  LCD_DisplayStringLine(Line8, "LEFT or RIGHT button");

  /* Clear the RTC Alarm flag */
  RTC_ClearFlag(RTC_FLAG_ALR);
  
  /* Save the GPIO pins current configuration then put all GPIO pins in Analog
     Input mode ...*/
  // GPIO_SaveConfig();

  /* Keep MENU key configuration which will be used as EXTI Line source for wakeup from stop mode */
  GPIO_InitTypeDef GPIO_InitStructure;  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_MENU;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_Init(GPIO_MENU, &GPIO_InitStructure);
    
  /* Request to enter STOP mode with regulator in low power */
  PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
  
  /* Restore the GPIO Configurations*/
  // GPIO_RestoreConfig();

  /* At this stage the system has resumed from STOP mode */
  /* Configure MENU key EXTI line to generate an interrupt on rising edge */  
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Line = GPIO_MENU_EXTI_Line;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable the EXTI1 Interrupt (MENU key) */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the EXTI4 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);    

  /* Configures system clock after wake-up from STOP: enable HSE, PLL and select PLL
     as system clock source (HSE and PLL are disabled in STOP mode) */
  SYSCLKConfig_STOP();

  if(STOPModeStatus != RESET)
  {
    LCD_DisplayStringLine(Line5, "      STOP Mode     ");
    LCD_DisplayStringLine(Line6, "Wake-Up by RTC Alarm");
    LCD_DisplayStringLine(Line7, "Press JoyStick to   ");
    LCD_DisplayStringLine(Line8, "continue...         ");
  }
  else
  {
    LCD_DisplayStringLine(Line5, "      STOP Mode     ");
    LCD_DisplayStringLine(Line6, "WakeUp by L/R Button");
    LCD_DisplayStringLine(Line7, "Press JoyStick to   ");
    LCD_DisplayStringLine(Line8, "continue...         ");
  }
  

  while(ReadKey() == NOKEY)
  {
  }

  /* Clear the LCD */
  LCD_Clear(White);
  /* Display the previous menu */
  DisplayMenu();
  /* Enable the JoyStick interrupts */
  IntExtOnOffConfig(ENABLE);
}

/*******************************************************************************
* Function Name  : EnterSTOPMode_RTCAlarm
* Description    : Enters MCU in STOP mode. The wake-up from STOP mode is 
*                  performed by an RTC Alarm.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EnterSTOPMode_RTCAlarm(void)
{  
  uint32_t tmp = 0;

  /* Clear the LCD */
  LCD_Clear(White);
  /* Set the LCD Back Color */
  LCD_SetBackColor(Blue);

  /* Set the LCD Text Color */
  LCD_SetTextColor(White);

  /* Disable the JoyStick interrupts */
  IntExtOnOffConfig(DISABLE);

  if(BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
  {
    LCD_DisplayStringLine(Line1, "Time and Date are   ");
    LCD_DisplayStringLine(Line2, "not configured,     ");
    LCD_DisplayStringLine(Line3, "please go to the    ");
    LCD_DisplayStringLine(Line4, "calendar menu and   ");
    LCD_DisplayStringLine(Line5, "set the time and    ");
    LCD_DisplayStringLine(Line6, "date parameters.    ");
    LCD_DisplayStringLine(Line7, "Press JoyStick to   ");
    LCD_DisplayStringLine(Line8, "continue...         ");
    while(ReadKey() == NOKEY)
    {
    }
    /* Clear the LCD */
    LCD_Clear(White);
    /* Display the previous menu */
    DisplayMenu();
    /* Enable the JoyStick interrupts */
    IntExtOnOffConfig(ENABLE);
    return;
  }
  tmp = RTC_GetCounter();

  /* Save the Alarm value in the Backup register */
  BKP_WriteBackupRegister(BKP_DR6, (tmp & 0x0000FFFF));
  BKP_WriteBackupRegister(BKP_DR7, (tmp >> 16));
  
  Alarm_PreAdjust();
  LCD_ClearLine(Line8);
  LCD_DisplayStringLine(Line7, "  MCU in STOP Mode  "); 
  LCD_DisplayStringLine(Line8, " Wait For RTC Alarm ");
  
  /* Save the GPIO pins current configuration then put all GPIO pins in Analog Input mode */
  GPIO_SaveConfig();
  
  /* Request to enter STOP mode with regulator in low power */
  PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);

  /* Restore the GPIO Configurations*/
  GPIO_RestoreConfig();
  
  /* Configures system clock after wake-up from STOP: enable HSE, PLL and select PLL
     as system clock source (HSE and PLL are disabled in STOP mode) */
  SYSCLKConfig_STOP();

  LCD_DisplayStringLine(Line5, "      STOP Mode     ");
  LCD_DisplayStringLine(Line6, "Wake-Up by RTC Alarm");
  LCD_DisplayStringLine(Line7, "Press JoyStick to   ");
  LCD_DisplayStringLine(Line8, "continue...         ");

  while(ReadKey() == NOKEY)
  {
  }

  /* Clear the LCD */
  LCD_Clear(White);
  /* Display the previous menu */
  DisplayMenu();
  /* Enable the JoyStick interrupts */
  IntExtOnOffConfig(ENABLE);
}

/*******************************************************************************
* Function Name  : EnterSTANDBYMode_WAKEUP
* Description    : Enters MCU in STANDBY mode. The wake-up from STANDBY mode is 
*                  performed when a rising edge is detected on WKP_STDBY pin.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EnterSTANDBYMode_WAKEUP(void)
{
  LCD_Clear(White);
  /* Set the LCD Back Color */
  LCD_SetBackColor(Blue);

  /* Set the LCD Text Color */
  LCD_SetTextColor(White);

  LCD_DisplayStringLine(Line7, " MCU in STANDBY Mode"); 
  LCD_DisplayStringLine(Line8, "To exit press Wakeup");

  /* Request to enter STANDBY mode (Wake Up flag is cleared in PWR_EnterSTANDBYMode function) */
  PWR_EnterSTANDBYMode();
}

/*******************************************************************************
* Function Name  : EnterSTANDBYMode_RTCAlarm
* Description    : Enters MCU in STANDBY mode. The wake-up from STANDBY mode is 
*                  performed by an RTC Alarm event.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EnterSTANDBYMode_RTCAlarm(void)
{
  uint32_t tmp = 0;

  LCD_Clear(White);
  /* Set the LCD Back Color */
  LCD_SetBackColor(Blue);

  /* Set the LCD Text Color */
  LCD_SetTextColor(White);

  /* Disable the JoyStick interrupts */
  IntExtOnOffConfig(DISABLE);
  if(BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
  {
    LCD_DisplayStringLine(Line1, "Time and Date are   ");
    LCD_DisplayStringLine(Line2, "not configured,     ");
    LCD_DisplayStringLine(Line3, "please go to the    ");
    LCD_DisplayStringLine(Line4, "calendar menu and   ");
    LCD_DisplayStringLine(Line5, "set the time and    ");
    LCD_DisplayStringLine(Line6, "date parameters.    ");
    LCD_DisplayStringLine(Line7, "Press JoyStick to   ");
    LCD_DisplayStringLine(Line8, "continue...         ");
    while(ReadKey() == NOKEY)
    {
    }
    /* Clear the LCD */
    LCD_Clear(White);
    /* Display the previous menu */
    DisplayMenu();
    /* Enable the JoyStick interrupts */
    IntExtOnOffConfig(ENABLE);
    return;
  }
  
  tmp = RTC_GetCounter();

  /* Save the Alarm value in the Backup register */
  BKP_WriteBackupRegister(BKP_DR6, (tmp & 0x0000FFFF));
  BKP_WriteBackupRegister(BKP_DR7, (tmp >> 16));

  Alarm_PreAdjust();

  LCD_DisplayStringLine(Line7, " MCU in STANDBY Mode");
  LCD_DisplayStringLine(Line8, " Wait For RTC Alarm ");

  /* Request to enter STANDBY mode (Wake Up flag is cleared in PWR_EnterSTANDBYMode function) */
  PWR_EnterSTANDBYMode();
}

/*******************************************************************************
* Function Name  : Set_STOPModeStatus
* Description    : Sets STOPModeStatus variable.
* Input          : None
* Output         : STOPModeStatus
* Return         : None
*******************************************************************************/
void Set_STOPModeStatus(void)
{
  STOPModeStatus = 1;
}

/*******************************************************************************
* Function Name  : GPIO_SaveConfig
* Description    : Save all GPIOs Configurations.
* Input          : None
* Output         : GPIOA_CRL, GPIOA_CRH, GPIOB_CRL, GPIOB_CRH, GPIOC_CRL, GPIOC_CRH,
*                  GPIOD_CRL, GPIOD_CRH, GPIOE_CRL, GPIOE_CRH.  
* Return         : None
*******************************************************************************/
static void GPIO_SaveConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIOA_CRL = GPIOA->CRL;
  GPIOA_CRH = GPIOA->CRH;
  
  GPIOB_CRL = GPIOB->CRL;
  GPIOB_CRH = GPIOB->CRH;
  
  GPIOC_CRL = GPIOC->CRL;
  GPIOC_CRH = GPIOC->CRH;
  
  GPIOD_CRL = GPIOD->CRL;
  GPIOD_CRH = GPIOD->CRH;
  
  GPIOE_CRL = GPIOE->CRL;
  GPIOE_CRH = GPIOE->CRH;
  
  /* Configure all GPIO port pins in Analog Input mode (floating input trigger OFF) */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  GPIO_Init(GPIOE, &GPIO_InitStructure);  
}

/*******************************************************************************
* Function Name  : GPIO_RestoreConfig
* Description    : Restores all GPIOs Configurations.
* Input          : None
* Output         : STOPModeStatus
* Return         : None
*******************************************************************************/
static void GPIO_RestoreConfig(void)
{  
  GPIOA->CRL = GPIOA_CRL;
  GPIOA->CRH = GPIOA_CRH;
  
  GPIOB->CRL = GPIOB_CRL;
  GPIOB->CRH = GPIOB_CRH;
  
  GPIOC->CRL = GPIOC_CRL;
  GPIOC->CRH = GPIOC_CRH;
  
  GPIOD->CRL = GPIOD_CRL;
  GPIOD->CRH = GPIOD_CRH;
  
  GPIOE->CRL = GPIOE_CRL;
  GPIOE->CRH = GPIOE_CRH;  
}

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
