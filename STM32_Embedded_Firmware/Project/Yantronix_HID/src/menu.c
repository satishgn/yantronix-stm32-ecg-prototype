/******************** (C) COPYRIGHT 2009 STMicroelectronics ********************
* File Name          : menu.c
* Author             : MCD Application Team
* Version            : V2.0.0
* Date               : 04/27/2009
* Description        : This file includes the menu navigation driver for the
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
typedef void (* tMenuFunc)(void);
typedef struct sMenuItem * tMenuItem;
typedef struct sMenu * tMenu;

/* Private define ------------------------------------------------------------*/
#define SHOW_LOGO_IMAGE;
#define SHOW_MENU_ICONS;

#define NUMBER_OF_ICONS     9

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t MenuItemIndex = 0, nMenuLevel = 0;
uint8_t ItemNumb[MAX_MENU_LEVELS];

tMenuItem psMenuItem, psCurrentMenuItem;
tMenu psPrevMenu[MAX_MENU_LEVELS];
tMenu psCurrentMenu;

struct sMenuItem
{
  uint8_t* pszTitle;
  tMenuFunc pfMenuFunc;
  tMenuFunc pfUpDownMenuFunc;
  tMenu psSubMenu;
};

struct sMenu
{
  uint8_t* pszTitle;
  tMenuItem psItems;
  uint8_t nItems;
};

#ifdef SHOW_LOGO_IMAGE
const u16 Logo_Image[] = { 
    #include "YantronixBlack_128x22.h"
    //#include "YantronixWhite_128x22.h"
};
#endif /* SHOW_LOGO_IMAGE */

#ifdef SHOW_MENU_ICONS
const uint16_t Icons_Image[NUMBER_OF_ICONS][1024] = {
    #include "Calendar_32x32.h"
    #include "Oscilloscope_32x32.h"
    #include "ECGMonitor_32x32.h"
    #include "DFU_32x32.h"
    #include "USBMassStorage_32x32.h"
    #include "USBVirtualCom_32x32.h"
    // #include "Help_32x32.h"
    #include "About_32x32.h"
    #include "LowPowerMode_32x32.h"
    #include "Shutdown_32x32.h"
};
#endif /* SHOW_MENU_ICONS */

/*------------------------------ Menu level 4 -------------------------------*/
/*---------------------------------------------------------------------------*/

/*------------------------------ Menu level 3 -------------------------------*/
struct sMenuItem STOPMenuItems[] = {
                                    {"    Exit: EXTI      ", EnterSTOPMode_EXTI, IdleFunc},
                                    {"  Exit: RTC Alarm   ", EnterSTOPMode_RTCAlarm, IdleFunc},
                                    {"       Return       ", ReturnFunc, IdleFunc}
                                   };
struct sMenu STOPMenu = {"     STOP Mode      ", STOPMenuItems, countof(STOPMenuItems)};

struct sMenuItem STANDBYMenuItems[] = {
                                       {"  Exit: Wakeup Pin  ", EnterSTANDBYMode_WAKEUP, IdleFunc},
                                       {"  Exit: RTC Alarm   ", EnterSTANDBYMode_RTCAlarm, IdleFunc},
                                       {"       Return       ", ReturnFunc, IdleFunc}
                                      };
struct sMenu STANDBYMenu = {"   STANDBY Mode     ", STANDBYMenuItems, countof(STANDBYMenuItems)};

struct sMenuItem SettingTimeMenuItems[] = {
                                           {"       Adjust       ", Time_Adjust, IdleFunc},
                                           {"        Show        ", Time_Show, IdleFunc},
                                           {"       Return       ", ReturnFunc, IdleFunc}
                                          };
struct sMenu TimeMenu = {"        Time        ", SettingTimeMenuItems, countof(SettingTimeMenuItems)};

struct sMenuItem SettingDateMenuItems[] = {
                                           {"       Adjust       ", Date_Adjust, IdleFunc},
                                           {"        Show        ", Date_Show, IdleFunc},
                                           {"       Return       ", ReturnFunc, IdleFunc}
                                          };
struct sMenu DateMenu = {"        Date        ", SettingDateMenuItems, countof(SettingDateMenuItems)};

