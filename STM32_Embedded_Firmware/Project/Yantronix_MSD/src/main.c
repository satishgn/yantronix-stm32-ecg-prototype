/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
* File Name          : main.c
* Author             : MCD Application Team
* Version            : V3.3.0
* Date               : 21-March-2011
* Description        : Mass Storage demo main file
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "stm32_eval.h"
#ifdef USE_STM32_SGN101
 #include "stm32_st7565_lcd.h"
#endif

extern uint16_t MAL_Init (uint8_t lun);

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : main.
* Description    : Main routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int main(void)
{
  /* Initialize the LCD */
#ifdef USE_STM32_SGN101
  LCD_Init();  

  LCD_Clear(LCD_COLOR_WHITE);
  LCD_DisplayStringLine(LCD_LINE_2, " Yantronix MSD Mode ");
  LCD_DisplayStringLine(LCD_LINE_4, "  USB Mass Storage  ");
  LCD_DisplayStringLine(LCD_LINE_5, " Press 'OK' to Exit ");
#endif

  Set_System();
  Set_USBClock();
#ifdef USE_LED
  Led_Config();
#endif
  USB_Interrupts_Config();
  USB_Init();
  while (bDeviceState != CONFIGURED);

#ifdef USE_LED  
  USB_Configured_LED();
#endif
  
  /* Enable PWR and BKP clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
  /* Enable write access to Backup domain */
  PWR_BackupAccessCmd(ENABLE);
  
#ifdef USE_STM32_PRIMER2
  STM_EVAL_PBInit(Button_SEL, Mode_GPIO);
#elif defined USE_STM32_H103
  STM_EVAL_PBInit(Button_SEL, Mode_GPIO);
  // STM_EVAL_PBInit(Button_WAKEUP, Mode_GPIO);
#else
  STM_EVAL_PBInit(Button_SEL, Mode_GPIO);
  // STM_EVAL_PBInit(Button_WAKEUP, Mode_GPIO);
#endif
  
  while (1)
  {
#ifdef USE_STM32_PRIMER2  
    if(STM_EVAL_PBGetState(Button_SEL))
#elif defined USE_STM32_H103
    if(STM_EVAL_PBGetState(Button_SEL))
    // if(!STM_EVAL_PBGetState(Button_WAKEUP))
#else
    if(STM_EVAL_PBGetState(Button_SEL))    
    // if(STM_EVAL_PBGetState(Button_WAKEUP))    
#endif
    {
        BKP_WriteBackupRegister(BKP_DR10, 0xB000);
        USB_Cable_Config(DISABLE);
        NVIC_SystemReset();
    }  
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
  {}
}
#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
