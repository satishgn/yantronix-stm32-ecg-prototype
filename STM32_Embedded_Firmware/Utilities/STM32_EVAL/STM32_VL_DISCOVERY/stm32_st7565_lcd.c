/******************************************************************************/
/**
*
* @file     stm32_st7565_lcd.c
* @brief    The LCD driver for the ST7565.
* @author   Satish Nair
* @date     09/2010
*
**/
/******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32_st7565_lcd.h"
#include "stm32_st7565_fonts.h"

/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Global variables to set the written text color */
static __IO uint16_t TextColor = LCD_COLOR_TEXT_NORMAL;

/* External variable ---------------------------------------------------------*/
/* Public variables ---------------------------------------------------------*/
GPIO_InitTypeDef             GPIO_InitStructure;

/******************************* Basic Important LCD Routines **********************************/
/* Private function prototypes -----------------------------------------------*/
#ifdef USE_Delay
extern void Delay(__IO uint32_t nCount);
#else
static void delay(__IO uint32_t nCount);
#endif /* USE_Delay*/

static void LCD_BackLightConfig( void );
static void LCD_DataLinesConfig( DataConfigMode_TypeDef Mode );
static void LCD_DataLinesWrite( GPIO_TypeDef* GPIOx, uint32_t PortVal );
static uint32_t LCD_DataLinesRead( GPIO_TypeDef* GPIOx );
static void LCD_CtrlLinesConfig( void );
static void LCD_CtrlLinesWrite( GPIO_TypeDef* GPIOx, uint32_t CtrlPins, BitAction BitVal );
static void LCD_ST7565_Controller( void );

/* Private functions ---------------------------------------------------------*/
#ifndef USE_Delay
/**
  * @brief  Inserts a delay time.
  * @param  nCount: specifies the delay time length.
  * @retval None
  */
static void delay(__IO uint32_t nCount)
{
  __IO uint32_t index = 0; 
  for(index = (100000 * nCount); index != 0; index--)
  {
  }
}
#endif /* USE_Delay*/

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
static void LCD_BackLightConfig( void )
{
    TIM_TimeBaseInitTypeDef      TIM_TimeBaseStructure;
    TIM_OCInitTypeDef            TIM_OCInitStructure;

    /* Enable GPIO clock  */
    RCC_APB2PeriphClockCmd( GPIO_LCD_BACKLIGHT_PERIPH | RCC_APB2Periph_AFIO, ENABLE );

    /* GPIO Configuration : TIM in Output */
    GPIO_InitStructure.GPIO_Pin   = LCD_BACKLIGHT_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOx_LCD_BACKLIGHT, &GPIO_InitStructure );

    /* Disable TIM clock */
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2, ENABLE );

    /* -----------------------------------------------------------------------------------
    TIM2 Configuration: generate 1 PWM signal with variable Duty cycle on CH4.
    TIM2CLK = SystemCoreClock = 2 * APB1 freq (if APB1 prescaler == 2)
    TIM2CLK = SystemCoreClock = APB1 freq (if APB1 prescaler == 1)
    To get TIM2 counter clock = 100 KHz,
     - Prescaler = (TIM2CLK / TIM2 counter clock) - 1 = (72000000 / 100000) - 1
    SystemCoreClock is set to 72 MHz for Low-density, Medium-density, High-density
    and Connectivity line devices and to 24 MHz for Low-Density Value line and
    Medium-Density Value line devices

    The TIM2 is running at 1000 Hz:
    TIM2 Frequency = TIM3 counter clock / (ARR + 1)
                   = 100000 / 100 = 1000 Hz
    TIM2 Channel4 duty cycle = (TIM2_CCR4 / (TIM2_ARR + 1)) * 100 = (50 / 100) * 100 = 50 %
    ------------------------------------------------------------------------------------- */
    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period          = 99; //TIM period = 1000Hz, autoreload = (100000 / 1000) - 1
    TIM_TimeBaseStructure.TIM_Prescaler       = (SystemCoreClock / 100000) - 1; //TIM counter clock = 100000 Hz 
    TIM_TimeBaseStructure.TIM_ClockDivision   = 0x0000; //0
    TIM_TimeBaseStructure.TIM_CounterMode     = TIM_CounterMode_Up;
    
    TIM_TimeBaseInit( TIM2, &TIM_TimeBaseStructure );

    /* PWM1 Mode configuration: Channel2 */
    TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse       = 0x0000;   //Backlight-Off
    TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;

    TIM_OC4Init( TIM2, &TIM_OCInitStructure );

    TIM_OC4PreloadConfig( TIM2, TIM_OCPreload_Disable );

    TIM_ARRPreloadConfig( TIM2, DISABLE );

    /* TIM2 disable counter */
    TIM_Cmd( TIM2, DISABLE );
    
    /* Disable TIM2 clock */
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2, DISABLE );    
}

