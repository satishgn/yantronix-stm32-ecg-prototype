/*****************************************************************************
* File    : @file
* Company : Yantronix
* Author  : Satish Nair
* Mailto  : satishgn77@gmail.com
*****************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"

/** @addtogroup STM32_BOARD_APP
  * @{
  */

/** @defgroup CALENDAR
  * @brief    This file includes the calendar driver for the STM32 board application.
  * @{
  */

/** @defgroup CALENDAR_Private_Types
  * @{
  */
/* Time Structure definition */
struct time_t
{
  uint8_t sec_l;
  uint8_t sec_h;
  uint8_t min_l;
  uint8_t min_h;
  uint8_t hour_l;
  uint8_t hour_h;
};
struct time_t time_struct;
/* Alarm Structure definition */
struct alarm_t
{
  uint8_t sec_l;
  uint8_t sec_h;
  uint8_t min_l;
  uint8_t min_h;
  uint8_t hour_l;
  uint8_t hour_h;
};
struct alarm_t alarm_struct;
/* Date Structure definition */
struct date_t
{
  uint8_t month;
  uint8_t day;
  uint16_t year;
};
struct date_t date_s;  
/**
  * @}
  */

/** @defgroup CALENDAR_Private_Defines
  * @{
  */
/**
  * @}
  */

/** @defgroup CALENDAR_Private_Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup CALENDAR_Private_Variables
  * @{
  */
static uint32_t wn = 0, dn = 0, dc = 0;
static uint16_t daycolumn = 0, dayline = 0;
uint8_t MonLen[12] =
  {
    32, 29, 32, 31, 32, 31, 32, 32, 31, 32, 31, 32
  };
uint8_t MonthNames[12][21] =
  {
    "   JANUARY          ", "   FEBRUARY         ", "   MARCH            ",
    "   APRIL            ", "   MAY              ", "   JUNE             ",
    "   JULY             ", "   AUGUST           ", "   SEPTEMBER        ",
    "   OCTOBER          ", "   NOVEMBER         ", "   DECEMBER         "
  };
/**
  * @}
  */


/** @defgroup CALENDAR_Private_FunctionPrototypes
  * @{
  */
static uint8_t Calendar_IsLeapYear(uint16_t nYear);
static void Calendar_WeekDayNum(uint32_t nyear, uint8_t nmonth, uint8_t nday);
static void Calendar_RTC_Configuration(void);
static void Calendar_RegulateYear(void);
static void Calendar_RegulateMonth(void);
static void Calendar_RegulateDay(void);
static void Calendar_TimePreAdjust(void);
static void Calendar_DatePreAdjust(void);
/**
  * @}
  */

/** @defgroup CALENDAR_Private_Functions
  * @{
  */

/**
  * @brief  Reads digit entered by user, using menu navigation keys.
  * @param  None
  * @retval Digit value
  */
uint8_t Calendar_ReadDigit(uint8_t ColBegin, uint8_t CountBegin, uint8_t ValueMax, uint8_t ValueMin)
{
  uint32_t MyKey = 0, tmpValue = 0;
  /* Initialize tmpValue */
  tmpValue = CountBegin;
  /* Set the Text Color */
  LCD_SetTextColor(LCD_COLOR_TEXT_REVERSE);
  /* Display  */
  LCD_DisplayChar(LCD_LINE_7, ColBegin, (tmpValue + 0x30));
  LCD_DrawRect(ColBegin-1, LCD_LINE_7*8, 7, 9, LCD_COLOR_BLACK);
  /* Endless loop */
  while (1)
  {
    /* Check which key is pressed */
    MyKey = Menu_ReadKey();
  
    /* If "UP" pushbutton is pressed */
    if (MyKey == UP)
    {
      /* Increase the value of the digit */
      if (tmpValue == ValueMax)
      {
        tmpValue = (ValueMin - 1);
      }
      /* Display new value */
      LCD_DisplayChar(LCD_LINE_7, ColBegin, ((++tmpValue) + 0x30));
    }

    /* If "DOWN" pushbutton is pressed */
    if (MyKey == DOWN)
    {
      /* Decrease the value of the digit */
      if (tmpValue == ValueMin)
      {
        tmpValue = (ValueMax + 1);
      }
      /* Display new value */
      LCD_DisplayChar(LCD_LINE_7, ColBegin, ((--tmpValue) + 0x30));
    }

    /* If "SEL" pushbutton is pressed */
    if (MyKey == SEL)
    {
      /* Set the Text Color */
      LCD_SetTextColor(LCD_COLOR_TEXT_NORMAL);
      /* Display new value */
      LCD_DisplayChar(LCD_LINE_7, ColBegin, (tmpValue + 0x30));
      LCD_DrawRect(ColBegin-1, LCD_LINE_7*8, 7, 9, LCD_COLOR_WHITE);
      /* Return the digit value and exit */
      return tmpValue;
    }
  }
}

/**
  * @brief  Initializes calendar application.
  * @param  None
  * @retval None
  */
void Calendar_Init(void)
{
  uint32_t i = 0, tmp = 0, pressedkey = 0;

  /* Initialize Date structure */
  date_s.month = 01;
  date_s.day = 01;
  date_s.year = 2011;

  if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
  {
    LCD_DisplayStringLine(LCD_LINE_5, "Time and Date Config");
    LCD_DisplayStringLine(LCD_LINE_6, "Select: Press SEL   ");
    LCD_DisplayStringLine(LCD_LINE_7, "Abort: Press Any Key");

    while (1)
    {
      pressedkey = Menu_ReadKey();
      if (pressedkey == SEL)
      {
        /* Adjust Time */
        Calendar_TimePreAdjust();
        return;
      }
      else if (pressedkey != NOKEY)
      {
        /******************* Update this code block depending on requirement ******************/
        /* Configure RTC to its default value for the Set_System_Shutdown_Time() method */
        /* RTC Configuration */
        Calendar_RTC_Configuration();
        /**************************************************************************************/
      
        return;
      }
    }
  }
  else
  {
    /* PWR and BKP clocks selection ------------------------------------------*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

    /* Allow access to BKP Domain */
    PWR_BackupAccessCmd(ENABLE);

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    /* Enable the RTC Second */
    RTC_ITConfig(RTC_IT_SEC, ENABLE);
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    /* Initialize Date structure */
    date_s.month = (BKP_ReadBackupRegister(BKP_DR3) & 0xFF00) >> 8;
    date_s.day = (BKP_ReadBackupRegister(BKP_DR3) & 0x00FF);
    date_s.year = BKP_ReadBackupRegister(BKP_DR2);
    daycolumn = BKP_ReadBackupRegister(BKP_DR4);
    dayline = BKP_ReadBackupRegister(BKP_DR5);
    if (RTC_GetCounter() / 86399 != 0)
    {
      for (i = 0; i < (RTC_GetCounter() / 86399); i++)
      {
        Calendar_DateUpdate();
      }
      RTC_SetCounter(RTC_GetCounter() % 86399);
      BKP_WriteBackupRegister(BKP_DR2, date_s.year);
      tmp = date_s.month << 8;
      tmp |= date_s.day;
      BKP_WriteBackupRegister(BKP_DR3, tmp);
      // LCD_DisplayStringLine(LCD_LINE_6, "       Days elapsed ");
      // tmp = i / 100;
      // LCD_DisplayChar(LCD_LINE_6, LCD_COLUMN_3, (tmp + 0x30));
      // tmp = ((i % 100) / 10);
      // LCD_DisplayChar(LCD_LINE_6, LCD_COLUMN_4, (tmp + 0x30));
      // tmp = ((i % 100) % 10);
      // LCD_DisplayChar(LCD_LINE_6, LCD_COLUMN_5, (tmp + 0x30));
      // LCD_DisplayStringLine(LCD_LINE_7, "      Press OK      ");
      // while (Menu_ReadKey() != SEL)
      // {}
      // LCD_ClearLine(LCD_LINE_6);
      // LCD_ClearLine(LCD_LINE_7);
      // LCD_DisplayStringLine(LCD_LINE_7, "Push OK to Continue ");
      // Calendar_DateDisplay(date_s.year, date_s.month, date_s.day);
      // while (Menu_ReadKey() != SEL)
      // {}
      BKP_WriteBackupRegister(BKP_DR4, daycolumn);
      BKP_WriteBackupRegister(BKP_DR5, dayline);
    }
  }
}

