/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
* File Name          : dfu_mal.c
* Author             : MCD Application Team
* Version            : V3.3.0
* Date               : 21-March-2011
* Description        : Generic media access Layer
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "dfu_mal.h"
#include "spi_if.h"
#include "flash_if.h"
#include "nor_if.h"
#include "fsmc_nor.h"
#include "usb_lib.h"
#include "usb_type.h"
#include "usb_desc.h"
#include "platform_config.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint16_t (*pMAL_Init) (void);
uint16_t (*pMAL_Erase) (uint32_t SectorAddress);
uint16_t (*pMAL_Write) (uint32_t SectorAddress, uint32_t DataLength);
uint8_t  *(*pMAL_Read)  (uint32_t SectorAddress, uint32_t DataLength);
uint8_t  MAL_Buffer[wTransferSize]; /* RAM Buffer for Downloaded Data */
NOR_IDTypeDef NOR_ID;
extern ONE_DESCRIPTOR DFU_String_Descriptor[7];

/* This table holds the Typical Sector Erase and 1024 Bytes Write timings.
   These timings will be returned to the host when it checks the device
   status during a write or erase operation to know how much time the host
   should wait before issuing the next get status request. 
   These defines are set in usb_conf.h file.
   The values of this table should be extracted from relative memories 
   datasheet (Typical or Maximum timming value for Sector Erase and for 
   1024 bytes Write). All timings are expressed in millisecond unit (ms).
   Note that "Sector" refers here to the memory unit used for Erase/Write 
   operations. It could be a sector, a page, a block, a word ...
   If the erase operation is not supported, it is advised to set the erase
   timing to 1 (which means 1ms: one USB frame). */
static const uint16_t  TimingTable[5][2] =
  { /*       Sector Erase time,            Sector Program time*/    
    { SPI_FLASH_SECTOR_ERASE_TIME,    SPI_FLASH_SECTOR_WRITE_TIME },    /* SPI Flash */
    { M29W128F_SECTOR_ERASE_TIME,     M29W128F_SECTOR_WRITE_TIME },     /* NOR Flash M29W128F */
    { INTERN_FLASH_SECTOR_ERASE_TIME, INTERN_FLASH_SECTOR_WRITE_TIME }, /* Internal Flash */
    { M29W128G_SECTOR_ERASE_TIME,     M29W128G_SECTOR_WRITE_TIME },     /* NOR Flash M29W128G */
    { S29GL128_SECTOR_ERASE_TIME,     S29GL128_SECTOR_WRITE_TIME }      /* NOR Flash S29GL128 */
  };

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : MAL_Init
* Description    : Initializes the Media on the STM32
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t MAL_Init(void)
{

  FLASH_If_Init(); /* Internal Flash */

#if defined(USE_STM3210B_EVAL) || defined(USE_STM3210E_EVAL)
  SPI_If_Init();   /* SPI Flash */
#endif /* USE_STM3210B_EVAL or USE_STM3210E_EVAL */

#ifdef USE_STM3210E_EVAL 
  NOR_If_Init();  /* NOR Flash */
  FSMC_NOR_ReadID(&NOR_ID);
    
  FSMC_NOR_ReturnToReadMode();

  /* select the alternate descriptor following NOR ID */
  if ((NOR_ID.Manufacturer_Code == 0x01)&&(NOR_ID.Device_Code2 == NOR_S29GL128))
  {
    DFU_String_Descriptor[6].Descriptor = DFU_StringInterface2_3;
  } 
  
  /* select the alternate descriptor following NOR ID */
  if  ((NOR_ID.Manufacturer_Code == 0x20)&&(NOR_ID.Device_Code2 == NOR_M29W128G))
  {
    DFU_String_Descriptor[6].Descriptor = DFU_StringInterface2_2;
  }
#endif /* USE_STM3210E_EVAL */

  return MAL_OK;
}