struct sMenuItem SettingAlarmMenuItems[] = {
                                            {"       Adjust       ", Alarm_Adjust, IdleFunc},
                                            {"        Show        ", Alarm_Show, IdleFunc},
                                            {"       Return       ", ReturnFunc, IdleFunc}
                                           };
struct sMenu AlarmMenu = {"        Alarm       ", SettingAlarmMenuItems, countof(SettingAlarmMenuItems)};
/*---------------------------------------------------------------------------*/

/*------------------------------ Menu level 2 -------------------------------*/
struct sMenuItem AboutMenuItems[] = {
                                     {"       About        ", AboutFunc, IdleFunc},
                                     {"       Return       ", ReturnFunc, IdleFunc}
                                    };
struct sMenu AboutMenu = {"        About       ", AboutMenuItems, countof(AboutMenuItems)};

// struct sMenuItem HelpMenuItems[] = {
                                    // {"       Start        ", HelpFunc, IdleFunc},
                                    // {"       Return       ", ReturnFunc, IdleFunc}
                                      // };
// struct sMenu HelpMenu = {"        Help        ", HelpMenuItems, countof(HelpMenuItems)};

struct sMenuItem USBVirtualComMenuItems[] = {
#ifdef USE_USB_VCP
                                             {"       Start        ", VCP_Start, IdleFunc},
#endif                                 
                                             {"       Return       ", ReturnFunc, IdleFunc}
                                            };
struct sMenu USBVirtualComMenu = {"  USB Virtual Com   ", USBVirtualComMenuItems, countof(USBVirtualComMenuItems)};

struct sMenuItem USBMassStorageMenuItems[] = {
#ifdef USE_USB_MSD
                                              {"       Start        ", MSD_Start, IdleFunc},
#endif                                   
                                              {"       Return       ", ReturnFunc, IdleFunc}
                                             };
struct sMenu USBMassStorageMenu = {"  USB Mass Storage  ", USBMassStorageMenuItems, countof(USBMassStorageMenuItems)};

struct sMenuItem DFUMenuItems[] = {
#ifdef USE_USB_DFU
                                   {"       Start        ", DFU_Start, IdleFunc},
#endif
                                   {"       Return       ", ReturnFunc, IdleFunc}
                                  };
struct sMenu DFUMenu = {"  Firmware Upgrade  ", DFUMenuItems, countof(DFUMenuItems)};

struct sMenuItem LowPowerMenuItems[] = {
                                        {"   Battery Monitor  ", Start_Battery_Monitor, IdleFunc},
                                        {"        STOP        ", IdleFunc, IdleFunc, &STOPMenu},
                                        {"       STANDBY      ", IdleFunc, IdleFunc, &STANDBYMenu},
                                        {"       Return       ", ReturnFunc, IdleFunc}
                                       };
struct sMenu LowPowerMenu = {"    Low Power Mode  ", LowPowerMenuItems, countof(LowPowerMenuItems)};

struct sMenuItem ECGMonitorMenuItems[] = {
                                          {"  Acquire & Log ECG ", Start_ECG_Data_Acquisition, IdleFunc},
                                          {"  Play ECG Log File ", Browse_Log_Files, IdleFunc},
                                          {"Delete All Log Files", Delete_All_Log_Files, IdleFunc},
                                          {"       Return       ", ReturnFunc, IdleFunc}
                                         };
struct sMenu ECGMonitorMenu = {"    ECG Monitor     ", ECGMonitorMenuItems, countof(ECGMonitorMenuItems)};

struct sMenuItem SignalScopeMenuItems[] = {
#ifdef USE_DAC_SIGNALGEN   
                                           {"Acquire & Log Signal", Start_SIGNALGEN_Data_Acquisition, IdleFunc},
#endif
                                           {"Play Signal Log File", Browse_Log_Files, IdleFunc},
                                           {"Delete All Log Files", Delete_All_Log_Files, IdleFunc},
                                           {"       Return       ", ReturnFunc, IdleFunc}
                                          };
struct sMenu SignalScopeMenu = {"    Signal Scope    ", SignalScopeMenuItems, countof(SignalScopeMenuItems)};

struct sMenuItem CalendarMenuItems[] = {
                                        {"        Time        ", IdleFunc, IdleFunc, &TimeMenu},
                                        {"        Date        ", IdleFunc, IdleFunc, &DateMenu},
                                        {"        Alarm       ", IdleFunc, IdleFunc, &AlarmMenu},
                                        {"        Return      ", ReturnFunc, IdleFunc}
                                       };
