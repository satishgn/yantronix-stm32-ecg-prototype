/********************* (C) COPYRIGHT 2007 RAISONANCE S.A.S. *******************/
/**
*
* @file     lcd.h
* @brief    The header file for ST7637 driver.
* @author   IB
* @date     07/2007
*
**/
/******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32_PRIMER2_LCD_H
#define __STM32_PRIMER2_LCD_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Type def  -----------------------------------------------------------------*/

typedef enum
  {
    V12=0,
    V3=1,
    V6=2,
    V9=3
} Rotate_H12_V_Match_TypeDef;

/* Data lines configuration mode */
typedef enum
   {
   Input,
   Output
   } DataConfigMode_TypeDef;

/* Constants -----------------------------------------------------------------*/
/*/ LCD addresses as seen by the FSMC*/
#define LCD_DATA_MODE_ADDR   ((u32)0x68020000)
#define LCD_CMD_MODE_ADDR    ((u32)0x68000000)

#define TIMER_IT_CHANN			 TIM_IT_CC2

/* LCD Control pins */
#define CtrlPin_RS               GPIO_Pin_11
#define CtrlPin_RD               GPIO_Pin_4
#define CtrlPin_WR               GPIO_Pin_5
#define CtrlPin_RST              GPIO_Pin_6
#define LCD_CTRL_PINS            (CtrlPin_RS|CtrlPin_RD|CtrlPin_WR)
#define GPIOx_CTRL_LCD           GPIOD
#define GPIO_LCD_CTRL_PERIPH     RCC_APB2Periph_GPIOD

#define CtrlPin_CS               GPIO_Pin_7
#define GPIOx_CS_LCD             GPIOD
#define GPIO_LCD_CS_PERIPH       RCC_APB2Periph_GPIOD

#define LCD_D0                   GPIO_Pin_7
#define LCD_D1                   GPIO_Pin_8
#define LCD_D2                   GPIO_Pin_9
#define LCD_D3                   GPIO_Pin_10
#define LCD_D4                   GPIO_Pin_11
#define LCD_D5                   GPIO_Pin_12
#define LCD_D6                   GPIO_Pin_13
#define LCD_D7                   GPIO_Pin_14
#define LCD_DATA_PINS            (LCD_D0|LCD_D1|LCD_D2|LCD_D3|LCD_D4|LCD_D5|LCD_D6|LCD_D7)
#define GPIOx_D_LCD              GPIOE
#define GPIO_LCD_D_PERIPH        RCC_APB2Periph_GPIOE

#define  GPIO_BACKLIGHT_PIN      GPIO_Pin_8

/* LCD Commands */
#define DISPLAY_ON               0xAF
#define DISPLAY_OFF              0xAE
#define START_LINE               0xC0
#define START_COLUMN             0x00
#define CLOCKWISE_OUTPUT         0xA0
#define DYNAMIC_DRIVE            0xA4
#define DUTY_CYCLE               0xA9
#define READ_MODIFY_WRITE_OFF    0xEE
#define SOFTWARE_RESET           0xE2

#define ST7732_NOP               0x00
#define ST7732_SWRESET           0x01
#define ST7732_RDDID             0x04
#define ST7732_RDDST             0x09
#define ST7732_RDDPM             0x0A
#define ST7732_RDDMADCTR         0x0B
#define ST7732_RDDCOLMOD         0x0C
#define ST7732_RDDIM             0x0D
#define ST7732_RDDSM             0x0E
#define ST7732_RDDSDR            0x0F

#define ST7732_SLPIN             0x10
#define ST7732_SLPOUT            0x11
#define ST7732_PTLON             0x12
#define ST7732_NORON             0x13

#define ST7732_INVOFF            0x20
#define ST7732_INVON             0x21
#define ST7732_GAMSET			 0x26
#define ST7732_DISPOFF           0x28
#define ST7732_DISPON            0x29
#define ST7732_CASET             0x2A
#define ST7732_RASET             0x2B
#define ST7732_RAMWR             0x2C
#define ST7732_RGBSET            0x2D
#define ST7732_RAMRD             0x2E