/*******************************************************************************
* Function Name  : MAL_Erase
* Description    : Erase sector
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t MAL_Erase(uint32_t SectorAddress)
{

  switch (SectorAddress & MAL_MASK)
  {
    case INTERNAL_FLASH_BASE:
      pMAL_Erase = FLASH_If_Erase;
      break;
      
#if defined(USE_STM3210B_EVAL) || defined(USE_STM3210E_EVAL)
    case SPI_FLASH_BASE:
      pMAL_Erase = SPI_If_Erase;
      break;
#endif /* USE_STM3210B_EVAL or USE_STM3210E_EVAL */
            
#ifdef USE_STM3210E_EVAL  
    case NOR_FLASH_BASE:
      pMAL_Erase = NOR_If_Erase;
      break;
#endif /* USE_STM3210E_EVAL */
      
    default:
      return MAL_FAIL;
  }
  return pMAL_Erase(SectorAddress);
}

/*******************************************************************************
* Function Name  : MAL_Write
* Description    : Write sectors
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t MAL_Write (uint32_t SectorAddress, uint32_t DataLength)
{

  switch (SectorAddress & MAL_MASK)
  {
    case INTERNAL_FLASH_BASE:
      pMAL_Write = FLASH_If_Write;
      break;

#if defined(USE_STM3210B_EVAL) || defined(USE_STM3210E_EVAL)    
    case SPI_FLASH_BASE:
      pMAL_Write = SPI_If_Write;
      break;
#endif /* USE_STM3210B_EVAL || USE_STM3210E_EVAL */      

#ifdef USE_STM3210E_EVAL
    case NOR_FLASH_BASE:
      pMAL_Write = NOR_If_Write;
      break;
#endif /* USE_STM3210E_EVAL */
    default:
      return MAL_FAIL;
  }
  return pMAL_Write(SectorAddress, DataLength);
}

/*******************************************************************************
* Function Name  : MAL_Read
* Description    : Read sectors
* Input          : None
* Output         : None
* Return         : Buffer pointer
*******************************************************************************/
uint8_t *MAL_Read (uint32_t SectorAddress, uint32_t DataLength)
{

  switch (SectorAddress & MAL_MASK)
  {
    case INTERNAL_FLASH_BASE:
      pMAL_Read = FLASH_If_Read;
      break;
      
#if defined(USE_STM3210B_EVAL) || defined(USE_STM3210E_EVAL)     
    case SPI_FLASH_BASE:
      pMAL_Read = SPI_If_Read;
      break;
#endif /* USE_STM3210B_EVAL or USE_STM3210E_EVAL */

#ifdef USE_STM3210E_EVAL
    case NOR_FLASH_BASE:
      pMAL_Read = NOR_If_Read;
      break;
#endif /* USE_STM3210E_EVAL */

    default:
      return 0;
  }
  return pMAL_Read (SectorAddress, DataLength);
}

/*******************************************************************************
* Function Name  : MAL_GetStatus
* Description    : Get status
* Input          : None
* Output         : None
* Return         : Buffer pointer
*******************************************************************************/
uint16_t MAL_GetStatus(uint32_t SectorAddress , uint8_t Cmd, uint8_t *buffer)
{
  uint8_t x = (SectorAddress  >> 26) & 0x03 ; /* 0x000000000 --> 0 */
  /* 0x640000000 --> 1 */
  /* 0x080000000 --> 2 */

  uint8_t y = Cmd & 0x01;

#if defined(USE_STM3210E_EVAL)  
  if ((x == 1) && (NOR_ID.Device_Code2 == NOR_M29W128G)&& (NOR_ID.Manufacturer_Code == 0x20))
  {
    x = 3 ;
  }
  else if((x == 1) && (NOR_ID.Device_Code2 == NOR_S29GL128) && (NOR_ID.Manufacturer_Code == 0x01))
  {
    x = 4 ;
  }  
#endif /* USE_STM3210E_EVAL */
  
  SET_POLLING_TIMING(TimingTable[x][y]);  /* x: Erase/Write Timing */
  /* y: Media              */
  return MAL_OK;
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
