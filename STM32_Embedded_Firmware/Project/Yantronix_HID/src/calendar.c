/******************** (C) COPYRIGHT 2009 STMicroelectronics ********************
* File Name          : calendar.c
* Author             : MCD Application Team
* Version            : V2.0.0
* Date               : 04/27/2009
* Description        : This file includes the calendar driver for the 
*                      STM3210B-EVAL demonstration.
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

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
  
static uint32_t wn = 0, dn = 0, dc = 0;
static uint16_t daycolumn = 0, dayline = 0;
uint8_t MonLen[12]= {32, 29, 32, 31, 32, 31, 32, 32, 31, 32, 31, 32};
uint8_t MonthNames[12][20] =
  {
    "   JANUARY          ", "   FEBRUARY         ", "   MARCH            ",
    "   APRIL            ", "   MAY              ", "   JUNE             ",
    "   JULY             ", "   AUGUST           ", "   SEPTEMBER        ",
    "   OCTOBER          ", "   NOVEMBER         ", "   DECEMBER         "
  };

/* Private function prototypes -----------------------------------------------*/
static uint8_t IsLeapYear(uint16_t nYear);
static void WeekDayNum(uint32_t nyear, uint8_t nmonth, uint8_t nday);
static void RTC_Configuration(void);
static void RegulateYear(void);
static void RegulateMonth(void);
static void RegulateDay(void);
static void Time_PreAdjust(void);
static void Date_PreAdjust(void);

/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : ReadDigit
* Description    : Reads digit entered by user, using menu navigation keys.
* Input          : None
* Output         : None
* Return         : Digit value
*******************************************************************************/
uint8_t ReadDigit(uint8_t ColBegin, uint8_t CountBegin, uint8_t ValueMax, uint8_t ValueMin)
{
  uint32_t MyKey = 0, tmpValue = 0;

  /* Set the Back Color */
  LCD_SetBackColor(Red);
  /* Set the Text Color */
  LCD_SetTextColor(White);

  /* Initialize tmpValue */
  tmpValue = CountBegin;
  /* Display  */
  LCD_DisplayChar(Line8, ColBegin, (tmpValue + 0x30));

  /* Endless loop */
  while(1)
  {
    /* Check which key is pressed */
    MyKey = ReadKey();

    /* If "UP" pushbutton is pressed */
    if(MyKey == UP)
    {
      /* Increase the value of the digit */
      if(tmpValue == ValueMax)
      {
        tmpValue = (ValueMin - 1);
      }
      /* Display new value */
      LCD_DisplayChar(Line8, ColBegin,((++tmpValue) + 0x30));
    }
    /* If "DOWN" pushbutton is pressed */
    if(MyKey == DOWN)
    {
      /* Decrease the value of the digit */
      if(tmpValue == ValueMin)
      {
        tmpValue = (ValueMax + 1);
      }
      /* Display new value */
      LCD_DisplayChar(Line8, ColBegin,((--tmpValue) + 0x30));
    }
    /* If "SEL" pushbutton is pressed */
    if(MyKey == SEL)
    {
      /* Set the Back Color */
      LCD_SetBackColor(White);
      /* Set the Text Color */
      LCD_SetTextColor(Red);
      /* Display new value */
      LCD_DisplayChar(Line8, ColBegin, (tmpValue + 0x30));
      /* Return the digit value and exit */
      return tmpValue;
    }
  } 
}

