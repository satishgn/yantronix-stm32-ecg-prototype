/********************* (C) COPYRIGHT 2007 RAISONANCE S.A.S. *******************/
/**
*
* @file     lcd.c
* @brief    The LCD driver for the ST7637.
* @author   FL
* @date     07/2007
*
**/
/******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32_primer2_lcd.h"

/// @cond Internal

/* Private define ------------------------------------------------------------*/
#ifdef USE_STM32_PRIMER2
#define V9_MADCTRVAL                0xd0     /*!< Left orientation value.             */
#define V12_MADCTRVAL               0x70     /*!< Up orientation value.        MX+MV  */
#define V3_MADCTRVAL                0x10     /*!< Right orientation value.  MY+MX     */
#define V6_MADCTRVAL                0xB0     /*!< Bottom orientation value. MY+   MV  */
#else
#define V9_MADCTRVAL                0x90     /*!< Left orientation value.   MY        */
#define V12_MADCTRVAL               0x30     /*!< Up orientation value.           MV  */
#define V3_MADCTRVAL                0x50     /*!< Right orientation value.     MX     */
#define V6_MADCTRVAL                0xF0     /*!< Bottom orientation value. MY+MX+MV  */
#endif
#define BACKLIGHT_DIVIDER           500      /*!< LCD handler step.           */

/* Private variables ---------------------------------------------------------*/

/* Global variables to set the written text color */
static  __IO uint16_t TextColor = 0x0000, BackColor = 0xFFFF;

/* vars for timer dedicated for lcd backlight*/
static TIM_TimeBaseInitTypeDef      TIM_TimeBaseStructure;
static TIM_OCInitTypeDef            TIM_OCInitStructure;

s32                                 Current_CCR_BackLightStart = DEFAULT_CCR_BACKLIGHTSTART;

/* External variable ---------------------------------------------------------*/

s32                        CurrentRotateScreen        = +1;
Rotate_H12_V_Match_TypeDef CurrentScreenOrientation   = V12;

static u16 CharMagniCoeff = 1;                // !< Current character magnify coefficient.  
u8* CurrentFont = (u8*) &AsciiDotsTable;           /* pointer on the current font*/