/**
  * @brief  Returns the time entered by user, using menu vavigation keys.
  * @param  None
  * @retval Current time RTC counter value
  */
uint32_t Calendar_TimeRegulate(void)
{
  uint32_t Tmp_HH = 0, Tmp_MM = 0, Tmp_SS = 0;
  /* Read time hours */
  Tmp_HH = Calendar_ReadDigit(LCD_COLUMN_6, time_struct.hour_h, 0x2, 0x0);
  if (Tmp_HH == 2)
  {
    if (time_struct.hour_l > 3)
    {
      time_struct.hour_l = 0;
    }
    Tmp_HH = Tmp_HH * 10 + Calendar_ReadDigit(LCD_COLUMN_7, time_struct.hour_l, 0x3, 0x0);
  }
  else
  {
    Tmp_HH = Tmp_HH * 10 + Calendar_ReadDigit(LCD_COLUMN_7, time_struct.hour_l, 0x9, 0x0);
  }
  /* Read time  minutes */
  Tmp_MM = Calendar_ReadDigit(LCD_COLUMN_9, time_struct.min_h, 5, 0x0);
  Tmp_MM = Tmp_MM * 10 + Calendar_ReadDigit(LCD_COLUMN_10, time_struct.min_l, 0x9, 0x0);
  /* Read time seconds */
  Tmp_SS = Calendar_ReadDigit(LCD_COLUMN_12, time_struct.sec_h, 5, 0x0);
  Tmp_SS = Tmp_SS * 10 + Calendar_ReadDigit(LCD_COLUMN_13, time_struct.sec_l, 0x9, 0x0);
  /* Return the value to store in RTC counter register */
  return((Tmp_HH*3600 + Tmp_MM*60 + Tmp_SS));
}

/**
  * @brief  Returns the time entered by user, using demoboard keys.
  * @param  None
  * @retval None
  */
static void Calendar_TimePreAdjust(void)
{
  LCD_Clear(LCD_COLOR_WHITE);

  /* Display Watch Bitmap image here */

  if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
  {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    /* Allow access to BKP Domain */
    PWR_BackupAccessCmd(ENABLE);

    LCD_DisplayStringLine(LCD_LINE_6, "RTC Config Plz Wait.");
    /* RTC Configuration */
    Calendar_RTC_Configuration();
    LCD_DisplayStringLine(LCD_LINE_6, "        TIME        ");
    /* Clear LCD_LINE_7 */
    LCD_ClearLine(LCD_LINE_7);
    /* Display time separators ":" */
    LCD_DisplayChar(LCD_LINE_7, LCD_COLUMN_8, ':');
    LCD_DisplayChar(LCD_LINE_7, LCD_COLUMN_11, ':');
    /* Display the current time */
    Calendar_TimeDisplay(RTC_GetCounter());
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
    /* Change the current time */
    RTC_SetCounter(Calendar_TimeRegulate());
    BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
    /* Clear LCD_LINE_6 */
    LCD_ClearLine(LCD_LINE_6);
    /* Clear LCD_LINE_7 */
    LCD_ClearLine(LCD_LINE_7);
    /* Adjust Date */
    Calendar_DatePreAdjust();
  }
  else
  {
    LCD_DisplayStringLine(LCD_LINE_6, "        TIME        ");
    /* Clear LCD_LINE_7 */
    LCD_ClearLine(LCD_LINE_7);

    /* Display time separators ":" */
    LCD_DisplayChar(LCD_LINE_7, LCD_COLUMN_8, ':');
    LCD_DisplayChar(LCD_LINE_7, LCD_COLUMN_11, ':');
    /* Display the current time */
    Calendar_TimeDisplay(RTC_GetCounter());
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
    /* Change the current time */
    RTC_SetCounter(Calendar_TimeRegulate());
  }
}

/**
  * @brief  Returns the time entered by user, using demoboard keys.
  * @param  None
  * @retval None
  */
void Calendar_TimeAdjust(void)
{
  /* Disable the button interrupts */
  IntExtOnOffConfig(DISABLE);
  /* PreAdjust Time */
  Calendar_TimePreAdjust();
  /* Clear the LCD */
  LCD_Clear(LCD_COLOR_WHITE);
  /* Display the menu */
  Menu_DisplayMenu();
  /* Enable the button interrupts */
  IntExtOnOffConfig(ENABLE);
}

/**
  * @brief  Displays the current time.
  * @param  None
  * @retval None
  */
void Calendar_TimeDisplay(uint32_t TimeVar)
{
  /* Display time hours */
  time_struct.hour_h = (uint8_t)( TimeVar / 3600) / 10;
  LCD_DisplayChar(LCD_LINE_7, LCD_COLUMN_6, (time_struct.hour_h + 0x30));
  time_struct.hour_l = (uint8_t)(((TimeVar) / 3600) % 10);
  LCD_DisplayChar(LCD_LINE_7, LCD_COLUMN_7, (time_struct.hour_l + 0x30));
  /* Display time minutes */
  time_struct.min_h = (uint8_t)(((TimeVar) % 3600) / 60) / 10;
  LCD_DisplayChar(LCD_LINE_7, LCD_COLUMN_9, (time_struct.min_h + 0x30));
  time_struct.min_l = (uint8_t)(((TimeVar) % 3600) / 60) % 10;
  LCD_DisplayChar(LCD_LINE_7, LCD_COLUMN_10, (time_struct.min_l + 0x30));
  /* Display time seconds */
  time_struct.sec_h = (uint8_t)(((TimeVar) % 3600) % 60) / 10;
  LCD_DisplayChar(LCD_LINE_7, LCD_COLUMN_12, (time_struct.sec_h + 0x30));
  time_struct.sec_l = (uint8_t)(((TimeVar) % 3600) % 60) % 10;
  LCD_DisplayChar(LCD_LINE_7, LCD_COLUMN_13, (time_struct.sec_l + 0x30));
}

/**
  * @brief  Shows the current time (HH/MM/SS) on LCD.
  * @param  None
  * @retval None
  */