/*******************************************************************************
*
*                                LCD_SetBackLight
*
*******************************************************************************/
/**
*  Modify the LCD Backlight PWM Duty cycle.
*
**/
/******************************************************************************/
void LCD_SetBackLight( uint8_t Brightness )
{
    uint16_t CCR4_Value = ((Brightness * 99) / 100); //Brightness = Duty_Cycle, TIM2_Autoreload = 99

    if(Brightness)
    {
        /* Enable TIM2 clock */
        RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2, ENABLE );

        /* TIM2 enable counter */
        TIM_Cmd( TIM2, ENABLE );    
    
        /* Set Timer2 channel2 duty cycle */
        TIM_SetCompare4(TIM2, CCR4_Value);
    }
    else
    {
        /* Set Timer2 channel2 duty cycle */
        TIM_SetCompare4(TIM2, CCR4_Value);
    
        /* TIM2 disable counter */
        TIM_Cmd( TIM2, DISABLE );
        
        /* Disable TIM2 clock */
        RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2, DISABLE );
    }
}

/*******************************************************************************
*
*                               LCD_DataLinesConfig
*
*******************************************************************************/
/**
*  Configure data lines D0~D7 in Input Floating mode for read from LCD or in
*  Output Push-Pull mode for write on LCD
*
*  @param[in]  Mode Specifies the configuration mode for data lines D0~D7.
*                @n @c Input: configure in Input Floating mode
*                @n @c Output: configure in Output Push-Pul mode
*
**/
/******************************************************************************/
static void LCD_DataLinesConfig( DataConfigMode_TypeDef Mode )
{
    GPIO_InitStructure.GPIO_Pin   =  LCD_DATA_PINS;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    if( Mode == Input )
    {
        /* Configure D0~D7 lines as Input */
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    }
    else
    {
        /* Configure D0~D7 lines in Output Push-Pull mode */
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    }

    GPIO_Init( GPIOx_LCD_DATA, &GPIO_InitStructure );
}

/*******************************************************************************
*
*                                LCD_DataLinesWrite
*
*******************************************************************************/
/**
*  Write a value on D0~D7
*
*  @param[in]  GPIOx    GPIO port to write on.
*  @param[in]  PortVal  The value to write. Only the lowest OR highest 8 bits are taken into
*                       account depending on hardware.
*
**/
/******************************************************************************/
static void LCD_DataLinesWrite( GPIO_TypeDef* GPIOx, uint32_t PortVal )
{
    /* Write only the lowest 8 bits! */
    GPIOx->ODR = ( (GPIOx->ODR) & 0xFF00 ) | (uint8_t)PortVal;
    // /* Write only the highest 8 bits! */
    // GPIOx->ODR = ((GPIOx->ODR) & 0x00FF) | (((uint8_t)PortVal) << 8);
}

/*******************************************************************************
*
*                                LCD_DataLinesRead
*
*******************************************************************************/
/**
*  Read a value from D0~D7
*
*  @param[in]  GPIOx    GPIO port to read from.
*
**/
/******************************************************************************/
static uint32_t LCD_DataLinesRead( GPIO_TypeDef* GPIOx )
{
    /* Read only the lowest 8 bits! */
    return (((uint16_t)GPIOx->IDR) & 0x00FF);
    // /* Read only the highest 8 bits! */
    // return ((((uint16_t)GPIOx->IDR) & 0xFF00) >> 8);
}

/*******************************************************************************
*
*                                LCD_CtrlLinesConfig
*
*******************************************************************************/
/**
*  Configure control lines in Output Push-Pull mode.
*
**/
/******************************************************************************/
static void LCD_CtrlLinesConfig( void )
{
#if defined (USE_SD_CARD_SDIO_INTERFACE)
    /* Disable JTAG-DP and Enable SW-DP */
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
#endif

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;

    GPIO_InitStructure.GPIO_Pin   =  CtrlPin_RST;
    GPIO_Init( GPIOx_LCD_CTRL_RST, &GPIO_InitStructure );
    GPIO_InitStructure.GPIO_Pin   =  CtrlPin_CS;
    GPIO_Init( GPIOx_LCD_CTRL_CS, &GPIO_InitStructure );
    GPIO_InitStructure.GPIO_Pin   =  CtrlPin_RS;
    GPIO_Init( GPIOx_LCD_CTRL_RS, &GPIO_InitStructure );
    GPIO_InitStructure.GPIO_Pin   =  CtrlPin_RD;
    GPIO_Init( GPIOx_LCD_CTRL_RD, &GPIO_InitStructure );
    GPIO_InitStructure.GPIO_Pin   =  CtrlPin_WR;
    GPIO_Init( GPIOx_LCD_CTRL_WR, &GPIO_InitStructure );

    LCD_CtrlLinesWrite( GPIOx_LCD_CTRL_RS, CtrlPin_RS,  Bit_SET );    /* RS = 1   */
    LCD_CtrlLinesWrite( GPIOx_LCD_CTRL_RD, CtrlPin_RD,  Bit_SET );    /* RD = 1   */
    LCD_CtrlLinesWrite( GPIOx_LCD_CTRL_CS, CtrlPin_CS,  Bit_SET );    /* CS = 1   */
    LCD_CtrlLinesWrite( GPIOx_LCD_CTRL_WR, CtrlPin_WR,  Bit_SET );    /* WR = 1   */
    LCD_CtrlLinesWrite( GPIOx_LCD_CTRL_RST, CtrlPin_RST, Bit_RESET );  /* RST = 0  */
}

