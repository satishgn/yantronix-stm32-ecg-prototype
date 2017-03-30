/*****************************************************************************
* File    : @file
* Company : Yantronix
* Author  : Satish Nair
* Mailto  : satishgn77@gmail.com
*****************************************************************************/
/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2010        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/

#include "diskio.h"
#if defined (USE_SD_CARD_SDIO_INTERFACE)
#include "stm32_eval_sdio_sd.h"
#elif defined (USE_SD_CARD_SPI_INTERFACE)
#include "stm32_eval_spi_sd.h"
#endif

/*-----------------------------------------------------------------------*/
/* Correspondence between physical drive number and physical drive.      */
/*-----------------------------------------------------------------------*/
// #define ATA		0
// #define MMC		1
// #define USB		2

#define SECTOR_SIZE 512 /* Block Size in Bytes */

#if defined (USE_SD_CARD_SDIO_INTERFACE)
// External variables
extern SD_CardInfo SDCardInfo;
#elif defined (USE_SD_CARD_SPI_INTERFACE)
// Private variables
SD_CardInfo SDCardInfo;
#endif

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
)
{
	// DSTATUS stat;
	// int result;
	// switch (drv) {
	// case ATA :
		// result = ATA_disk_initialize();
		// /* translate the reslut code here */
		// return stat;
	// case MMC :
		// result = MMC_disk_initialize();
		// /* translate the reslut code here */
		// return stat;
	// case USB :
		// result = USB_disk_initialize();
		// /* translate the reslut code here */
		// return stat;
	// }
    
#if defined (USE_SD_CARD_SDIO_INTERFACE)
    SD_Error status; 

    status = SD_Init();

    if (status == SD_OK)
        return 0;
    else
        return STA_NOINIT;
#elif defined (USE_SD_CARD_SPI_INTERFACE)
    SD_Error status; 
    
    status = SD_Init();

    if (status == SD_RESPONSE_NO_ERROR)
    {
        /* Read CSD/CID MSD registers */
        status = SD_GetCardInfo(&SDCardInfo);
    }

    if (status == SD_RESPONSE_NO_ERROR)
        return 0;
    else
        return STA_NOINIT;
#endif

	return STA_NOINIT;
}


/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{
	// DSTATUS stat;
	// int result;
	// switch (drv) {
	// case ATA :
		// result = ATA_disk_status();
		// /* translate the reslut code here */
		// return stat;
	// case MMC :
		// result = MMC_disk_status();
		// /* translate the reslut code here */
		// return stat;
	// case USB :
		// result = USB_disk_status();
		// /* translate the reslut code here */
		// return stat;
	// }

    return 0;

	// return STA_NOINIT;
}


/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..255) */
)
{
	// DRESULT res;
	// int result;
	// switch (drv) {
	// case ATA :
		// result = ATA_disk_read(buff, sector, count);
		// /* translate the reslut code here */
		// return res;
	// case MMC :
		// result = MMC_disk_read(buff, sector, count);
		// /* translate the reslut code here */
		// return res;
	// case USB :
		// result = USB_disk_read(buff, sector, count);
		// /* translate the reslut code here */
		// return res;
	// }

#if defined (USE_SD_CARD_SDIO_INTERFACE)
    SD_Error status; 

    if (count == 1)
    {
        status = SD_ReadBlock((uint32_t *)buff, sector << 9, SECTOR_SIZE);        
    }
    else
    {
        status = SD_ReadMultiBlocks((uint32_t *)buff, sector << 9, SECTOR_SIZE, count);        
    } 

    if(status == SD_OK)
    {
        return RES_OK;
    }
    else
    {
        return RES_ERROR;
    }  
#elif defined (USE_SD_CARD_SPI_INTERFACE)
    SD_Error status; 

    if (count == 1)
    {
        status = SD_ReadBlock((uint32_t *)buff, sector << 9, SECTOR_SIZE);        
    }
    else
    {
        status = SD_ReadMultiBlocks((uint32_t *)buff, sector << 9, SECTOR_SIZE, count);        
    } 

    if(status == SD_RESPONSE_NO_ERROR)
    {
        return RES_OK;
    }
    else
    {
        return RES_ERROR;
    }  
#endif

	return RES_PARERR;
}