void Calendar_TimeShow(void)
{
  uint32_t testvalue = 0, pressedkey = 0;

  /* Disable the button interrupts */
  IntExtOnOffConfig(DISABLE);
  LCD_Clear(LCD_COLOR_WHITE);

  /* Display Watch Bitmap image here */

  if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
  {
    LCD_DisplayStringLine(LCD_LINE_5, "Time and Date Config");
    LCD_DisplayStringLine(LCD_LINE_6, "Select: Press SEL   ");
    LCD_DisplayStringLine(LCD_LINE_7, "Abort: Press Any Key");

    while (testvalue == 0)
    {
      pressedkey = Menu_ReadKey();
      if (pressedkey == SEL)
      {
        /* Adjust Time */
        Calendar_TimePreAdjust();
        /* Clear the LCD */
        LCD_Clear(LCD_COLOR_WHITE);
        testvalue = 0x01;
      }
      else if (pressedkey != NOKEY)
      {
        /* Clear the LCD */
        LCD_ClearLine(LCD_LINE_7);
        /* Display the menu */
        Menu_DisplayMenu();
        /* Enable the button interrupts */
        IntExtOnOffConfig(ENABLE);
        return;
      }
    }
    /* Display time separators ":" */
    LCD_DisplayChar(LCD_LINE_7, LCD_COLUMN_8, ':');
    LCD_DisplayChar(LCD_LINE_7, LCD_COLUMN_11, ':');
    /* Wait a press on any JoyStick pushbuttons */
    while (Menu_ReadKey() == NOKEY)
    {
      /* Display current time */
      Calendar_TimeDisplay(RTC_GetCounter());
    }
  }
  else
  {
    while (Menu_ReadKey() != NOKEY)
    {}
    LCD_DisplayStringLine(LCD_LINE_6, "        TIME        ");
    /* Clear LCD_LINE_7 */
    LCD_ClearLine(LCD_LINE_7);    
    /* Display time separators ":" */
    LCD_DisplayChar(LCD_LINE_7, LCD_COLUMN_8, ':');
    LCD_DisplayChar(LCD_LINE_7, LCD_COLUMN_11, ':');
    /* Wait a press on any JoyStick pushbuttons */
    while (Menu_ReadKey() == NOKEY)
    {
      /* Display current time */
      Calendar_TimeDisplay(RTC_GetCounter());
    }
  }
  /* Clear the LCD */
  LCD_ClearLine(LCD_LINE_7);
  /* Display the menu */
  Menu_DisplayMenu();
  /* Enable the button interrupts */
  IntExtOnOffConfig(ENABLE);
}

/**
  * @brief  Sets the date entered by user, using menu navigation keys.
  * @param  None
  * @retval None
  */
void Calendar_DateRegulate(void)
{
  LCD_DisplayStringLineColumn(LCD_LINE_3, LCD_COLUMN_2, "    Set Year    ", 16);
  LCD_DisplayStringLineColumn(LCD_LINE_4, LCD_COLUMN_2, " Press->UP/DOWN ", 16);
  LCD_DrawRect(LCD_COLUMN_2, LCD_LINE_4*8, 96, 17, LCD_COLOR_BLACK);
  LCD_DrawRect(LCD_COLUMN_2-1, LCD_LINE_4*8-1, 98, 19, LCD_COLOR_BLACK);
  while (Menu_ReadKey() == NOKEY)
  {
  }
  LCD_Clear(LCD_COLOR_WHITE);
  Calendar_DateDisplay(date_s.year, date_s.month, date_s.day);
  /* Regulate year */
  Calendar_RegulateYear();

  LCD_DisplayStringLineColumn(LCD_LINE_3, LCD_COLUMN_2, "   Set Month    ", 16);
  LCD_DisplayStringLineColumn(LCD_LINE_4, LCD_COLUMN_2, " Press->UP/DOWN ", 16);
  LCD_DrawRect(LCD_COLUMN_2, LCD_LINE_4*8, 96, 17, LCD_COLOR_BLACK);
  LCD_DrawRect(LCD_COLUMN_2-1, LCD_LINE_4*8-1, 98, 19, LCD_COLOR_BLACK);
  while (Menu_ReadKey() == NOKEY)
  {
  }
  LCD_Clear(LCD_COLOR_WHITE);
  Calendar_DateDisplay(date_s.year, date_s.month, date_s.day);
  /* Regulate the month */
  Calendar_RegulateMonth();

  LCD_DisplayStringLineColumn(LCD_LINE_3, LCD_COLUMN_2, "     Set Day    ", 16);
  LCD_DisplayStringLineColumn(LCD_LINE_4, LCD_COLUMN_2, " Press->UP/DOWN ", 16);
  LCD_DrawRect(LCD_COLUMN_2, LCD_LINE_4*8, 96, 17, LCD_COLOR_BLACK);
  LCD_DrawRect(LCD_COLUMN_2-1, LCD_LINE_4*8-1, 98, 19, LCD_COLOR_BLACK);
  while (Menu_ReadKey() == NOKEY)
  {
  }
  LCD_Clear(LCD_COLOR_WHITE);
  Calendar_DateDisplay(date_s.year, date_s.month, date_s.day);
  /* Regulate day */
  Calendar_RegulateDay();
}

/**
  * @brief  Pre-Adjusts the current date (MM/DD/YYYY).
  * @param  None
  * @retval None
  */
static void Calendar_DatePreAdjust(void)
{
  uint32_t tmp = 0, pressedkey = 0;

  /* Clear the LCD */
  LCD_Clear(LCD_COLOR_WHITE);

  if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
  {
    LCD_DisplayStringLine(LCD_LINE_5, "Time and Date Config");
    LCD_DisplayStringLine(LCD_LINE_6, "Select: Press SEL   ");
    LCD_DisplayStringLine(LCD_LINE_7, "Abort: Press Any Key");

    while (1)
    {
      pressedkey = Menu_ReadKey();
      if (pressedkey == SEL)
      {
        /* Adjust Time */
        Calendar_TimePreAdjust();
        /* Clear the LCD */
        LCD_Clear(LCD_COLOR_WHITE);
        return;
      }
      else if (pressedkey != NOKEY)
      {
        return;
      }
    }
  }
  else
  {
	/* Display the current date */
    Calendar_DateDisplay(date_s.year, date_s.month, date_s.day);
    /* Change the current date */
    Calendar_DateRegulate();
    BKP_WriteBackupRegister(BKP_DR2, date_s.year);
    tmp = date_s.month << 8;
    tmp |= date_s.day;
    BKP_WriteBackupRegister(BKP_DR3, tmp);
    BKP_WriteBackupRegister(BKP_DR4, daycolumn);
    BKP_WriteBackupRegister(BKP_DR5, dayline);
  }
}

/**
  * @brief  Adjusts the current date (MM/DD/YYYY).
  * @param  None
  * @retval None
  */
void Calendar_DateAdjust(void)
{
  /* Disable the button interrupts */
  IntExtOnOffConfig(DISABLE);
  /* Preadjust the date */
  Calendar_DatePreAdjust();
  /* Clear the LCD */
  LCD_Clear(LCD_COLOR_WHITE);
  /* Display the menu */
  Menu_DisplayMenu();
  /* Enable the button interrupts */
  IntExtOnOffConfig(ENABLE);
}

/**
  * @brief  Displays the date in a graphic mode.
  * @param  None
  * @retval None
  */