/*******************************************************************************
*
*                                LCD_CtrlLinesWrite
*
*******************************************************************************/
/**
*  Set or reset control lines.
*
*  @param[in]  GPIOx       Where x can be 0, 1 or 2 to select the GPIO peripheral.
*  @param[in]  CtrlPins    The Control line.
*  @param[in]  BitVal
*
**/
/******************************************************************************/
static void LCD_CtrlLinesWrite( GPIO_TypeDef* GPIOx, uint32_t CtrlPins, BitAction BitVal )
{
    /* Set or Reset the control line */
    GPIO_WriteBit( GPIOx, CtrlPins, BitVal );
}

/*******************************************************************************
*
*                                LCD_ST7565_Controller
*
*******************************************************************************/
/**
*  Initialization of the controller registers.
*
**/
/******************************************************************************/
static void LCD_ST7565_Controller( void )
{
    /** Apply hardware reset **/
    LCD_CtrlLinesWrite( GPIOx_LCD_CTRL_RST, CtrlPin_RST, Bit_SET );    /* RST = 1  */
    LCD_CtrlLinesWrite( GPIOx_LCD_CTRL_RST, CtrlPin_RST, Bit_RESET );  /* RST = 0  */
    _delay_(50);    /* delay 500ms */
    LCD_CtrlLinesWrite( GPIOx_LCD_CTRL_RST, CtrlPin_RST, Bit_SET );    /* RST = 1  */
    _delay_(50);    /* delay 500ms */

    /*default mode is output*/
    LCD_DataLinesConfig( Output );

    // LCD_CheckLCDStatus();

    /* Software Reset */
    LCD_SendLCDCmd(0xE2);   //$E2 Resets Internal functions
    /* LCD bias set */
    LCD_SendLCDCmd(0xA2);  //Bias: $A2 Set bias 1/9 (Duty 1/65)
    /* ADC set */
    LCD_SendLCDCmd(0xA0);  //ADC Direction: $A1 ADC reverse, $A0 ADC normal
    /* Common output mode select */
    LCD_SendLCDCmd(0xC0);  //SHL/COM Output Scan Direction: $C0 Normal COM0->COM63, $C8 Reverse COM63->COM0
    /* V0 voltage regulator set */
    LCD_SendLCDCmd(0x25);  //Voltage ref: $20 to $27
    /* Electronic volume mode set */
    LCD_SendLCDCmd(0x81);  //Volume mode: $81
    LCD_SendLCDCmd(0x18);  //Volume set:  $00 to $3F
    /* Power control set */
    LCD_SendLCDCmd(0x2F);  //$2F Booster, Regulator and Follower on
    /* Static indicator set */
    LCD_SendLCDCmd(0xAC);  //$AC No indicator
    LCD_SendLCDCmd(0x00);  //$00 No indicator

    /* Display start line set */
    LCD_SendLCDCmd(0x40);  //Initial display line: $40 Display start line 0
    /* Display normal/reverse */
    LCD_SendLCDCmd(0xA6);  //Normal display: $A6 Display normal    
    /* Display ON/OFF */
    LCD_SendLCDCmd(0xAF);  //turn display on: $AF Display on

    /* Page address set */
    LCD_SendLCDCmd(0xB0);   //page address = 0
    /* Column address set */
    LCD_SendLCDCmd(0x10);   //column address high = 0, send MSB nibble
    LCD_SendLCDCmd(0x00);   //column address low = 0, send LSB nibble
    // /* Do not increment Column address counter during Read operation */
    // LCD_SendLCDCmd(0xE0);   //issue RMW
    
    /* Clear LCD */
    LCD_Clear(LCD_COLOR_WHITE);        
}