struct sMenu CalendarMenu = {"       Calendar     ", CalendarMenuItems, countof(CalendarMenuItems)};
/*---------------------------------------------------------------------------*/

/*------------------------------ Menu level 1 -------------------------------*/
struct sMenuItem MainMenuItems[] = {
  {"      Calendar      ", IdleFunc, IdleFunc, &CalendarMenu},
  {"    Signal Scope    ", IdleFunc, IdleFunc, &SignalScopeMenu},
  {"    ECG Monitor     ", IdleFunc, IdleFunc, &ECGMonitorMenu},
  {"  Firmware Upgrade  ", IdleFunc, IdleFunc, &DFUMenu},
  {"  USB Mass Storage  ", IdleFunc, IdleFunc, &USBMassStorageMenu},
  {"  USB Virtual Com   ", IdleFunc, IdleFunc, &USBVirtualComMenu},
  // {"        Help        ", IdleFunc, IdleFunc, &HelpMenu},
  {"        About       ", IdleFunc, IdleFunc, &AboutMenu},
  {"   Low Power Mode   ", IdleFunc, IdleFunc, &LowPowerMenu},
  {"      Shutdown      ", Shutdown_System, IdleFunc}
};
struct sMenu MainMenu = {"      Main menu     ", MainMenuItems, countof(MainMenuItems)};
/*---------------------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static uint8_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);

/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : Menu_Init
* Description    : Initializes the navigation menu.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Menu_Init(void)
{
  psCurrentMenu = &MainMenu;
  psPrevMenu[nMenuLevel] = psCurrentMenu;
  psMenuItem = MainMenuItems;
}

/*******************************************************************************
* Function Name  : DisplayMenu
* Description    : Displays the current menu.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DisplayMenu(void)
{
  uint32_t Line = Line0, index = 0;
  tMenuItem psMenuItem2;

  /* Set the Back Color */
  LCD_SetBackColor(Blue);
  /* Set the Text Color */
  LCD_SetTextColor(White);

  /* Clear the LCD Screen */
  LCD_Clear(White);

  LCD_DisplayStringLine(Line, psCurrentMenu->pszTitle);
  Line -= CHAR_HEIGHT;

  /* Set the Back Color */
  LCD_SetBackColor(White);

  /* Set the Text Color */
  LCD_SetTextColor(Blue);

  while(!(index >= (psCurrentMenu->nItems)))
  {
    psMenuItem2 = &(psCurrentMenu->psItems[index]);
    LCD_DisplayStringLine(Line, psMenuItem2->pszTitle);
    index++;
    Line -= CHAR_HEIGHT;  
  }
  /* Set the Back Color */
  LCD_SetBackColor(Green);

  /* Set the Text Color */
  LCD_SetTextColor(White);

  /* Get the current menu */
  psMenuItem = &(psCurrentMenu->psItems[MenuItemIndex]);

  LCD_DisplayStringLine((SCREEN_HEIGHT - CHAR_HEIGHT - ((MenuItemIndex + 1) * CHAR_HEIGHT)), psMenuItem->pszTitle);
}

/*******************************************************************************
* Function Name  : SelFunc
* Description    : This function is executed when "SEL" push-buttton is pressed.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SelFunc(void)
{
  Delay(20);//Delay 200 msec

  psCurrentMenuItem = psMenuItem;

  if(psMenuItem->psSubMenu != '\0')
  {
    /* Update the current Item by the submenu */
    MenuItemIndex = 0;
    psCurrentMenu = psMenuItem->psSubMenu;
    psMenuItem = &(psCurrentMenu->psItems)[MenuItemIndex];
    DisplayMenu();
    nMenuLevel++;
    psPrevMenu[nMenuLevel] = psCurrentMenu;
  } 
  psCurrentMenuItem->pfMenuFunc();
}