/*******************************************************************************
* Function Name  : Calendar_Init
* Description    : Initializes calendar application.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Calendar_Init(void)
{
  uint32_t i = 0, tmp = 0, pressedkey = 0;
  
  /* Initialize Date structure */
  date_s.month = 01;
  date_s.day = 01;
  date_s.year = 2010;
    
  if(BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
  {
    LCD_SetBackColor(Blue);
    LCD_SetTextColor(White);
    LCD_DisplayStringLine(Line6, "Time and Date Config");
    LCD_DisplayStringLine(Line7, "Select: Press SEL   ");
    LCD_DisplayStringLine(Line8, "Abort: Press Any Key");
    
    while(1)
    { 
      pressedkey = ReadKey();

      if(pressedkey == SEL)
      {
        /* Adjust Time */
        Time_PreAdjust();
        return;
      }
      else if (pressedkey != NOKEY)
      {
        /******************* Update this code block depending on requirement ******************/
        /* Configure RTC to its default value for the EnterAutoShutdownMode_RTCAlarm() method */
        /* RTC Configuration */
        RTC_Configuration();
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
    if(RTC_GetCounter() / 86399 != 0)
    {
      for(i = 0; i < (RTC_GetCounter() / 86399); i++)
      {
        Date_Update();
      }
      RTC_SetCounter(RTC_GetCounter() % 86399);
      BKP_WriteBackupRegister(BKP_DR2, date_s.year);
      tmp = date_s.month << 8;
      tmp |= date_s.day; 
      BKP_WriteBackupRegister(BKP_DR3, tmp);
      // LCD_DisplayStringLine(Line7, "       Days elapsed ");
      // tmp = i/100;
      // LCD_DisplayChar(Line7, Column3,(tmp + 0x30));
      // tmp = ((i%100)/10);
      // LCD_DisplayChar(Line7, Column4,(tmp + 0x30));
      // tmp = ((i%100)%10);
      // LCD_DisplayChar(Line7, Column5,(tmp + 0x30));
      // LCD_DisplayStringLine(Line8, "      Press SEL     ");
      // while(ReadKey() != SEL)
      // {
      // }
      // LCD_ClearLine(Line7);
      // LCD_ClearLine(Line8);
      // LCD_DisplayStringLine(Line8, "Push SEL to Continue");
      // Date_Display(date_s.year, date_s.month, date_s.day);
      // while(ReadKey() != SEL)
      // {
      // }
      BKP_WriteBackupRegister(BKP_DR4, daycolumn);
      BKP_WriteBackupRegister(BKP_DR5, dayline);
    }
  }
}

/*******************************************************************************
* Function Name  : Time_Regulate
* Description    : Returns the time entered by user, using menu vavigation keys.
* Input          : None
* Output         : None
* Return         : Current time RTC counter value
*******************************************************************************/
uint32_t Time_Regulate(void)
{
  uint32_t Tmp_HH = 0, Tmp_MM = 0, Tmp_SS = 0;

  /* Read time hours */
  Tmp_HH = ReadDigit(Column5, time_struct.hour_h, 0x2, 0x0);
  if(Tmp_HH == 2)
  {
    if(time_struct.hour_l > 3)
    {
      time_struct.hour_l = 0;
    }
    Tmp_HH = Tmp_HH*10 + ReadDigit(Column6, time_struct.hour_l, 0x3, 0x0);
  }
  else
  {
    Tmp_HH = Tmp_HH*10 + ReadDigit(Column6, time_struct.hour_l, 0x9, 0x0);
  }
  /* Read time  minutes */
  Tmp_MM = ReadDigit(Column8, time_struct.min_h,5, 0x0);
  Tmp_MM = Tmp_MM*10 + ReadDigit(Column9, time_struct.min_l, 0x9, 0x0);
  /* Read time seconds */
  Tmp_SS = ReadDigit(Column11, time_struct.sec_h,5, 0x0);
  Tmp_SS = Tmp_SS*10 + ReadDigit(Column12, time_struct.sec_l, 0x9, 0x0);

  /* Return the value to store in RTC counter register */
  return((Tmp_HH*3600 + Tmp_MM*60 + Tmp_SS));
}

/*******************************************************************************
* Function Name  : Time_PreAdjust
* Description    : Returns the time entered by user, using demoboard keys.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void Time_PreAdjust(void)
{
  LCD_Clear(White);

  // LCD_SetDisplayWindow(160, 223, 128, 128);
  // LCD_DrawBMP(0x0063FB00);
  // /* Disable LCD Window mode */
  // LCD_WindowModeDisable();

  if(BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
  {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    /* Allow access to BKP Domain */
    PWR_BackupAccessCmd(ENABLE);
   
    LCD_DisplayStringLine(Line7, "RTC Config PLZ Wait.");

    /* RTC Configuration */
    RTC_Configuration();
    LCD_DisplayStringLine(Line7, "       TIME         ");
    /* Clear Line8 */
    LCD_ClearLine(Line8);

    /* Display time separators ":" */
    LCD_DisplayChar(Line8, Column7, ':');
    LCD_DisplayChar(Line8, Column10, ':');

    /* Display the current time */
    Time_Display(RTC_GetCounter());

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();  
    /* Change the current time */
    RTC_SetCounter(Time_Regulate());

    BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);

    /* Clear Line7 */
    LCD_ClearLine(Line7);    
    /* Clear Line8 */
    LCD_ClearLine(Line8);    
    /* Adjust Date */
    Date_PreAdjust();    
  }
  else
  {  
    /* Clear Line8 */
    LCD_ClearLine(Line8);
    
    /* Display time separators ":" */
    LCD_DisplayChar(Line8, Column7, ':');
    LCD_DisplayChar(Line8, Column10, ':');

    /* Display the current time */
    Time_Display(RTC_GetCounter());

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();  
    /* Change the current time */
    RTC_SetCounter(Time_Regulate());  
  }
}

/*******************************************************************************
* Function Name  : Time_Adjust
* Description    : Returns the time entered by user, using demoboard keys.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Time_Adjust(void)
{
  /* Disable the JoyStick interrupts */
  IntExtOnOffConfig(DISABLE);
  /* PreAdjust Time */
  Time_PreAdjust();
  /* Clear the LCD */
  LCD_Clear(White);
  /* Display the menu */
  DisplayMenu();
  /* Enable the JoyStick interrupts */
  IntExtOnOffConfig(ENABLE);
}
/*******************************************************************************
* Function Name  : Time_Display
* Description    : Displays the current time.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Time_Display(uint32_t TimeVar)
{
  /* Display time hours */
  time_struct.hour_h=(uint8_t)( TimeVar / 3600)/10;
  LCD_DisplayChar(Line8, Column5,(time_struct.hour_h + 0x30));
  time_struct.hour_l=(uint8_t)(((TimeVar)/3600)%10);
  LCD_DisplayChar(Line8, Column6,(time_struct.hour_l + 0x30));

  /* Display time minutes */
  time_struct.min_h=(uint8_t)(((TimeVar)%3600)/60)/10;
  LCD_DisplayChar(Line8, Column8,(time_struct.min_h + 0x30));
  time_struct.min_l=(uint8_t)(((TimeVar)%3600)/60)%10;
  LCD_DisplayChar(Line8, Column9,(time_struct.min_l + 0x30));

  /* Display time seconds */
  time_struct.sec_h=(uint8_t)(((TimeVar)%3600)%60)/10;
  LCD_DisplayChar(Line8, Column11,(time_struct.sec_h + 0x30));
  time_struct.sec_l=(uint8_t)(((TimeVar)%3600)%60)%10;
  LCD_DisplayChar(Line8, Column12,(time_struct.sec_l + 0x30));
}

/*******************************************************************************
* Function Name  : Time_Show
* Description    : Shows the current time (HH/MM/SS) on LCD.
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
void Time_Show(void)
{
  uint32_t testvalue = 0, pressedkey = 0;
  
  /* Set the Back Color */
  LCD_SetBackColor(White);
  /* Set the Text Color */
  LCD_SetTextColor(Blue);
  /* Disable the JoyStick interrupts */
  IntExtOnOffConfig(DISABLE);

  LCD_Clear(White);

  // LCD_SetDisplayWindow(160, 223, 128, 128);
  // LCD_DrawBMP(0x0063FB00);
  // /* Disable LCD Window mode */
  // LCD_WindowModeDisable(); 

  if(BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
  {
    LCD_DisplayStringLine(Line6, "Time and Date Config");
    LCD_DisplayStringLine(Line7, "Select: Press SEL   ");
    LCD_DisplayStringLine(Line8, "Abort: Press Any Key");    
    
    while(testvalue == 0)
    {
      pressedkey = ReadKey();

      if(pressedkey == SEL)
      {
        /* Adjust Time */
        Time_PreAdjust();
        /* Clear the LCD */
        LCD_Clear(White);
        testvalue = 0x01;
      }
      else if (pressedkey != NOKEY)
      {
        /* Clear the LCD */
        LCD_ClearLine(Line8);
        /* Display the menu */
        DisplayMenu();
        /* Enable the JoyStick interrupts */
        IntExtOnOffConfig(ENABLE);
        return;
      }
    }
    /* Display time separators ":" on Line8 */
    LCD_DisplayChar(Line8, Column7, ':');
    LCD_DisplayChar(Line8, Column10, ':');
    /* Wait a press on any JoyStick pushbuttons */
    while(ReadKey() == NOKEY)
    {
      /* Display current time */
      Time_Display(RTC_GetCounter());
    }
  } 
  else
  {  
    while(ReadKey() != NOKEY)
    { 
    }
    /* Display time separators ":" on Line8 */
    LCD_DisplayChar(Line8, Column7, ':');
    LCD_DisplayChar(Line8, Column10, ':');
    /* Wait a press on any JoyStick pushbuttons */
    while(ReadKey() == NOKEY)
    {
      /* Display current time */
      Time_Display(RTC_GetCounter());
    }
  }
  /* Clear the LCD */
  LCD_ClearLine(Line8);
  /* Display the menu */
  DisplayMenu();
  /* Enable the JoyStick interrupts */
  IntExtOnOffConfig(ENABLE);
}