/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/
/* The FatFs module will issue multiple sector transfer request
/  (count > 1) to the disk I/O layer. The disk function should process
/  the multiple sector transfer properly Do. not translate it into
/  multiple single sector transfers to the media, or the data read/write
/  performance may be drasticaly decreased. */

#if _READONLY == 0
DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..255) */
)
{
	// DRESULT res;
	// int result;
	// switch (drv) {
	// case ATA :
		// result = ATA_disk_write(buff, sector, count);
		// /* translate the reslut code here */
		// return res;
	// case MMC :
		// result = MMC_disk_write(buff, sector, count);
		// /* translate the reslut code here */
		// return res;
	// case USB :
		// result = USB_disk_write(buff, sector, count);
		// /* translate the reslut code here */
		// return res;
	// }

#if defined (USE_SD_CARD_SDIO_INTERFACE)
    SD_Error status; 

    if (count == 1)
    {
        status = SD_WriteBlock((uint32_t *)buff, sector << 9, SECTOR_SIZE);        
    }
    else
    {
        status = SD_WriteMultiBlocks((uint32_t *)buff, sector << 9, SECTOR_SIZE, count);        
    } 

    if(status == SD_OK)
    {
        return RES_OK;
    }
    else
    {
        return RES_ERROR;
    }  
#elif defined (USE_SD_CARD_SPI_INTERFACE)
    SD_Error status; 

    if (count == 1)
    {
        status = SD_WriteBlock((uint32_t *)buff, sector << 9, SECTOR_SIZE);        
    }
    else
    {
        status = SD_WriteMultiBlocks((uint32_t *)buff, sector << 9, SECTOR_SIZE, count);        
    } 

    if(status == SD_RESPONSE_NO_ERROR)
    {
        return RES_OK;
    }
    else
    {
        return RES_ERROR;
    }  
#endif

	return RES_PARERR;
}
#endif /* _READONLY */

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
    // DRESULT res;
	// int result;
	// switch (drv) {
	// case ATA :
		// /* pre-process here */
		// result = ATA_disk_ioctl(ctrl, buff);
		// /* post-process here */
		// return res;
	// case MMC :
		// /* pre-process here */
		// result = MMC_disk_ioctl(ctrl, buff);
		// /* post-process here */
		// return res;
	// case USB :
		// /* pre-process here */
		// result = USB_disk_ioctl(ctrl, buff);
		// /* post-process here */
		// return res;
	// }

#if defined (USE_SD_CARD_SDIO_INTERFACE)
    SDTransferState status;
    
    switch (ctrl) {
    case CTRL_SYNC :            /* Make sure that no pending write process */
        status = SD_GetTransferState();
        if (status == SD_TRANSFER_OK)
            return RES_OK;
        else
            return RES_ERROR;
        break;

    case GET_SECTOR_SIZE :      /* Get R/W sector size (WORD) */ 
        *(WORD*)buff = SECTOR_SIZE;
        return RES_OK;
        break;
        
    case GET_SECTOR_COUNT :     /* Get number of sectors on the disk (DWORD) */
        *(DWORD*)buff = SDCardInfo.CardCapacity / SECTOR_SIZE;
        return RES_OK;
        break;

    case GET_BLOCK_SIZE :       /* Get erase block size in unit of sector (DWORD) */
        *(DWORD*)buff = SDCardInfo.CardBlockSize;
        return RES_OK;
        break;    
      }
#elif defined (USE_SD_CARD_SPI_INTERFACE)    
    switch (ctrl) {
    case CTRL_SYNC :            /* Make sure that no pending write process */
        // if (SD_GetStatus() == 0xFF)
            // return RES_OK;
        // else
            // return RES_ERROR;
        return RES_OK;
        break;

    case GET_SECTOR_SIZE :      /* Get R/W sector size (WORD) */ 
        *(WORD*)buff = SECTOR_SIZE;
        return RES_OK;
        break;
        
    case GET_SECTOR_COUNT :     /* Get number of sectors on the disk (DWORD) */
        *(DWORD*)buff = SDCardInfo.CardCapacity / SECTOR_SIZE;
        return RES_OK;
        break;

    case GET_BLOCK_SIZE :       /* Get erase block size in unit of sector (DWORD) */
        *(DWORD*)buff = SDCardInfo.CardBlockSize;
        return RES_OK;
        break;    
      }  
#endif
  
	return RES_PARERR;
}