/*******************************************************************************
* Function Name  : UpFunc
* Description    : This function is executed when any of "UP" push-butttons
*                  is pressed.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UpFunc(void)
{
  Delay(20);//Delay 200 msec

  /* Set the Back Color */
  LCD_SetBackColor(White);
  /* Set the Text Color */
  LCD_SetTextColor(Blue);
  psMenuItem = &psCurrentMenu->psItems[MenuItemIndex];
  LCD_DisplayStringLine((SCREEN_HEIGHT - CHAR_HEIGHT - ((MenuItemIndex + 1) * CHAR_HEIGHT)), psMenuItem->pszTitle);

  if(MenuItemIndex > 0)
  {
    MenuItemIndex--;
  }
  else
  {
    MenuItemIndex = psCurrentMenu->nItems - 1;
  }
  /* Set the Back Color */
  LCD_SetBackColor(Green);
  /* Set the Text Color */
  LCD_SetTextColor(White);
  psMenuItem = &psCurrentMenu->psItems[MenuItemIndex];
  LCD_DisplayStringLine((SCREEN_HEIGHT - CHAR_HEIGHT - ((MenuItemIndex + 1) * CHAR_HEIGHT)), psMenuItem->pszTitle);
  ItemNumb[nMenuLevel] = MenuItemIndex;
}

/*******************************************************************************
* Function Name  : DownFunc
* Description    : This function is executed when any of "Down" push-butttons
*                  is pressed.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DownFunc(void)
{
  Delay(20);//Delay 200 msec

  /* Set the Back Color */
  LCD_SetBackColor(White);
  /* Set the Text Color */
  LCD_SetTextColor(Blue);
  psMenuItem = &psCurrentMenu->psItems[MenuItemIndex];
  LCD_DisplayStringLine((SCREEN_HEIGHT - CHAR_HEIGHT - ((MenuItemIndex + 1) * CHAR_HEIGHT)), psMenuItem->pszTitle);
      
  /* Test on the MenuItemIndex value before incrementing it */
  if(MenuItemIndex >= ((psCurrentMenu->nItems)-1))
  {
    MenuItemIndex = 0;
  }
  else
  {
    MenuItemIndex++;
  }
  /* Set the Back Color */
  LCD_SetBackColor(Green);
  /* Set the Text Color */
  LCD_SetTextColor(White);
  /* Get the current menu */
  psMenuItem = &(psCurrentMenu->psItems[MenuItemIndex]);
  LCD_DisplayStringLine((SCREEN_HEIGHT - CHAR_HEIGHT - ((MenuItemIndex + 1) * CHAR_HEIGHT)), psMenuItem->pszTitle);
  ItemNumb[nMenuLevel] = MenuItemIndex;
}

/*******************************************************************************
* Function Name  : ReturnFunc
* Description    : This function is executed when the "RETURN" menu is selected.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ReturnFunc(void)
{
  psMenuItem->pfUpDownMenuFunc();

  if(nMenuLevel == 0)
  {
    nMenuLevel++;
  }

  psCurrentMenu = psPrevMenu[nMenuLevel-1];
  psMenuItem = &psCurrentMenu->psItems[0];
  ItemNumb[nMenuLevel] = 0;
  MenuItemIndex = 0;
  nMenuLevel--;
  if(nMenuLevel != 0)
  {
    DisplayMenu();
  }
  else
  {
    ShowMenuIcons();
  }
}

/**
  * @brief  This function is executed when the MENU key is pressed.
  * @param  None
  * @retval None
  */
void ReturnMainMenu(void)
{
    while(nMenuLevel != 0)
    {
        psMenuItem->pfUpDownMenuFunc();
        if (nMenuLevel == 0)
        {
            nMenuLevel++;
        }
        psCurrentMenu = psPrevMenu[nMenuLevel-1];
        psMenuItem = &psCurrentMenu->psItems[0];
        ItemNumb[nMenuLevel] = 0;
        MenuItemIndex = 0;
        nMenuLevel--;
    }
    if (nMenuLevel == 0)
        ShowMenuIcons();
}