/*******************************************************************************
* Function Name  : Date_Regulate
* Description    : Sets the date entered by user, using menu navigation keys.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Date_Regulate(void)
{
  LCD_DisplayStringLine(Line8, " UP/DOWN: Set Year  ");
  /* Regulate year */
  RegulateYear();

  LCD_DisplayStringLine(Line8, " UP/DOWN: Set Month ");
  /* Regulate the month */
  RegulateMonth();
  
  LCD_DisplayStringLine(Line8, "Set Day- SEL To exit");
  /* Regulate day */
  RegulateDay();
}

/*******************************************************************************
* Function Name  : Date_PreAdjust
* Description    : Pre-Adjusts the current date (MM/DD/YYYY).
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void Date_PreAdjust(void)
{
  uint32_t tmp = 0, pressedkey = 0;
   
  /* Clear the LCD */
  LCD_Clear(White);
  LCD_SetBackColor(Blue);
  LCD_SetTextColor(White);

  if(BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
  {
    LCD_DisplayStringLine(Line6, "Time and Date Config");
    LCD_DisplayStringLine(Line7, "Select: Press SEL   ");
    LCD_DisplayStringLine(Line8, "Abort: Press Any Key");   
    
    while(1)
    {
      pressedkey = ReadKey(); 
      if(pressedkey == SEL)
      {
        /* Adjust Time */
        Time_PreAdjust();
        /* Clear the LCD */
        LCD_Clear(White);
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
    Date_Display(date_s.year, date_s.month, date_s.day);

    /* Change the current date */
    Date_Regulate();
    BKP_WriteBackupRegister(BKP_DR2, date_s.year);
    tmp = date_s.month << 8;
    tmp |= date_s.day; 
    BKP_WriteBackupRegister(BKP_DR3, tmp);
    BKP_WriteBackupRegister(BKP_DR4, daycolumn);
    BKP_WriteBackupRegister(BKP_DR5, dayline);
  }
}

/*******************************************************************************
* Function Name  : Date_Adjust
* Description    : Adjusts the current date (MM/DD/YYYY).
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Date_Adjust(void)
{
  /* Disable the JoyStick interrupts */
  IntExtOnOffConfig(DISABLE);
  /* Preadjust the date */
  Date_PreAdjust();
  /* Clear the LCD */
  LCD_Clear(White);
  /* Display the menu */
  DisplayMenu();
  /* Enable the JoyStick interrupts */
  IntExtOnOffConfig(ENABLE);
}

/*******************************************************************************
* Function Name  : Date_Display
* Description    : Displays the date in a graphic mode.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Date_Display(uint16_t nYear, uint8_t nMonth, uint8_t nDay)
{
  uint32_t mline = 0, mcolumn = 0, month = 0;
  uint32_t monthlength = 0;

  if(nMonth == 2)
  {
    if(IsLeapYear(nYear))
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

  /* Set the Back Color */
  LCD_SetBackColor(Blue2);
  /* Set the Text Color */
  LCD_SetTextColor(Yellow);
  
  LCD_DisplayStringLine(Line0, MonthNames[nMonth - 1]);

  LCD_DisplayChar(Line0, Column12, ((nYear/1000)+ 0x30));
  LCD_DisplayChar(Line0, Column13, (((nYear%1000)/100)+ 0x30));
  LCD_DisplayChar(Line0, Column14, ((((nYear%1000)%100)/10)+ 0x30));
  LCD_DisplayChar(Line0, Column15, ((((nYear%1000)%100)%10)+ 0x30));

  WeekDayNum(nYear, nMonth, nDay);
  /*
  LCD_SetTextColor(White);
  LCD_DisplayStringLine(Line1, " WEEK     DAY N:    ");
  if(wn/10)
  {
    LCD_DisplayChar(Line1, Column6, ((wn/10)+ 0x30));
    LCD_DisplayChar(Line1, Column7, ((wn%10)+ 0x30));
  }
  else
  {
    LCD_DisplayChar(Line1, Column6, ((wn%10)+ 0x30));
  }
  if(dc/100)
  {
    LCD_DisplayChar(Line1, Column17, ((dc/100)+ 0x30));
    LCD_DisplayChar(Line1, Column18, (((dc%100)/10)+ 0x30));
    LCD_DisplayChar(Line1, Column19, (((dc%100)%10)+ 0x30));    
  }
  else if(dc/10)
  {
    LCD_DisplayChar(Line1, Column17, ((dc/10)+ 0x30));
    LCD_DisplayChar(Line1, Column18, ((dc%10)+ 0x30));
  }
  else
  {
    LCD_DisplayChar(Line1, Column17, ((dc%10)+ 0x30));  
  }
  */
  /* Set the Back Color */
  LCD_SetBackColor(Red);
  LCD_SetTextColor(White);
  LCD_DisplayStringLine(Line1, "Mo Tu We Th Fr Sa Su");
  LCD_SetBackColor(White);
  LCD_SetTextColor(Blue2);
  
  /* Determines the week number, day of the week of the selected date */
  WeekDayNum(nYear, nMonth, 1);

  mline = Line2;
  mcolumn = Column0 + (3 * CHAR_WIDTH * dn);

  for(month = 1; month < monthlength; month++)
  {
    if(month == nDay)
    {
      daycolumn = mcolumn;
      dayline = mline;
    }
    if(month/10)
    {
      LCD_DisplayChar(mline, mcolumn, ((month/10)+ 0x30));
    }
    else
    {
      LCD_DisplayChar(mline, mcolumn, ' ');
    }
    mcolumn += CHAR_WIDTH;
    LCD_DisplayChar(mline, mcolumn, ((month%10)+ 0x30));

    if(mcolumn >= Column19)        
    {
      mcolumn = Column0;
      mline -= CHAR_HEIGHT;
    }
    else
    {
      mcolumn += CHAR_WIDTH;
      LCD_DisplayChar(mline, mcolumn, ' ');
      mcolumn += CHAR_WIDTH;
    }
  }
  LCD_SetTextColor(Red);   
  LCD_DrawRect(dayline, daycolumn, CHAR_HEIGHT, 2*CHAR_WIDTH);
}