void Calendar_DateDisplay(uint16_t nYear, uint8_t nMonth, uint8_t nDay)
{
  uint32_t mline = 0, mcolumn = 0, month = 0;
  uint32_t monthlength = 0;
  if (nMonth == 2)
  {
    if (Calendar_IsLeapYear(nYear))
    {
      monthlength = 30;
    }
    else
    {
      monthlength = MonLen[nMonth - 1];
    }
  }
  else
  {
    monthlength = MonLen[nMonth - 1];
  }

  LCD_DisplayStringLine(LCD_LINE_0, MonthNames[nMonth - 1]);
  LCD_DisplayChar(LCD_LINE_0, LCD_COLUMN_13, ((nYear / 1000) + 0x30));
  LCD_DisplayChar(LCD_LINE_0, LCD_COLUMN_14, (((nYear % 1000) / 100) + 0x30));
  LCD_DisplayChar(LCD_LINE_0, LCD_COLUMN_15, ((((nYear % 1000) % 100) / 10) + 0x30));
  LCD_DisplayChar(LCD_LINE_0, LCD_COLUMN_16, ((((nYear % 1000) % 100) % 10) + 0x30));
  Calendar_WeekDayNum(nYear, nMonth, nDay);
/*
  LCD_DisplayStringLine(LCD_LINE_1, " WEEK     DAY N:    ");
  if (wn / 10)
  {
    LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_6, ((wn / 10) + 0x30));
    LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_7, ((wn % 10) + 0x30));
  }
  else
  {
    LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_6, ((wn % 10) + 0x30));
  }
  if (dc / 100)
  {
    LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_17, ((dc / 100) + 0x30));
    LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_18, (((dc % 100) / 10) + 0x30));
    LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_19, (((dc % 100) % 10) + 0x30));
  }
  else if (dc / 10)
  {
    LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_17, ((dc / 10) + 0x30));
    LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_18, ((dc % 10) + 0x30));
  }
  else
  {
    LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_17, ((dc % 10) + 0x30));
  }
*/
  // LCD_SetTextColor(LCD_COLOR_TEXT_REVERSE);
  LCD_DisplayStringLine(LCD_LINE_1, "Mo Tu We Th Fr Sa Su");
  // LCD_SetTextColor(LCD_COLOR_TEXT_NORMAL);
  /* Determines the week number, day of the week of the selected date */
  Calendar_WeekDayNum(nYear, nMonth, 1);
  mline = LCD_LINE_2;
  mcolumn = LCD_COLUMN_0 + (3 * LCD_FONT_WIDTH * dn);
  for (month = 1; month < monthlength; month++)
  {
    if (month == nDay)
    {
      daycolumn = mcolumn;
      dayline = mline;
    }
    if (month / 10)
    {
      LCD_DisplayChar(mline, mcolumn, ((month / 10) + 0x30));
    }
    else
    {
      LCD_DisplayChar(mline, mcolumn, ' ');
    }
    mcolumn += LCD_FONT_WIDTH;
    LCD_DisplayChar(mline, mcolumn, ((month % 10) + 0x30));
    if (mcolumn >= LCD_COLUMN_19)
    {
      mcolumn = LCD_COLUMN_0;
      mline -= 1;
    }
    else
    {
      mcolumn += LCD_FONT_WIDTH;
      LCD_DisplayChar(mline, mcolumn, ' ');
      mcolumn += LCD_FONT_WIDTH;
    }
  }
  LCD_DrawRect(daycolumn-1, dayline*8, 13, 9, LCD_COLOR_BLACK);
}

/**
  * @brief  Shows date in a graphic mode on LCD.
  * @param  None
  * @retval None
  */