/*******************************************************************************
*
*                                LCD_Init
*
*******************************************************************************/
/**
*
*  Initialize LCD.
*
**/
/******************************************************************************/
void LCD_Init( void )
{
    /* Enable GPIO clock for LCD */
    RCC_APB2PeriphClockCmd( GPIO_LCD_CTRL_RST_PERIPH | GPIO_LCD_CTRL_CS_PERIPH | GPIO_LCD_CTRL_RS_PERIPH |
                            GPIO_LCD_CTRL_RD_PERIPH | GPIO_LCD_CTRL_WR_PERIPH, ENABLE );
    RCC_APB2PeriphClockCmd( GPIO_LCD_DATA_PERIPH, ENABLE );

    /* LCD Init */
    /* Configure control lines signals as output mode */
    LCD_CtrlLinesConfig();
    /* ST7565 init */
    LCD_ST7565_Controller();
    /* Configure LCD Backlight - default no backlight */
    LCD_BackLightConfig();
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
*  @param[in]  Cmd   An uint8_t containing the user command to send to the LCD.
*
**/
/******************************************************************************/
void LCD_SendLCDCmd( uint8_t Cmd )
{
    /* Start the LCD send data sequence */
    LCD_CtrlLinesWrite( GPIOx_LCD_CTRL_RS, CtrlPin_RS, Bit_RESET );     /* RS = 0 */
    LCD_CtrlLinesWrite( GPIOx_LCD_CTRL_RD, CtrlPin_RD, Bit_SET );       /* RD = 1 */
    LCD_CtrlLinesWrite( GPIOx_LCD_CTRL_CS, CtrlPin_CS, Bit_RESET );     /* CS = 0 */
    LCD_CtrlLinesWrite( GPIOx_LCD_CTRL_WR, CtrlPin_WR, Bit_RESET );     /* WR = 0 */

    /* Write data to the LCD */
    LCD_DataLinesWrite( GPIOx_LCD_DATA, (uint32_t)Cmd );
    LCD_CtrlLinesWrite( GPIOx_LCD_CTRL_WR, CtrlPin_WR, Bit_SET );       /* WR = 1 */
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
void LCD_SendLCDData( uint8_t Data )
{
    /* Configure Data lines as Output */
    LCD_CtrlLinesWrite( GPIOx_LCD_CTRL_RS, CtrlPin_RS, Bit_SET );
    LCD_CtrlLinesWrite( GPIOx_LCD_CTRL_RD, CtrlPin_RD, Bit_SET );
    LCD_CtrlLinesWrite( GPIOx_LCD_CTRL_CS, CtrlPin_CS, Bit_RESET );
    LCD_CtrlLinesWrite( GPIOx_LCD_CTRL_WR, CtrlPin_WR, Bit_RESET );

    /* Write data to the LCD */
    LCD_DataLinesWrite( GPIOx_LCD_DATA,(uint32_t)Data );
    LCD_CtrlLinesWrite( GPIOx_LCD_CTRL_WR, CtrlPin_WR, Bit_SET );
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
*  @return     An unsigned 8 bit byte containing the data returned by a LCD command.
*  @pre        An LCD_SendLCDCmd was done with a command returning data.
*
**/
/********************************************************************************/
uint8_t LCD_ReadLCDData( void )
{
    uint32_t LCDData = 0;

    /* Configure Data lines as Input */
    LCD_DataLinesConfig(Input);

    /* Start the LCD send data sequence */
    LCD_CtrlLinesWrite( GPIOx_LCD_CTRL_RS, CtrlPin_RS, Bit_SET );         /* RS = 1 */
    LCD_CtrlLinesWrite( GPIOx_LCD_CTRL_WR, CtrlPin_WR, Bit_SET );         /* WR = 1 */
    LCD_CtrlLinesWrite( GPIOx_LCD_CTRL_CS, CtrlPin_CS, Bit_RESET );          /* CS = 0 */
    LCD_CtrlLinesWrite( GPIOx_LCD_CTRL_RD, CtrlPin_RD, Bit_RESET );       /* RD = 0 */

    /* Read data from the LCD */
    // LCDData = (GPIO_ReadInputData( GPIOx_LCD_DATA ) & LCD_DATA_PINS );
    LCDData = LCD_DataLinesRead(GPIOx_LCD_DATA);

    LCD_CtrlLinesWrite( GPIOx_LCD_CTRL_RD, CtrlPin_RD, Bit_SET );          /* RD = 1 */

    /* Read the LCD returned data */
    LCD_DataLinesConfig( Output );

    return (uint8_t)LCDData;
}

// /**
  // * @brief  Sets the cursor position.
  // * @param  Xpos: specifies the X position.
  // * @param  Ypos: specifies the Y position. 
  // * @retval None
  // */
// void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
// {
    // /* set Y position / set page address*/
    // LCD_SendLCDCmd(0xB0 | ((Ypos>>3) % (LCD_HEIGHT/8)));

    // /* set X position / set column address */
    // LCD_SendLCDCmd(0x10 | (((Xpos % LCD_WIDTH) >> 4) & 0x0F));    //First send MSB nibble
    // LCD_SendLCDCmd(0x00 | ((Xpos % LCD_WIDTH) & 0x0F));   //Then send LSB nibble
// }

/**
  * @brief  Clears the whole LCD.
  * @param  color: the color of the background.
  * @retval None
  */
void LCD_Clear(uint16_t color)
{
    uint8_t p, c; 
    for(p = 0; p < 8; p++)
    {
        LCD_SendLCDCmd(0xB0 | p);
        LCD_SendLCDCmd(0x10 | ((0x00 >> 4) & 0x0F));
        LCD_SendLCDCmd(0x00 | (0x00 & 0x0F));

        for(c = 0; c < LCD_WIDTH; c++)
        {
            LCD_SendLCDData((uint8_t)color);
        }     
    }
}

uint16_t LCD_GetPixel(uint8_t x, uint8_t y)
{
    LCD_SendLCDCmd(0xB0 | (y >> 3));   //y/8
    LCD_SendLCDCmd(0x10 | (x >> 4));
    LCD_SendLCDCmd(x & 0x0F);  
    LCD_SendLCDCmd(0xE0);   //issue RMW
    LCD_ReadLCDData();  //dummy read
    if(LCD_ReadLCDData() & (1 << (y % 8)))   //if((LCD_ReadLCDData() >> (y % 8)) & 1)
        return LCD_COLOR_BLACK;
    else
        return LCD_COLOR_WHITE;
    LCD_SendLCDCmd(0xEE);   //release RMW 
}

void LCD_DrawPixel(uint8_t x, uint8_t y, uint16_t color)
{
    LCD_SendLCDCmd(0xB0 | (y >> 3));   //y/8
    LCD_SendLCDCmd(0x10 | (x >> 4));
    LCD_SendLCDCmd(x & 0x0F);  
    LCD_SendLCDCmd(0xE0);   //issue RMW
    LCD_ReadLCDData();  //dummy read
    if(color)
        LCD_SendLCDData(LCD_ReadLCDData() | (1 << (y % 8)));  //Set Pixel
    else
        LCD_SendLCDData(LCD_ReadLCDData() & ~(1 << (y % 8))); //Clear Pixel
    LCD_SendLCDCmd(0xEE);   //release RMW 
}

/**
* @brief  Sets the Text color.
* @param  Color: specifies the Text color(Normal / Reverse).
* @retval None
*/
void LCD_SetTextColor(__IO uint16_t Color)
{
    TextColor = Color;
}

/**
* @brief  Displays one character (6dots width, 7dots height).
* @param  Line: the Line where to display the character shape .
*   This parameter can be one of the following values:
*     @arg Linex: where x can be 0..7
* @param  Column: start column address.
* @param  Ascii: character ascii code, must be between 0x20 and 0x7E.
* @retval None
*/
void LCD_DisplayChar(uint8_t Line, uint8_t Column, uint8_t Ascii)
{
    LCD_SendLCDCmd(0xB0 | Line);
    LCD_SendLCDCmd(0x10 | (Column >> 4));
    LCD_SendLCDCmd(Column & 0x0F);  
    uint8_t c = 0;
    for(c = 0; c < LCD_FONT_WIDTH; c++)
    {
        if(TextColor)
            LCD_SendLCDData(ASCII6x7_Table[((Ascii - 32) * LCD_FONT_WIDTH) + c]);//Normal: black text on white background
        else
            LCD_SendLCDData(~(ASCII6x7_Table[((Ascii - 32) * LCD_FONT_WIDTH) + c]));//Reverse: white text on black background           
    }
}

void LCD_DisplayStringLineColumn(uint8_t Line, uint16_t Column, uint8_t *ptr, int8_t Length)
{
    int8_t i = 0;
    uint16_t refcolumn = Column;
    /* Send the string character by character on lCD */
    while (refcolumn < LCD_WIDTH)
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
        refcolumn += LCD_FONT_WIDTH;
        /* Increment character count */
        i++;
    }
}

/**
* @brief  Displays a maximum of 20 char on the LCD.
* @param  Line: the Line where to display the character shape .
*   This parameter can be one of the following values:
*     @arg Linex: where x can be 0..7
* @param  *ptr: pointer to string to display on LCD.
* @retval None
*/
void LCD_DisplayStringLine(uint8_t Line, uint8_t *ptr)
{
    // uint16_t refcolumn = 0;
    // /* Send the string character by character on lCD */
    // while (refcolumn < LCD_WIDTH)
    // {
        // if(*ptr != 0)
        // {
            // /* Display one character on LCD */
            // LCD_DisplayChar(Line, refcolumn, *ptr);
            // /* Point on the next character */
            // ptr++;
        // }
        // else
        // {
            // LCD_DisplayChar(Line, refcolumn, ' ');
        // }
        // /* Increment the column position by the current font width */
        // refcolumn += FONT_WIDTH;
    // }

    LCD_DisplayStringLineColumn(Line, LCD_COLUMN_0, ptr, 20);
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
    // uint16_t refcolumn = 0;   
    // /* Send the string character by character on lCD */
    // while (refcolumn < LCD_WIDTH)
    // {
        // /* Display one character on LCD */
        // LCD_DisplayChar(Line, refcolumn, ' ');
        // /* Increment the column position by the current font width */
        // refcolumn += LCD_FONT_WIDTH;
    // }

    LCD_DisplayStringLineColumn(Line, LCD_COLUMN_0, "                    ", 20);
}
/***********************************************************************************************/

/********************************* Low Level Drawing Utilities *********************************/
/* Display Image */
void LCD_DisplayImage(const uint8_t* imageptr, uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
    uint8_t p, c; 
    uint8_t pStart, pEnd;

    pStart = (y >> 3);
    pEnd = ((y + height) >> 3);

    for(p = pStart; p < pEnd; p++)
    {
        LCD_SendLCDCmd(0xB0 | p);
        LCD_SendLCDCmd(0x10 | (x >> 4));
        LCD_SendLCDCmd(x & 0x0F);

        for(c = 0; c < width; c++)
        {
            LCD_SendLCDData(*imageptr++);
        }     
    }
}

/**
*  Draw a line on the LCD screen. Optimized for horizontal/vertical lines,
*  and use the Bresenham algorithm for other cases.
**/
void LCD_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t color)
{
    int32_t i, dx, dy;
    int32_t sdx, sdy, dxabs, dyabs;
    int32_t x, y, px, py;

    #define abs(X) ( ( (X) < 0 ) ? -(X) : (X) )
    #define sgn(X) ( ( (X) < 0 ) ? -1 : 1 )

    if ( x1 == x2 )        /* Vertical Line*/
    {
        if ( y1 > y2 )    /* We assume y2>y1 and invert if not*/
        {
            i = y2;
            y2 = y1;
            y1 = i;
        }
        LCD_FillRect( x1, y1, 1, y2 - y1 + 1, color );
        return;
    }
    else if ( y1 == y2 )  /* Horizontal Line*/
    {
        if ( x1 > x2 )   /* We assume x2>x1 and we swap them if not*/
        {
            i = x2;
            x2 = x1;
            x1 = i;
        }
        LCD_FillRect( x1, y1, x2 - x1 + 1, 1, color );
        return;
    }

    dx = x2 - x1;      /* the horizontal distance of the line */
    dy = y2 - y1;      /* the vertical distance of the line */
    dxabs = abs( dx );
    dyabs = abs( dy );
    sdx = sgn( dx );
    sdy = sgn( dy );
    x = dyabs >> 1;
    y = dxabs >> 1;
    px = x1;
    py = y1;

    if ( dxabs >= dyabs ) /* the line is more horizontal than vertical */
    {
        for(i = 0; i < dxabs; i++)
        {
            y += dyabs;
            if (y >= dxabs)
            {
                y -= dxabs;
                py += sdy;
            }
            px += sdx;
            LCD_DrawPixel(px, py, color);
        }
    }
    else /* the line is more vertical than horizontal */
    {
        for(i = 0; i < dyabs; i++)
        {
            x += dxabs;
            if (x >= dyabs)
            {
                x -= dyabs;
                px += sdx;
            }
            py += sdy;
            LCD_DrawPixel(px, py, color);
        }
    }
}

/* Draw a Rectangle */
void LCD_DrawRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t color)
{
    /* Draw horizontal sides.*/
    LCD_FillRect( x, y,              width, 1, color );
    LCD_FillRect( x, y + height - 1, width, 1, color );

    /* Draw vertical sides.*/
    LCD_FillRect( x,              y, 1, height, color );
    LCD_FillRect( x + width - 1,  y, 1, height, color );
}