/*******************************************************************************
* Function Name  : ReadKey
* Description    : Reads key from demoboard.
* Input          : None
* Output         : None
* Return         : Return RIGHT, LEFT, SEL, UP, DOWN or NOKEY
*******************************************************************************/
uint8_t ReadKey(void)
{
  Delay(10);//Delay 100 msec

  /* "menu" key is pressed */
  if(GPIO_ReadInputDataBit(GPIO_MENU, GPIO_Pin_MENU))
  {
    while(GPIO_ReadInputDataBit(GPIO_MENU, GPIO_Pin_MENU) != Bit_RESET);
    return MENU; 
  }
  /* "sel" key is pressed */
  if(GPIO_ReadInputDataBit(GPIO_SEL, GPIO_Pin_SEL))
  {
    while(GPIO_ReadInputDataBit(GPIO_SEL, GPIO_Pin_SEL) != Bit_RESET);
    return SEL; 
  }
  /* "up" key is pressed */
  if(GPIO_ReadInputDataBit(GPIO_UP, GPIO_Pin_UP))
  {
    while(GPIO_ReadInputDataBit(GPIO_UP, GPIO_Pin_UP) != Bit_RESET);
    return UP; 
  }
  /* "down" key is pressed */
  if(GPIO_ReadInputDataBit(GPIO_DOWN, GPIO_Pin_DOWN))
  {
    while(GPIO_ReadInputDataBit(GPIO_DOWN, GPIO_Pin_DOWN) != Bit_RESET);
    return DOWN; 
  }
  /* No key is pressed */
  else 
  {
    return NOKEY;
  }
}

/*******************************************************************************
* Function Name  : IdleFunc
* Description    : Idle function.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void IdleFunc(void)
{
  /* Nothing to execute: return */
  return;
}

/*******************************************************************************
* Function Name  : DisplayIcons
* Description    : Display menu icons.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DisplayIcons(void)
{
#ifdef SHOW_MENU_ICONS
  uint32_t i = 0, j = 0, l = 0,  iconline = 0, iconcolumn = 0;
  
  iconline = 77;
  iconcolumn = 16;
    
  for(i = 0; i < 3; i++)
  {
    for(j = 0; j < 3; j++)
    {
      // LCD_DrawRect(iconline, iconcolumn, 32, 32);
      DRAW_SetImage(Icons_Image[l], iconcolumn, iconline, 32, 32); /* 32x32 Image Size*/
      iconcolumn += 48;
      l++;
      if(l == NUMBER_OF_ICONS)
      {
        return;
      }
    }
    iconline -= 36;
    iconcolumn = 16;
  }
#endif /* SHOW_MENU_ICONS */
}