/*******************************************************************************
* Function Name  : Date_Show
* Description    : Shows date in a graphic mode on LCD.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Date_Show(void)
{
  uint32_t tmpValue = 0;
  uint32_t MyKey = 0, ValueMax = 0;
  uint32_t firstdaycolumn = 0, lastdaycolumn = 0, lastdayline = 0;
  uint32_t testvalue = 0, pressedkey = 0;
  
  /* Disable the JoyStick interrupts */
  IntExtOnOffConfig(DISABLE);
  LCD_Clear(White);

  while(ReadKey()!= NOKEY)
  {
  }
  LCD_SetBackColor(Blue);
  LCD_SetTextColor(White);

  if(BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
  {
    LCD_DisplayStringLine(Line6, "Time and Date Config");
    LCD_DisplayStringLine(Line7, "Select: Press SEL   ");
    LCD_DisplayStringLine(Line8, "Abort: Press Any Key");
    
    while(testvalue == 0)
    {
      pressedkey = ReadKey(); 
      if(pressedkey == SEL)
      {
        /* Adjust Time */
        Time_PreAdjust();
        /* Clear the LCD */
        LCD_Clear(White);
        testvalue = 0x01;
      }
      else if (pressedkey != NOKEY)
      {
        /* Clear the LCD */
        LCD_Clear(White);
        /* Display the menu */
        DisplayMenu();
        /* Enable the JoyStick interrupts */
        IntExtOnOffConfig(ENABLE);
        return;
      }
    }
  }
  /* Clear the LCD */
  LCD_Clear(White);
  
  LCD_DisplayStringLine(Line8, " To Exit Press SEL  ");

  /* Display the current date */
  Date_Display(date_s.year, date_s.month, date_s.day);

  if(date_s.month == 2)
  {
    if(IsLeapYear(date_s.year))
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

  firstdaycolumn = Column0 + (3 * CHAR_WIDTH * dn);

  lastdaycolumn = ValueMax - (7 - dn);
  lastdayline = lastdaycolumn / 7;
  lastdaycolumn %= 7;

  if(lastdaycolumn == 0)
  {
    lastdayline = Line2 - (lastdayline * CHAR_HEIGHT);
    lastdaycolumn = Column18;  
  }
  else
  {
    lastdayline = Line3 - (lastdayline * CHAR_HEIGHT);
    lastdaycolumn = Column0 + (3 * CHAR_WIDTH * (lastdaycolumn - 1));
  }

  /* Initialize tmpValue */
  tmpValue = date_s.day;
  
  /* Endless loop */
  while(1)
  {
    /* Check which key is pressed */
    MyKey = ReadKey();

    /* If "RIGHT/DOWN" pushbutton is pressed */
    if (MyKey == DOWN)    //Since "RIGHT" key is not present
    {
      LCD_SetTextColor(White);   
      LCD_DrawRect(dayline, daycolumn, CHAR_HEIGHT, 2*CHAR_WIDTH);
      
      /* Increase the value of the digit */
      if(tmpValue == ValueMax)
      {
        tmpValue = 0;
        dayline = Line2;
        daycolumn = firstdaycolumn - (3 * CHAR_WIDTH);
      }
             
      if(daycolumn == Column18)
      {
        daycolumn = Column0 - (3 * CHAR_WIDTH);
        dayline -= CHAR_HEIGHT;
      }

      daycolumn += (3 * CHAR_WIDTH);
      LCD_SetTextColor(Red);   
      LCD_DrawRect(dayline, daycolumn, CHAR_HEIGHT, 2*CHAR_WIDTH);
      tmpValue++;
      WeekDayNum(date_s.year, date_s.month, tmpValue);
      /*
      LCD_SetBackColor(Blue2);
      LCD_SetTextColor(White);
      LCD_DisplayStringLine(Line1, " WEEK     DAY N:    ");
      if(wn/10)
      {
        LCD_DisplayChar(Line1, Column6, ((wn/10)+ 0x30));
        LCD_DisplayChar(Line1, Column7, ((wn%10)+ 0x30));
      }
      else
      {
        LCD_DisplayChar(Line1, Column6, ((wn%10)+ 0x30));
      }
      if(dc/100)
      {
        LCD_DisplayChar(Line1, Column17, ((dc/100)+ 0x30));
        LCD_DisplayChar(Line1, Column18, (((dc%100)/10)+ 0x30));
        LCD_DisplayChar(Line1, Column19, (((dc%100)%10)+ 0x30));
      }
      else if(dc/10)
      {
        LCD_DisplayChar(Line1, Column17, ((dc/10)+ 0x30));
        LCD_DisplayChar(Line1, Column18, ((dc%10)+ 0x30));
      }
      else
      {
        LCD_DisplayChar(Line1, Column17, ((dc%10)+ 0x30));
      }
      */
      LCD_SetBackColor(White);          
    }
    /* If "LEFT/UP" pushbutton is pressed */
    if (MyKey == UP)    //Since "LEFT" key is not present
    {
      LCD_SetTextColor(White);   
      LCD_DrawRect(dayline, daycolumn, CHAR_HEIGHT, 2*CHAR_WIDTH);
      
      /* Decrease the value of the digit */
      if(tmpValue == 1)
      {
        tmpValue = ValueMax + 1;
        dayline = lastdayline;
        daycolumn = lastdaycolumn + (3 * CHAR_WIDTH);        
      }
      
      if(daycolumn == Column0)
      {
        daycolumn = Column21;
        dayline += CHAR_HEIGHT;
      } 

      daycolumn -= (3 * CHAR_WIDTH);      
      LCD_SetTextColor(Red);   
      LCD_DrawRect(dayline, daycolumn, CHAR_HEIGHT, 2*CHAR_WIDTH);
      tmpValue--;
      WeekDayNum(date_s.year, date_s.month, tmpValue);
      /*
      LCD_SetBackColor(Blue2);
      LCD_SetTextColor(White);
      LCD_DisplayStringLine(Line1, " WEEK     DAY N:    ");
      if(wn/10)
      {
        LCD_DisplayChar(Line1, Column6, ((wn/10)+ 0x30));
        LCD_DisplayChar(Line1, Column7, ((wn%10)+ 0x30));
      }
      else
      {
        LCD_DisplayChar(Line1, Column6, ((wn%10)+ 0x30));
      }
      if(dc/100)
      {
        LCD_DisplayChar(Line1, Column17, ((dc/100)+ 0x30));
        LCD_DisplayChar(Line1, Column18, (((dc%100)/10)+ 0x30));
        LCD_DisplayChar(Line1, Column19, (((dc%100)%10)+ 0x30));
      }
      else if(dc/10)
      {
        LCD_DisplayChar(Line1, Column17, ((dc/10)+ 0x30));
        LCD_DisplayChar(Line1, Column18, ((dc%10)+ 0x30));
      }
      else
      {
        LCD_DisplayChar(Line1, Column17, ((dc%10)+ 0x30));
      }
      */
      LCD_SetBackColor(White);
    }
    // /* If "UP" pushbutton is pressed */
    // if(MyKey == UP)
    // {
      // LCD_SetTextColor(White);   
      // LCD_DrawRect(dayline, daycolumn, CHAR_HEIGHT, 2*CHAR_WIDTH);
      
      // if(tmpValue == 1)
      // {
        // dayline = lastdayline;
        // daycolumn =  lastdaycolumn;
        // tmpValue = ValueMax;
      // }
      // else if(tmpValue < 8)
      // {
        // tmpValue = 1;
        // dayline = Line2;
        // daycolumn = firstdaycolumn;
      // }
      // else
      // {
        // dayline += CHAR_HEIGHT;
        // tmpValue -= 7;
      // }

      // LCD_SetTextColor(Red);   
      // LCD_DrawRect(dayline, daycolumn, CHAR_HEIGHT, 2*CHAR_WIDTH);
      // WeekDayNum(date_s.year, date_s.month, tmpValue);
      // /*
      // LCD_SetBackColor(Blue2);
      // LCD_SetTextColor(White);
      // LCD_DisplayStringLine(Line1, " WEEK     DAY N:    ");
      // if(wn/10)
      // {
        // LCD_DisplayChar(Line1, Column6, ((wn/10)+ 0x30));
        // LCD_DisplayChar(Line1, Column7, ((wn%10)+ 0x30));
      // }
      // else
      // {
        // LCD_DisplayChar(Line1, Column6, ((wn%10)+ 0x30));
      // }
      // if(dc/100)
      // {
        // LCD_DisplayChar(Line1, Column17, ((dc/100)+ 0x30));
        // LCD_DisplayChar(Line1, Column18, (((dc%100)/10)+ 0x30));
        // LCD_DisplayChar(Line1, Column19, (((dc%100)%10)+ 0x30));
      // }
      // else if(dc/10)
      // {
        // LCD_DisplayChar(Line1, Column17, ((dc/10)+ 0x30));
        // LCD_DisplayChar(Line1, Column18, ((dc%10)+ 0x30));
      // }
      // else
      // {
        // LCD_DisplayChar(Line1, Column17, ((dc%10)+ 0x30));
      // }
      // */
      // LCD_SetBackColor(White);
    // } 
    // /* If "DOWN" pushbutton is pressed */
    // if(MyKey == DOWN)
    // {
      // LCD_SetTextColor(White);   
      // LCD_DrawRect(dayline, daycolumn, CHAR_HEIGHT, 2*CHAR_WIDTH);
      // if(tmpValue == ValueMax)
      // {
        // dayline = Line2;
        // daycolumn =  firstdaycolumn;
        // tmpValue = 1;
      // }
      // else
      // {
        // dayline -= CHAR_HEIGHT;
        // tmpValue += 7;
      // }

      // if(tmpValue > ValueMax)
      // {
        // tmpValue = ValueMax;
        // dayline = lastdayline;
        // daycolumn = lastdaycolumn;
      // }
      // LCD_SetTextColor(Red);   
      // LCD_DrawRect(dayline, daycolumn, CHAR_HEIGHT, 2*CHAR_WIDTH);
      // WeekDayNum(date_s.year, date_s.month, tmpValue);
      // /*
      // LCD_SetBackColor(Blue2);
      // LCD_SetTextColor(White);
      // LCD_DisplayStringLine(Line1, " WEEK     DAY N:    ");
      // if(wn/10)
      // {
        // LCD_DisplayChar(Line1, Column6, ((wn/10)+ 0x30));
        // LCD_DisplayChar(Line1, Column7, ((wn%10)+ 0x30));
      // }
      // else
      // {
        // LCD_DisplayChar(Line1, Column6, ((wn%10)+ 0x30));
      // }
      // if(dc/100)
      // {
        // LCD_DisplayChar(Line1, Column17, ((dc/100)+ 0x30));
        // LCD_DisplayChar(Line1, Column18, (((dc%100)/10)+ 0x30));
        // LCD_DisplayChar(Line1, Column19, (((dc%100)%10)+ 0x30));
      // }
      // else if(dc/10)
      // {
        // LCD_DisplayChar(Line1, Column17, ((dc/10)+ 0x30));
        // LCD_DisplayChar(Line1, Column18, ((dc%10)+ 0x30));
      // }
      // else
      // {
        // LCD_DisplayChar(Line1, Column17, ((dc%10)+ 0x30));
      // }
      // */
      // LCD_SetBackColor(White);
    // }
    /* If "SEL" pushbutton is pressed */
    if(MyKey == SEL)
    {
      /* Clear the LCD */
      LCD_Clear(White);
      /* Display the menu */
      DisplayMenu();
      /* Enable the JoyStick interrupts */
      IntExtOnOffConfig(ENABLE);
      return;
    }
  }
}

/*******************************************************************************
* Function Name  : Date_Update
* Description    : Updates date when time is 23:59:59.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Date_Update(void)
{
  if(date_s.month == 1 || date_s.month == 3 || date_s.month == 5 || date_s.month == 7 ||
     date_s.month == 8 || date_s.month == 10 || date_s.month == 12)
  {
    if(date_s.day < 31)
    {
      date_s.day++;
    }
    /* Date structure member: date_s.day = 31 */
    else
    {
      if(date_s.month != 12)
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
  else if(date_s.month == 4 || date_s.month == 6 || date_s.month == 9 ||
          date_s.month == 11)
  {
    if(date_s.day < 30)
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
  else if(date_s.month == 2)
  {
    if(date_s.day < 28)
    {
      date_s.day++;
    }
    else if(date_s.day == 28)
    {
      /* Leap year */
      if(IsLeapYear(date_s.year))
      {
        date_s.day++;
      }
      else
      {
        date_s.month++;
        date_s.day = 1;
      }
    }
    else if(date_s.day == 29)
    {
      date_s.month++;
      date_s.day = 1;
    }
  }
}

/*******************************************************************************
* Function Name  : RegulateYear
* Description    : Regulates the year.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void RegulateYear(void)
{
  uint32_t tmpValue = 0;
  uint32_t MyKey = 0;

  /* Initialize tmpValue */
  tmpValue = date_s.year;

  /* Endless loop */
  while(1)
  {
    /* Check which key is pressed */
    MyKey = ReadKey();

    /* If "UP" pushbutton is pressed */
    if(MyKey == UP)
    {
      /* Increase the value of the digit */
      if(tmpValue == 2099)
      {
        tmpValue = 1874;
      }
      LCD_ClearLine(Line2);
      LCD_ClearLine(Line7);
      LCD_ClearLine(Line8);
      Date_Display(++tmpValue, date_s.month, date_s.day);
    }
    /* If "DOWN" pushbutton is pressed */
    if(MyKey == DOWN)
    {
      /* Decrease the value of the digit */
      if(tmpValue == 1875)
      {
        tmpValue = 2100;
      }
      LCD_ClearLine(Line2);
      LCD_ClearLine(Line7);
      LCD_ClearLine(Line8);
      /* Display new value */
      Date_Display(--tmpValue, date_s.month, date_s.day);
    }
    /* If "SEL" pushbutton is pressed */
    if(MyKey == SEL)
    {
      LCD_ClearLine(Line2);
      LCD_ClearLine(Line7);
      LCD_ClearLine(Line8);
      /* Display new value */
      Date_Display(tmpValue, date_s.month, date_s.day);
      /* Return the digit value and exit */
      date_s.year = tmpValue;
      return;
    }
  }
}

/*******************************************************************************
* Function Name  : RegulateMonth
* Description    : Regulates month.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void RegulateMonth(void)
{
  uint32_t tmpValue = 0;
  uint32_t MyKey = 0;

  /* Initialize tmpValue */
  tmpValue = date_s.month;

  /* Endless loop */
  while(1)
  {
    /* Check which key is pressed */
    MyKey = ReadKey();

    /* If "UP" pushbutton is pressed */
    if(MyKey == UP)
    {
      /* Increase the value of the digit */
      if(tmpValue == 12)
      {
        tmpValue = 0;
      }
      LCD_ClearLine(Line2);
      LCD_ClearLine(Line7);
      LCD_ClearLine(Line8);
      Date_Display(date_s.year, ++tmpValue, date_s.day);
    }
    /* If "DOWN" pushbutton is pressed */
    if(MyKey == DOWN)
    {
      /* Decrease the value of the digit */
      if(tmpValue == 1)
      {
        tmpValue = 13;
      }
      LCD_ClearLine(Line2);
      LCD_ClearLine(Line7);
      LCD_ClearLine(Line8);     
      /* Display new value */
      Date_Display(date_s.year, --tmpValue, date_s.day);
    }
    /* If "SEL" pushbutton is pressed */
    if(MyKey == SEL)
    {
      LCD_ClearLine(Line2);
      LCD_ClearLine(Line7);
      LCD_ClearLine(Line8);     
      /* Display new value */
      Date_Display(date_s.year, tmpValue, date_s.day);
      /* Return the digit value and exit */
      date_s.month = tmpValue;
      return;
    }
  }
}