/* Filled Rectangle */
void LCD_FillRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t color)
{
    uint8_t i = y;
    uint8_t j = 0;

    while(i < (y + height))
    {
        if((i % 8) || ((i + 8) > (y + height)))
        {
            LCD_SendLCDCmd(0xB0 | (i >> 3));
            LCD_SendLCDCmd(0x10 | (x >> 4));
            LCD_SendLCDCmd(x & 0x0F);  
            LCD_SendLCDCmd(0xE0);   //issue RMW      
            for(j = 0; j < width; j++)
            {
                LCD_ReadLCDData();  //dummy read
                if(color)
                    LCD_SendLCDData(LCD_ReadLCDData() | (1 << (i % 8)));  //Set Pixel
                else
                    LCD_SendLCDData(LCD_ReadLCDData() & ~(1 << (i % 8))); //Clear Pixel
            }            
            LCD_SendLCDCmd(0xEE);   //release RMW        
            i++; 
        }
        else
        {
            LCD_SendLCDCmd(0xB0 | (i >> 3));
            LCD_SendLCDCmd(0x10 | (x >> 4));
            LCD_SendLCDCmd(x & 0x0F);  
            for(j = 0; j < width; j++)
            {  
                LCD_SendLCDData((uint8_t)color);  
            }                    
            i += 8;
        }
    }
}