void Calendar_DateShow(void)
{
  uint32_t tmpValue = 0;
  uint32_t MyKey = 0, ValueMax = 0;
  uint32_t firstdaycolumn = 0, lastdaycolumn = 0, lastdayline = 0;
  uint32_t testvalue = 0, pressedkey = 0;

  /* Disable the button interrupts */
  IntExtOnOffConfig(DISABLE);
  LCD_Clear(LCD_COLOR_WHITE);
  while (Menu_ReadKey() != NOKEY)
  {}
  if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
  {
    LCD_DisplayStringLine(LCD_LINE_5, "Time and Date Config");
    LCD_DisplayStringLine(LCD_LINE_6, "Select: Press SEL   ");
    LCD_DisplayStringLine(LCD_LINE_7, "Abort: Press Any Key");

    while (testvalue == 0)
    {
      pressedkey = Menu_ReadKey();
      if (pressedkey == SEL)
      {
        /* Adjust Time */
        Calendar_TimePreAdjust();
        /* Clear the LCD */
        LCD_Clear(LCD_COLOR_WHITE);
        testvalue = 0x01;
      }
      else if (pressedkey != NOKEY)
      {
        /* Clear the LCD */
        LCD_Clear(LCD_COLOR_WHITE);
        /* Display the menu */
        Menu_DisplayMenu();
        /* Enable the button interrupts */
        IntExtOnOffConfig(ENABLE);
        return;
      }
    }
  }
  /* Clear the LCD */
  LCD_Clear(LCD_COLOR_WHITE);

  // LCD_DisplayStringLine(LCD_LINE_0, " To Exit Press OK.  ");
  /* Display the current date */
  Calendar_DateDisplay(date_s.year, date_s.month, date_s.day);
  if (date_s.month == 2)
  {
    if (Calendar_IsLeapYear(date_s.year))
    {
      ValueMax = 29;
    }
    else
    {
      ValueMax = (MonLen[date_s.month - 1] - 1);
    }
  }
  else
  {
    ValueMax = (MonLen[date_s.month - 1] - 1);
  }
  firstdaycolumn = LCD_COLUMN_0 + (3 * LCD_FONT_WIDTH * dn);

  lastdaycolumn = ValueMax - (7 - dn);
  lastdayline = lastdaycolumn / 7;
  lastdaycolumn %= 7;
  if (lastdaycolumn == 0)
  {
    lastdayline = LCD_LINE_2 - lastdayline;
    lastdaycolumn = LCD_COLUMN_18;
  }
  else
  {
    lastdayline = LCD_LINE_3 - lastdayline;
    lastdaycolumn = LCD_COLUMN_0 + (3 * LCD_FONT_WIDTH * (lastdaycolumn - 1));
  }
  /* Initialize tmpValue */
  tmpValue = date_s.day;

  /* Endless loop */
  while (1)
  {
    /* Check which key is pressed */
    MyKey = Menu_ReadKey();
  
    /* If "RIGHT/DOWN" pushbutton is pressed */
    if (MyKey == DOWN)    //Since "RIGHT" key is not present
    {
      LCD_DrawRect(daycolumn-1, dayline*8, 13, 9, LCD_COLOR_WHITE);
      /* Increase the value of the digit */
      if (tmpValue == ValueMax)
      {
        tmpValue = 0;
        dayline = LCD_LINE_2;
        daycolumn = firstdaycolumn - (3 * LCD_FONT_WIDTH);
      }
      if (daycolumn == LCD_COLUMN_18)
      {
        daycolumn = LCD_COLUMN_0 - (3 * LCD_FONT_WIDTH);
        dayline -= 1;
      }
      daycolumn += (3 * LCD_FONT_WIDTH);
      LCD_DrawRect(daycolumn-1, dayline*8, 13, 9, LCD_COLOR_BLACK);
      tmpValue++;
      Calendar_WeekDayNum(date_s.year, date_s.month, tmpValue);
/*  
      LCD_DisplayStringLine(LCD_LINE_1, " WEEK     DAY N:    ");
      if (wn / 10)
      {
        LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_6, ((wn / 10) + 0x30));
        LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_7, ((wn % 10) + 0x30));
      }
      else
      {
        LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_6, ((wn % 10) + 0x30));
      }
      if (dc / 100)
      {
        LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_17, ((dc / 100) + 0x30));
        LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_18, (((dc % 100) / 10) + 0x30));
        LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_19, (((dc % 100) % 10) + 0x30));
      }
      else if (dc / 10)
      {
        LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_17, ((dc / 10) + 0x30));
        LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_18, ((dc % 10) + 0x30));
      }
      else
      {
        LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_17, ((dc % 10) + 0x30));
      }
*/      
    }
    
    /* If "LEFT/UP" pushbutton is pressed */
    if (MyKey == UP)    //Since "LEFT" key is not present
    {
      LCD_DrawRect(daycolumn-1, dayline*8, 13, 9, LCD_COLOR_WHITE);
      /* Decrease the value of the digit */
      if (tmpValue == 1)
      {
        tmpValue = ValueMax + 1;
        dayline = lastdayline;
        daycolumn = lastdaycolumn + (3 * LCD_FONT_WIDTH);
      }
      if (daycolumn == LCD_COLUMN_0)
      {
        daycolumn = LCD_COLUMN_21;
        dayline += 1;
      }
      daycolumn -= (3 * LCD_FONT_WIDTH);
      LCD_DrawRect(daycolumn-1, dayline*8, 13, 9, LCD_COLOR_BLACK);
      tmpValue--;
      Calendar_WeekDayNum(date_s.year, date_s.month, tmpValue);
/*  
      LCD_DisplayStringLine(LCD_LINE_1, " WEEK     DAY N:    ");
      if (wn / 10)
      {
        LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_6, ((wn / 10) + 0x30));
        LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_7, ((wn % 10) + 0x30));
      }
      else
      {
        LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_6, ((wn % 10) + 0x30));
      }
      if (dc / 100)
      {
        LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_17, ((dc / 100) + 0x30));
        LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_18, (((dc % 100) / 10) + 0x30));
        LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_19, (((dc % 100) % 10) + 0x30));
      }
      else if (dc / 10)
      {
        LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_17, ((dc / 10) + 0x30));
        LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_18, ((dc % 10) + 0x30));
      }
      else
      {
        LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_17, ((dc % 10) + 0x30));
      }
*/      
    }

    // /* If "UP" pushbutton is pressed */
    // if (MyKey == UP)
    // {
      // LCD_DrawRect(daycolumn-1, dayline*8, 13, 9, LCD_COLOR_WHITE);

      // if (tmpValue == 1)
      // {
        // dayline = lastdayline;
        // daycolumn =  lastdaycolumn;
        // tmpValue = ValueMax;
      // }
      // else if (tmpValue < 8)
      // {
        // tmpValue = 1;
        // dayline = LCD_LINE_2;
        // daycolumn = firstdaycolumn;
      // }
      // else
      // {
        // dayline += 1;
        // tmpValue -= 7;
      // }
      // LCD_DrawRect(daycolumn-1, dayline*8, 13, 9, LCD_COLOR_BLACK);
      // Calendar_WeekDayNum(date_s.year, date_s.month, tmpValue);
// /*
      // LCD_DisplayStringLine(LCD_LINE_1, " WEEK     DAY N:    ");
      // if (wn / 10)
      // {
        // LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_6, ((wn / 10) + 0x30));
        // LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_7, ((wn % 10) + 0x30));
      // }
      // else
      // {
        // LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_6, ((wn % 10) + 0x30));
      // }
      // if (dc / 100)
      // {
        // LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_17, ((dc / 100) + 0x30));
        // LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_18, (((dc % 100) / 10) + 0x30));
        // LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_19, (((dc % 100) % 10) + 0x30));
      // }
      // else if (dc / 10)
      // {
        // LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_17, ((dc / 10) + 0x30));
        // LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_18, ((dc % 10) + 0x30));
      // }
      // else
      // {
        // LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_17, ((dc % 10) + 0x30));
      // }
// */
    // }

    // /* If "DOWN" pushbutton is pressed */
    // if (MyKey == DOWN)
    // {
      // LCD_DrawRect(daycolumn-1, dayline*8, 13, 9, LCD_COLOR_WHITE);
      // if (tmpValue == ValueMax)
      // {
        // dayline = LCD_LINE_2;
        // daycolumn =  firstdaycolumn;
        // tmpValue = 1;
      // }
      // else
      // {
        // dayline -= 1;
        // tmpValue += 7;
      // }
      // if (tmpValue > ValueMax)
      // {
        // tmpValue = ValueMax;
        // dayline = lastdayline;
        // daycolumn = lastdaycolumn;
      // }
      // LCD_DrawRect(daycolumn-1, dayline*8, 13, 9, LCD_COLOR_BLACK);
      // Calendar_WeekDayNum(date_s.year, date_s.month, tmpValue);
// /*
      // LCD_DisplayStringLine(LCD_LINE_1, " WEEK     DAY N:    ");
      // if (wn / 10)
      // {
        // LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_6, ((wn / 10) + 0x30));
        // LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_7, ((wn % 10) + 0x30));
      // }
      // else
      // {
        // LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_6, ((wn % 10) + 0x30));
      // }
      // if (dc / 100)
      // {
        // LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_17, ((dc / 100) + 0x30));
        // LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_18, (((dc % 100) / 10) + 0x30));
        // LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_19, (((dc % 100) % 10) + 0x30));
      // }
      // else if (dc / 10)
      // {
        // LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_17, ((dc / 10) + 0x30));
        // LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_18, ((dc % 10) + 0x30));
      // }
      // else
      // {
        // LCD_DisplayChar(LCD_LINE_1, LCD_COLUMN_17, ((dc % 10) + 0x30));
      // }
// */
    // }
    
    /* If "SEL" pushbutton is pressed */
    if (MyKey == SEL)
    {
      /* Clear the LCD */
      LCD_Clear(LCD_COLOR_WHITE);
      /* Display the menu */
      Menu_DisplayMenu();
      /* Enable the button interrupts */
      IntExtOnOffConfig(ENABLE);
      return;
    }
  }
}

/**
  * @brief  Updates date when time is 23:59:59.
  * @param  None
  * @retval None
  */
void Calendar_DateUpdate(void)
{
  if (date_s.month == 1 || date_s.month == 3 || date_s.month == 5 || date_s.month == 7 ||
      date_s.month == 8 || date_s.month == 10 || date_s.month == 12)
  {
    if (date_s.day < 31)
    {
      date_s.day++;
    }
    /* Date structure member: date_s.day = 31 */
    else
    {
      if (date_s.month != 12)
      {
        date_s.month++;
        date_s.day = 1;
      }
      /* Date structure member: date_s.day = 31 & date_s.month =12 */
      else
      {
        date_s.month = 1;
        date_s.day = 1;
        date_s.year++;
      }
    }
  }
  else if (date_s.month == 4 || date_s.month == 6 || date_s.month == 9 ||
           date_s.month == 11)
  {
    if (date_s.day < 30)
    {
      date_s.day++;
    }
    /* Date structure member: date_s.day = 30 */
    else
    {
      date_s.month++;
      date_s.day = 1;
    }
  }
  else if (date_s.month == 2)
  {
    if (date_s.day < 28)
    {
      date_s.day++;
    }
    else if (date_s.day == 28)
    {
      /* Leap year */
      if (Calendar_IsLeapYear(date_s.year))
      {
        date_s.day++;
      }
      else
      {
        date_s.month++;
        date_s.day = 1;
      }
    }
    else if (date_s.day == 29)
    {
      date_s.month++;
      date_s.day = 1;
    }
  }
}

