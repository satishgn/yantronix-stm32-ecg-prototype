/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
* File Name          : main.c
* Author             : MCD Application Team
* Version            : V3.3.0
* Date               : 21-March-2011
* Description        : Main program body.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#ifdef USE_STM32_SGN101
 #include "stm32_st7565_lcd.h"
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t TimingDelay;

#ifdef USE_STM32_SGN10X
uint8_t LEDToggleCount = 0;
#endif

/* Private function prototypes -----------------------------------------------*/
void NVIC_Configuration(void);
void Delay(__IO uint32_t nTime);

/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f10x_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f10x.c file
     */  

#ifdef USE_STM32_PRIMER2
  GPIO_InitTypeDef GPIO_InitStructure;
  // turn LCD backlight OFF
  // !!! this prevents overcharging the 3.1V regulator in case of ESD !!!
  // also reduces power consumption
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  // Configure PB.08 as output
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOB, GPIO_Pin_8);
#endif

  /* NVIC configuration */
  NVIC_Configuration();

  /* Initialize the LCD */
#ifdef USE_STM32_SGN101
  LCD_Init();  

  LCD_Clear(LCD_COLOR_WHITE);
  LCD_DisplayStringLine(LCD_LINE_3, " Firmware Not Loaded");
  LCD_DisplayStringLine(LCD_LINE_5, " Press 'OK' to Exit ");
#endif

  /* Enable PWR and BKP clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
  /* Enable write access to Backup domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Configure the LEDs */
  STM_EVAL_LEDInit(LED1);
  // STM_EVAL_LEDInit(LED2);  
  // STM_EVAL_LEDInit(LED3);
  // STM_EVAL_LEDInit(LED4);  

#ifdef USE_STM32_PRIMER2
  STM_EVAL_PBInit(Button_SEL, Mode_GPIO);
#elif defined (USE_STM32_H103)
  STM_EVAL_PBInit(Button_SEL, Mode_GPIO);
  // STM_EVAL_PBInit(Button_WAKEUP, Mode_GPIO);
#elif defined (USE_STM32_SGN10X)
  // Nothing to Do  
#else
  STM_EVAL_PBInit(Button_SEL, Mode_GPIO);
  // STM_EVAL_PBInit(Button_WAKEUP, Mode_GPIO);
#endif
  
  /* Setup SysTick Timer for 1 msec interrupts  */
  if (SysTick_Config(SystemCoreClock / 1000))
  { 
    /* Capture error */ 
    while (1);
  }

  while (1)
  {
    /* Toggle Leds */
    STM_EVAL_LEDToggle(LED1);
    // STM_EVAL_LEDToggle(LED2);
    // STM_EVAL_LEDToggle(LED3);
    // STM_EVAL_LEDToggle(LED4);

    /* Insert 500 ms delay */
    Delay(500);

    /* Toggle Leds */
    STM_EVAL_LEDToggle(LED1);
    // STM_EVAL_LEDToggle(LED2);
    // STM_EVAL_LEDToggle(LED3);
    // STM_EVAL_LEDToggle(LED4);

    /* Insert 500 ms delay */
    Delay(500);
  
#ifdef USE_STM32_SGN10X
    ++LEDToggleCount;
#endif  
  }
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
  /* Set the Vector Table base location at 0x3000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x3000);
}

/*******************************************************************************
* Function Name  : Delay
* Description    : Inserts a delay time.
* Input          : nTime: specifies the delay time length, in milliseconds.
* Output         : None
* Return         : None
*******************************************************************************/
void Delay(uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

/*******************************************************************************
* Function Name  : TimingDelay_Decrement
* Description    : Decrements the TimingDelay variable.
* Input          : None
* Output         : TimingDelay
* Return         : None
*******************************************************************************/
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}

#ifdef USE_FULL_ASSERT
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