// /* Filled Rectangle */
// void LCD_FillRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t color)
// {
    // uint8_t i = 0, p = 0, c = 0; 
    // uint8_t pStart = 0, pEnd = 0;
    // uint8_t pData = 0x00, pStartData = 0x00, pEndData = 0x00;

    // pStart = (y >> 3);
    // pEnd = ((y + height) >> 3);
    
    // if(pStart == pEnd)
    // {
        // p = pStart;
        // for(i = y; i < y + height; i++)
        // {
            // pData = (pData | (1 << (i % 8)));
        // }
        // LCD_SendLCDCmd(0xB0 | p);
        // LCD_SendLCDCmd(0x10 | (x >> 4));
        // LCD_SendLCDCmd(x & 0x0F);
        // LCD_SendLCDCmd(0xE0);   //issue RMW
        // for(c = 0; c < width; c++)
        // {
            // LCD_ReadLCDData();  //dummy read
            // if(color)
                // LCD_SendLCDData(LCD_ReadLCDData() | pData);
            // else
                // LCD_SendLCDData(LCD_ReadLCDData() & ~pData);            
        // }
        // LCD_SendLCDCmd(0xEE);   //release RMW
        // return;
    // }

    // switch(y % 8)
    // {
        // case 0:
            // pStartData = 0xFF;
            // break;
        // case 1:
            // pStartData = 0xFE;
            // break;
        // case 2:
            // pStartData = 0xFC;
            // break;
        // case 3:
            // pStartData = 0xF8;
            // break;
        // case 4:
            // pStartData = 0xF0;
            // break;
        // case 5:
            // pStartData = 0xE0;
            // break;
        // case 6:
            // pStartData = 0xC0;
            // break;
        // case 7:
            // pStartData = 0x80;
            // break;
    // }

    // switch((y + height) % 8)
    // {
        // case 0:
            // pEndData = 0x00;
            // break;
        // case 1:
            // pEndData = 0x01;
            // break;
        // case 2:
            // pEndData = 0x03;
            // break;
        // case 3:
            // pEndData = 0x07;
            // break;
        // case 4:
            // pEndData = 0x0F;
            // break;
        // case 5:
            // pEndData = 0x1F;
            // break;
        // case 6:
            // pEndData = 0x3F;
            // break;
        // case 7:
            // pEndData = 0x7F;
            // break;
    // }

    // for(p = pStart; p < (pEnd + 1); p++)
    // {
        // LCD_SendLCDCmd(0xB0 | p);
        // LCD_SendLCDCmd(0x10 | (x >> 4));
        // LCD_SendLCDCmd(x & 0x0F);
        // LCD_SendLCDCmd(0xE0);   //issue RMW
        // for(c = 0; c < width; c++)
        // {
            // if(color)
            // {
                // if(p == pStart)
                // {
                    // LCD_ReadLCDData();  //dummy read
                    // LCD_SendLCDData(LCD_ReadLCDData() | pStartData);
                // }
                // else if(p == pEnd)
                // {
                    // LCD_ReadLCDData();  //dummy read
                    // LCD_SendLCDData(LCD_ReadLCDData() | pEndData);
                // }
                // else
                // {
                    // LCD_SendLCDData(0xFF);
                // }
            // }
            // else
            // {
                // if(p == pStart)
                // {
                    // LCD_ReadLCDData();  //dummy read
                    // LCD_SendLCDData(LCD_ReadLCDData() & ~pStartData);
                // }
                // else if(p == pEnd)
                // {
                    // LCD_ReadLCDData();  //dummy read
                    // LCD_SendLCDData(LCD_ReadLCDData() & ~pEndData);
                // }
                // else
                // {
                    // LCD_SendLCDData(0x00);
                // }
            // }
        // }    
        // LCD_SendLCDCmd(0xEE);   //release RMW  
    // }