#define ST7732_PTLAR             0x30
#define ST7732_SCRLAR            0x33
#define ST7732_TEOFF             0x34
#define ST7732_TEON              0x35
#define ST7732_MADCTR            0x36
#define ST7732_VSCSAD            0x37
#define ST7732_IDMOFF            0x38
#define ST7732_IDMON             0x39
#define ST7732_COLMOD            0x3A

#define ST7732_RDID1             0xDA
#define ST7732_RDID2             0xDB
#define ST7732_RDID3             0xDC

#define ST7732_RGBCTR            0xB0
#define ST7732_FRMCTR1           0xB1
#define ST7732_FRMCTR2           0xB2
#define ST7732_FRMCTR3           0xB3
#define ST7732_INVCTR            0xB4
#define ST7732_RGBBPCTR          0xB5
#define ST7732_DISSET5           0xB6

#define ST7732_PWCTR1		 	 0xC0
#define ST7732_PWCTR2 		 	 0xC1
#define ST7732_PWCTR3 		 	 0xC2
#define ST7732_PWCTR4 			 0xC3
#define ST7732_PWCTR5 			 0xC4
#define ST7732_VMCTR1 			 0xC5
#define ST7732_VMOFCTR 			 0xC7

#define ST7732_WRID2             0xD1
#define ST7732_WRID3             0xD2
#define ST7732_RDID4	         0xD3
#define ST7732_NVCTR1	         0xD9
#define ST7732_NVCTR2	         0xDE
#define ST7732_NVCTR3	         0xDF

#define ST7732_GAMCTRP1          0xE0
#define ST7732_GAMCTRN1          0xE1

#define ST7732_AUTOCTRL          0xF1
#define ST7732_OSCADJ            0xF2
#define ST7732_DISPCTRL          0xF5
#define ST7732_DEFADJ            0xF6

// /*RGB is 16-bit coded as    G2G1G0B4 B3B2B1B0 R4R3R2R1 R0G5G4G3*/
// #define RGB_MAKE(xR,xG,xB)    ( ( (((xG)>>2)&0x07)<<13 ) + ( (((xG)>>2))>>3 )  +      \
                                // ( ((xB)>>3) << 8 )          +      \
                                // ( ((xR)>>3) << 3 ) )

                                
// #define RGB_RED         0x00F8
// #define RGB_BLACK       0x0000
// #define RGB_WHITE       0xffff
// #define RGB_BLUE        0x1F00
// #define RGB_GREEN       0xE007
// #define RGB_YELLOW      (RGB_GREEN|RGB_RED)
// #define RGB_MAGENTA     (RGB_BLUE|RGB_RED)
// #define RGB_LIGHTBLUE   (RGB_BLUE|RGB_GREEN)
// #define RGB_ORANGE      (RGB_RED | 0xE001)       /* green/2 + red*/
// #define RGB_PINK        (RGB_MAGENTA | 0xE001)   /* green/2 + magenta*/

/* SCREEN Infos*/
#define SCREEN_WIDTH         160    //128
#define SCREEN_HEIGHT        128    //128

/* Characters Infos*/
#define CHAR_WIDTH            7
#define CHAR_HEIGHT           14

#define LINE(x) (SCREEN_HEIGHT - CHAR_HEIGHT - (x * CHAR_HEIGHT))
#define COLUMN(x) (CHAR_WIDTH + (x * CHAR_WIDTH))

/* PWM rates.*/
#define BACKLIGHTMIN                0x1000   /*!< Minimal PWM rate.           */
#define DEFAULT_CCR_BACKLIGHTSTART  0x8000   /*!< Default PWM rate.           */

/** 
  * @brief  LCD color  
  * Note: This doesn't work for STM32 Primer hardware
  */ 
#define LCD_COLOR_WHITE          0xFFFF
#define LCD_COLOR_BLACK          0x0000
#define LCD_COLOR_GREY           0xF7DE
#define LCD_COLOR_BLUE           0x001F
#define LCD_COLOR_BLUE2          0x051F
#define LCD_COLOR_RED            0xF800
#define LCD_COLOR_MAGENTA        0xF81F
#define LCD_COLOR_GREEN          0x07E0
#define LCD_COLOR_CYAN           0x7FFF
#define LCD_COLOR_YELLOW         0xFFE0

/** 
  * @brief  LCD Lines depending on the chosen fonts.  
  */
#define LCD_LINE_0               LINE(0)
#define LCD_LINE_1               LINE(1)
#define LCD_LINE_2               LINE(2)
#define LCD_LINE_3               LINE(3)
#define LCD_LINE_4               LINE(4)
#define LCD_LINE_5               LINE(5)
#define LCD_LINE_6               LINE(6)
#define LCD_LINE_7               LINE(7)
#define LCD_LINE_8               LINE(8)
#define LCD_LINE_9               LINE(9)
#define LCD_LINE_10              LINE(10)
#define LCD_LINE_11              LINE(11)
#define LCD_LINE_12              LINE(12)
#define LCD_LINE_13              LINE(13)
#define LCD_LINE_14              LINE(14)
#define LCD_LINE_15              LINE(15)
#define LCD_LINE_16              LINE(16)
#define LCD_LINE_17              LINE(17)
#define LCD_LINE_18              LINE(18)
#define LCD_LINE_19              LINE(19)
#define LCD_LINE_20              LINE(20)
#define LCD_LINE_21              LINE(21)
#define LCD_LINE_22              LINE(22)
#define LCD_LINE_23              LINE(23)
#define LCD_LINE_24              LINE(24)
#define LCD_LINE_25              LINE(25)
#define LCD_LINE_26              LINE(26)
#define LCD_LINE_27              LINE(27)
#define LCD_LINE_28              LINE(28)
#define LCD_LINE_29              LINE(29)

/** 
  * @brief  LCD Columns depending on the chosen fonts.  
  */
#define LCD_COLUMN_0               COLUMN(0)
#define LCD_COLUMN_1               COLUMN(1)
#define LCD_COLUMN_2               COLUMN(2)
#define LCD_COLUMN_3               COLUMN(3)
#define LCD_COLUMN_4               COLUMN(4)
#define LCD_COLUMN_5               COLUMN(5)
#define LCD_COLUMN_6               COLUMN(6)
#define LCD_COLUMN_7               COLUMN(7)
#define LCD_COLUMN_8               COLUMN(8)
#define LCD_COLUMN_9               COLUMN(9)
#define LCD_COLUMN_10              COLUMN(10)
#define LCD_COLUMN_11              COLUMN(11)
#define LCD_COLUMN_12              COLUMN(12)
#define LCD_COLUMN_13              COLUMN(13)
#define LCD_COLUMN_14              COLUMN(14)
#define LCD_COLUMN_15              COLUMN(15)
#define LCD_COLUMN_16              COLUMN(16)
#define LCD_COLUMN_17              COLUMN(17)
#define LCD_COLUMN_18              COLUMN(18)
#define LCD_COLUMN_19              COLUMN(19)
#define LCD_COLUMN_20              COLUMN(20)
#define LCD_COLUMN_21              COLUMN(21)
#define LCD_COLUMN_22              COLUMN(22)

/** 
  * @brief LCD default font 
  */ 
#define LCD_DEFAULT_FONT         Font8x8    //Font12x12

#define ASSEMBLE_RGB(R, G, B)    ((((R)& 0xF8) << 8) | (((G) & 0xFC) << 3) | (((B) & 0xF8) >> 3))

/* Exported vars -------------------------------------------------------------*/
extern Rotate_H12_V_Match_TypeDef Screen_Orientation;
extern s32 rotate_screen;

extern const u8 AsciiDotsTable[224 * 14 ];

/* Exported functions --------------------------------------------------------*/
void delayms(s32 count);

void LCD_Init(void);    /* Important */
void LCD_SetScreenOrientation( Rotate_H12_V_Match_TypeDef ScreenOrientation );
void LCD_SendLCDCmd( u8 Cmd );  /* Important */
void _LCD_SendLCDCmd(void); /* Important */
void LCD_SendLCDData( u8 Data );    /* Important */
u32  LCD_ReadLCDData( void );   /* Important */ 
void LCD_SetRect_For_Cmd( s16 x, s16 y, s16 width, s16 height );    /* Important */

u16  LCD_GetPixel( u8 x, u8 y );    /* Important */
void LCD_DrawPixel( u8 x, u8 y, u16 Pixel );    /* Important */
void LCD_FillRect_Circle( u16 x, u16 y, u16 width, u16 height, u16 color ); /* Important */
// void LCD_FillRect( u16 x, u16 y, u16 width, u16 height, u16 color );    /* Depends on LCD_FillRect_Circle */
void LCD_DrawRect_Circle( u16 x, u16 y, u16 width, u16 height, u16 color );    /* Depends on LCD_FillRect_Circle */

