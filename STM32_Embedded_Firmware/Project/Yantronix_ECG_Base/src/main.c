/*****************************************************************************
* File    : @file
* Company : Yantronix
* Author  : Satish Nair
* Mailto  : satishgn77@gmail.com
*****************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/

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
    /* Enable PWR and BKP clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    /* Enable WKUP pin */
    PWR_WakeUpPinCmd(ENABLE);
    /* Allow access to BKP Domain */
    PWR_BackupAccessCmd(ENABLE);

    Set_System();

    // USB_Interrupts_Config();
    // Set_USBClock();
    // USB_Init();

    // /* Enable PWR and BKP clock */
    // RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    // /* Enable write access to Backup domain */
    // PWR_BackupAccessCmd(ENABLE);

    /* Main loop */
    while (1)
    {  
        #if defined (USE_ECG_MONITOR) || defined (USE_DAC_SIGNALGEN)  
        ADC_DMA_Process_Data();
        #endif

        // #ifdef USE_LED
        // /* Turn on LED */
        // GPIO_SetBits(GPIO_LED, GPIO_LED_PIN);
        // /* Insert delay */
        // Delay(50);
        // /* Turn off LED */
        // GPIO_ResetBits(GPIO_LED, GPIO_LED_PIN);
        // /* Insert delay */
        // Delay(50);
        // #endif  
    }
}

#ifdef  USE_FULL_ASSERT
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
    while(1)
    {
    }
}
#endif