/**
  * @brief  Regulates the year.
  * @param  None
  * @retval None
  */
static void Calendar_RegulateYear(void)
{
  uint32_t tmpValue = 0;
  uint32_t MyKey = 0;
  /* Initialize tmpValue */
  tmpValue = date_s.year;
  /* Endless loop */
  while (1)
  {
    /* Check which key is pressed */
    MyKey = Menu_ReadKey();
    /* If "UP" pushbutton is pressed */
    if (MyKey == UP)
    {
      /* Increase the value of the digit */
      if (tmpValue == 2099)
      {
        tmpValue = 1874;
      }
      // LCD_ClearLine(LCD_LINE_2);
      // LCD_ClearLine(LCD_LINE_6);
      // LCD_ClearLine(LCD_LINE_7);
      LCD_Clear(LCD_COLOR_WHITE);
      Calendar_DateDisplay(++tmpValue, date_s.month, date_s.day);
    }
    /* If "DOWN" pushbutton is pressed */
    if (MyKey == DOWN)
    {
      /* Decrease the value of the digit */
      if (tmpValue == 1875)
      {
        tmpValue = 2100;
      }
      // LCD_ClearLine(LCD_LINE_2);
      // LCD_ClearLine(LCD_LINE_6);
      // LCD_ClearLine(LCD_LINE_7);
      LCD_Clear(LCD_COLOR_WHITE);
      /* Display new value */
      Calendar_DateDisplay(--tmpValue, date_s.month, date_s.day);
    }
    /* If "SEL" pushbutton is pressed */
    if (MyKey == SEL)
    {
      // LCD_ClearLine(LCD_LINE_2);
      // LCD_ClearLine(LCD_LINE_6);
      // LCD_ClearLine(LCD_LINE_7);
      LCD_Clear(LCD_COLOR_WHITE);
      /* Display new value */
      Calendar_DateDisplay(tmpValue, date_s.month, date_s.day);
      /* Return the digit value and exit */
      date_s.year = tmpValue;
      return;
    }
  }
}

/**
  * @brief  Regulates month.
  * @param  None
  * @retval None
  */
static void Calendar_RegulateMonth(void)
{
  uint32_t tmpValue = 0;
  uint32_t MyKey = 0;
  
  /* Initialize tmpValue */
  tmpValue = date_s.month;
  
  /* Endless loop */
  while (1)
  {
    /* Check which key is pressed */
    MyKey = Menu_ReadKey();
    /* If "UP" pushbutton is pressed */
    if (MyKey == UP)
    {
      /* Increase the value of the digit */
      if (tmpValue == 12)
      {
        tmpValue = 0;
      }
      // LCD_ClearLine(LCD_LINE_2);
      // LCD_ClearLine(LCD_LINE_6);
      // LCD_ClearLine(LCD_LINE_7);
      LCD_Clear(LCD_COLOR_WHITE);
      Calendar_DateDisplay(date_s.year, ++tmpValue, date_s.day);
    }
    /* If "DOWN" pushbutton is pressed */
    if (MyKey == DOWN)
    {
      /* Decrease the value of the digit */
      if (tmpValue == 1)
      {
        tmpValue = 13;
      }
      // LCD_ClearLine(LCD_LINE_2);
      // LCD_ClearLine(LCD_LINE_6);
      // LCD_ClearLine(LCD_LINE_7);
      LCD_Clear(LCD_COLOR_WHITE);
      /* Display new value */
      Calendar_DateDisplay(date_s.year, --tmpValue, date_s.day);
    }
    /* If "SEL" pushbutton is pressed */
    if (MyKey == SEL)
    {
      // LCD_ClearLine(LCD_LINE_2);
      // LCD_ClearLine(LCD_LINE_6);
      // LCD_ClearLine(LCD_LINE_7);
      LCD_Clear(LCD_COLOR_WHITE);
      /* Display new value */
      Calendar_DateDisplay(date_s.year, tmpValue, date_s.day);
      /* Return the digit value and exit */
      date_s.month = tmpValue;
      return;
    }
  }
}

/**
  * @brief  Regulates day.
  * @param  None
  * @retval None
  */
static void Calendar_RegulateDay(void)
{
  uint32_t tmpValue = 0;
  uint32_t MyKey = 0, ValueMax = 0;
  uint32_t firstdaycolumn = 0, lastdaycolumn = 0, lastdayline = 0;

  if (date_s.month == 2)
  {
    if (Calendar_IsLeapYear(date_s.year))
      ValueMax = 29;
    else
      ValueMax = (MonLen[date_s.month - 1] - 1);
  }
  else
  {
    ValueMax = (MonLen[date_s.month - 1] - 1);
  }
  firstdaycolumn = LCD_COLUMN_0 + (3 * LCD_FONT_WIDTH * dn);
  lastdaycolumn = ValueMax - (7 - dn);
  lastdayline = lastdaycolumn / 7;
  lastdaycolumn %= 7;
  if (lastdaycolumn == 0)
  {
    lastdayline = LCD_LINE_2 - lastdayline;
    lastdaycolumn = LCD_COLUMN_18;
  }
  else
  {
    lastdayline = LCD_LINE_3 - lastdayline;
    lastdaycolumn = LCD_COLUMN_0 + (3 * LCD_FONT_WIDTH * (lastdaycolumn - 1));
  }

  /* Initialize tmpValue */
  tmpValue = date_s.day;

  /* Endless loop */
  while (1)
  {
    /* Check which key is pressed */
    MyKey = Menu_ReadKey();
  
    /* If "RIGHT/DOWN" pushbutton is pressed */
    if (MyKey == DOWN)    //Since "RIGHT" key is not present
    {
      LCD_DrawRect(daycolumn-1, dayline*8, 13, 9, LCD_COLOR_WHITE);
      /* Increase the value of the digit */
      if (tmpValue == ValueMax)
      {
        tmpValue = 0;
        dayline = LCD_LINE_2;
        daycolumn = firstdaycolumn - (3 * LCD_FONT_WIDTH);
      }
      if (daycolumn == LCD_COLUMN_18)
      {
        daycolumn = LCD_COLUMN_0 - (3 * LCD_FONT_WIDTH);
        dayline -= 1;
      }
      daycolumn += (3 * LCD_FONT_WIDTH);
      LCD_DrawRect(daycolumn-1, dayline*8, 13, 9, LCD_COLOR_BLACK);
      tmpValue++;
    }

    /* If "LEFT/UP" pushbutton is pressed */
    if (MyKey == UP)    //Since "LEFT" key is not present
    {
      LCD_DrawRect(daycolumn-1, dayline*8, 13, 9, LCD_COLOR_WHITE);   
      /* Decrease the value of the digit */
      if (tmpValue == 1)
      {
        tmpValue = ValueMax + 1;
        dayline = lastdayline;
        daycolumn = lastdaycolumn + (3 * LCD_FONT_WIDTH);
      }
      if (daycolumn == LCD_COLUMN_0)
      {
        daycolumn = LCD_COLUMN_21;
        dayline += 1;
      }
      daycolumn -= (3 * LCD_FONT_WIDTH);
      LCD_DrawRect(daycolumn-1, dayline*8, 13, 9, LCD_COLOR_BLACK);
      tmpValue--;
    }

    // /* If "UP" pushbutton is pressed */
    // if (MyKey == UP)
    // {
      // LCD_DrawRect(daycolumn-1, dayline*8, 13, 9, LCD_COLOR_WHITE);
      // if (tmpValue == 1)
      // {
        // dayline = lastdayline;
        // daycolumn =  lastdaycolumn;
        // tmpValue = ValueMax;
      // }
      // else if (tmpValue < 8)
      // {
        // tmpValue = 1;
        // dayline = LCD_LINE_2;
        // daycolumn = firstdaycolumn;
      // }
      // else
      // {
        // dayline += 1;
        // tmpValue -= 7;
      // }
      // LCD_DrawRect(daycolumn-1, dayline*8, 13, 9, LCD_COLOR_BLACK);
    // }

    // /* If "DOWN" pushbutton is pressed */
    // if (MyKey == DOWN)
    // {
      // LCD_DrawRect(daycolumn-1, dayline*8, 13, 9, LCD_COLOR_WHITE);
      // if (tmpValue == ValueMax)
      // {
        // dayline = LCD_LINE_2;
        // daycolumn =  firstdaycolumn;
        // tmpValue = 1;
      // }
      // else
      // {
        // dayline -= 1;
        // tmpValue += 7;
      // }
      // if (tmpValue > ValueMax)
      // {
        // tmpValue = ValueMax;
        // dayline = lastdayline;
        // daycolumn = lastdaycolumn;
      // }
      // LCD_DrawRect(daycolumn-1, dayline*8, 13, 9, LCD_COLOR_BLACK);
    // }
    
    /* If "SEL" pushbutton is pressed */
    if (MyKey == SEL)
    {
      /* Return the digit value and exit */
      date_s.day = tmpValue;
      return;
    }
  }
}

