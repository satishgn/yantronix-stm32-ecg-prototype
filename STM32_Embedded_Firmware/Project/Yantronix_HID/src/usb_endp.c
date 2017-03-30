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
    case SHUTDOWN_REPORT_ID: /* Shutdown Primer2 -> 20 */
     Shutdown_System();
     break;
    
    case LED_REPORT_ID: /* Led -> 21 */
#ifdef USE_LED          
     GPIO_WriteBit(GPIO_LED, GPIO_LED1_PIN, Bit_State);
     GPIO_WriteBit(GPIO_LED, GPIO_LED2_PIN, Bit_State);
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
}
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