/*******************************************************************************
* Function Name  : ShowMenuIcons
* Description    : Show the main menu icon.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ShowMenuIcons(void)
{
#ifdef SHOW_MENU_ICONS
  uint32_t MyKey = 0, i = 0;  
  uint16_t IconRect[NUMBER_OF_ICONS][2] ={{75, 8}, {75, 56}, {75, 104}, 
                       {39, 8}, {39, 56}, {39, 104},
                       {3, 8}, {3, 56}, {3, 104}};

  /* Disable the JoyStick interrupts */
  IntExtOnOffConfig(DISABLE);
                   
  /* Initializes the Menu state machine */
  Menu_Init();

  MenuItemIndex = 0;
  
  /* Clear*/
  LCD_Clear(White);

  /* Set the Back Color */
  LCD_SetBackColor(Blue);

  /* Set the Text Color */
  LCD_SetTextColor(White);

  LCD_DisplayStringLine(Line0, psMenuItem->pszTitle);
  
  /* Set the Back Color */
  LCD_SetBackColor(White);

  /* Set the Text Color */
  LCD_SetTextColor(Blue);

  /* Displays Icons */    
  DisplayIcons();

  LCD_DrawRect(IconRect[0][0], IconRect[0][1], 36, 48);

  EnterAutoShutdownMode_RTCAlarm(180);  //3 minutes for auto shutdown
  
  /* Endless loop */
  while(1)
  {
    /* Check which key is pressed */
    MyKey = ReadKey();

    // /* If "UP" pushbutton is pressed */
    // if(MyKey == UP)
    // {
      // /* Set the Text Color */
      // LCD_SetTextColor(White);
      // LCD_DrawRect(IconRect[i][0], IconRect[i][1], 36, 48);
      // if(i <= 2)
      // {
        // i += 6;
        // if(i >= NUMBER_OF_ICONS)
        // {
          // i = (NUMBER_OF_ICONS - 1);
        // }
      // }
      // else
      // {
        // i -= 3;
      // }
      // /* Set the Text Color */
      // LCD_SetTextColor(Blue);
      // LCD_DrawRect(IconRect[i][0], IconRect[i][1], 36, 48);

      // /* Set the Back Color */
      // LCD_SetBackColor(Blue);
      // /* Set the Text Color */
      // LCD_SetTextColor(White);
      // /* Test on the MenuItemIndex value before incrementing it */
      // if(MenuItemIndex <= 2)
      // {
        // MenuItemIndex += 6;
        // if(MenuItemIndex >= NUMBER_OF_ICONS)
        // {
          // MenuItemIndex = (NUMBER_OF_ICONS - 1);
        // }
      // }
      // else
      // {
        // MenuItemIndex -= 3;
      // }
      // /* Get the current menu */
      // psMenuItem = &(psCurrentMenu->psItems[MenuItemIndex]);
      // LCD_DisplayStringLine(Line0, psMenuItem->pszTitle);
      // ItemNumb[nMenuLevel] = MenuItemIndex;
    // }
    // /* If "DOWN" pushbutton is pressed */
    // if(MyKey == DOWN)
    // {
      // /* Set the Text Color */
      // LCD_SetTextColor(White);
      // LCD_DrawRect(IconRect[i][0], IconRect[i][1], 36, 48);
      // if(i >= 6)
      // {
        // i -= 6;
      // }
      // else
      // {
        // i += 3;
        // if(i >= NUMBER_OF_ICONS)
        // {
          // i = (NUMBER_OF_ICONS - 1);
        // }
      // }
      // /* Set the Text Color */
      // LCD_SetTextColor(Blue);
      // LCD_DrawRect(IconRect[i][0], IconRect[i][1], 36, 48);

      // /* Set the Back Color */
      // LCD_SetBackColor(Blue);
      // /* Set the Text Color */
      // LCD_SetTextColor(White);
      // /* Test on the MenuItemIndex value before incrementing it */
      // if(MenuItemIndex >= 6)
      // {
        // MenuItemIndex -= 6;
      // }
      // else
      // {
        // MenuItemIndex += 3;
        // if(MenuItemIndex >= NUMBER_OF_ICONS)
        // {
          // MenuItemIndex = (NUMBER_OF_ICONS - 1);
        // }
      // }
      // /* Get the current menu */
      // psMenuItem = &(psCurrentMenu->psItems[MenuItemIndex]);
      // LCD_DisplayStringLine(Line0, psMenuItem->pszTitle);
      // ItemNumb[nMenuLevel] = MenuItemIndex;
    // }
    /* If "LEFT/UP" pushbutton is pressed */
    if (MyKey == UP)    //Since "LEFT" key is not present
    {
      /* Set the Text Color */
      LCD_SetTextColor(White);
      LCD_DrawRect(IconRect[i][0], IconRect[i][1], 36, 48);
      if(i == 0)
      {
        i = (NUMBER_OF_ICONS - 1);
      }
      else
      {
        i--;
      }
      /* Set the Text Color */
      LCD_SetTextColor(Blue);
      LCD_DrawRect(IconRect[i][0], IconRect[i][1], 36, 48);

      /* Set the Back Color */
      LCD_SetBackColor(Blue);
      /* Set the Text Color */
      LCD_SetTextColor(White);
      if(MenuItemIndex > 0)
      {
        MenuItemIndex--;
      }
      else
      {
        MenuItemIndex = psCurrentMenu->nItems - 1;
      }

      psMenuItem = &psCurrentMenu->psItems[MenuItemIndex];
      LCD_DisplayStringLine(Line0, psMenuItem->pszTitle);
      ItemNumb[nMenuLevel] = MenuItemIndex;
    }
    /* If "RIGHT/DOWN" pushbutton is pressed */
    if (MyKey == DOWN)    //Since "RIGHT" key is not present
    {
      /* Set the Text Color */
      LCD_SetTextColor(White);
      LCD_DrawRect(IconRect[i][0], IconRect[i][1], 36, 48);
      if(i == (NUMBER_OF_ICONS - 1))
      {
        i = 0x00;
      }
      else
      {
        i++;
      }
      /* Set the Text Color */
      LCD_SetTextColor(Blue);
      LCD_DrawRect(IconRect[i][0], IconRect[i][1], 36, 48);

      /* Set the Back Color */
      LCD_SetBackColor(Blue);
      /* Set the Text Color */
      LCD_SetTextColor(White);
      /* Test on the MenuItemIndex value before incrementing it */
      if(MenuItemIndex >= ((psCurrentMenu->nItems) - 1))
      {
        MenuItemIndex = 0;
      }
      else
      {
        MenuItemIndex++;
      }
      /* Get the current menu */
      psMenuItem = &(psCurrentMenu->psItems[MenuItemIndex]);
      LCD_DisplayStringLine(Line0, psMenuItem->pszTitle);
      ItemNumb[nMenuLevel] = MenuItemIndex;
    }
    /* If "SEL" pushbutton is pressed */
    if(MyKey == SEL)
    {
      SelFunc();
      IntExtOnOffConfig(ENABLE);
      return;
    }
  }  