/**
  * @brief  Determines the week number, the day number and the week day number.
  * @param  None
  * @retval None
  */
static void Calendar_WeekDayNum(uint32_t nyear, uint8_t nmonth, uint8_t nday)
{
  uint32_t a = 0, b = 0, c = 0, s = 0, e = 0, f = 0, g = 0, d = 0;
  int32_t n = 0;
  if (nmonth < 3)
  {
    a = nyear - 1;
  }
  else
  {
    a = nyear;
  }

  b = (a / 4) - (a / 100) + (a / 400);
  c = ((a - 1) / 4) - ((a - 1) / 100) + ((a - 1) / 400);
  s = b - c;
  if (nmonth < 3)
  {
    e = 0;
    f =  nday - 1 + 31 * (nmonth - 1);
  }
  else
  {
    e = s + 1;
    f = nday + (153 * (nmonth - 3) + 2) / 5 + 58 + s;
  }
  g = (a + b) % 7;
  d = (f + g - e) % 7;
  n = f + 3 - d;
  if (n < 0)
  {
    wn = 53 - ((g - s) / 5);
  }
  else if (n > (364 + s))
  {
    wn = 1;
  }
  else
  {
    wn = (n / 7) + 1;
  }
  dn = d;
  dc = f + 1;
}

/**
  * @brief  Check whether the passed year is Leap or not.
  * @param  None
  * @retval 1: leap year
  *         0: not leap year
  */
static uint8_t Calendar_IsLeapYear(uint16_t nYear)
{
  if (nYear % 4 != 0) return 0;
  if (nYear % 100 != 0) return 1;
  return (uint8_t)(nYear % 400 == 0);
}

/**
  * @brief  Returns the alarm time entered by user, using demoboard keys.
  * @param  None
  * @retval Alarm time value to be loaded in RTC alarm register.
  */
uint32_t Calendar_AlarmRegulate(void)
{
  uint32_t Alarm_HH = 0, Alarm_MM = 0, Alarm_SS = 0;

  /* Read alarm hours */
  Alarm_HH = Calendar_ReadDigit(LCD_COLUMN_6, alarm_struct.hour_h, 0x2, 0x0);
  if (Alarm_HH == 0x2)
  {
    if (alarm_struct.hour_l > 0x3)
    {
      alarm_struct.hour_l = 0x0;
    }
    Alarm_HH = Alarm_HH * 10 + Calendar_ReadDigit(LCD_COLUMN_7, alarm_struct.hour_l, 0x3, 0x0);
  }
  else
  {
    Alarm_HH = Alarm_HH * 10 + Calendar_ReadDigit(LCD_COLUMN_7, alarm_struct.hour_l, 0x9, 0x0);
  }
  /* Read alarm minutes */
  Alarm_MM = Calendar_ReadDigit(LCD_COLUMN_9, alarm_struct.min_h, 0x5, 0x0);
  Alarm_MM = Alarm_MM * 10 + Calendar_ReadDigit(LCD_COLUMN_10, alarm_struct.min_l, 0x9, 0x0);
  /* Read alarm seconds */
  Alarm_SS = Calendar_ReadDigit(LCD_COLUMN_12, alarm_struct.sec_h, 0x5, 0x0);
  Alarm_SS = Alarm_SS * 10 + Calendar_ReadDigit(LCD_COLUMN_13, alarm_struct.sec_l, 0x9, 0x0);

  /* Return the alarm value to store in the RTC Alarm register */
  return((Alarm_HH*3600 + Alarm_MM*60 + Alarm_SS));
}

/**
  * @brief  Configures an alarm event to occurs within the current day.
  * @param  None
  * @retval None
  */
void Calendar_AlarmPreAdjust(void)
{
  uint32_t tmp = 0;

  if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
  {
    LCD_DisplayStringLine(LCD_LINE_6, "Time not configured ");
    LCD_DisplayStringLine(LCD_LINE_7, "      Press OK      ");
    while (Menu_ReadKey() == NOKEY)
    {}
    return;
  }
  
  /* Read the alarm value stored in the Backup register */
  tmp = BKP_ReadBackupRegister(BKP_DR6);
  tmp |= BKP_ReadBackupRegister(BKP_DR7) << 16;
  LCD_DisplayStringLine(LCD_LINE_6, "       ALARM        ");
  /* Clear LCD_LINE_7 */
  LCD_ClearLine(LCD_LINE_7);

  /* Display time separators ":" */
  LCD_DisplayChar(LCD_LINE_7, LCD_COLUMN_8, ':');
  LCD_DisplayChar(LCD_LINE_7, LCD_COLUMN_11, ':');
  /* Display the alarm value */
  Calendar_AlarmDisplay(tmp);
  /* Store new alarm value */
  tmp = Calendar_AlarmRegulate();
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
  /* Set RTC Alarm register with the new value */
  RTC_SetAlarm(tmp);
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
  /* Save the Alarm value in the Backup register */
  BKP_WriteBackupRegister(BKP_DR6, (tmp & 0x0000FFFF));
  BKP_WriteBackupRegister(BKP_DR7, (tmp >> 16));
}

/**
  * @brief  Adjusts an alarm event to occurs within the current day.
  * @param  None
  * @retval None
  */