/*******************************************************************************
* Function Name  : RegulateDay
* Description    : Regulates day.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void RegulateDay(void)
{
  uint32_t tmpValue = 0;
  uint32_t MyKey = 0, ValueMax = 0;
  uint32_t firstdaycolumn = 0, lastdaycolumn = 0, lastdayline = 0;
  
  if(date_s.month == 2)
  {
    if(IsLeapYear(date_s.year))
      ValueMax = 29;
    else
      ValueMax = (MonLen[date_s.month - 1] - 1);
  }
  else
  {
    ValueMax = (MonLen[date_s.month - 1] - 1);
  }  

  firstdaycolumn = Column0 + (3 * CHAR_WIDTH * dn);

  lastdaycolumn = ValueMax - (7 - dn);
  lastdayline = lastdaycolumn / 7;
  lastdaycolumn %= 7;

  if(lastdaycolumn == 0)
  {
    lastdayline = Line2 - (lastdayline * CHAR_HEIGHT);
    lastdaycolumn = Column18;
  }
  else
  {
    lastdayline = Line3 - (lastdayline * CHAR_HEIGHT);
    lastdaycolumn = Column0 + (3 * CHAR_WIDTH * (lastdaycolumn - 1));
  }

  /* Initialize tmpValue */
  tmpValue = date_s.day;
  
  /* Endless loop */
  while(1)
  {
    /* Check which key is pressed */
    MyKey = ReadKey();

    /* If "RIGHT/DOWN" pushbutton is pressed */
    if (MyKey == DOWN)    //Since "RIGHT" key is not present
    {
      LCD_SetTextColor(White);   
      LCD_DrawRect(dayline, daycolumn, CHAR_HEIGHT, 2*CHAR_WIDTH);
      
      /* Increase the value of the digit */
      if(tmpValue == ValueMax)
      {
        tmpValue = 0;
        dayline = Line2;
        daycolumn = firstdaycolumn - (3 * CHAR_WIDTH);
      }
             
      if(daycolumn == Column18)
      {
        daycolumn = Column0 - (3 * CHAR_WIDTH);
        dayline -= CHAR_HEIGHT;
      }

      daycolumn += (3 * CHAR_WIDTH);
      LCD_SetTextColor(Red);   
      LCD_DrawRect(dayline, daycolumn, CHAR_HEIGHT, 2*CHAR_WIDTH);
      tmpValue++;   
    }
    /* If "LEFT/UP" pushbutton is pressed */
    if (MyKey == UP)    //Since "LEFT" key is not present
    {
      LCD_SetTextColor(White);   
      LCD_DrawRect(dayline, daycolumn, CHAR_HEIGHT, 2*CHAR_WIDTH);
      
      /* Decrease the value of the digit */
      if(tmpValue == 1)
      {
        tmpValue = ValueMax + 1;
        dayline = lastdayline;
        daycolumn = lastdaycolumn + (3 * CHAR_WIDTH);        
      }
      
      if(daycolumn == Column0)
      {
        daycolumn = Column21;
        dayline += CHAR_HEIGHT;
      } 

      daycolumn -= (3 * CHAR_WIDTH);      
      LCD_SetTextColor(Red);   
      LCD_DrawRect(dayline, daycolumn, CHAR_HEIGHT, 2*CHAR_WIDTH);
      tmpValue--;
    }
    // /* If "UP" pushbutton is pressed */
    // if(MyKey == UP)
    // {
      // LCD_SetTextColor(White);   
      // LCD_DrawRect(dayline, daycolumn, CHAR_HEIGHT, 2*CHAR_WIDTH);
         
      // if(tmpValue == 1)
      // {
        // dayline = lastdayline;
        // daycolumn =  lastdaycolumn;
        // tmpValue = ValueMax;
      // }
      // else if(tmpValue < 8)
      // {
        // tmpValue = 1;
        // dayline = Line2;
        // daycolumn = firstdaycolumn; 
      // }
      // else
      // {
        // dayline += CHAR_HEIGHT;
        // tmpValue -= 7;
      // }
      // LCD_SetTextColor(Red);   
      // LCD_DrawRect(dayline, daycolumn, CHAR_HEIGHT, 2*CHAR_WIDTH);    
    // } 
    // /* If "DOWN" pushbutton is pressed */
    // if(MyKey == DOWN)
    // {
      // LCD_SetTextColor(White);   
      // LCD_DrawRect(dayline, daycolumn, CHAR_HEIGHT, 2*CHAR_WIDTH);
      // if(tmpValue == ValueMax)
      // {
        // dayline = Line2;
        // daycolumn =  firstdaycolumn;
        // tmpValue = 1;
      // }
      // else
      // {
        // dayline -= CHAR_HEIGHT;
        // tmpValue += 7;
      // }
      // if(tmpValue > ValueMax)
      // {
        // tmpValue = ValueMax;
        // dayline = lastdayline;
        // daycolumn = lastdaycolumn;
      // }
      // LCD_SetTextColor(Red);   
      // LCD_DrawRect(dayline, daycolumn, CHAR_HEIGHT, 2*CHAR_WIDTH);     
    // }       
    /* If "SEL" pushbutton is pressed */
    if(MyKey == SEL)
    {
      /* Return the digit value and exit */
      date_s.day = tmpValue;
      return;
    }
  }
}