/*! ASCII Table. Each character is 7 column (7 dots large) on two pages (16 dots high)
7 column character: Two 8 bit data to display one column*/
const u8 AsciiDotsTable[224 * 14] = {
    /* ASCII 32  */  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    /* ASCII 33  */  0x00,0x00,0x00,0x00,0x00,0x00,0x13,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,
    /* ASCII 34  */  0x00,0x00,0x00,0xe0,0x00,0x20,0x00,0x00,0x00,0xe0,0x00,0x20,0x00,0x00,
    /* ASCII 35  */  0x00,0x00,0x35,0x00,0x0f,0x80,0x35,0x60,0x0f,0x80,0x05,0x60,0x00,0x00,
    /* ASCII 36  */  0x00,0x00,0x0d,0x80,0x0a,0x40,0x3a,0x60,0x06,0x40,0x00,0x00,0x00,0x00,
    /* ASCII 37  */  0x00,0x00,0x02,0x40,0x02,0xa0,0x0a,0x40,0x15,0x00,0x09,0x00,0x00,0x00,
    /* ASCII 38  */  0x00,0x00,0x0c,0x00,0x13,0x00,0x14,0x80,0x08,0x80,0x14,0x00,0x00,0x00,
    /* ASCII 39  */  0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,
    /* ASCII 40  */  0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0x80,0x60,0x60,0x00,0x00,0x00,0x00,
    /* ASCII 41  */  0x00,0x00,0x00,0x00,0x60,0x60,0x1f,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
    /* ASCII 42  */  0x00,0x00,0x00,0x40,0x03,0x40,0x00,0xe0,0x03,0x40,0x00,0x40,0x00,0x00,
    /* ASCII 43  */  0x02,0x00,0x02,0x00,0x02,0x00,0x1f,0xc0,0x02,0x00,0x02,0x00,0x02,0x00,
    /* ASCII 44  */  0x00,0x00,0x00,0x00,0x60,0x00,0x38,0x00,0x08,0x00,0x00,0x00,0x00,0x00,
    /* ASCII 45  */  0x00,0x00,0x02,0x00,0x02,0x00,0x02,0x00,0x02,0x00,0x02,0x00,0x00,0x00,
    /* ASCII 46  */  0x00,0x00,0x00,0x00,0x18,0x00,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    /* ASCII 47  */  0x00,0x00,0x20,0x00,0x18,0x00,0x06,0x00,0x01,0x80,0x00,0x60,0x00,0x00,
    /* ASCII 48  */  0x00,0x00,0x0f,0xc0,0x10,0x20,0x10,0x20,0x10,0x20,0x0f,0xc0,0x00,0x00,
    /* ASCII 49  */  0x00,0x00,0x10,0x00,0x10,0x20,0x1f,0xe0,0x10,0x00,0x10,0x00,0x00,0x00,
    /* ASCII 50  */  0x00,0x00,0x18,0x40,0x14,0x20,0x12,0x20,0x11,0x20,0x18,0xc0,0x00,0x00,
    /* ASCII 51  */  0x00,0x00,0x08,0x40,0x10,0x20,0x11,0x20,0x11,0x20,0x0e,0xc0,0x00,0x00,
    /* ASCII 52  */  0x00,0x00,0x06,0x00,0x05,0x00,0x04,0xc0,0x14,0x20,0x1f,0xe0,0x14,0x00,
    /* ASCII 53  */  0x00,0x00,0x08,0x00,0x11,0xe0,0x11,0x20,0x11,0x20,0x0e,0x20,0x00,0x00,
    /* ASCII 54  */  0x00,0x00,0x0f,0x80,0x11,0x40,0x11,0x20,0x11,0x20,0x0e,0x20,0x00,0x00,
    /* ASCII 55  */  0x00,0x00,0x00,0x60,0x00,0x20,0x18,0x20,0x07,0x20,0x00,0xe0,0x00,0x00,
    /* ASCII 56  */  0x00,0x00,0x0e,0xc0,0x11,0x20,0x11,0x20,0x11,0x20,0x0e,0xc0,0x00,0x00,
    /* ASCII 57  */  0x00,0x00,0x11,0xc0,0x12,0x20,0x12,0x20,0x0a,0x20,0x07,0xc0,0x00,0x00,
    /* ASCII 58  */  0x00,0x00,0x00,0x00,0x19,0x80,0x19,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
    /* ASCII 59  */  0x00,0x00,0x00,0x00,0x30,0x00,0x19,0x80,0x09,0x80,0x00,0x00,0x00,0x00,
    /* ASCII 60  */  0x02,0x00,0x05,0x00,0x05,0x00,0x08,0x80,0x10,0x40,0x10,0x40,0x00,0x00,
    /* ASCII 61  */  0x00,0x00,0x05,0x00,0x05,0x00,0x05,0x00,0x05,0x00,0x05,0x00,0x00,0x00,
    /* ASCII 62  */  0x10,0x40,0x10,0x40,0x08,0x80,0x05,0x00,0x05,0x00,0x02,0x00,0x00,0x00,
    /* ASCII 63  */  0x00,0x00,0x00,0x00,0x10,0x80,0x14,0x40,0x02,0x40,0x01,0x80,0x00,0x00,
    /* ASCII 64  */  0x00,0x00,0x1f,0xe0,0x20,0x10,0x23,0x10,0x24,0x90,0x17,0xe0,0x00,0x00,
    /* ASCII 65  */  0x10,0x00,0x1c,0x00,0x17,0xa0,0x04,0x60,0x17,0x80,0x1c,0x00,0x10,0x00,
    /* ASCII 66  */  0x10,0x20,0x1f,0xe0,0x11,0x20,0x11,0x20,0x11,0x20,0x0e,0xc0,0x00,0x00,
    /* ASCII 67  */  0x00,0x00,0x0f,0xc0,0x10,0x20,0x10,0x20,0x10,0x20,0x08,0x60,0x00,0x00,
    /* ASCII 68  */  0x10,0x20,0x1f,0xe0,0x10,0x20,0x10,0x20,0x08,0x40,0x07,0x80,0x00,0x00,
    /* ASCII 69  */  0x10,0x20,0x1f,0xe0,0x11,0x20,0x13,0xa0,0x10,0x20,0x18,0x60,0x00,0x00,
    /* ASCII 70  */  0x00,0x00,0x10,0x20,0x1f,0xe0,0x11,0x20,0x03,0xa0,0x00,0x20,0x00,0x60,
    /* ASCII 71  */  0x00,0x00,0x0f,0xc0,0x10,0x20,0x10,0x20,0x12,0x20,0x0e,0x60,0x02,0x00,
    /* ASCII 72  */  0x10,0x20,0x1f,0xe0,0x11,0x20,0x01,0x00,0x11,0x20,0x1f,0xe0,0x10,0x20,
    /* ASCII 73  */  0x00,0x00,0x10,0x20,0x10,0x20,0x1f,0xe0,0x10,0x20,0x10,0x20,0x00,0x00,
    /* ASCII 74  */  0x00,0x00,0x0e,0x00,0x10,0x20,0x10,0x20,0x0f,0xe0,0x00,0x20,0x00,0x00,
    /* ASCII 75  */  0x10,0x20,0x1f,0xe0,0x12,0x20,0x03,0x00,0x04,0xa0,0x18,0x60,0x10,0x20,
    /* ASCII 76  */  0x00,0x00,0x10,0x20,0x1f,0xe0,0x10,0x20,0x10,0x00,0x1c,0x00,0x00,0x00,
    /* ASCII 77  */  0x10,0x20,0x1f,0xe0,0x10,0xe0,0x03,0x00,0x10,0xe0,0x1f,0xe0,0x10,0x20,
    /* ASCII 78  */  0x10,0x20,0x1f,0xe0,0x10,0xe0,0x07,0x00,0x18,0x20,0x1f,0xe0,0x00,0x20,
    /* ASCII 79  */  0x00,0x00,0x0f,0xc0,0x10,0x20,0x10,0x20,0x10,0x20,0x0f,0xc0,0x00,0x00,
    /* ASCII 80  */  0x00,0x00,0x10,0x20,0x1f,0xe0,0x12,0x20,0x02,0x20,0x01,0xc0,0x00,0x00,
    /* ASCII 81  */  0x00,0x00,0x0f,0xc0,0x10,0x20,0x30,0x20,0x30,0x20,0x2f,0xc0,0x00,0x00,
    /* ASCII 82  */  0x10,0x20,0x1f,0xe0,0x12,0x20,0x02,0x20,0x06,0x20,0x09,0xc0,0x10,0x00,
    /* ASCII 83  */  0x00,0x00,0x18,0xc0,0x09,0x20,0x11,0x20,0x11,0x40,0x0e,0x60,0x00,0x00,
    /* ASCII 84  */  0x00,0x60,0x00,0x20,0x10,0x20,0x1f,0xe0,0x10,0x20,0x00,0x20,0x00,0x60,
    /* ASCII 85  */  0x00,0x20,0x0f,0xe0,0x10,0x20,0x10,0x00,0x10,0x20,0x0f,0xe0,0x00,0x20,
    /* ASCII 86  */  0x00,0x20,0x00,0xe0,0x07,0x20,0x18,0x00,0x07,0x20,0x00,0xe0,0x00,0x20,
    /* ASCII 87  */  0x00,0x20,0x0f,0xe0,0x10,0x20,0x0f,0x00,0x10,0x20,0x0f,0xe0,0x00,0x20,
    /* ASCII 88  */  0x10,0x20,0x18,0x60,0x04,0x80,0x03,0x00,0x04,0x80,0x18,0x60,0x10,0x20,
    /* ASCII 89  */  0x00,0x20,0x00,0x60,0x11,0xa0,0x1e,0x00,0x11,0xa0,0x00,0x60,0x00,0x20,
    /* ASCII 90  */  0x00,0x00,0x18,0x60,0x14,0x20,0x13,0x20,0x10,0xa0,0x18,0x60,0x00,0x00,
    /* ASCII 91  */  0x00,0x00,0x00,0x00,0x7f,0xe0,0x40,0x20,0x40,0x20,0x00,0x00,0x00,0x00,
    /* ASCII 92  */  0x00,0x00,0x00,0x20,0x01,0xc0,0x06,0x00,0x38,0x00,0x00,0x00,0x00,0x00,
    /* ASCII 93  */  0x00,0x00,0x00,0x00,0x40,0x20,0x40,0x20,0x7f,0xe0,0x00,0x00,0x00,0x00,
    /* ASCII 94  */  0x00,0x00,0x01,0x00,0x00,0x80,0x00,0x60,0x00,0x80,0x01,0x00,0x00,0x00,
    /* ASCII 95  */  0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,
    /* ASCII 96  */  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x40,0x00,0x00,0x00,0x00,
    /* ASCII 97  */  0x00,0x00,0x0d,0x00,0x12,0x80,0x12,0x80,0x12,0x80,0x1f,0x00,0x10,0x00,
    /* ASCII 98  */  0x10,0x20,0x1f,0xe0,0x11,0x00,0x10,0x80,0x10,0x80,0x0f,0x00,0x00,0x00,
    /* ASCII 99  */  0x00,0x00,0x0f,0x00,0x10,0x80,0x10,0x80,0x10,0x80,0x09,0x80,0x00,0x00,
    /* ASCII 100 */  0x00,0x00,0x0f,0x00,0x10,0x80,0x10,0x80,0x11,0x20,0x1f,0xe0,0x10,0x00,
    /* ASCII 101 */  0x00,0x00,0x0f,0x00,0x12,0x80,0x12,0x80,0x12,0x80,0x13,0x00,0x00,0x00,
    /* ASCII 102 */  0x00,0x00,0x10,0x80,0x1f,0xc0,0x10,0xa0,0x10,0xa0,0x10,0xa0,0x00,0x00,
    /* ASCII 103 */  0x00,0x00,0x0f,0x00,0x50,0x80,0x50,0x80,0x51,0x00,0x3f,0x80,0x00,0x80,
    /* ASCII 104 */  0x10,0x20,0x1f,0xe0,0x11,0x00,0x00,0x80,0x10,0x80,0x1f,0x00,0x10,0x00,
    /* ASCII 105 */  0x00,0x00,0x10,0x80,0x10,0x80,0x1f,0xa0,0x10,0x00,0x10,0x00,0x00,0x00,
    /* ASCII 106 */  0x00,0x00,0x40,0x80,0x40,0x80,0x40,0xa0,0x3f,0x80,0x00,0x00,0x00,0x00,
    /* ASCII 107 */  0x10,0x20,0x1f,0xe0,0x02,0x00,0x16,0x80,0x19,0x80,0x10,0x80,0x00,0x00,
    /* ASCII 108 */  0x00,0x00,0x10,0x00,0x10,0x20,0x1f,0xe0,0x10,0x00,0x10,0x00,0x00,0x00,
    /* ASCII 109 */  0x10,0x80,0x1f,0x80,0x10,0x80,0x1f,0x00,0x10,0x80,0x1f,0x00,0x10,0x00,
    /* ASCII 110 */  0x10,0x80,0x1f,0x80,0x11,0x00,0x00,0x80,0x10,0x80,0x1f,0x00,0x10,0x00,
    /* ASCII 111 */  0x00,0x00,0x0f,0x00,0x10,0x80,0x10,0x80,0x10,0x80,0x0f,0x00,0x00,0x00,
    /* ASCII 112 */  0x40,0x80,0x7f,0x80,0x51,0x00,0x10,0x80,0x10,0x80,0x0f,0x00,0x00,0x00,
    /* ASCII 113 */  0x00,0x00,0x0f,0x00,0x10,0x80,0x10,0x80,0x51,0x00,0x7f,0x80,0x40,0x80,
    /* ASCII 114 */  0x00,0x00,0x10,0x80,0x1f,0x80,0x11,0x00,0x10,0x80,0x10,0x80,0x00,0x00,
    /* ASCII 115 */  0x00,0x00,0x19,0x00,0x12,0x80,0x12,0x80,0x12,0x80,0x0d,0x80,0x00,0x00,
    /* ASCII 116 */  0x00,0x00,0x00,0x80,0x0f,0xc0,0x10,0x80,0x10,0x80,0x10,0x80,0x08,0x00,
    /* ASCII 117 */  0x00,0x80,0x0f,0x80,0x10,0x00,0x10,0x00,0x08,0x80,0x1f,0x80,0x10,0x00,
    /* ASCII 118 */  0x00,0x80,0x03,0x80,0x0c,0x80,0x10,0x00,0x0c,0x80,0x03,0x80,0x00,0x80,
    /* ASCII 119 */  0x00,0x80,0x0f,0x80,0x10,0x80,0x0e,0x00,0x10,0x80,0x0f,0x80,0x00,0x80,
    /* ASCII 120 */  0x10,0x80,0x19,0x80,0x06,0x00,0x06,0x00,0x19,0x80,0x10,0x80,0x00,0x00,
    /* ASCII 121 */  0x00,0x80,0x41,0x80,0x46,0x80,0x78,0x00,0x4c,0x80,0x03,0x80,0x00,0x80,
    /* ASCII 122 */  0x00,0x00,0x19,0x80,0x14,0x80,0x12,0x80,0x11,0x80,0x18,0x80,0x00,0x00,
    /* ASCII 123 */  0x00,0x00,0x00,0x00,0x04,0x00,0x3b,0xc0,0x40,0x20,0x00,0x00,0x00,0x00,
    /* ASCII 124 */  0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,
    /* ASCII 125 */  0x00,0x00,0x00,0x00,0x40,0x20,0x3b,0xc0,0x04,0x00,0x00,0x00,0x00,0x00,
    /* ASCII 126 */  0x00,0x00,0x04,0x00,0x02,0x00,0x04,0x00,0x04,0x00,0x02,0x00,0x00,0x00,
    /* ASCII 127 */  0x00,0x00,0x03,0x80,0x02,0x40,0x02,0x20,0x02,0x40,0x03,0x80,0x00,0x00,
    /* ASCII 128 */  0x00,0x00,0x0F,0xC0,0x50,0x20,0x70,0x20,0x10,0x20,0x08,0x60,0x00,0x00,
    /* ASCII 129 */  0x00,0x80,0x0F,0xA0,0x10,0x00,0x10,0x00,0x08,0x80,0x1F,0xA0,0x10,0x00,
    /* ASCII 130 */  0x00,0x00,0x0F,0x00,0x12,0xA0,0x12,0x90,0x12,0x90,0x13,0x00,0x00,0x00,
    /* ASCII 131 */  0x00,0x00,0x0D,0x00,0x12,0xA0,0x12,0x90,0x12,0x90,0x1F,0x20,0x10,0x00,
    /* ASCII 132 */  0x00,0x00,0x0D,0x00,0x12,0xB0,0x12,0x80,0x12,0x80,0x1F,0x30,0x10,0x00,
    /* ASCII 133 */  0x00,0x00,0x0D,0x00,0x12,0x90,0x12,0x90,0x12,0xA0,0x1F,0x00,0x10,0x00,
    /* ASCII 134 */  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    /* ASCII 135 */  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    /* ASCII 136 */  0x00,0x00,0x0F,0x00,0x12,0xA0,0x12,0x90,0x12,0x90,0x13,0x20,0x00,0x00,
    /* ASCII 137 */  0x00,0x00,0x0F,0x00,0x12,0xB0,0x12,0x80,0x12,0x80,0x13,0x30,0x00,0x00,
    /* ASCII 138 */  0x00,0x00,0x0F,0x10,0x12,0x90,0x12,0xB0,0x12,0x80,0x13,0x00,0x00,0x00,
    /* ASCII 139 */  0x00,0x00,0x10,0x08,0x10,0x20,0x1F,0xE0,0x10,0x08,0x10,0x00,0x00,0x00,
    /* ASCII 140 */  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    /* ASCII 141 */  0x00,0x00,0x10,0x00,0x10,0x22,0x1F,0xE4,0x10,0x08,0x10,0x00,0x00,0x00,
    /* ASCII 142 */  0x10,0x00,0x1C,0x00,0x17,0x88,0x04,0x60,0x17,0x88,0x1C,0x00,0x10,0x00,
    /* ASCII 143 */  0x10,0x00,0x1C,0x00,0x17,0x84,0x04,0x6A,0x17,0x84,0x1C,0x00,0x10,0x00,
    /* ASCII 144 */  0x10,0x20,0x1F,0xE0,0x11,0x30,0x13,0xA8,0x10,0x24,0x18,0x60,0x00,0x00,
    /* ASCII 145 */  0x00,0x00,0x0D,0x00,0x15,0x00,0x1F,0x00,0x15,0x00,0x16,0x00,0x00,0x00,
    /* ASCII 146 */  0x00,0x00,0x1F,0x80,0x02,0x40,0x1F,0xC0,0x12,0x40,0x12,0x40,0x00,0x00,
    /* ASCII 147 */  0x00,0x00,0x0F,0x00,0x10,0xA0,0x10,0x90,0x10,0xA0,0x0F,0x00,0x00,0x00,
    /* ASCII 148 */  0x00,0x00,0x0F,0x00,0x10,0xA0,0x10,0x80,0x10,0xA0,0x0F,0x00,0x00,0x00,
    /* ASCII 149 */  0x00,0x00,0x0F,0x00,0x10,0x88,0x10,0x90,0x10,0xA0,0x0F,0x00,0x00,0x00,
    /* ASCII 150 */  0x00,0x80,0x0F,0x80,0x10,0x20,0x10,0x10,0x08,0xA0,0x1F,0x80,0x10,0x00,
    /* ASCII 151 */  0x00,0x80,0x0F,0x90,0x10,0x10,0x10,0x30,0x08,0x80,0x1F,0x80,0x10,0x00,
    /* ASCII 152 */  0x00,0x80,0x41,0x80,0x46,0xA0,0x78,0x00,0x4C,0xA0,0x03,0x80,0x00,0x80,
    /* ASCII 153 */  0x00,0x00,0x0F,0xC0,0x10,0x28,0x10,0x20,0x10,0x28,0x0F,0xC0,0x00,0x00,
    /* ASCII 154 */  0x00,0x20,0x0F,0xE8,0x10,0x20,0x10,0x00,0x10,0x20,0x0F,0xE8,0x00,0x20,
    /* ASCII 155 */  0x00,0x00,0x3F,0x00,0x18,0x80,0x16,0x80,0x11,0x80,0x0F,0xC0,0x00,0x00,
    /* ASCII 156 */  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    /* ASCII 157 */  0x00,0x00,0x3F,0xC0,0x1C,0x20,0x13,0x20,0x10,0xE0,0x0F,0xF0,0x00,0x00,
    /* ASCII 158 */  0x00,0x00,0x08,0x80,0x05,0x00,0x02,0x00,0x05,0x00,0x08,0x80,0x00,0x00,
    /* ASCII 159 */  0x00,0x00,0x20,0x00,0x42,0x00,0x3F,0xE0,0x02,0x10,0x00,0x20,0x00,0x00,
    /* ASCII 160 */  0x00,0x00,0x0D,0x00,0x12,0xA0,0x12,0x90,0x12,0x90,0x1F,0x00,0x10,0x00,
    /* ASCII 161 */  0x00,0x00,0x10,0x00,0x10,0x28,0x1F,0xE4,0x10,0x02,0x10,0x00,0x00,0x00,
    /* ASCII 162 */  0x00,0x00,0x0F,0x00,0x10,0xA0,0x10,0x90,0x10,0x88,0x0F,0x00,0x00,0x00,
    /* ASCII 163 */  0x00,0x20,0x0F,0xE0,0x10,0x28,0x10,0x04,0x10,0x22,0x0F,0xE0,0x00,0x20,
    /* ASCII 164 */  0x10,0x80,0x1F,0x80,0x11,0x20,0x00,0x90,0x10,0xA0,0x1F,0x10,0x10,0x00,
    /* ASCII 165 */  0x10,0x20,0x1F,0xE0,0x10,0xE8,0x07,0x04,0x18,0x28,0x1F,0xE4,0x00,0x20,
    /* ASCII 166 */  0x00,0x00,0x13,0x40,0x14,0xA0,0x14,0xA0,0x14,0xA0,0x17,0xC0,0x14,0x00,
    /* ASCII 167 */  0x00,0x00,0x13,0xC0,0x14,0x20,0x14,0x20,0x14,0x20,0x13,0xC0,0x00,0x00,
    /* ASCII 168 */  0x00,0x00,0x0C,0x00,0x12,0x00,0x11,0x40,0x08,0x40,0x00,0x00,0x00,0x00,
    /* ASCII 169 */  0x0F,0xC0,0x10,0x20,0x17,0x20,0x12,0xA0,0x15,0x20,0x10,0x20,0x0F,0xC0,
    /* ASCII 170 */  0x00,0x08,0x00,0x08,0x00,0x08,0x00,0x08,0x00,0x08,0x00,0x18,0x00,0x00,
    /* ASCII 171 */  0x00,0x00,0x05,0xC0,0x02,0x00,0x13,0x00,0x1A,0x80,0x14,0x00,0x00,0x00,
    /* ASCII 172 */  0x00,0x00,0x05,0xC0,0x02,0x00,0x0D,0x00,0x0A,0x80,0x1E,0x00,0x08,0x00,
    /* ASCII 173 */  0x00,0x00,0x0C,0x00,0x1F,0x40,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    /* ASCII 174 */  0x00,0x00,0x04,0x00,0x0A,0x00,0x00,0x00,0x04,0x00,0x0A,0x00,0x00,0x00,
    /* ASCII 175 */  0x00,0x00,0x0A,0x00,0x04,0x00,0x00,0x00,0x0A,0x00,0x04,0x00,0x00,0x00,
    /* ASCII 176 */  0x08,0x80,0x02,0x20,0x08,0x80,0x02,0x20,0x08,0x80,0x02,0x20,0x00,0x00,
    /* ASCII 177 */  0x15,0x40,0x0A,0xA0,0x15,0x40,0x0A,0xA0,0x15,0x40,0x0A,0xA0,0x00,0x00,
    /* ASCII 178 */  0x17,0x60,0x1D,0xC0,0x17,0x60,0x1D,0xC0,0x17,0x60,0x1D,0xC0,0x00,0x00,
    /* ASCII 179 */  0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,
    /* ASCII 180 */  0x01,0x00,0x01,0x00,0x01,0x00,0x1F,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,
    /* ASCII 181 */  0x10,0x00,0x1C,0x00,0x17,0x90,0x04,0x68,0x17,0x84,0x1C,0x00,0x10,0x00,
    /* ASCII 182 */  0x10,0x00,0x1C,0x10,0x17,0x88,0x04,0x64,0x17,0x88,0x1C,0x10,0x10,0x00,
    /* ASCII 183 */  0x10,0x00,0x1C,0x00,0x17,0x84,0x04,0x68,0x17,0x90,0x1C,0x00,0x10,0x00,
    /* ASCII 184 */  0x0F,0xC0,0x10,0x20,0x17,0xA0,0x14,0xA0,0x14,0xA0,0x10,0x20,0x0F,0xC0,
    /* ASCII 185 */  0x02,0x80,0x02,0x80,0xFE,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,
    /* ASCII 186 */  0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,
    /* ASCII 187 */  0x02,0x80,0x02,0x80,0xFE,0x80,0x00,0x80,0xFF,0x80,0x00,0x00,0x00,0x00,
    /* ASCII 188 */  0x02,0x80,0x02,0x80,0x02,0xFF,0x02,0x00,0x03,0xFF,0x00,0x00,0x00,0x00,
    /* ASCII 189 */  0x00,0x00,0x06,0x00,0x09,0x00,0x19,0x80,0x09,0x00,0x00,0x00,0x00,0x00,
    /* ASCII 190 */  0x00,0x00,0x0A,0x60,0x0A,0x80,0x1F,0x00,0x0A,0x80,0x0A,0x60,0x00,0x00,
    /* ASCII 191 */  0x01,0x00,0x01,0x00,0x01,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    /* ASCII 192 */  0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xFF,0x01,0x00,0x01,0x00,0x01,0x00,
    /* ASCII 193 */  0x01,0x00,0x01,0x00,0x01,0x00,0x01,0xFF,0x01,0x00,0x01,0x00,0x01,0x00,
    /* ASCII 194 */  0x01,0x00,0x01,0x00,0x01,0x00,0xFF,0x00,0x01,0x00,0x01,0x00,0x01,0x00,
    /* ASCII 195 */  0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x01,0x00,0x01,0x00,0x01,0x00,
    /* ASCII 196 */  0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,
    /* ASCII 197 */  0x01,0x00,0x01,0x00,0x01,0x00,0xFF,0xFF,0x01,0x00,0x01,0x00,0x01,0x00,
    /* ASCII 198 */  0x00,0x00,0x0D,0x20,0x12,0x90,0x12,0xA0,0x12,0x90,0x1F,0x00,0x10,0x00,
    /* ASCII 199 */  0x10,0x00,0x1C,0x00,0x17,0x88,0x04,0x64,0x17,0x88,0x1C,0x04,0x10,0x00,
    /* ASCII 200 */  0x00,0x00,0x00,0x00,0x03,0xFF,0x02,0x00,0x02,0xFF,0x02,0x80,0x02,0x80,
    /* ASCII 201 */  0x00,0x00,0x00,0x00,0xFF,0x80,0x00,0x80,0xFE,0x80,0x02,0x80,0x02,0x80,
    /* ASCII 202 */  0x02,0x80,0x02,0x80,0x02,0xFF,0x02,0x00,0x02,0xFF,0x02,0x80,0x02,0x80,
    /* ASCII 203 */  0x02,0x80,0x02,0x80,0xFE,0x80,0x00,0x80,0xFE,0x80,0x02,0x80,0x02,0x80,
    /* ASCII 204 */  0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFE,0xFF,0x02,0x80,0x02,0x80,
    /* ASCII 205 */  0x02,0x80,0x02,0x80,0x02,0x80,0x02,0x80,0x02,0x80,0x02,0x80,0x02,0x80,
    /* ASCII 206 */  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    /* ASCII 207 */  0x17,0xA0,0x08,0x40,0x08,0x40,0x08,0x40,0x08,0x40,0x17,0xA0,0x00,0x00,
    /* ASCII 208 */  0x00,0x00,0x08,0x80,0x15,0x40,0x16,0x40,0x0C,0x00,0x00,0x00,0x00,0x00,
    /* ASCII 209 */  0x11,0x20,0x1F,0xE0,0x11,0x20,0x10,0x20,0x08,0x40,0x07,0x80,0x00,0x00,
    /* ASCII 210 */  0x10,0x20,0x1F,0xE0,0x11,0x30,0x13,0xA8,0x10,0x30,0x18,0x60,0x00,0x00,
    /* ASCII 211 */  0x10,0x20,0x1F,0xE0,0x11,0x28,0x13,0xA0,0x10,0x28,0x18,0x60,0x00,0x00,
    /* ASCII 212 */  0x10,0x20,0x1F,0xE0,0x11,0x24,0x13,0xA8,0x10,0x30,0x18,0x60,0x00,0x00,
    /* ASCII 213 */  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0x00,0x00,0x00,0x00,0x00,0x00,
    /* ASCII 214 */  0x00,0x00,0x10,0x20,0x10,0x20,0x1F,0xF0,0x10,0x28,0x10,0x24,0x00,0x00,
    /* ASCII 215 */  0x00,0x00,0x10,0x30,0x10,0x28,0x1F,0xE4,0x10,0x28,0x10,0x30,0x00,0x00,
    /* ASCII 216 */  0x00,0x00,0x10,0x20,0x10,0x28,0x1F,0xE0,0x10,0x28,0x10,0x20,0x00,0x00,
    /* ASCII 217 */  0x01,0x00,0x01,0x00,0x01,0x00,0x01,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,
    /* ASCII 218 */  0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x00,0x01,0x00,0x01,0x00,0x01,0x00,
    /* ASCII 219 */  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    /* ASCII 220 */  0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,
    /* ASCII 221 */  0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,
    /* ASCII 222 */  0x00,0x00,0x10,0x24,0x10,0x28,0x1F,0xF0,0x10,0x20,0x10,0x20,0x00,0x00,
    /* ASCII 223 */  0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,
    /* ASCII 224 */  0x00,0x00,0x0F,0xC0,0x10,0x20,0x10,0x30,0x10,0x28,0x0F,0xC0,0x00,0x00,
    /* ASCII 225 */  0x00,0x00,0x7F,0xE0,0x11,0x20,0x11,0x20,0x11,0x20,0x0E,0xC0,0x00,0x00,
    /* ASCII 226 */  0x00,0x00,0x0F,0xC0,0x10,0x28,0x10,0x24,0x10,0x28,0x0F,0xC0,0x00,0x00,
    /* ASCII 227 */  0x00,0x00,0x0F,0xC0,0x10,0x28,0x10,0x30,0x10,0x20,0x0F,0xC0,0x00,0x00,
    /* ASCII 228 */  0x00,0x00,0x0F,0x00,0x10,0xA0,0x10,0x90,0x10,0xA0,0x0F,0x10,0x00,0x00,
    /* ASCII 229 */  0x00,0x00,0x0F,0xC0,0x10,0x28,0x10,0x24,0x10,0x28,0x0F,0xC4,0x00,0x00,
    /* ASCII 230 */  0x40,0x80,0x3F,0x80,0x10,0x00,0x10,0x00,0x08,0x80,0x1F,0x80,0x10,0x00,
    /* ASCII 231 */  0x00,0x00,0x1F,0xC0,0x15,0x40,0x05,0x00,0x02,0x00,0x00,0x00,0x00,0x00,
    /* ASCII 232 */  0x00,0x00,0x1F,0xE0,0x14,0xA0,0x04,0x80,0x03,0x00,0x00,0x00,0x00,0x00,
    /* ASCII 233 */  0x00,0x20,0x0F,0xE0,0x10,0x20,0x10,0x0C,0x10,0x24,0x0F,0xE4,0x00,0x20,
    /* ASCII 234 */  0x00,0x20,0x0F,0xE0,0x10,0x28,0x10,0x04,0x10,0x28,0x0F,0xE0,0x00,0x20,
    /* ASCII 235 */  0x00,0x20,0x0F,0xE4,0x10,0x24,0x10,0x0C,0x10,0x20,0x0F,0xE0,0x00,0x20,
    /* ASCII 236 */  0x00,0x80,0x41,0x80,0x46,0xA0,0x78,0x10,0x4C,0x88,0x03,0x80,0x00,0x80,
    /* ASCII 237 */  0x00,0x20,0x00,0x60,0x11,0xA0,0x1E,0x10,0x11,0xA8,0x00,0x64,0x00,0x20,
    /* ASCII 228 */  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    /* ASCII 239 */  0x00,0x00,0x00,0x04,0x00,0x03,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,
    /* ASCII 240 */  0x00,0x00,0x02,0x00,0x02,0x00,0x02,0x00,0x02,0x00,0x02,0x00,0x00,0x00,
    /* ASCII 241 */  0x00,0x00,0x11,0x00,0x11,0x00,0x17,0xC0,0x11,0x00,0x11,0x00,0x00,0x00,
    /* ASCII 242 */  0xA0,0x00,0xA0,0x00,0xA0,0x00,0xA0,0x00,0xA0,0x00,0xA0,0x00,0xA0,0x00,
    /* ASCII 243 */  0x02,0x20,0x06,0xA0,0x03,0xE0,0x0D,0x00,0x0A,0x80,0x1E,0x00,0x08,0x00,
    /* ASCII 244 */  0x00,0x00,0x00,0xC0,0x01,0x20,0x1F,0xE0,0x00,0x20,0x1F,0xE0,0x00,0x00,
    /* ASCII 245 */  0x00,0x00,0x0C,0x38,0x10,0xC4,0x11,0x44,0x11,0x84,0x0E,0x18,0x00,0x00,
    /* ASCII 246 */  0x00,0x00,0x02,0x00,0x02,0x00,0x0A,0x80,0x02,0x00,0x02,0x00,0x00,0x00,
    /* ASCII 247 */  0x00,0x00,0x80,0x00,0x60,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    /* ASCII 248 */  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x50,0x00,0x20,0x00,0x00,
    /* ASCII 249 */  0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,
    /* ASCII 250 */  0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    /* ASCII 251 */  0x00,0x00,0x01,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    /* ASCII 252 */  0x00,0x00,0x02,0xA0,0x03,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    /* ASCII 253 */  0x00,0x00,0x02,0x40,0x03,0x20,0x02,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,
    /* ASCII 254 */  0x00,0x00,0x0F,0x80,0x0F,0x80,0x0F,0x80,0x0F,0x80,0x0F,0x80,0x00,0x00,
    /* ASCII 255 */  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

    static s32 CharFilter_XMin = 0;         /*restricted area when painting characters*/
    static s32 CharFilter_XMax = 160;       /*restricted area when painting characters*/
    static s32 CharFilter_YMin = 0;         /*restricted area when painting characters*/
    static s32 CharFilter_YMax = 128;       /*restricted area when painting characters*/

    const s32 OrientationOffsetX [] = { +0   /* V12*/,  0    /* V3*/,    0/* V6*/,  +32   /* V9*/ , +0 /*V12 at reset */};
    const s32 OrientationOffsetY [] = { 0    /* V12*/,  0    /* V3*/,+32  /* V6*/,  0     /* V9*/ , +0 /*V12 at reset */};

    /* Private function prototypes -----------------------------------------------*/

    void LCD_FSMC_Init(void);   /* Important */
    static void LCD_ST7732S_Controller_init( void );
    static void LCD_BackLightChange( void );
    static void LCD_BackLightConfig( void );
    static void LCD_CtrlLinesWrite( GPIO_TypeDef* GPIOx, u32 CtrlPins, BitAction BitVal );

    /* Private functions ---------------------------------------------------------*/
    /*******************************************************************************
    *
    *                                LCD_CheckLCDStatus
    *
    *******************************************************************************/
    /**
    *  Check whether LCD LCD is busy or not.
    *
    **/
    /******************************************************************************/
    static void LCD_CheckLCDStatus( void )
        {
        /* send command RDDID*/
        LCD_SendLCDCmd(ST7732_RDDID);
        /* Read LCD device IDs*/
        LCD_ReadLCDData();
        LCD_ReadLCDData();
        LCD_ReadLCDData();
        }

    /*******************************************************************************
    *
    *                                LCD_DisplayRotate
    *
    *******************************************************************************/
    /**
    *  Configure the LCD controller for a given orientation.
    *
    *  @param[in]  H12 The new screen orientation.
    *
    **/
    /******************************************************************************/
    static void LCD_DisplayRotate( Rotate_H12_V_Match_TypeDef H12 )
        {
        /* Memory Access Control 0x36*/
        LCD_SendLCDCmd( ST7732_MADCTR );

        switch( H12 )
            {
            case V3  :
                LCD_SendLCDData( V3_MADCTRVAL );
                break;

            case V6  :
                LCD_SendLCDData( V6_MADCTRVAL );
                break;

            case V9  :
                LCD_SendLCDData( V9_MADCTRVAL );
                break;

            case V12 :
            default  :
                LCD_SendLCDData( V12_MADCTRVAL );
                break;
            }
        }

    void delayms(s32 count) 
        {
            vu32 index = 0; 
            for(index = (10000 * count); index != 0; index--)
            {
            }
        }

    /*******************************************************************************
    *
    *                                LCD_ST7732S_Controller_init
    *
    *******************************************************************************/
    /**
    *  Initialization of the controller registers.
    *
    **/
    /******************************************************************************/
        void LCD_ST7732S_Controller_init( void )				
        {
        s32 i;

#define WriteCOM LCD_SendLCDCmd
#define WriteData LCD_SendLCDData

#define WRITE_LCD(addr,val)   { WriteCOM(val); }

        delayms(100);
        GPIO_WriteBit( GPIOD, CtrlPin_RST, Bit_RESET ); /* RST = 0  */
        delayms(100);
        GPIO_WriteBit( GPIOD, CtrlPin_RST, Bit_SET );   /* RST = 1  */
        delayms(100);

        WriteCOM(ST7732_SWRESET);          /*Software Reset*/
        delayms(180);                      /*DELAY 150MS*/

        WriteCOM(ST7732_SLPIN);                    /*sleep IN*/
        delayms(100);
        WriteCOM(ST7732_SLPOUT);                    /*sleep out*/
        delayms(150);

        /* WriteCOM(0xc5);                   //VCOM Control 1*/
        /* WriteData(40);                    //set vcomh=4.475V*/
        /* WriteData(45);                    //SET VCOML=-0.800V*/

        WriteCOM(ST7732_FRMCTR1);          /*	 frame*/
        WriteData(0x06);
        WriteData(0x03);
        WriteData(0x02);  
        //delayms(10);

        WriteCOM(ST7732_INVCTR);           /*	0xb4*/
        WriteData(0x03);

        WriteCOM(ST7732_DISSET5);          /*  frame 0xB6*/
        WriteData(0x02);
        WriteData(0x0e);

        WriteCOM(ST7732_DISPCTRL);         /* 0xF5*/
        WriteData(0x1a);

        WriteCOM(0xc0);                    /*Power Control 1 ( gvdd vci1)*/
        WriteData(0x02);
        WriteData(0x00);

        WriteCOM(0xc1);                    /*Power Control 2( avdd vcl vgh vgl)	*/
        WriteData(0x05);

        WriteCOM(0xc2);                    /*Power Control 3 ( in normal)   */
        WriteData(0x02);
        WriteData(0x02);

        WriteCOM(0xc3);                    /*Power Control 4 ( in Idle mode)   */
        WriteData(0x01);
        WriteData(0x02);

        WriteCOM(0xc4);                    /*Power Control 5 ( in partial mode)  */
        WriteData(0x01);
        WriteData(0x02);

        WriteCOM(0xc5);                    /*Vcom Control ( vcomh,voml)  */
        WriteData(0x47);
        WriteData(0x3a);

        WriteCOM(ST7732_OSCADJ);           /* 0xF2 intern OSC 80Hz*/
        WriteData(0x02);

        WriteCOM(ST7732_DEFADJ);           /*0xF6*/
        WriteData(0x4c);

        WriteCOM(0xf8);   /*cmd 0xf8,dat 0x06; LOAD */
        WriteData(0x06);  /*SLEEP OUT LOAD DEFAULT*/


        /****************gamma adjust **********************/
        WriteCOM(0xe0);    		           /*gamma*/
        WriteData(0x06);
        WriteData(0x1c);
        WriteData(0x1f);
        WriteData(0x1f);
        WriteData(0x18);
        WriteData(0x13);
        WriteData(0x06);
        WriteData(0x03);
        WriteData(0x03);
        WriteData(0x04);
        WriteData(0x07);
        WriteData(0x07);
        WriteData(0x00);

        WriteCOM(0xe1);    		           /*gamma*/
        WriteData(0x0a);
        WriteData(0x14);
        WriteData(0x1b);
        WriteData(0x18);
        WriteData(0x12);
        WriteData(0x0e);
        WriteData(0x02);
        WriteData(0x01);
        WriteData(0x00);
        WriteData(0x01);
        WriteData(0x08);
        WriteData(0x07);
        WriteData(0x00);

        WriteCOM(ST7732_MADCTR);           /* Memory Access Control 0x36  */
        WriteData(V12_MADCTRVAL);

        WriteCOM(0x2a);    	               /*Column Range*/
        WriteData(0x00);
        WriteData(0x00);
        WriteData(0x00);
        WriteData(0x7F);

        WriteCOM(0x2b);    	               /*page Range*/
        WriteData(0x00);
        WriteData(0x00);
        WriteData(0x00);
        WriteData(0x7F);                   /* configured as 128x128*/

        WriteCOM(ST7732_COLMOD);           /* Color mode = 65k 0x3A*/
        WriteData(0x55);

        WriteCOM(ST7732_TEON);             /* 0x35*/
        WriteData(0x00);

        WriteCOM(0x29);                     /*display on*/
        delayms(20);

        /*LCD_FillRect ( 0,0,161,131, RGB_WHITE );*/
        LCD_FillRect_Circle ( 0,0,131,161, LCD_COLOR_WHITE );
        }

    /*******************************************************************************
    *
    *                                LCD_BackLightConfig
    *
    *******************************************************************************/
    /**
    *  Setting of the PWM that drives the backlight intensity.
    *
    **/
    /******************************************************************************/
void LCD_BackLightConfig( void )
        {
        GPIO_InitTypeDef GPIO_InitStructure;

        /* Enable GPIOB clock  */
        RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE );

        /* GPIOB Configuration:TIM4 2 in Output */
        GPIO_InitStructure.GPIO_Pin   = GPIO_BACKLIGHT_PIN;

        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

        GPIO_Init( GPIOB, &GPIO_InitStructure );


        /* TIM4 Configuration -----------------------------------------------------*/
        /* TIM4CLK = 12 MHz, Prescaler = 0x0 */

        /* Enable TIM4 clock */
        RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM4, ENABLE );

        TIM_DeInit( TIM4 );
        TIM_TimeBaseStructInit( &TIM_TimeBaseStructure );
        TIM_OCStructInit( &TIM_OCInitStructure );

        /* Time base configuration */
        TIM_TimeBaseStructure.TIM_Period          = 0xFFFF;
        TIM_TimeBaseStructure.TIM_Prescaler       = 0x00;
        TIM_TimeBaseStructure.TIM_ClockDivision   = 0x0;
        TIM_TimeBaseStructure.TIM_CounterMode     = TIM_CounterMode_Up;

        TIM_TimeBaseInit( TIM4, &TIM_TimeBaseStructure );

        /* Output Compare Toggle Mode configuration: Channel2 */
        TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1;
        //TIM_OCInitStructure.TIM_Channel     = TIM_Channel_2;			 /* FWLib v2.0 : replaced with next line*/
        TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   /* FWLib v2.0*/
        TIM_OCInitStructure.TIM_Pulse       = Current_CCR_BackLightStart;

        TIM_OC3Init( TIM4, &TIM_OCInitStructure );
        TIM_OC3PreloadConfig( TIM4, TIM_OCPreload_Disable );

        TIM_ARRPreloadConfig( TIM4, ENABLE );

        /* Enable TIM4 IT */
        /*TIM_ITConfig( TIM4, TIMER_IT_CHANN, ENABLE );*/

        /* Go !!!*/
        TIM_Cmd( TIM4, ENABLE );
        }

    /*******************************************************************************
    *
    *                                LCD_BackLightChange
    *
    *******************************************************************************/
    /**
    *  Modify the PWM rate.
    *
    **/
    /******************************************************************************/
    static void LCD_BackLightChange( void )
        {
        /* Output Compare Toggle Mode configuration: Channel2 */
        TIM_OCInitStructure.TIM_Pulse = Current_CCR_BackLightStart;

        TIM_OC3Init( TIM4, &TIM_OCInitStructure );
        }

    /* Public functions for CircleOS ---------------------------------------------*/

    /*******************************************************************************
    *
    *                                LCD_Init
    *
    *******************************************************************************/
    /**
    *
    *  Initialize LCD. Called at CircleOS startup.
    *
    *  @attention  This function must <b>NOT</b> be called by the user.
    *
    **/
    /******************************************************************************/
    void LCD_Init( void )
        {
        LCD_SetBackLight( Current_CCR_BackLightStart );

        /* Do some gpio configs*/
        GPIO_InitTypeDef GPIO_InitStructure;

        /* Enable GPIO clock for LCD */
        RCC_APB2PeriphClockCmd( GPIO_LCD_CTRL_PERIPH, ENABLE );
        RCC_APB2PeriphClockCmd( GPIO_LCD_D_PERIPH, ENABLE );
        RCC_APB2PeriphClockCmd( GPIO_LCD_CS_PERIPH, ENABLE );

        /* Enable GPIOC clock */
        RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE );

        /* Init BackLight*/
        LCD_BackLightConfig();

        /* LCD Init */

        /* Enable the FSMC Clock */
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

        /* Configure FSMC Bank1 NOR/SRAM3 */
        LCD_FSMC_Init();

        LCD_ST7732S_Controller_init();
        }

    /*******************************************************************************
    *
    *                                LCD_FSMC_Init
    *
    *******************************************************************************/
    /**
    *
    *  @attention  This function must <b>NOT</b> be called by the user.
    *
    **/
    /******************************************************************************/
    void LCD_FSMC_Init( void )
        {
        FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
        FSMC_NORSRAMTimingInitTypeDef  p;
        GPIO_InitTypeDef GPIO_InitStructure; 

        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | GPIO_LCD_D_PERIPH, ENABLE);


        /*-- GPIO Configuration ------------------------------------------------------*/
        /* SRAM Data lines configuration */

        GPIO_InitStructure.GPIO_Pin = LCD_DATA_PINS;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOx_D_LCD, &GPIO_InitStructure); 

        /* NOE, NWE and NE1 configuration */  
        GPIO_InitStructure.GPIO_Pin = CtrlPin_RD | CtrlPin_WR | CtrlPin_RS;
        GPIO_Init(GPIOx_CTRL_LCD, &GPIO_InitStructure);

        /* Reset : configured as regular GPIO, is not FSMC-controlled */
        GPIO_InitStructure.GPIO_Pin = CtrlPin_RST | CtrlPin_CS;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOD, &GPIO_InitStructure); 

        GPIO_WriteBit( GPIOD, CtrlPin_RST, Bit_SET ); /*reset active at LOW*/
        GPIO_WriteBit( GPIOD, CtrlPin_CS, Bit_RESET ); /*reset active at LOW*/


        /*-- FSMC Configuration ------------------------------------------------------*/
        p.FSMC_AddressSetupTime = 2;
        p.FSMC_AddressHoldTime = 2;
        p.FSMC_DataSetupTime = 2;
        p.FSMC_BusTurnAroundDuration = 5;
        p.FSMC_CLKDivision = 5;
        p.FSMC_DataLatency = 5;
        p.FSMC_AccessMode = FSMC_AccessMode_A;

        FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM3;
        FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
        FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
        FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;        /* changed from 16b*/
        FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
        FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;  
        FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;  /* cf RM p363 + p384*/
        FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
        FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
        FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
        FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
        FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
        FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
        FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
        FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

        FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 

        /* Enable FSMC Bank1_SRAM Bank */
        FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);  
        }

    /*******************************************************************************
    *
    *                                LCD_FillRect_Circle
    *
    *******************************************************************************/
    /**
    *
    *  Fill a rectangle with a provided color.
    *
    *  @param[in]  x        The horizontal coordinate of the rectangle low left corner.
    *  @param[in]  y        The vertical coordinate of the rectangle low left corner.
    *  @param[in]  width    The rectangle width in pixels.
    *  @param[in]  height   The rectangle height in pixels.
    *  @param[in]  color    The RGB color to fill the rectangle with.
    *
    *  @warning    The (0x0) point in on the low left corner.
    *
    *  Rq : this function does not check parameters validity 
    *
    **/
    /******************************************************************************/
        void LCD_FillRect_Circle( u16 x, u16 y, u16 width, u16 height, u16 color )
            {
            u8 Line;
            u8 Column;

            /* Select LCD screen area. */
            LCD_SetRect_For_Cmd( x, y, width, height );

            /* Send LCD RAM write command. */
            LCD_SendLCDCmd( ST7732_RAMWR );

            /* Fill selected LCD screen area with provided color. */
            for( Line = 0; Line < width; Line++ )
                {
                for( Column = 0; Column < height; Column++ )
                    {
                    LCD_SendLCDData( ( color >> 8 ) & 0xff );
                    LCD_SendLCDData( color & 0xff );
                    }
                }

            }

        // /*******************************************************************************
        // *
        // *                                LCD_FillRect
        // *
        // *******************************************************************************/
        // /**
        // *
        // *  Fill a rectangle with a provided color.
        // *
        // *  @param[in]  x        The horizontal coordinate of the rectangle low left corner.
        // *  @param[in]  y        The vertical coordinate of the rectangle low left corner.
        // *  @param[in]  width    The rectangle width in pixels.
        // *  @param[in]  height   The rectangle height in pixels.
        // *  @param[in]  color    The RGB color to fill the rectangle with.
        // *
        // *  @warning    The (0x0) point in on the low left corner.
        // *
        // **/
        // /******************************************************************************/
        // void LCD_FillRect( u16 x, u16 y, u16 width, u16 height, u16 color )
            // {
     
            // /* Rq : this fuction is just a wrapper with parameter limit checks*/
            
            // /* Check the x parameters*/
            // if (x < 0)
                // x = 0;
            // if (x > SCREEN_WIDTH)
                // x = SCREEN_WIDTH;
            // if ( (x + width) > SCREEN_WIDTH )
                // width = SCREEN_WIDTH - x;
            
            // /* Check the y parameters*/
            // if (y < 0)
                // y = 0;
            // if (y > SCREEN_HEIGHT)
                // y = SCREEN_HEIGHT;
            // if ( (y + height) > SCREEN_HEIGHT )
                // height = SCREEN_HEIGHT - y;

            // /* Call the native function*/
            // LCD_FillRect_Circle( x, y, width, height, color );
            // }

        /*******************************************************************************
        *
        *                                LCD_DrawRect_Circle
        *
        *******************************************************************************/
        /**
        *
        *  Draw a rectangle with a provided color.
        *
        *  @param[in]  x        The horizontal coordinate of the rectangle low left corner.
        *  @param[in]  y        The vertical coordinate of the rectangle low left corner.
        *  @param[in]  width    The rectangle width in pixels.
        *  @param[in]  height   The rectangle height in pixels.
        *  @param[in]  color    The RGB color to draw the rectangle with.
        *
        *  @warning    The (0x0) point in on the low left corner.
        *
        **/
        /******************************************************************************/
        void LCD_DrawRect_Circle( u16 x, u16 y, u16 width, u16 height, u16 color )
            {
            // /* Draw horizontal sides.*/
            // LCD_FillRect( x, y,              width, 1, color );
            // LCD_FillRect( x, y + height - 1, width, 1, color );

            // /* Draw vertical sides.*/
            // LCD_FillRect( x,              y, 1, height, color );
            // LCD_FillRect( x + width - 1,  y, 1, height, color );
            
            /* Draw horizontal sides.*/
            LCD_FillRect_Circle( x, y,              width, 1, color );
            LCD_FillRect_Circle( x, y + height - 1, width, 1, color );

            /* Draw vertical sides.*/
            LCD_FillRect_Circle( x,              y, 1, height, color );
            LCD_FillRect_Circle( x + width - 1,  y, 1, height, color );
            }
        
    /*******************************************************************************
    *
    *                                _LCD_SendLCDCmd
    *
    *******************************************************************************/
    /**
    *   Call LCD_SendLCDCmd function, with the ST7637_RAMWR or ST7732_RAMWR parameter
    *
    *
    **/
    /******************************************************************************/
    void _LCD_SendLCDCmd(void)
    {
        LCD_SendLCDCmd(ST7732_RAMWR);
    }

        /* Public functions ----------------------------------------------------------*/

        /*******************************************************************************
        *
        *                                LCD_SendLCDCmd
        *
        *******************************************************************************/
        /**
        *
        *  Send on command byte to the LCD.
        *
        *  @param[in]  Cmd   An u8 containing the user command to send to the LCD.
        *
        **/
        /******************************************************************************/
        void LCD_SendLCDCmd( u8 Cmd )
            {
            /* Send command to the LCD */
            *(u16 volatile*) (LCD_CMD_MODE_ADDR) = (Cmd<<4);  
            }

        /*******************************************************************************
        *
        *                                LCD_SendLCDData
        *
        *******************************************************************************/
        /**
        *
        *  Send one data byte to the LCD.
        *
        *  @param[in]  Data  An unsigned character containing the data to send to the LCD.
        *  @pre        An LCD_SendLCDCmd was done with a command waiting for data.
        *
        *
        **/
        /******************************************************************************/
        void LCD_SendLCDData( u8 Data )
            {
            /* Transfer data to the FSMC */
            *(u16 volatile*) (LCD_DATA_MODE_ADDR) = (Data<<4);  
            }

        /***********************************************************************************
        *
        *                                LCD_ReadLCDData
        *
        ************************************************************************************/
        /**
        *
        *  Read one data byte from the LCD.
        *
        *  @return     An unsigned 32 bit word containing the data returned by a LCD command.
        *  @pre        An LCD_SendLCDCmd was done with a command returning data.
        *
        **/
        /********************************************************************************/
        u32 LCD_ReadLCDData( void )
            {
            /* Transfer data from the memory */
            return   ( ( *(u16 volatile*) (LCD_DATA_MODE_ADDR)  )  >>4) ;
            }

        /*******************************************************************************
        *
        *                                LCD_DrawPixel
        *
        *******************************************************************************/
        /**
        *
        *  Draw a pixel on the LCD with the provided color.
        *
        *  @param[in]  XPos     The horizontal coordinate of the pixel.
        *  @param[in]  YPos     The vertical coordinate of the pixel.
        *  @param[in]  Color    The RGB color to draw the pixel with.
        *
        *  @warning    The (0x0) point in on the low left corner.
        *
        **/
        /******************************************************************************/
        void LCD_DrawPixel( u8 XPos, u8 YPos, u16 Color )
            {
            /* Select LCD screen area. */
            LCD_SetRect_For_Cmd( XPos, YPos, 1, 1 );

            /* Send LCD RAM write command. */
            LCD_SendLCDCmd( ST7732_RAMWR );

            /* Draw pixel.*/
            LCD_SendLCDData( Color >> 8 );
            LCD_SendLCDData( Color );
            }

        /*******************************************************************************
        *
        *                                LCD_RectRead
        *
        *******************************************************************************/
        /**
        *
        *  Save the pixels of a rectangle part of the LCD into a RAM variable.
        *
        *  @param[in]  x        The horizontal coordinate of the rectangle low left corner.
        *  @param[in]  y        The vertical coordinate of the rectangle low left corner.
        *  @param[in]  width    The rectangle width in pixels.
        *  @param[in]  height   The rectangle height in pixels.
        *  @param[out] bmp      The variable to store the read data into.
        *
        *  @warning    One pixel weights 2 bytes.
        *  @warning    The (0x0) point in on the low left corner.
        *
        **/
        /******************************************************************************/
        void LCD_RectRead( u16 x, u16 y, u16 width, u16 height, u8* bmp )
            {
            s32 i;
            s32 bytesize = (width * height) *2; /* 2 bytes per pixel.*/

            /* Select LCD screen area. */
            LCD_SetRect_For_Cmd( x, y, width, height );

            /* Restore 18 bit color mode */
            LCD_SendLCDCmd( ST7732_COLMOD );
            LCD_SendLCDData( 0x66 );

            /* Send LCD RAM read command. */
            LCD_SendLCDCmd( ST7732_RAMRD );
 
            /* First read byte is dummy!*/
            LCD_ReadLCDData();

            /* Read pixels from LCD screen.*/
            for( i = 0; i < bytesize; i++ )
                {
                u16 red_val  =   LCD_ReadLCDData() ;
                u16 byte0 = red_val & 0xf8;   /*red: keep only 5 bits */
                u16 green_val = LCD_ReadLCDData();
                *bmp++ = (byte0 | ((green_val>>5) & 0x7) );
                u16 blue_val =  LCD_ReadLCDData();
                *bmp++ = ( ( (blue_val>>3) & 0x1F ) | ((green_val<<3)&0xE0) );
                i++; /*don't multiply by two...*/
                }
             /* Restore 16 bit color mode */
            LCD_SendLCDCmd( ST7732_COLMOD );
            LCD_SendLCDData( 0x55 );
            }

        /*******************************************************************************
        *
        *                                LCD_GetPixel
        *
        *******************************************************************************/
        /**
        *
        *  Read the RGB color of the pixel the coordinate are provided in parameter.
        *
        *  @param[in]  x        The horizontal coordinate of the pixel.
        *  @param[in]  y        The vertical coordinate of the pixel.
        *  @return              An unsigned 16 bit word containing the RGB color of the pixel.
        *
        *  @warning    The (0x0) point in on the low left corner.
        *  @see        LCD_RectRead
        *
        **/
        /******************************************************************************/
        u16 LCD_GetPixel( u8 x, u8 y )
            {
            u16 val;

            LCD_RectRead( x, y, 1, 1, (u8*)&val );

            return val;
            }

        /*******************************************************************************
        *
        *                                LCD_SetRect_For_Cmd
        *
        *******************************************************************************/
        /**
        *
        *  Define the rectangle for the next command to be applied.
        *
        *  @param[in]  x        The horizontal coordinate of the rectangle low left corner.
        *  @param[in]  y        The vertical coordinate of the rectangle low left corner.
        *  @param[in]  width    The rectangle width in pixels.
        *  @param[in]  height   The rectangle height in pixels.
        *
        *  @warning    The (0x0) point in on the low left corner.
        *
        **/
        /******************************************************************************/
        void LCD_SetRect_For_Cmd( s16 x, s16 y, s16 width, s16 height )
            {
            LCD_SendLCDCmd( ST7732_CASET );
            LCD_SendLCDData( 0 );
            LCD_SendLCDData( y + OrientationOffsetY[ CurrentScreenOrientation ] );
            LCD_SendLCDData( 0 );
            LCD_SendLCDData( y + OrientationOffsetY[ CurrentScreenOrientation ] + height - 1 );
            LCD_SendLCDCmd( ST7732_RASET );
            LCD_SendLCDData( 0 );
            LCD_SendLCDData( x + OrientationOffsetX[ CurrentScreenOrientation ] );
            LCD_SendLCDData( 0 );
            LCD_SendLCDData( x + OrientationOffsetX[ CurrentScreenOrientation ] + width - 1 );
            }

        /*******************************************************************************
        *
        *                                LCD_SetBackLight
        *
        *******************************************************************************/
        /**
        *
        *  Modify the PWM rate. Any value below BACKLIGHTMIN reset the value to the
        *  default value (DEFAULT_CCR_BACKLIGHTSTART).
        *
        *  @param[in]  newBacklightStart The new PWM rate.
        *
        **/
        /******************************************************************************/
        void LCD_SetBackLight( u32 newBacklightStart )
            {
            if( newBacklightStart >= BACKLIGHTMIN )
                {
                Current_CCR_BackLightStart = newBacklightStart;
                }
            else
                {
                Current_CCR_BackLightStart = DEFAULT_CCR_BACKLIGHTSTART;
                }
            }

        /*******************************************************************************
        *
        *                                LCD_SetBackLightOff
        *
        *******************************************************************************/
        /**
        *
        *  Switch the LCD back light off.
        *
        **/
        /******************************************************************************/
        void LCD_SetBackLightOff( void )
            {
            Current_CCR_BackLightStart = 0;
            }

        /*******************************************************************************
        *
        *                                LCD_SetBackLightOn
        *
        *******************************************************************************/
        /**
        *
        *  Switch the LCD back light on.
        *
        **/
        /******************************************************************************/
        void LCD_SetBackLightOn( void )
            {
            Current_CCR_BackLightStart = DEFAULT_CCR_BACKLIGHTSTART;
            }

        /*******************************************************************************
        *
        *                                LCD_GetBackLight
        *
        *******************************************************************************/
        /**
        *
        *  Returns le LCD PWM rate.
        *
        *  @return The current LCD PWM rate.
        *
        **/
        /******************************************************************************/
        u32 LCD_GetBackLight( void )
            {
            return Current_CCR_BackLightStart;
            }

        /*******************************************************************************
        *
        *                                LCD_SetRotateScreen
        *
        *******************************************************************************/
        /**
        *
        *  Enable or disable the ability of the screen display to rotate according to
        *  the MEMs information.
        *
        *  @param[in]  RotateScreen 0 to disable screen rotation and 1 to enable.
        *
        **/
        /******************************************************************************/
        void LCD_SetRotateScreen( u8 RotateScreen)
            {
            CurrentRotateScreen = RotateScreen;
            }

        /*******************************************************************************
        *
        *                                LCD_GetRotateScreen
        *
        *******************************************************************************/
        /**
        *
        *  Return the screen rotation mode.
        *
        *  @retval 0 screen rotation is disabled.
        *  @retval 1 screen rotation is enabled.
        *
        **/
        /******************************************************************************/
        u8 LCD_GetRotateScreen( void )
            {
            return CurrentRotateScreen;
            }

        /*******************************************************************************
        *
        *                                LCD_SetScreenOrientation
        *
        *******************************************************************************/
        /**
        *
        *  Set the screen orientation.
        *
        *  @param[in]  ScreenOrientation The new screen orientation.
        *
        **/
        /******************************************************************************/
        void LCD_SetScreenOrientation( Rotate_H12_V_Match_TypeDef ScreenOrientation )
            {
            CurrentScreenOrientation = ScreenOrientation;

            LCD_DisplayRotate( CurrentScreenOrientation );
            }

        /*******************************************************************************
        *
        *                                LCD_GetScreenOrientation
        *
        *******************************************************************************/
        /**
        *
        *  Return current screen orientation.
        *
        *  @return   A Rotate_H12_V_Match_TypeDef telling the current screen orientation.
        *
        **/
        /******************************************************************************/
        Rotate_H12_V_Match_TypeDef LCD_GetScreenOrientation( void )
            {
            return CurrentScreenOrientation;
            }

        
    /*******************************************************************************
    *
    *                                DRAW_SetImage
    *
    *******************************************************************************/
    /**
    *
    *  The provided bitmap is made width * height 2 byte words. Each 2 byte word contains
    *  the RGB color of a pixel.
    *
    *  @brief      Draw a color bitmap at the provided coordinates.
    *  @param[in]  imageptr    A pointer to an array of width * height 2 byte words.
    *  @param[in]  x           The horizontal coordinate of the low left corner of the bitmap.
    *  @param[in]  y           The vertical coordinate of the low left corner of the bitmap.
    *  @param[in]  width       The bitmap width.
    *  @param[in]  height      The bitmap height.
    *
    *  @warning       The (0x0) point in on the low left corner.
    *
    **/
    /******************************************************************************/
    void DRAW_SetImage( const u16* imageptr, u8 x, u8 y, u8 width, u8 height )
        {
        s32 i;

        /* Select screen area to access.*/
        LCD_SetRect_For_Cmd( x, y, width, height );
        _LCD_SendLCDCmd(); 
        //starting_delay(50);
        
        
        for( i = 0; i < ( width * height ); i++ )
            {
            if ( imageptr )
                {
                LCD_SendLCDData( imageptr[ i ] & 0xff );
                LCD_SendLCDData( ( imageptr[ i ] >> 8 ) & 0xff );
                }
            else
                {
                /* Bit not set: draw pixel with background color.*/
                LCD_SendLCDData( ( BackColor >> 8 ) & 0xff );
                LCD_SendLCDData( BackColor & 0xff );
                }
            }
        }

    /*******************************************************************************
    *
    *                                DRAW_SetImageSel
    *
    *******************************************************************************/
    /**
    *
    *  The provided bitmap is made width * height 2 byte words. Each 2 byte word contains
    *  the RGB color of a pixel.
    *  All pixels with the oldBgndColor are replaced with the newBgndColor
    *
    *  @brief      Draw a color bitmap at the provided coordinates, with selected background color.
    *  @param[in]  imageptr     A pointer to an array of width * height 2 byte words.
    *  @param[in]  x            The horizontal coordinate of the low left corner of the bitmap.
    *  @param[in]  y            The vertical coordinate of the low left corner of the bitmap.
    *  @param[in]  width        The bitmap width.
    *  @param[in]  height       The bitmap height.
    *  @param[in]  oldBgndColor The color of the background to replace.
    *  @param[in]  newBgndColor The new color.
    *
    *  @warning       The (0x0) point in on the low left corner.
    *
    **/
    /******************************************************************************/
    void DRAW_SetImageSel( const u16* imageptr, u8 x, u8 y, u8 width, u8 height, u16 oldBgndColor, u16 newBgndColor )
        {
        s32 i;

        /* Check the validity of the pointer*/
        if (imageptr == 0)
            return;

        /* Select screen area to access.*/
        LCD_SetRect_For_Cmd( x, y, width, height );


        /* Send command to write data on the LCD screen.*/
        _LCD_SendLCDCmd();


        for( i = 0; i < ( width * height ); i++ )
            {
                if ( imageptr[i] != oldBgndColor)
                    {
                    LCD_SendLCDData( imageptr[ i ] & 0xff );
                    LCD_SendLCDData( ( imageptr[ i ] >> 8 ) & 0xff );
                    }
                else
                    {
                    LCD_SendLCDData( ( newBgndColor >> 8 ) & 0xff );
                    LCD_SendLCDData( newBgndColor & 0xff );
                    }
            }
        }

    /*******************************************************************************
    *
    *                                DRAW_SetImageBW
    *
    *******************************************************************************/
    /**
    *
    *  The provided bitmap is made of width * height bits where a set bit means a pixel
    *  drawn in the current text color, whereas an unset bit means a pixel drawn in the
    *  current background color.
    *
    *  @brief      Draw a monochrome bitmap at the provided coordinates.
    *  @param[in]  imageptr    A pointer to an array of width * height bits.
    *  @param[in]  x           The horizontal coordinate of the low left corner of the bitmap.
    *  @param[in]  y           The vertical coordinate of the low left corner of the bitmap.
    *  @param[in]  width       The bitmap width.
    *  @param[in]  height      The bitmap height.
    *
    *  @warning       The (0x0) point in on the low left corner.
    *
    **/
    /******************************************************************************/
    void DRAW_SetImageBW( const u8* imageptr, u8 x, u8 y, u8 width, u8 height )
        {
        s32 i;
        s32 j;

        /* Select screen area to access.*/
        LCD_SetRect_For_Cmd( x, y, width, height );


        /* Send command to write data on the LCD screen.*/
        _LCD_SendLCDCmd();

        /* Loop on all bitmap bytes.*/
        for( i = 0; i < ( ( width * height ) / 8 ); i++ )
            {
            /* Loop on all byte bits.*/
            for( j = 0; j < 8; j++ )
                {
                /* Bit set: draw pixel with text color.*/
                if( imageptr && ( ( ( imageptr[i] ) >> ( 7 - j ) ) & 0x1 ) )
                    {
                    LCD_SendLCDData( ( TextColor >> 8 ) & 0xff );
                    LCD_SendLCDData( TextColor & 0xff );
                    }
                else
                    {
                    /* Bit not set: draw pixel with background color.*/
                    LCD_SendLCDData( ( BackColor >> 8 ) & 0xff );
                    LCD_SendLCDData( BackColor & 0xff );
                    }
                }
            }
        }

    /*******************************************************************************
    *
    *                                DRAW_Line_Circle
    *
    *******************************************************************************/
    /**
    *  Draw a line on the LCD screen. Optimized for horizontal/vertical lines,
    *  and use the Bresenham algorithm for other cases.
    *
    *  @param[in]  x1          The x-coordinate of the first line endpoint.
    *  @param[in]  x2          The x-coordinate of the second line endpoint.
    *  @param[in]  y1          The y-coordinate of the first line endpoint.
    *  @param[in]  y2          The y-coordinate of the second line endpoint.
    *  @param[in]  color       The line color.
    *
    * Rq : this fucntion does not check parameter validity
    *
    **/
    void DRAW_Line_Circle (s16 x1, s16 y1, s16 x2, s16 y2, u16 color )
        {
        s32 i,dx,dy,sdx,sdy,dxabs,dyabs,x,y,px,py;

    #define abs(X) ( ( (X) < 0 ) ? -(X) : (X) )
    #define sgn(X) ( ( (X) < 0 ) ? -1 : 1 )

        if ( x1==x2 )        /* Vertical Line*/
            {
            if ( y1 > y2 )    /* We assume y2>y1 and invert if not*/
                {
                i = y2;
                y2 = y1;
                y1 = i;
                }
            LCD_FillRect_Circle( x1, y1, 1, y2-y1+1, color );
            return;
            }
        else if ( y1==y2 )  /* Horizontal Line*/
            {
            if ( x1 > x2 )   /* We assume x2>x1 and we swap them if not*/
                {
                i = x2;
                x2 = x1;
                x1 = i;
                }
            LCD_FillRect_Circle( x1, y1, x2-x1+1, 1, color );
            return;
            }

        dx=x2-x1;      /* the horizontal distance of the line */
        dy=y2-y1;      /* the vertical distance of the line */
        dxabs=abs(dx);
        dyabs=abs(dy);
        sdx=sgn(dx);
        sdy=sgn(dy);
        x=dyabs>>1;
        y=dxabs>>1;
        px=x1;
        py=y1;

        if (dxabs>=dyabs) /* the line is more horizontal than vertical */
            {
            for(i=0;i<dxabs;i++)
                {
                y+=dyabs;
                if (y>=dxabs)
                    {
                    y-=dxabs;
                    py+=sdy;
                    }
                px+=sdx;
                LCD_DrawPixel(px,py,color);
                }
            }
        else /* the line is more vertical than horizontal */
            {
            for(i=0;i<dyabs;i++)
                {
                x+=dxabs;
                if (x>=dyabs)
                    {
                    x-=dyabs;
                    px+=sdx;
                    }
                py+=sdy;
                LCD_DrawPixel(px,py,color);
                }
            }
        }

    /*******************************************************************************
    *
    *                                DRAW_Line
    *
    *******************************************************************************/
    /**
    *  Draw a line on the LCD screen. Optimized for horizontal/vertical lines,
    *  and use the Bresenham algorithm for other cases.
    *
    *  @param[in]  x1          The x-coordinate of the first line endpoint.
    *  @param[in]  x2          The x-coordinate of the second line endpoint.
    *  @param[in]  y1          The y-coordinate of the first line endpoint.
    *  @param[in]  y2          The y-coordinate of the second line endpoint.
    *  @param[in]  color       The line color.
    *
    **/
    /******************************************************************************/
    void DRAW_Line (s16 x1, s16 y1, s16 x2, s16 y2, u16 color )
        {
        /* Rq : this fuction is just a wrapper with parameter limit checks*/

        /* Check the x parameters*/
        if (x1 < 0)
            x1 = 0;
        if (x2 < 0)
            x2 = 0;
        if (x1 > SCREEN_WIDTH-1)
            x1 = SCREEN_WIDTH-1;
        if (x2 > SCREEN_WIDTH-1)
            x2 = SCREEN_WIDTH-1;

        /* Check the y parameters*/
        if (y1 < 0)
            y1 = 0;
        if (y2 < 0)
            y2 = 0;
        if (y1 > SCREEN_HEIGHT-1)
            y1 = SCREEN_HEIGHT-1;
        if (y2 > SCREEN_HEIGHT-1)
            y2 = SCREEN_HEIGHT-1;

        /* Call the native function*/
        DRAW_Line_Circle (x1, y1, x2, y2, color );

        }

    /**
      * @brief  Sets the LCD Text and Background colors.
      * @param  _TextColor: specifies the Text Color.
      * @param  _BackColor: specifies the Background Color.
      * @retval None
      */
    void LCD_SetColors(__IO uint16_t _TextColor, __IO uint16_t _BackColor)
    {
      TextColor = _TextColor; 
      BackColor = _BackColor;
    }

    /**
      * @brief  Gets the LCD Text and Background colors.
      * @param  _TextColor: pointer to the variable that will contain the Text 
                Color.
      * @param  _BackColor: pointer to the variable that will contain the Background 
                Color.
      * @retval None
      */
    void LCD_GetColors(__IO uint16_t *_TextColor, __IO uint16_t *_BackColor)
    {
      *_TextColor = TextColor; *_BackColor = BackColor;
    }

    /**
      * @brief  Sets the Text color.
      * @param  Color: specifies the Text color code RGB(5-6-5).
      * @retval None
      */
    void LCD_SetTextColor(__IO uint16_t Color)
    {
      TextColor = Color;
    }

    /**
      * @brief  Sets the Background color.
      * @param  Color: specifies the Background color code RGB(5-6-5).
      * @retval None
      */
    void LCD_SetBackColor(__IO uint16_t Color)
    {
      BackColor = Color;
    }

    /**
      * @brief  Clears the whole LCD.
      * @param  Color: the color of the background.
      * @retval None
      */
    void LCD_Clear(uint16_t Color)
    {
      uint32_t index = 0;
      
      /* Select screen area to access.*/
      LCD_SetRect_For_Cmd( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT );

      /* Select LCD output mode.*/
      LCD_SendLCDCmd( ST7732_RAMWR );

      for(index = 0; index < (SCREEN_WIDTH * SCREEN_HEIGHT); index++)
      {
        LCD_SendLCDData( Color >> 8 );        
        LCD_SendLCDData( Color );
      }  
    }

    /*******************************************************************************
    *
    *                                LCD_DrawChar_Circle
    *
    *******************************************************************************/
    /**
    *  Draw a character on the LCD screen.
    *
    *  @param[in]  x           The line where to display the character shape.
    *  @param[in]  y           The column start address.
    *  @param[in]  width       The number of columns (dots) in a character width.
    *  @param[in]  bmp         The character (monochrome) bitmap. A pointer of the dot matrix data.
    *  @param[in]  textColor   The character color.
    *  @param[in]  bGndColor   The character background color.
    *  @param[in]  charMagniCoeff The character magnifying coefficient.
    *
    *  @warning    The (0x0) point in on the low left corner.
    *
    **/
    /******************************************************************************/
    void LCD_DrawChar_Circle( u8 x, u8 y, u8 width, const u8* bmp, u16 textColor, u16 bGndColor, u16 charMagniCoeff )
        {
        s32 i, i0 = 0, i1 = 7;
        s32 j;
        s32 k1;
        s32 k2;
        #define MAX(i,j) (((i)>(j))?(i):(j))
        #define MIN(i,j) (((i)<(j))?(i):(j))
        s32 x0 = MAX(x, CharFilter_XMin);
        s32 y0 = MAX(y, CharFilter_YMin);
        static s32 width0, height0 ;
        width0 = MIN(CHAR_WIDTH * charMagniCoeff,  CharFilter_XMax - x0 );
        height0 = MIN(CHAR_HEIGHT * charMagniCoeff,  CharFilter_YMax - y0 );
        if ( y0 > y ) 
            height0 -= (y0-y);
        if ( x0 > x ) 
            width0 -= (x0-x);
        
        static s32 lin0;
        static s32 lin1;
        
        if ( (width0 <= 0) || (height0 <= 0) ) 
            return;
        

        /* Select the area for LCD output.*/
        LCD_SetRect_For_Cmd( x0, y0, width0, height0);

        /* Select LCD output mode.*/
        LCD_SendLCDCmd( ST7732_RAMWR );

        i0 = (x0 - x) / charMagniCoeff;
        i1 = width0 / charMagniCoeff;
        lin0 = (y0 - y) ;
        lin1 = height0 + lin0;

        for( i = i0; i < i1; i++ )
            {
            for( k1 = 0; k1 < charMagniCoeff; k1++ )
                {
                s32 n = 0;
                for( j = 0x80; j; j >>= 1 ) // 8
                    {
                    for( k2 = 0; k2 < charMagniCoeff; k2++ , n++)
                        {
                        if ( ( n < lin0 ) || (n>=lin1) ) continue;                           
                        LCD_SendLCDData( ( bmp[2*i] & j ) ? ( textColor >> 8  ) : ( bGndColor >> 8 ) );
                        LCD_SendLCDData( ( bmp[2*i] & j ) ? ( textColor & 255 ) : ( bGndColor &  255 ) );
                        }
                    }

                for( j = 0x80; j > 2; j >>= 1 )  // 8
                    {
                    for( k2 = 0; k2 < charMagniCoeff; k2++ , n++)
                        {
                        if ( ( n < lin0 ) || (n>=lin1) ) continue;
                        LCD_SendLCDData( ( bmp[2*i+1] & j ) ? ( textColor >> 8  ) : ( bGndColor >> 8 ) );
                        LCD_SendLCDData( ( bmp[2*i+1] & j ) ? ( textColor & 255 ) : ( bGndColor & 255 ) );
                        }
                    }
                }
            }
        }

    /*******************************************************************************
    *
    *                                DRAW_DisplayChar_Circle
    *
    *******************************************************************************/
    /**
    *
    *  Display at provided coordinates the provided ASCII character with the provided
    *  text and background colors and with the provided magnify coefficient.
    *
    *  @param[in]  x              The horizontal coordinate of the character.
    *  @param[in]  y              The vertical coordinate of the character.
    *  @param[in]  Ascii          The ASCII code of the character to display.
    *                             @n Ascii must be higher than 31 and lower than 255.
    *  @param[in]  TextColor      The color used to draw the character.
    *  @param[in]  BGndColor      The background color of the drawn character.
    *  @param[in]  CharMagniCoeff The magnify coefficient used to draw the character.
    *
    *  @warning    The (0x0) point in on the low left corner.
    *
    **/
    /******************************************************************************/
    void DRAW_DisplayChar_Circle( u8 x, u8 y, u8 Ascii, u16 TextColor, u16 BGndColor, u16 CharMagniCoeff)
        {
        /* Display the selected bitmap according to the provided ASCII character.*/
        LCD_DrawChar_Circle( x, y, CHAR_WIDTH, (u8*)&CurrentFont[ (Ascii-32) * CHAR_HEIGHT ], TextColor, BGndColor, CharMagniCoeff );
        }

    /*******************************************************************************
    *
    *                                DRAW_DisplayStringWithMode_Circle
    *
    *******************************************************************************/
    /**
    *
    *  This function is used to display a character string (17 char max)
    *  at given X, Y coordinates on the LCD display.
    *  Note:
    *  This function is the user interface to use the LCD driver.
    *
    *  @param[in] x      The horizontal coordinate of the string.
    *  @param[in] y      The vertical coordinate of the string.
    *  @param[in] *ptr   Pointer to the string.
    *  @param[in] len    String length i.e., number of characters to display.
    *  @param[in]  mode  Display mode: 0 normal, 1 inverted colors.
    *
    *  @note          Characters are displayed as 7x14 pixels blocks.
    *  @warning       The (0x0) point in on the low left corner.
    *
    *  @see           DRAW_DisplayString
    *  @see           DRAW_DisplayStringInverted
    *
    **/
    /******************************************************************************/
    void DRAW_DisplayStringWithMode_Circle( u8 x, u8 y, const u8* ptr, u8 len, s32 mode )
        {
        u8 ref_x = x, i = 0, c;

        /* Up to 23 characters*/
        if ( len >= 23 )
            {
            len = 23;
            }

        /* Display each character on LCD */
        for ( i = 0 ; i < len ; i++ )
            {
            c = *ptr;
            if ( c )
                {
                ptr++;      /* Point to the next character */
                }
            else
                {
                c = ' ';    /* fill with space when len exceeds strlen(ptr) */
                }
            /* Display one character on LCD */
            DRAW_DisplayChar_Circle( ref_x, y, c, mode ? BackColor : TextColor,  mode ? TextColor : BackColor, CharMagniCoeff );

            /* Increment the column position by 7 */
            ref_x+= (CHAR_WIDTH * CharMagniCoeff);
            }
        }

    /*******************************************************************************
    *
    *                                DRAW_DisplayString_Circle
    *
    *******************************************************************************/
    /**
    *
    *  This function is used to display a character string (17 u8 max)
    *  at given X, Y coordinates on the LCD display.
    *
    *  @param[in] x      The horizontal coordinate of the string.
    *  @param[in] y      The vertical coordinate of the string.
    *  @param[in] *ptr   Pointer to the string.
    *  @param[in] len    String length i.e., number of characters to display.
    *
    *  @note          Characters are displayed as 7x14 pixels blocks.
    *  @warning       The (0x0) point in on the low left corner.
    *
    **/
    /******************************************************************************/
    void DRAW_DisplayString_Circle( u8 x, u8 y, const u8* ptr, u8 len )
        {
        DRAW_DisplayStringWithMode_Circle( x, y, ptr, len, 0 );
        }

    /*******************************************************************************
    *
    *                                DRAW_DisplayStringInverted_Circle
    *
    *******************************************************************************/
    /**
    *
    *  This function is used to display a character string (17 char max)
    *  at given X, Y coordinates on the LCD display, with inverted colors.
    *
    *  @param[in] x      The horizontal coordinate of the string.
    *  @param[in] y      The vertical coordinate of the string.
    *  @param[in] *ptr   Pointer to the string.
    *  @param[in] len    String length i.e., number of characters to display.
    *
    *  @note          Characters are displayed as 7x14 pixels blocks.
    *  @warning       The (0x0) point in on the low left corner.
    *
    **/
    /******************************************************************************/
    void DRAW_DisplayStringInverted_Circle( u8 x, u8 y, const u8* ptr, u8 len )
        {
        //BackGround and Text Colors are inverted
        DRAW_DisplayStringWithMode_Circle( x, y, ptr, len, 1 );
        }

    /**
      * @brief  Displays one character (16dots width, 24dots height).
      * @param  Line: the Line where to display the character shape .
      *   This parameter can be one of the following values:
      *     @arg Linex: where x can be 0..9
      * @param  Column: start column address.
      * @param  Ascii: character ascii code, must be between 0x20 and 0x7E.
      * @retval None
      */
    void LCD_DisplayChar(uint8_t Line, uint16_t Column, uint8_t Ascii)
    {
      LCD_DrawChar_Circle(Column, Line, CHAR_WIDTH, (u8*)&CurrentFont[ (Ascii-32) * CHAR_HEIGHT ], TextColor, BackColor, CharMagniCoeff );
    }


    /**
      * @brief  Displays a maximum of 20 char on the LCD.
      * @param  Line: the Line where to display the character shape .
      *   This parameter can be one of the following values:
      *     @arg Linex: where x can be 0..9
      * @param  *ptr: pointer to string to display on LCD.
      * @retval None
      */
    void LCD_DisplayStringLine(uint8_t Line, uint8_t *ptr)
    {
      uint16_t refcolumn = 0;
      LCD_DisplayChar(Line, refcolumn, ' ');   
      refcolumn += CHAR_WIDTH;        
      /* Send the string character by character on lCD */
      while (refcolumn < SCREEN_WIDTH)
      {
        if(*ptr != 0)
        {
            /* Display one character on LCD */
            LCD_DisplayChar(Line, refcolumn, *ptr);
            /* Point on the next character */
            ptr++;
        }
        else
        {
            LCD_DisplayChar(Line, refcolumn, ' ');
        }
        /* Increment the column position by the current font width */
        refcolumn += CHAR_WIDTH;
      }
    }

    void LCD_DisplayStringLineColumn(uint8_t Line, uint16_t Column, uint8_t *ptr, int8_t Length)
    {
      int8_t i = 0;
      uint16_t refcolumn = Column;
      /* Send the string character by character on lCD */
      while (refcolumn < SCREEN_WIDTH)
      {
        if((Length >= 0) && (i >= Length))
            break;
        
        if(*ptr != 0)
        {
            /* Display one character on LCD */
            LCD_DisplayChar(Line, refcolumn, *ptr);
            /* Point on the next character */
            ptr++;
        }
        else
        {
            LCD_DisplayChar(Line, refcolumn, ' ');
        }
        /* Increment the column position by the current font width */
        refcolumn += CHAR_WIDTH;
        /* Increment character count */
        i++;
      }
    }

    /**
      * @brief  Clears the selected line.
      * @param  Line: the Line to be cleared.
      *   This parameter can be one of the following values:
      *     @arg Linex: where x can be 0..n
      * @retval None
      */
    void LCD_ClearLine(uint8_t Line)
    {
      uint16_t refcolumn = 0;   
      LCD_DisplayChar(Line, refcolumn, ' ');   
      refcolumn += CHAR_WIDTH;        
      /* Send the string character by character on lCD */
      while (refcolumn < SCREEN_WIDTH)
      {
        /* Display one character on LCD */
        LCD_DisplayChar(Line, refcolumn, ' ');
        /* Increment the column position by the current font width */
        refcolumn += CHAR_WIDTH;
      }
    }

void LCD_DrawRect(uint8_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width)
{
    LCD_DrawRect_Circle(Ypos, Xpos, Width, Height, TextColor);
}