// }

/* Draw a Circle Outline */
// void LCD_DrawCircle(uint8_t x0, uint8_t y0, uint8_t r, uint16_t color)
// {
// }

// /* Filled Circle */
// void LCD_FillCircle(uint8_t x0, uint8_t y0, uint8_t r, uint16_t color)
// {
// }
/***********************************************************************************************/

/*********************** Drawing Utilities Using "LCD_DrawPixel" Method ************************/
// /* Display Image */
// void LCD_DisplayImage(const uint8_t* imageptr, uint8_t x, uint8_t y, uint8_t width, uint8_t height)
// {
// }

// /* Draw a Line - bresenham's algorithm */
// void LCD_DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t color)
// {
    // uint8_t steep = abs(y1 - y0) > abs(x1 - x0);
    // if (steep)
    // {
        // swap(x0, y0);
        // swap(x1, y1);
    // }

    // if (x0 > x1) 
    // {
        // swap(x0, x1);
        // swap(y0, y1);
    // }

    // uint8_t dx, dy;
    // dx = x1 - x0;
    // dy = abs(y1 - y0);

    // int8_t err = dx / 2;
    // int8_t ystep;

    // if (y0 < y1)
    // {
        // ystep = 1;
    // }
    // else
    // {
        // ystep = -1;
    // }

    // for (; x0<x1; x0++)
    // {
        // if (steep) 
        // {
            // LCD_DrawPixel(y0, x0, color);
        // } 
        // else 
        // {
            // LCD_DrawPixel(x0, y0, color);
        // }
        // err -= dy;
        // if (err < 0)
        // {
            // y0 += ystep;
            // err += dx;
        // }
    // }
