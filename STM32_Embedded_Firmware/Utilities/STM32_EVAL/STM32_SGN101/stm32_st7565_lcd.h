/******************************************************************************/
/**
*
* @file     stm32_st7565_lcd.h
* @brief    The header file for ST7565 driver.
* @author   Satish Nair
* @date     09/2010
*
**/
/******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32_ST7565_LCD_H
#define __STM32_ST7565_LCD_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Type def  -----------------------------------------------------------------*/
/* Data lines configuration mode */
typedef enum
{
    Input,
    Output
} DataConfigMode_TypeDef;

/* Constants -----------------------------------------------------------------*/
/**
 * @brief Uncomment the line below if you want to use user defined Delay function
 *        (for precise timing), otherwise default _delay_ function defined within
 *         this driver is used (less precise timing).  
 */
// #define USE_Delay

#ifdef USE_Delay
  #define _delay_     Delay  /* !< User can provide more timing precise _delay_ function                                   (with 10ms time base), using SysTick for example */
#else
  #define _delay_     delay      /* !< Default _delay_ function with less precise timing */
#endif                                     

/* LCD Data pins */
#define LCD_DB0                     GPIO_Pin_0
#define LCD_DB1                     GPIO_Pin_1
#define LCD_DB2                     GPIO_Pin_2
#define LCD_DB3                     GPIO_Pin_3
#define LCD_DB4                     GPIO_Pin_4
#define LCD_DB5                     GPIO_Pin_5
#define LCD_DB6                     GPIO_Pin_6
#define LCD_DB7                     GPIO_Pin_7
#define LCD_DATA_PINS               (LCD_DB0|LCD_DB1|LCD_DB2|LCD_DB3|LCD_DB4|LCD_DB5|LCD_DB6|LCD_DB7)
#define GPIOx_LCD_DATA              GPIOC
#define GPIO_LCD_DATA_PERIPH        RCC_APB2Periph_GPIOC

/* LCD Control pins */
#define CtrlPin_RS                  GPIO_Pin_8
#define GPIOx_LCD_CTRL_RS           GPIOC
#define GPIO_LCD_CTRL_RS_PERIPH     RCC_APB2Periph_GPIOC
#define CtrlPin_RD                  GPIO_Pin_9
#define GPIOx_LCD_CTRL_RD           GPIOC
#define GPIO_LCD_CTRL_RD_PERIPH     RCC_APB2Periph_GPIOC
#define CtrlPin_WR                  GPIO_Pin_10
#define GPIOx_LCD_CTRL_WR           GPIOC
#define GPIO_LCD_CTRL_WR_PERIPH     RCC_APB2Periph_GPIOC
#define CtrlPin_CS                  GPIO_Pin_11
#define GPIOx_LCD_CTRL_CS           GPIOC
#define GPIO_LCD_CTRL_CS_PERIPH     RCC_APB2Periph_GPIOC
#define CtrlPin_RST                 GPIO_Pin_12
#define GPIOx_LCD_CTRL_RST          GPIOC
#define GPIO_LCD_CTRL_RST_PERIPH    RCC_APB2Periph_GPIOC

/* LCD Backlight pin */
#define LCD_BACKLIGHT_PIN           GPIO_Pin_3
#define GPIOx_LCD_BACKLIGHT         GPIOA
#define GPIO_LCD_BACKLIGHT_PERIPH   RCC_APB2Periph_GPIOA

/* LCD Commands */
#define CMD_DISPLAY_OFF   0xAE
#define CMD_DISPLAY_ON    0xAF

#define CMD_SET_DISP_START_LINE  0x40
#define CMD_SET_PAGE  0xB0

#define CMD_SET_COLUMN_UPPER  0x10
#define CMD_SET_COLUMN_LOWER  0x00

#define CMD_SET_ADC_NORMAL  0xA0
#define CMD_SET_ADC_REVERSE 0xA1

#define CMD_SET_DISP_NORMAL 0xA6
#define CMD_SET_DISP_REVERSE 0xA7

#define CMD_SET_ALLPTS_NORMAL 0xA4
#define CMD_SET_ALLPTS_ON  0xA5
#define CMD_SET_BIAS_9 0xA2 
#define CMD_SET_BIAS_7 0xA3

#define CMD_RMW  0xE0
#define CMD_RMW_CLEAR 0xEE
#define CMD_INTERNAL_RESET  0xE2
#define CMD_SET_COM_NORMAL  0xC0
#define CMD_SET_COM_REVERSE  0xC8
#define CMD_SET_POWER_CONTROL  0x28
#define CMD_SET_RESISTOR_RATIO  0x20
#define CMD_SET_VOLUME_FIRST  0x81
#define CMD_SET_VOLUME_SECOND  0
#define CMD_SET_STATIC_OFF  0xAC
#define CMD_SET_STATIC_ON  0xAD
#define CMD_SET_STATIC_REG  0x0
#define CMD_SET_BOOSTER_FIRST  0xF8
#define CMD_SET_BOOSTER_234  0
#define CMD_SET_BOOSTER_5  1
#define CMD_SET_BOOSTER_6  3
#define CMD_NOP  0xE3
#define CMD_TEST  0xF0
 
/* LCD Color */ 
#define LCD_COLOR_BLACK 0xFFFF
#define LCD_COLOR_WHITE 0x0000
#define LCD_COLOR_TEXT_NORMAL  0xFFFF
#define LCD_COLOR_TEXT_REVERSE 0x0000

/* LCD Size */
#define LCD_WIDTH 128
#define LCD_HEIGHT 64

/* Font Size */
#define LCD_FONT_WIDTH  6
#define LCD_FONT_HEIGHT 7
 
#define swap(a, b) { uint8_t t = a; a = b; b = t; } 
#define LINE(x) (7 - x)   
#define COLUMN(x) (4 + (x * LCD_FONT_WIDTH))

/* LCD Lines depending on the chosen fonts */
#define LCD_LINE_0               LINE(0)
#define LCD_LINE_1               LINE(1)
#define LCD_LINE_2               LINE(2)
#define LCD_LINE_3               LINE(3)
#define LCD_LINE_4               LINE(4)
#define LCD_LINE_5               LINE(5)
#define LCD_LINE_6               LINE(6)
#define LCD_LINE_7               LINE(7)

/* LCD Columns depending on the chosen fonts */
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

/* LCD Backlight Brightness */
#define LCD_BRIGHTNESS_NIL              0   //0% duty cycle
#define LCD_BRIGHTNESS_LOW              10  //10% duty cycle
#define LCD_BRIGHTNESS_MEDIUM           25  //25% duty cycle
#define LCD_BRIGHTNESS_HIGH             50  //50% duty cycle
#define LCD_BRIGHTNESS_FULL             100 //100% duty cycle

/* Exported functions */
/*********************************** LCD Backlight Routines ************************************/
void LCD_BackLightConfig( void );
void LCD_SetBackLight( uint8_t Brightness );
/***********************************************************************************************/

/******************************* Basic Important LCD Routines **********************************/
void LCD_Init( void );
void LCD_SendLCDCmd( uint8_t Cmd );
void LCD_SendLCDData( uint8_t Data );
uint8_t LCD_ReadLCDData( void );
void LCD_Clear(uint16_t color);
uint16_t LCD_GetPixel(uint8_t x, uint8_t y);
void LCD_DrawPixel(uint8_t x, uint8_t y, uint16_t color);
void LCD_SetTextColor(__IO uint16_t Color);
void LCD_DisplayChar(uint8_t Line, uint8_t Column, uint8_t Ascii);
void LCD_DisplayStringLineColumn(uint8_t Line, uint16_t Column, uint8_t *ptr, int8_t Length);
void LCD_DisplayStringLine(uint8_t Line, uint8_t *ptr);
void LCD_ClearLine(uint8_t Line);
/***********************************************************************************************/

/********************************* Low Level Drawing Utilities *********************************/
void LCD_DisplayImage(const uint8_t* imageptr, uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void LCD_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t color);
void LCD_DrawRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t color);
void LCD_FillRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t color);
// void LCD_DrawCircle(uint8_t x0, uint8_t y0, uint8_t r, uint16_t color);
// void LCD_FillCircle(uint8_t x0, uint8_t y0, uint8_t r, uint16_t color);
/***********************************************************************************************/

/*********************** Drawing Utilities Using "LCD_DrawPixel" Method ************************/
// void LCD_DisplayImage(const uint8_t* imageptr, uint8_t x, uint8_t y, uint8_t width, uint8_t height);
// void LCD_DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t color);
// void LCD_DrawRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t color);
// void LCD_FillRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t color);
void LCD_DrawCircle(uint8_t x0, uint8_t y0, uint8_t r, uint16_t color);
void LCD_FillCircle(uint8_t x0, uint8_t y0, uint8_t r, uint16_t color);
/***********************************************************************************************/

#endif /*__STM32_ST7565_LCD_H */