/*******************************************************************************
* Function Name  : WeekDayNum
* Description    : Determines the week number, the day number and the week day 
*                  number.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void WeekDayNum(uint32_t nyear, uint8_t nmonth, uint8_t nday)
{
  uint32_t a = 0, b = 0, c = 0, s = 0, e = 0, f = 0, g = 0, d = 0;
  int32_t n = 0;

  if(nmonth < 3)
  {
    a = nyear - 1;
  }
  else
  {
    a = nyear;
  }
  
  b = (a/4) - (a/100) + (a/400);
  c = ((a - 1)/4) - ((a - 1)/100) + ((a - 1)/400);
  s = b - c;

  if(nmonth < 3)
  {
    e = 0;
    f =  nday - 1 + 31 * (nmonth - 1);
  }
  else
  {
    e = s + 1;
    f = nday + (153*(nmonth - 3) + 2)/5 + 58 + s; 
  }

  g = (a + b) % 7;
  d = (f + g - e) % 7;
  n = f + 3 - d;

  if (n < 0)
  {
    wn = 53 - ((g - s)/5);
  }
  else if (n > (364 + s))
  {
    wn = 1;
  }
  else
  {
    wn = (n/7) + 1;
  }

  dn = d;
  dc = f + 1;
}

/*******************************************************************************
* Function Name  : IsLeapYear
* Description    : Check whether the passed year is Leap or not.
* Input          : None
* Output         : None
* Return         : 1: leap year
*                  0: not leap year
*******************************************************************************/
static uint8_t IsLeapYear(uint16_t nYear)
{
  if(nYear % 4 != 0) return 0;
  if(nYear % 100 != 0) return 1;
  return (uint8_t)(nYear % 400 == 0);
}