#else
  LCD_Clear(White);
  DisplayMenu();
  IntExtOnOffConfig(ENABLE);
  EnterAutoShutdownMode_RTCAlarm(180);  //3 minutes for auto shutdown
  return;
#endif /* SHOW_MENU_ICONS */
}

/*******************************************************************************
* Function Name  : STM32Intro
* Description    : Display the STM32 introduction.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void STM32Intro(void)
{
  /* Clear the LCD */
  LCD_Clear(White);
  /* Set the Back Color */
  LCD_SetBackColor(White);
  /* Set the Text Color */
  LCD_SetTextColor(Blue);
  
#ifdef SHOW_LOGO_IMAGE
  DRAW_SetImage(Logo_Image, 16, Line4, 128, 22); /* 128x22 Image Size */
  Delay(500);
#else
  LCD_DisplayStringLine(Line4, "      YANTRONIX     ");  
  Delay(100);
#endif /* SHOW_LOGO_IMAGE */  
}

/*******************************************************************************
* Function Name  : HelpFunc
* Description    : Display the Help menu.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void HelpFunc(void)
{ 
  IntExtOnOffConfig(DISABLE);

  /* Set the Back Color */
  LCD_SetBackColor(Blue);
  /* Set the Text Color */
  LCD_SetTextColor(White);

  LCD_DisplayStringLine(Line0, "UP, DOWN, RIGHT and ");
  LCD_DisplayStringLine(Line1, "LEFT push-buttons   ");
  LCD_DisplayStringLine(Line2, "perform circular    ");
  LCD_DisplayStringLine(Line3, "navigation in the   ");
  LCD_DisplayStringLine(Line4, "main menu, current  ");
  LCD_DisplayStringLine(Line5, "menu items.         ");
  LCD_DisplayStringLine(Line6, "SEL selects the item");
  LCD_DisplayStringLine(Line7, "UP and DOWN perform ");
  LCD_DisplayStringLine(Line8, "vertical navigation.");
  
  while(ReadKey() == NOKEY)
  {
  }

  LCD_Clear(White);
  DisplayMenu();
  IntExtOnOffConfig(ENABLE);
}

/*******************************************************************************
* Function Name  : AboutFunc
* Description    : Display the About menu.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void AboutFunc(void)
{
  LCD_Clear(White);

  IntExtOnOffConfig(DISABLE);

  while(ReadKey() != NOKEY)
  {
  } 

  /* Set the Back Color */
  LCD_SetBackColor(Blue);
  /* Set the Text Color */
  LCD_SetTextColor(White);

  LCD_ClearLine(Line0);
  LCD_DisplayStringLine(Line1, "   STM32-PRIMER2    ");
  LCD_DisplayStringLine(Line2, "   Demonstration    ");
  LCD_DisplayStringLine(Line3, "   Version 1.0.0    ");
  LCD_ClearLine(Line4);
  LCD_DisplayStringLine(Line5, "   COPYRIGHT 2010   ");
  LCD_DisplayStringLine(Line6, "      YANTRONIX     ");
  LCD_ClearLine(Line7);
  LCD_DisplayStringLine(Line8, "  www.yantronix.com ");    

  while(ReadKey() == NOKEY)
  {
  }

  LCD_Clear(White);
  DisplayMenu();
  IntExtOnOffConfig(ENABLE);
}

/*******************************************************************************
* Function Name  : Buffercmp8
* Description    : Compares two buffers.
* Input          : - pBuffer1, pBuffer2: buffers to be compared.
*                : - BufferLength: buffer's length
* Output         : None
* Return         : 0: pBuffer1 identical to pBuffer2
*                  1: pBuffer1 differs from pBuffer2
*******************************************************************************/
static uint8_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer1 != *pBuffer2)
    {
      return 1;
    }

    pBuffer1++;
    pBuffer2++;
  }

  return 0;
}

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
