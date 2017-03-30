/******************** (C) COPYRIGHT 2010 STMicroelectronics ********************
* File Name          : main.c
* Author             : MCD Application Team
* Version            : V3.2.1
* Date               : 07/05/2010
* Description        : Device Firmware Upgrade(DFU) demo main file
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
#include "usb_conf.h"
#include "usb_prop.h"
#include "usb_pwr.h"
#include "dfu_mal.h"
#include "hw_config.h"
#include "platform_config.h"
#include "stm32_eval.h"
#ifdef USE_STM32_SGN101
 #include "stm32_st7565_lcd.h"
#endif

/* Private typedef -----------------------------------------------------------*/
typedef  void (*pFunction)(void);

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
uint8_t DeviceState;
uint8_t DeviceStatus[6];
pFunction Jump_To_Application;
uint32_t JumpAddress;
uint32_t ApplicationAddress;
extern uint32_t Pointer;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : main.
* Description    : main routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int main(void)
{
  // DFU_Button_Config();

  switch(BKP_ReadBackupRegister(BKP_DR10))
  {
    case 0x5000:
        ApplicationAddress = MSD_ApplicationAddress;
        break;
    case 0x9000:
        ApplicationAddress = VCP_ApplicationAddress;
        break;
    case 0xC000:
        ApplicationAddress = HID_ApplicationAddress;
        break;
    case 0xFFFF:
        ApplicationAddress = DFU_ApplicationAddress;
        break;
    default:
        ApplicationAddress = HID_ApplicationAddress;
        break;
  }

  Pointer = ApplicationAddress;

  // /* Check if the Key push-button on STM3210x-EVAL Board is pressed */
  // if (DFU_Button_Read() != 0x00)
  if (ApplicationAddress != DFU_ApplicationAddress)
  {
    /* Test if user code is programmed starting from address 0x800X000 */
    if (((*(__IO uint32_t*)ApplicationAddress) & 0x2FFE0000 ) != 0x20000000)
        ApplicationAddress = SYSTICK_ApplicationAddress;

    /* Test if user code is programmed starting from address 0x800X000 */
    if (((*(__IO uint32_t*)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000)
    {
      /* Jump to user application */
      JumpAddress = *(__IO uint32_t*) (ApplicationAddress + 4);
      Jump_To_Application = (pFunction) JumpAddress;
      /* Initialize user application's Stack Pointer */
      __set_MSP(*(__IO uint32_t*) ApplicationAddress);
      Jump_To_Application();
    }
  } /* Otherwise enters DFU mode to allow user to program his application */

  /* Initialize the LCD */
#ifdef USE_STM32_SGN101
  LCD_Init();  

  LCD_Clear(LCD_COLOR_WHITE);
  LCD_DisplayStringLine(LCD_LINE_2, " Yantronix DFU Mode ");
  LCD_DisplayStringLine(LCD_LINE_4, "USB Firmware Upgrade");
  LCD_DisplayStringLine(LCD_LINE_5, " Press 'OK' to Exit ");
#endif

  /* Enter DFU mode */
  DeviceState = STATE_dfuERROR;
  DeviceStatus[0] = STATUS_ERRFIRMWARE;
  DeviceStatus[4] = DeviceState;

  Set_System();
  Set_USBClock();
  USB_Init();

  /* Enable PWR and BKP clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
  /* Enable write access to Backup domain */
  PWR_BackupAccessCmd(ENABLE);

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
  
  /* Main loop */
  while (1)
  {
#ifdef USE_STM32_PRIMER2  
    if(STM_EVAL_PBGetState(Button_SEL) && (DeviceState == STATE_dfuIDLE || DeviceState == STATE_dfuERROR))
#elif defined (USE_STM32_H103)
    if(STM_EVAL_PBGetState(Button_SEL) && (DeviceState == STATE_dfuIDLE || DeviceState == STATE_dfuERROR))
    // if(!STM_EVAL_PBGetState(Button_WAKEUP) && (DeviceState == STATE_dfuIDLE || DeviceState == STATE_dfuERROR))
#elif defined (USE_STM32_SGN10X)
    if(0)   //Nothing to Do
#else
    if(STM_EVAL_PBGetState(Button_SEL) && (DeviceState == STATE_dfuIDLE || DeviceState == STATE_dfuERROR))
    // if(STM_EVAL_PBGetState(Button_WAKEUP) && (DeviceState == STATE_dfuIDLE || DeviceState == STATE_dfuERROR))    
#endif
    {
        Reset_Device();
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
/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