/*******************************************************************************
* Function Name  : Alarm_Regulate
* Description    : Returns the alarm time entered by user, using demoboard keys.
* Input          : None
* Output         : None
* Return         : Alarm time value to be loaded in RTC alarm register.
*******************************************************************************/
uint32_t Alarm_Regulate(void)
{
  uint32_t Alarm_HH = 0, Alarm_MM = 0, Alarm_SS = 0;

  /* Read alarm hours */
  Alarm_HH = ReadDigit(Column5, alarm_struct.hour_h, 0x2, 0x0);

  if(Alarm_HH == 0x2)
  {
    if(alarm_struct.hour_l > 0x3)
    {
      alarm_struct.hour_l = 0x0;
    }
    Alarm_HH = Alarm_HH*10 + ReadDigit(Column6, alarm_struct.hour_l, 0x3, 0x0);
  }
  else
  {
    Alarm_HH = Alarm_HH*10 + ReadDigit(Column6, alarm_struct.hour_l, 0x9, 0x0);
  }

  /* Read alarm minutes */
  Alarm_MM = ReadDigit(Column8, alarm_struct.min_h, 0x5, 0x0);
  Alarm_MM = Alarm_MM*10 + ReadDigit(Column9, alarm_struct.min_l, 0x9, 0x0);

  /* Read alarm seconds */
  Alarm_SS = ReadDigit(Column11, alarm_struct.sec_h, 0x5, 0x0);
  Alarm_SS = Alarm_SS*10 + ReadDigit(Column12, alarm_struct.sec_l, 0x9, 0x0);
  
  /* Return the alarm value to store in the RTC Alarm register */
  return((Alarm_HH*3600 + Alarm_MM*60 + Alarm_SS));
}

/*******************************************************************************
* Function Name  : Alarm_PreAdjust
* Description    : Configures an alarm event to occurs within the current day.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Alarm_PreAdjust(void)
{
  uint32_t tmp = 0;

  /* Set the LCD Back Color */
  LCD_SetBackColor(Blue);

  /* Set the LCD Text Color */
  LCD_SetTextColor(White);

  if(BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
  {
    LCD_DisplayStringLine(Line7, "Time not configured ");
    LCD_DisplayStringLine(Line8, "     Press SEL      ");
    while(ReadKey() == NOKEY)
    {
    }
    return;
  }
  /* Read the alarm value stored in the Backup register */
  tmp = BKP_ReadBackupRegister(BKP_DR6);
  tmp |= BKP_ReadBackupRegister(BKP_DR7) << 16;
  
  LCD_DisplayStringLine(Line7, "       ALARM        ");
  /* Clear Line8 */
  LCD_ClearLine(Line8);
    
  /* Display time separators ":" on */
  LCD_DisplayChar(Line8, Column7, ':');
  LCD_DisplayChar(Line8, Column10, ':');

  /* Display the alarm value */
  Alarm_Display(tmp);
  /* Store new alarm value */
  tmp = Alarm_Regulate();

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

/*******************************************************************************
* Function Name  : Alarm_Adjust
* Description    : Adjusts an alarm event to occurs within the current day.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Alarm_Adjust(void)
{
  /* Disable the JoyStick interrupts */
  IntExtOnOffConfig(DISABLE);
  
  LCD_Clear(White);
  
  // LCD_SetDisplayWindow(160, 223, 128, 128);
  // LCD_DrawBMP(0x00637A00);
  // /* Disable LCD Window mode */
  // LCD_WindowModeDisable(); 
  
  Alarm_PreAdjust();

  /* Clear the LCD */
  LCD_Clear(White);
  /* Display the menu */
  DisplayMenu();
  /* Enable the JoyStick interrupts */
  IntExtOnOffConfig(ENABLE);  
}

/*******************************************************************************
* Function Name  : Alarm_Display
* Description    : Displays alarm time.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Alarm_Display(uint32_t AlarmVar)
{
  /* Display alarm hours */
  alarm_struct.hour_h=(uint8_t)( AlarmVar / 3600)/10;
  LCD_DisplayChar(Line8, Column5,(alarm_struct.hour_h + 0x30));

  alarm_struct.hour_l=(uint8_t)(((AlarmVar)/3600)%10);
  LCD_DisplayChar(Line8, Column6,(alarm_struct.hour_l + 0x30));

  /* Display alarm minutes */
  alarm_struct.min_h=(uint8_t)(((AlarmVar)%3600)/60)/10;
  LCD_DisplayChar(Line8, Column8,(alarm_struct.min_h + 0x30));

  alarm_struct.min_l=(uint8_t)(((AlarmVar)%3600)/60)%10;
  LCD_DisplayChar(Line8, Column9,(alarm_struct.min_l + 0x30));

  /* Display alarm seconds */
  alarm_struct.sec_h=(uint8_t)(((AlarmVar)%3600)%60)/10;
  LCD_DisplayChar(Line8, Column11,(alarm_struct.sec_h + 0x30));

  alarm_struct.sec_l=(uint8_t)(((AlarmVar)%3600)%60)%10;
  LCD_DisplayChar(Line8, Column12,(alarm_struct.sec_l + 0x30));
}

/*******************************************************************************
* Function Name  : Alarm_Show
* Description    : Shows alarm time (HH/MM/SS) on LCD.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Alarm_Show(void)
{
  uint32_t tmp = 0;
  
  /* Disable the JoyStick interrupts */
  IntExtOnOffConfig(DISABLE);

  LCD_Clear(White);
 
  // LCD_SetDisplayWindow(160, 223, 128, 128);
  // LCD_DrawBMP(0x00637A00);
  // /* Disable LCD Window mode */
  // LCD_WindowModeDisable(); 
  
  /* Set the LCD Back Color */
  LCD_SetBackColor(Blue);

  /* Set the LCD Text Color */
  LCD_SetTextColor(White);

  if(BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
  {
    LCD_DisplayStringLine(Line7, "Time not configured ");
    LCD_DisplayStringLine(Line8, "     Press SEL      ");
    while(ReadKey() == NOKEY)
    {
    }
    /* Clear the LCD */
    LCD_Clear(White);
    /* Display the menu */
    DisplayMenu();
    /* Enable the JoyStick interrupts */
    IntExtOnOffConfig(ENABLE);
    return;
  }
  
  /* Read the alarm value stored in the Backup register */
  tmp = BKP_ReadBackupRegister(BKP_DR6);
  tmp |= BKP_ReadBackupRegister(BKP_DR7) << 16;
  
  LCD_DisplayStringLine(Line7, "       ALARM        ");
  LCD_ClearLine(Line8);

  /* Display alarm separators ":" */
  LCD_DisplayChar(Line8, Column7, ':');
  LCD_DisplayChar(Line8, Column10, ':');

  /* Display actual alarm value */
  Alarm_Display(tmp);

  /* Wait a press on pushbuttons */
  while(ReadKey() != NOKEY)
  {
  }

  /* Wait a press on pushbuttons */
  while(ReadKey() == NOKEY)
  {
  }
  /* Clear the LCD */
  LCD_Clear(White);
  /* Display the menu */
  DisplayMenu();
  /* Enable the JoyStick interrupts */
  IntExtOnOffConfig(ENABLE);
}

/*******************************************************************************
* Function Name  : RTC_Configuration
* Description    : Configures the RTC.
* Input          : None.
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_Configuration(void)
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
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
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

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