// }

// /* Draw a Rectangle */
// void LCD_DrawRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t color)
// {
    // uint8_t i = 0;
    // for (i=x; i<x+width; i++)
    // {
        // LCD_DrawPixel(i, y, Color);
        // LCD_DrawPixel(i, y+height-1, color);
    // }
    // for (i=y; i<y+height; i++)
    // {
        // LCD_DrawPixel(x, i, Color);
        // LCD_DrawPixel(x+width-1, i, color);
    // } 
// }

// /* Filled Rectangle */
// void LCD_FillRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t color)
// {
    // uint8_t i = 0, j = 0;
    // for (i=x; i<x+width; i++)
    // {
        // for (j=y; j<y+height; j++) 
        // {
            // LCD_DrawPixel(i, j, color);
        // }
    // }
// }

/* Draw a Circle Outline */
void LCD_DrawCircle(uint8_t x0, uint8_t y0, uint8_t r, uint16_t color)
{
    int8_t f = 1 - r;
    int8_t ddF_x = 1;
    int8_t ddF_y = -2 * r;
    int8_t x = 0;
    int8_t y = r;

    LCD_DrawPixel(x0, y0+r, color);
    LCD_DrawPixel(x0, y0-r, color);
    LCD_DrawPixel(x0+r, y0, color);
    LCD_DrawPixel(x0-r, y0, color);

    while (x<y)
    {
        if (f >= 0) 
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        LCD_DrawPixel(x0 + x, y0 + y, color);
        LCD_DrawPixel(x0 - x, y0 + y, color);
        LCD_DrawPixel(x0 + x, y0 - y, color);
        LCD_DrawPixel(x0 - x, y0 - y, color);

        LCD_DrawPixel(x0 + y, y0 + x, color);
        LCD_DrawPixel(x0 - y, y0 + x, color);
        LCD_DrawPixel(x0 + y, y0 - x, color);
        LCD_DrawPixel(x0 - y, y0 - x, color);
    }
}

/* Filled Circle */
void LCD_FillCircle(uint8_t x0, uint8_t y0, uint8_t r, uint16_t color)
{
    int8_t f = 1 - r;
    int8_t ddF_x = 1;
    int8_t ddF_y = -2 * r;
    int8_t x = 0;
    int8_t y = r;
    uint8_t i = 0;

    for (i=y0-r; i<=y0+r; i++)
    {
        LCD_DrawPixel(x0, i, color);
    }

    while (x<y)
    {
        if (f >= 0) 
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        for (i=y0-y; i<=y0+y; i++) 
        {
            LCD_DrawPixel(x0+x, i, color);
            LCD_DrawPixel(x0-x, i, color);
        } 
        for (i=y0-x; i<=y0+x; i++) 
        {
            LCD_DrawPixel(x0+y, i, color);
            LCD_DrawPixel(x0-y, i, color);
        }    
    }
}
/***********************************************************************************************/