void LCD_RectRead( u16 x, u16 y, u16 width, u16 height, u8* bmp );  /* Important */

void LCD_SetBackLight (u32 newBacklightStart);  /* Important */
u32  LCD_GetBackLight ( void ); /* Important */
void LCD_SetBackLightOff( void );   /* Important */
void LCD_SetBackLightOn( void );    /* Important */

void LCD_Clear(uint16_t Color);

void LCD_DrawChar_Circle( u8 x, u8 y, u8 width, const u8* bmp, u16 textColor, u16 bGndColor, u16 charMagniCoeff );
void DRAW_DisplayChar_Circle( u8 x, u8 y, u8 Ascii, u16 TextColor, u16 BGndColor, u16 CharMagniCoeff);
void DRAW_DisplayStringWithMode_Circle( u8 x, u8 y, const u8* ptr, u8 len, s32 mode );
void DRAW_DisplayString_Circle( u8 x, u8 y, const u8* ptr, u8 len );
void DRAW_DisplayStringInverted_Circle( u8 x, u8 y, const u8* ptr, u8 len );

void DRAW_SetImage( const u16* imageptr, u8 x, u8 y, u8 width, u8 height );  /* Important */
void DRAW_SetImageSel( const u16* imageptr, u8 x, u8 y, u8 width, u8 height, u16 oldBgndColor, u16 newBgndColor );   /* Important */
void DRAW_SetImageBW( const u8* imageptr, u8 x, u8 y, u8 width, u8 height );     /* Important */

void DRAW_Line_Circle (s16 x1, s16 y1, s16 x2, s16 y2, u16 color ); /* Important */
void DRAW_Line (s16 x1, s16 y1, s16 x2, s16 y2, u16 color );

void LCD_SetColors(__IO uint16_t _TextColor, __IO uint16_t _BackColor); 
void LCD_GetColors(__IO uint16_t *_TextColor, __IO uint16_t *_BackColor);
void LCD_SetTextColor(__IO uint16_t Color);
void LCD_SetBackColor(__IO uint16_t Color);
void LCD_DisplayChar(uint8_t Line, uint16_t Column, uint8_t Ascii);
void LCD_DisplayStringLine(uint8_t Line, uint8_t *ptr);
void LCD_DisplayStringLineColumn(uint8_t Line, uint16_t Column, uint8_t *ptr, int8_t Length);
void LCD_ClearLine(uint8_t Line);
void LCD_DrawRect(uint8_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width);

/* Use for HID */
/*
void LCD_Init(void);//0
void LCD_SetBackLight (u32 newBacklightStart);//1
void LCD_SetBackLightOff( void );//2
void LCD_SetBackLightOn( void );//3
void LCD_Clear(uint16_t Color);//4
void LCD_SetScreenOrientation( Rotate_H12_V_Match_TypeDef ScreenOrientation );//5
void LCD_SetColors(__IO uint16_t _TextColor, __IO uint16_t _BackColor); //6
void LCD_SetFont(sFONT *fonts);//7
void LCD_DisplayChar(uint8_t Line, uint16_t Column, uint8_t Ascii);//8
void LCD_DisplayStringLine(uint8_t Line, uint8_t *ptr);//9
void LCD_ClearLine(uint8_t Line);//10
void DRAW_DisplayChar_Circle( u8 x, u8 y, u8 Ascii, u16 TextColor, u16 BGndColor, u16 CharMagniCoeff);//11
void DRAW_DisplayString_Circle( u8 x, u8 y, const u8* ptr, u8 len );//12
void LCD_DrawPixel( u8 x, u8 y, u16 Pixel );//13
void LCD_FillRect( u16 x, u16 y, u16 width, u16 height, u16 color );//14
void LCD_DrawRect( u16 x, u16 y, u16 width, u16 height, u16 color );//15
void DRAW_Line (s16 x1, s16 y1, s16 x2, s16 y2, u16 color );//16
void DRAW_SetImage( const u16* imageptr, u8 x, u8 y, u8 width, u8 height );//17
*/
#endif /* __STM32_PRIMER2_LCD_H */