void Calendar_AlarmAdjust(void)
{
  /* Disable the button interrupts */
  IntExtOnOffConfig(DISABLE);

  LCD_Clear(LCD_COLOR_WHITE);

  /* Display Alarm Bitmap image here */

  Calendar_AlarmPreAdjust();
  /* Clear the LCD */
  LCD_Clear(LCD_COLOR_WHITE);
  /* Display the menu */
  Menu_DisplayMenu();
  /* Enable the button interrupts */
  IntExtOnOffConfig(ENABLE);
}

/**
  * @brief  Displays alarm time.
  * @param  None
  * @retval None
  */
void Calendar_AlarmDisplay(uint32_t AlarmVar)
{
  /* Display alarm hours */
  alarm_struct.hour_h = (uint8_t)( AlarmVar / 3600) / 10;
  LCD_DisplayChar(LCD_LINE_7, LCD_COLUMN_6, (alarm_struct.hour_h + 0x30));
  alarm_struct.hour_l = (uint8_t)(((AlarmVar) / 3600) % 10);
  LCD_DisplayChar(LCD_LINE_7, LCD_COLUMN_7, (alarm_struct.hour_l + 0x30));
  /* Display alarm minutes */
  alarm_struct.min_h = (uint8_t)(((AlarmVar) % 3600) / 60) / 10;
  LCD_DisplayChar(LCD_LINE_7, LCD_COLUMN_9, (alarm_struct.min_h + 0x30));
  alarm_struct.min_l = (uint8_t)(((AlarmVar) % 3600) / 60) % 10;
  LCD_DisplayChar(LCD_LINE_7, LCD_COLUMN_10, (alarm_struct.min_l + 0x30));
  /* Display alarm seconds */
  alarm_struct.sec_h = (uint8_t)(((AlarmVar) % 3600) % 60) / 10;
  LCD_DisplayChar(LCD_LINE_7, LCD_COLUMN_12, (alarm_struct.sec_h + 0x30));
  alarm_struct.sec_l = (uint8_t)(((AlarmVar) % 3600) % 60) % 10;
  LCD_DisplayChar(LCD_LINE_7, LCD_COLUMN_13, (alarm_struct.sec_l + 0x30));
}

/**
  * @brief  Shows alarm time (HH/MM/SS) on LCD.
  * @param  None
  * @retval None
  */
void Calendar_AlarmShow(void)
{
  uint32_t tmp = 0;

  /* Disable the button interrupts */
  IntExtOnOffConfig(DISABLE);
  LCD_Clear(LCD_COLOR_WHITE);

  /* Display Alarm Bitmap image here */

  if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
  {
    LCD_DisplayStringLine(LCD_LINE_6, "Time not configured ");
    LCD_DisplayStringLine(LCD_LINE_7, "      Press OK      ");
    while (Menu_ReadKey() == NOKEY)
    {}
    /* Clear the LCD */
    LCD_Clear(LCD_COLOR_WHITE);
    /* Display the menu */
    Menu_DisplayMenu();
    /* Enable the button interrupts */
    IntExtOnOffConfig(ENABLE);
    return;
  }

  /* Read the alarm value stored in the Backup register */
  tmp = BKP_ReadBackupRegister(BKP_DR6);
  tmp |= BKP_ReadBackupRegister(BKP_DR7) << 16;
  LCD_DisplayStringLine(LCD_LINE_6, "       ALARM        ");
  LCD_ClearLine(LCD_LINE_7);
  /* Display alarm separators ":" */
  LCD_DisplayChar(LCD_LINE_7, LCD_COLUMN_8, ':');
  LCD_DisplayChar(LCD_LINE_7, LCD_COLUMN_11, ':');
  /* Display actual alarm value */
  Calendar_AlarmDisplay(tmp);
  /* Wait a press on pushbuttons */
  while (Menu_ReadKey() != NOKEY)
  {}
  /* Wait a press on pushbuttons */
  while (Menu_ReadKey() == NOKEY)
  {}
  /* Clear the LCD */
  LCD_Clear(LCD_COLOR_WHITE);
  /* Display the menu */
  Menu_DisplayMenu();
  /* Enable the button interrupts */
  IntExtOnOffConfig(ENABLE);
}

/**
  * @brief  Configures the RTC.
  * @param  None
  * @retval None
  */
void Calendar_RTC_Configuration(void)
{
  /* PWR and BKP clocks selection --------------------------------------------*/
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);
  /* Reset Backup Domain */
  BKP_DeInit();
  /* Enable the LSE OSC */
  RCC_LSEConfig(RCC_LSE_ON);
  /* Wait till LSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {
  }
  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
  /* Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);
  /* Wait for RTC registers synchronization */
  RTC_WaitForSynchro();
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Enable the RTC Second */
  RTC_ITConfig(RTC_IT_SEC, ENABLE);
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Set RTC prescaler: set RTC period to 1sec */
  RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
}

/*-----------------------------------------------------------------------*/
/* Get current time for FatFs                                            */
/*-----------------------------------------------------------------------*/
DWORD get_fattime ()
{
	DWORD res;

    uint32_t rtc_counter = RTC_GetCounter();
	
	res =  (((DWORD)date_s.year - 1980) << 25)
			| ((DWORD)date_s.month << 21)
			| ((DWORD)date_s.day << 16)
			| (WORD)(((rtc_counter)/3600) << 11)
			| (WORD)((((rtc_counter)%3600)/60) << 5)
			| (WORD)((((rtc_counter)%3600)%60) >> 1);

	return res;
}

void Create_Date_Time_Filename(uint8_t* Filename, uint8_t* Filetype)
{
    uint8_t* pfname = Filename;
    uint32_t rtc_counter = RTC_GetCounter();

    *pfname++ = (uint8_t) date_s.day/10 + 0x30;    
    *pfname++ = (uint8_t) date_s.day%10 + 0x30;   
    *pfname++ = '_';
    *pfname++ = (uint8_t) MonthNames[date_s.month - 1][3];
    *pfname++ = (uint8_t) MonthNames[date_s.month - 1][4];
    *pfname++ = (uint8_t) MonthNames[date_s.month - 1][5];
    *pfname++ = '_';
    *pfname++ = (uint8_t)(date_s.year/1000) + 0x30;
    *pfname++ = (uint8_t)((date_s.year%1000)/100) + 0x30;
    *pfname++ = (uint8_t)((date_s.year%1000)%100)/10 + 0x30;
    *pfname++ = (uint8_t)((date_s.year%1000)%100)%10 + 0x30; 
    *pfname++ = '_';
    *pfname++ = (uint8_t)(rtc_counter/3600)/10 + 0x30;    
    *pfname++ = (uint8_t)(rtc_counter/3600)%10 + 0x30;    
    *pfname++ = '_';
    *pfname++ = (uint8_t)((rtc_counter%3600)/60)/10 + 0x30;  
    *pfname++ = (uint8_t)((rtc_counter%3600)/60)%10 + 0x30;  
    *pfname++ = '_';
    *pfname++ = (uint8_t)((rtc_counter%3600)%60)/10 + 0x30;   
    *pfname++ = (uint8_t)((rtc_counter%3600)%60)%10 + 0x30;    
    *pfname++ = *Filetype++;
    *pfname++ = *Filetype++;    
    *pfname++ = *Filetype++;    
    *pfname++ = *Filetype;
    *pfname++ = '\0';
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */


