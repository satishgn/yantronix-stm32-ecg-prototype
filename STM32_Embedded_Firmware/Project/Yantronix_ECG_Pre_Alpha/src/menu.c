/**
  ******************************************************************************
  * @file     menu.c
  * @author   MCD Application Team
  * @version  V1.0.0
  * @date     03/01/2010
  * @brief    This file includes the menu navigation driver for the STM32 board application.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */

/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"

/** @addtogroup STM32_BOARD_APP
  * @{
  */

/** @defgroup MENU
  * @brief    This file includes the menu navigation driver for the STM32 board application.
  * @{
  */

/** @defgroup MENU_Private_Types
  * @{
  */
typedef void (* tMenuFunc)(void);
typedef struct sMenuItem * tMenuItem;
typedef struct sMenu * tMenu;
/**
  * @}
  */


/** @defgroup MENU_Private_Defines
  * @{
  */
#define SHOW_LOGO_IMAGE
#define SHOW_MENU_ICONS
#define USE_ICONS_40_24
// #define USE_ICONS_56_24
#ifdef USE_ICONS_40_24
 #define NUMBER_OF_ICONS    6
 #define ICON_WIDTH         40             
 #define ICON_HEIGHT        24
 #define ICON_ROW_START     32          
 #define ICON_COL_START     4
 #define MAX_ICON_ROWS      2
 #define MAX_ICON_COLS      3
#elif defined USE_ICONS_56_24
 #define NUMBER_OF_ICONS    4
 #define ICON_WIDTH         56             
 #define ICON_HEIGHT        24
 #define ICON_ROW_START     32          
 #define ICON_COL_START     8
 #define MAX_ICON_ROWS      2
 #define MAX_ICON_COLS      2
#endif

#ifdef SHOW_LOGO_IMAGE
const uint8_t Logo_Image[] = { 
    #include "Yantronix_128x32.h"
};
#endif /* SHOW_LOGO_IMAGE */

#ifdef SHOW_MENU_ICONS
const uint8_t Icons_Image[NUMBER_OF_ICONS][ICON_WIDTH*(ICON_HEIGHT>>3)] = {
#ifdef USE_ICONS_40_24
    #include "Calendar_40x24.h"
    #include "Oscilloscope_40x24.h"
    #include "ECGMonitor_40x24.h"
#if defined (USE_USB_DFU) || defined (USE_USB_MSD) || defined (USE_USB_VCP)
    #include "USBApps_40x24.h"
#else
    #include "LowPowerMode_40x24.h"
#endif
    // #include "Help_40x24.h"
    #include "About_40x24.h"
    #include "Shutdown_40x24.h"
#elif defined USE_ICONS_56_24
    #include "Calendar_56x24.h"
    #include "Oscilloscope_56x24.h"
    #include "ECGMonitor_56x24.h"
#if defined (USE_USB_DFU) || defined (USE_USB_MSD) || defined (USE_USB_VCP)
    #include "USBApps_56x24.h"
#else
    #include "LowPowerMode_56x24.h"
#endif
#endif
};
#endif /* SHOW_MENU_ICONS */

/**
  * @}
  */

/** @defgroup MENU_Private_Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup MENU_Private_Variables
  * @{
  */
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

/*------------------------------ Menu level 4 -------------------------------*/
/*---------------------------------------------------------------------------*/

/*------------------------------ Menu level 3 -------------------------------*/
#ifdef USE_USB_DFU
struct sMenuItem DFUMenuItems[] = 
  {
    {"       Start        ", DFU_Start, Menu_IdleFunc},
    {"       Return       ", Menu_ReturnFunc, Menu_IdleFunc}
  };
struct sMenu DFUMenu = 
  {"  Firmware Upgrade  ", DFUMenuItems, countof(DFUMenuItems)};
#endif

#ifdef USE_USB_MSD
struct sMenuItem USBMassStorageMenuItems[] = 
  {
    {"       Start        ", MSD_Start, Menu_IdleFunc},
    {"       Return       ", Menu_ReturnFunc, Menu_IdleFunc}
  };
struct sMenu USBMassStorageMenu = 
  {"  USB Mass Storage  ", USBMassStorageMenuItems, countof(USBMassStorageMenuItems)};
#endif

#ifdef USE_USB_VCP
struct sMenuItem USBVirtualComMenuItems[] = 
  {
    {"       Start        ", VCP_Start, Menu_IdleFunc},
    {"       Return       ", Menu_ReturnFunc, Menu_IdleFunc}
  };
struct sMenu USBVirtualComMenu = 
  {"  USB Virtual Com   ", USBVirtualComMenuItems, countof(USBVirtualComMenuItems)};
#endif

struct sMenuItem LOWPOWERSTOPMenuItems[] =
  {
    {"    Exit: EXTI      ", LowPower_EnterSTOPMode_EXTI, Menu_IdleFunc},
    {"  Exit: RTC Alarm   ", LowPower_EnterSTOPMode_RTCAlarm, Menu_IdleFunc},
    {"       Return       ", Menu_ReturnFunc, Menu_IdleFunc}
  };
struct sMenu LOWPOWERSTOPMenu =
  {"     STOP Mode      ", LOWPOWERSTOPMenuItems, countof(LOWPOWERSTOPMenuItems)};

struct sMenuItem LOWPOWERSTANDBYMenuItems[] =
  {
    {"  Exit: Wakeup Pin  ", LowPower_EnterSTANDBYMode_WAKEUP, Menu_IdleFunc},
    {"  Exit: RTC Alarm   ", LowPower_EnterSTANDBYMode_RTCAlarm, Menu_IdleFunc},
    {"       Return       ", Menu_ReturnFunc, Menu_IdleFunc}
  };
struct sMenu LOWPOWERSTANDBYMenu =
  {"   STANDBY Mode     ", LOWPOWERSTANDBYMenuItems, countof(LOWPOWERSTANDBYMenuItems)};

struct sMenuItem SettingTimeMenuItems[] =
  {
    {"       Adjust       ", Calendar_TimeAdjust, Menu_IdleFunc},
    {"        Show        ", Calendar_TimeShow, Menu_IdleFunc},
    {"       Return       ", Menu_ReturnFunc, Menu_IdleFunc}
  };
struct sMenu TimeMenu =
  {"        Time        ", SettingTimeMenuItems, countof(SettingTimeMenuItems)};

struct sMenuItem SettingDateMenuItems[] =
  {
    {"       Adjust       ", Calendar_DateAdjust, Menu_IdleFunc},
    {"        Show        ", Calendar_DateShow, Menu_IdleFunc},
    {"       Return       ", Menu_ReturnFunc, Menu_IdleFunc}
  };
struct sMenu DateMenu =
  {"        Date        ", SettingDateMenuItems, countof(SettingDateMenuItems)};

struct sMenuItem SettingAlarmMenuItems[] =
  {
    {"       Adjust       ", Calendar_AlarmAdjust, Menu_IdleFunc},
    {"        Show        ", Calendar_AlarmShow, Menu_IdleFunc},
    {"       Return       ", Menu_ReturnFunc, Menu_IdleFunc}
  };
struct sMenu AlarmMenu =
  {"        Alarm       ", SettingAlarmMenuItems, countof(SettingAlarmMenuItems)};
/*---------------------------------------------------------------------------*/

/*------------------------------ Menu level 2 -------------------------------*/
struct sMenuItem AboutMenuItems[] =
  {
    {"       About        ", Menu_AboutFunc, Menu_IdleFunc},
    {"       Return       ", Menu_ReturnFunc, Menu_IdleFunc}
  };
struct sMenu AboutMenu =
  {"        About       ", AboutMenuItems, countof(AboutMenuItems)};

struct sMenuItem HelpMenuItems[] =
  {
    {"       Start        ", Menu_HelpFunc, Menu_IdleFunc},
    {"       Return       ", Menu_ReturnFunc, Menu_IdleFunc}
  };
struct sMenu HelpMenu =
  {"        Help        ", HelpMenuItems, countof(HelpMenuItems)};

#if defined (USE_USB_DFU) || defined (USE_USB_MSD) || defined (USE_USB_VCP)
struct sMenuItem USBAppsMenuItems[] =
  {
#ifdef USE_USB_DFU
    {"  Firmware Upgrade  ", Menu_IdleFunc, Menu_IdleFunc, &DFUMenu},
#endif
#ifdef USE_USB_MSD
    {"  USB Mass Storage  ", Menu_IdleFunc, Menu_IdleFunc, &USBMassStorageMenu},
#endif
#ifdef USE_USB_VCP
    {"  USB Virtual Com   ", Menu_IdleFunc, Menu_IdleFunc, &USBVirtualComMenu},
#endif
    {"       Return       ", Menu_ReturnFunc, Menu_IdleFunc}
  };
struct sMenu USBAppsMenu =
  {"      USB Apps      ", USBAppsMenuItems, countof(USBAppsMenuItems)};
#else
struct sMenuItem LowPowerMenuItems[] =
  {
    {"   Battery Monitor  ", Start_Battery_Monitor, Menu_IdleFunc},
    {"        STOP        ", Menu_IdleFunc, Menu_IdleFunc, &LOWPOWERSTOPMenu},
    {"       STANDBY      ", Menu_IdleFunc, Menu_IdleFunc, &LOWPOWERSTANDBYMenu},
    {"       Return       ", Menu_ReturnFunc, Menu_IdleFunc}
  };
struct sMenu LowPowerMenu =
  {"    Low Power Mode  ", LowPowerMenuItems, countof(LowPowerMenuItems)};
#endif

struct sMenuItem CalendarMenuItems[] =
  {
    {"        Time        ", Menu_IdleFunc, Menu_IdleFunc, &TimeMenu},
    {"        Date        ", Menu_IdleFunc, Menu_IdleFunc, &DateMenu},
    {"        Alarm       ", Menu_IdleFunc, Menu_IdleFunc, &AlarmMenu},
    {"        Return      ", Menu_ReturnFunc, Menu_IdleFunc}
  };
struct sMenu CalendarMenu =
  {"       Calendar     ", CalendarMenuItems, countof(CalendarMenuItems)};

struct sMenuItem ECGMonitorMenuItems[] = 
  {
    {"  Acquire & Log ECG ", Start_ECG_Data_Acquisition, Menu_IdleFunc},
    {"  Play ECG Log File ", Browse_Log_Files, Menu_IdleFunc},
    {"Delete All Log Files", Delete_All_Log_Files, Menu_IdleFunc},
    {"       Return       ", Menu_ReturnFunc, Menu_IdleFunc}
  };
struct sMenu ECGMonitorMenu = 
  {"    ECG Monitor     ", ECGMonitorMenuItems, countof(ECGMonitorMenuItems)};

struct sMenuItem SignalScopeMenuItems[] = 
  {
#ifdef USE_DAC_SIGNALGEN   
    {"Acquire & Log Signal", Start_SIGNALGEN_Data_Acquisition, Menu_IdleFunc},
#endif
    {"Play Signal Log File", Browse_Log_Files, Menu_IdleFunc},
    {"Delete All Log Files", Delete_All_Log_Files, Menu_IdleFunc},
    {"       Return       ", Menu_ReturnFunc, Menu_IdleFunc}
  };
struct sMenu SignalScopeMenu = 
  {"    Signal Scope    ", SignalScopeMenuItems, countof(SignalScopeMenuItems)};
/*---------------------------------------------------------------------------*/

/*------------------------------ Menu level 1 -------------------------------*/
struct sMenuItem MainMenuItems[] =
  {
    {"       Calendar     ", Menu_IdleFunc, Menu_IdleFunc, &CalendarMenu},
	{"    Signal Scope    ", Menu_IdleFunc, Menu_IdleFunc, &SignalScopeMenu},
	{"    ECG Monitor     ", Menu_IdleFunc, Menu_IdleFunc, &ECGMonitorMenu},
#if defined (USE_USB_DFU) || defined (USE_USB_MSD) || defined (USE_USB_VCP)
    {"      USB Apps      ", Menu_IdleFunc, Menu_IdleFunc, &USBAppsMenu},
#else
    {"    Low Power Mode  ", Menu_IdleFunc, Menu_IdleFunc, &LowPowerMenu},
#endif
    // {"        Help        ", Menu_IdleFunc, Menu_IdleFunc, &HelpMenu},
    {"        About       ", Menu_IdleFunc, Menu_IdleFunc, &AboutMenu},
	{"      Shutdown      ", Shutdown_System, Menu_ReturnFunc}
  };
struct sMenu MainMenu =
  {"     Main menu      ", MainMenuItems, countof(MainMenuItems)};
/*---------------------------------------------------------------------------*/

/**
  * @}
  */

/** @defgroup MENU_Private_FunctionPrototypes
  * @{
  */
/**
  * @}
  */

/** @defgroup MENU_Private_Functions
  * @{
  */

/**
  * @brief  Initializes the navigation menu.
  * @param  None
  * @retval None
  */
void Menu_Init(void)
{
  psCurrentMenu = &MainMenu;
  psPrevMenu[nMenuLevel] = psCurrentMenu;
  psMenuItem = MainMenuItems;
}


/**
  * @brief  Displays the current menu.
  * @param  None
  * @retval None
  */
void Menu_DisplayMenu(void)
{
  uint32_t index = 0;
  tMenuItem psMenuItem2;
  /* Clear the LCD Screen */
  LCD_Clear(LCD_COLOR_WHITE);
  /* Set the Text Color */
  LCD_SetTextColor(LCD_COLOR_TEXT_NORMAL);
  LCD_DisplayStringLine(LCD_LINE_0, psCurrentMenu->pszTitle);
  /* Set the Text Color */
  LCD_SetTextColor(LCD_COLOR_TEXT_NORMAL);
  while (!(index >= (psCurrentMenu->nItems)))
  {
    psMenuItem2 = &(psCurrentMenu->psItems[index]);
    LCD_DisplayStringLine(6-index, psMenuItem2->pszTitle);
    index++;
  }
  /* Get the current menu */
  psMenuItem = &(psCurrentMenu->psItems[MenuItemIndex]);
  /* Set the Text Color */
  LCD_SetTextColor(LCD_COLOR_TEXT_REVERSE);
  LCD_DisplayStringLine(6-MenuItemIndex, psMenuItem->pszTitle);
  LCD_DrawRect(LCD_COLUMN_0-1, (6-MenuItemIndex)*8, 121, 9, LCD_COLOR_BLACK);
}

/**
  * @brief  This function is executed when "SEL" push-buttton is pressed.
  * @param  None
  * @retval None
  */
void Menu_SelFunc(void)
{
  Delay(20);//Delay 200 msec

  /* Set the Text Color */
  LCD_SetTextColor(LCD_COLOR_TEXT_NORMAL);

  psCurrentMenuItem = psMenuItem;
  if (psMenuItem->psSubMenu != '\0')
  {
    /* Update the current Item by the submenu */
    MenuItemIndex = 0;
    psCurrentMenu = psMenuItem->psSubMenu;
    psMenuItem = &(psCurrentMenu->psItems)[MenuItemIndex];
    Menu_DisplayMenu();
    nMenuLevel++;
    psPrevMenu[nMenuLevel] = psCurrentMenu;
  }
  psCurrentMenuItem->pfMenuFunc();
}


/**
  * @brief  This function is executed when any of "UP" push-butttons is pressed.
  * @param  None
  * @retval None
  */
void Menu_UpFunc(void)
{
  Delay(20);//Delay 200 msec

  psMenuItem = &psCurrentMenu->psItems[MenuItemIndex];
  /* Set the Text Color */
  LCD_SetTextColor(LCD_COLOR_TEXT_NORMAL);
  LCD_DisplayStringLine(6-MenuItemIndex, psMenuItem->pszTitle);
  LCD_DrawRect(LCD_COLUMN_0-1, (6-MenuItemIndex)*8, 121, 9, LCD_COLOR_WHITE);
  if (MenuItemIndex > 0)
  {
    MenuItemIndex--;
  }
  else
  {
    MenuItemIndex = psCurrentMenu->nItems - 1;
  }
  psMenuItem = &psCurrentMenu->psItems[MenuItemIndex];
  /* Set the Text Color */
  LCD_SetTextColor(LCD_COLOR_TEXT_REVERSE);
  LCD_DisplayStringLine(6-MenuItemIndex, psMenuItem->pszTitle);
  LCD_DrawRect(LCD_COLUMN_0-1, (6-MenuItemIndex)*8, 121, 9, LCD_COLOR_BLACK);
  ItemNumb[nMenuLevel] = MenuItemIndex;
}

/**
  * @brief  This function is executed when any of "Down" push-butttons is pressed.
  * @param  None
  * @retval None
  */
void Menu_DownFunc(void)
{
  Delay(20);//Delay 200 msec

  psMenuItem = &psCurrentMenu->psItems[MenuItemIndex];
  /* Set the Text Color */
  LCD_SetTextColor(LCD_COLOR_TEXT_NORMAL);
  LCD_DisplayStringLine(6-MenuItemIndex, psMenuItem->pszTitle);
  LCD_DrawRect(LCD_COLUMN_0-1, (6-MenuItemIndex)*8, 121, 9, LCD_COLOR_WHITE);
  /* Test on the MenuItemIndex value before incrementing it */
  if (MenuItemIndex >= ((psCurrentMenu->nItems) - 1))
  {
    MenuItemIndex = 0;
  }
  else
  {
    MenuItemIndex++;
  }
  /* Get the current menu */
  psMenuItem = &(psCurrentMenu->psItems[MenuItemIndex]);
  /* Set the Text Color */
  LCD_SetTextColor(LCD_COLOR_TEXT_REVERSE);
  LCD_DisplayStringLine(6-MenuItemIndex, psMenuItem->pszTitle);
  LCD_DrawRect(LCD_COLUMN_0-1, (6-MenuItemIndex)*8, 121, 9, LCD_COLOR_BLACK);
  ItemNumb[nMenuLevel] = MenuItemIndex;
}

/**
  * @brief  This function is executed when the "RETURN" menu is selected.
  * @param  None
  * @retval None
  */
void Menu_ReturnFunc(void)
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
  if (nMenuLevel != 0)
  {
    Menu_DisplayMenu();
  }
  else
  {
    Menu_ShowMenuIcons();
  }
}

/**
  * @brief  This function is executed when the MENU key is pressed.
  * @param  None
  * @retval None
  */
void Menu_ReturnMainMenu(void)
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
        Menu_ShowMenuIcons();
}

/**
  * @brief  Reads key from demoboard.
  * @param  None
  * @retval Return SEL, UP, DOWN or NOKEY
  */
uint8_t Menu_ReadKey(void)
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

/**
  * @brief  Idle function.
  * @param  None
  * @retval None
  */
void Menu_IdleFunc(void)
{
  /* Nothing to execute: return */
  return;
}

/**
  * @brief  Display menu icons.
  * @param  None
  * @retval None
  */
void Menu_DisplayIcons(void)
{
#ifdef SHOW_MENU_ICONS
  uint32_t i = 0, j = 0, l = 0;
  uint32_t iconline = ICON_ROW_START, iconcolumn = ICON_COL_START; 
    
  for(i = 0; i < MAX_ICON_ROWS; i++)
  {
    for(j = 0; j < MAX_ICON_COLS; j++)
    {
      LCD_DisplayImage(Icons_Image[l], iconcolumn, iconline, ICON_WIDTH, ICON_HEIGHT); /* 40x24 or 56x24 Image Size */
      // LCD_DrawRect(iconcolumn, iconline, ICON_WIDTH, ICON_HEIGHT, LCD_COLOR_BLACK);    
      iconcolumn += ICON_WIDTH;
      l++;
      if(l == NUMBER_OF_ICONS)
      {
        return;
      }
    }
    iconline -= ICON_HEIGHT;
    iconcolumn = ICON_COL_START;
  }
#endif /* SHOW_MENU_ICONS */
}

/**
  * @brief  Show the main menu icon.
  * @param  None
  * @retval None
  */
void Menu_ShowMenuIcons(void)
{
#ifdef SHOW_MENU_ICONS
  uint32_t MyKey = 0, i = 0;  
#ifdef USE_ICONS_40_24
  uint16_t IconRect[NUMBER_OF_ICONS][2] = {{4, 32}, {44, 32}, {84, 32},
                                           {4, 8}, {44, 8}, {84, 8}};
#elif defined USE_ICONS_56_24
  uint16_t IconRect[NUMBER_OF_ICONS][2] = {{8, 32}, {64, 32},
                                           {8, 8}, {64, 8}};
#endif

  /* Disable the button interrupts */
  IntExtOnOffConfig(DISABLE);

  // while (Menu_ReadKey() != NOKEY)
  // {
  // }

  /* Initializes the Menu state machine */
  Menu_Init();

  MenuItemIndex = 0;

  /* Clear*/
  LCD_Clear(LCD_COLOR_WHITE);
  /* Set the Text Color */
  LCD_SetTextColor(LCD_COLOR_TEXT_NORMAL);
  LCD_DisplayStringLine(LCD_LINE_0, psMenuItem->pszTitle);

  /* Displays Icons */
  Menu_DisplayIcons();

  LCD_DrawRect(IconRect[0][0], IconRect[0][1], ICON_WIDTH, ICON_HEIGHT, LCD_COLOR_BLACK);

  EnterAutoShutdownMode_RTCAlarm(180);  //3 minutes for auto shutdown
  
  /* Endless loop */
  while (1)
  {
    /* Check which key is pressed */
    MyKey = Menu_ReadKey();

    /* If "MENU" pushbutton is pressed */
    if (MyKey == MENU)
    {
        continue;
    }                  

    // /* If "UP" pushbutton is pressed */
    // if (MyKey == UP)
    // {
      // LCD_DrawRect(IconRect[i][0], IconRect[i][1], ICON_WIDTH, ICON_HEIGHT, LCD_COLOR_WHITE);

      // if (i <= (MAX_ICON_COLS - 1))
      // {
        // i += MAX_ICON_COLS;
        // if (i >= NUMBER_OF_ICONS)
        // {
          // i = (NUMBER_OF_ICONS - 1);
        // }
      // }
      // else
      // {
        // i -= MAX_ICON_COLS;
      // }
  
      // LCD_DrawRect(IconRect[i][0], IconRect[i][1], ICON_WIDTH, ICON_HEIGHT, LCD_COLOR_BLACK);

      // /* Test on the MenuItemIndex value before incrementing it */
      // if (MenuItemIndex <= (MAX_ICON_COLS - 1))
      // {
        // MenuItemIndex += MAX_ICON_COLS;
        // if (MenuItemIndex >= NUMBER_OF_ICONS)
        // {
          // MenuItemIndex = (NUMBER_OF_ICONS - 1);
        // }
      // }
      // else
      // {
        // MenuItemIndex -= MAX_ICON_COLS;
      // }
  
      // /* Get the current menu */
      // psMenuItem = &(psCurrentMenu->psItems[MenuItemIndex]);
      // LCD_DisplayStringLine(LCD_LINE_0, psMenuItem->pszTitle);
      // ItemNumb[nMenuLevel] = MenuItemIndex;
    // }

    // /* If "DOWN" pushbutton is pressed */
    // if (MyKey == DOWN)
    // {
      // LCD_DrawRect(IconRect[i][0], IconRect[i][1], ICON_WIDTH, ICON_HEIGHT, LCD_COLOR_WHITE);
    
      // if (i >= MAX_ICON_COLS)
      // {
        // i -= MAX_ICON_COLS;
      // }
      // else
      // {
        // i += MAX_ICON_COLS;
        // if (i >= NUMBER_OF_ICONS)
        // {
          // i = (NUMBER_OF_ICONS - 1);
        // }
      // }
  
      // LCD_DrawRect(IconRect[i][0], IconRect[i][1], ICON_WIDTH, ICON_HEIGHT, LCD_COLOR_BLACK);

      // /* Test on the MenuItemIndex value before incrementing it */
      // if (MenuItemIndex >= MAX_ICON_COLS)
      // {
        // MenuItemIndex -= MAX_ICON_COLS;
      // }
      // else
      // {
        // MenuItemIndex += MAX_ICON_COLS;
        // if (MenuItemIndex >= NUMBER_OF_ICONS)
        // {
          // MenuItemIndex = (NUMBER_OF_ICONS - 1);
        // }
      // }
  
      // /* Get the current menu */
      // psMenuItem = &(psCurrentMenu->psItems[MenuItemIndex]);
      // LCD_DisplayStringLine(LCD_LINE_0, psMenuItem->pszTitle);
      // ItemNumb[nMenuLevel] = MenuItemIndex;
    // }

    /* If "LEFT/UP" pushbutton is pressed */
    if (MyKey == UP)    //Since "LEFT" key is not present
    {
      LCD_DrawRect(IconRect[i][0], IconRect[i][1], ICON_WIDTH, ICON_HEIGHT, LCD_COLOR_WHITE);
    
      if(i == 0)
      {
        i = (NUMBER_OF_ICONS - 1);
      }
      else
      {
        i--;
      }
  
      LCD_DrawRect(IconRect[i][0], IconRect[i][1], ICON_WIDTH, ICON_HEIGHT, LCD_COLOR_BLACK);

      if(MenuItemIndex > 0)
      {
        MenuItemIndex--;
      }
      else
      {
        MenuItemIndex = psCurrentMenu->nItems - 1;
      }

      /* Get the current menu */
      psMenuItem = &psCurrentMenu->psItems[MenuItemIndex];
      LCD_DisplayStringLine(LCD_LINE_0, psMenuItem->pszTitle);
      ItemNumb[nMenuLevel] = MenuItemIndex;
    }
    
    /* If "RIGHT/DOWN" pushbutton is pressed */
    if (MyKey == DOWN)    //Since "RIGHT" key is not present
    {
      LCD_DrawRect(IconRect[i][0], IconRect[i][1], ICON_WIDTH, ICON_HEIGHT, LCD_COLOR_WHITE);
    
      if(i == (NUMBER_OF_ICONS - 1))
      {
        i = 0;
      }
      else
      {
        i++;
      }
  
      LCD_DrawRect(IconRect[i][0], IconRect[i][1], ICON_WIDTH, ICON_HEIGHT, LCD_COLOR_BLACK);

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
      LCD_DisplayStringLine(LCD_LINE_0, psMenuItem->pszTitle);
      ItemNumb[nMenuLevel] = MenuItemIndex;
    }
    
    /* If "SEL" pushbutton is pressed */
    if (MyKey == SEL)
    {
      Menu_SelFunc();
      IntExtOnOffConfig(ENABLE);
      return;
    }
  }
#else
  LCD_Clear(LCD_COLOR_WHITE);
  Menu_DisplayMenu();
  IntExtOnOffConfig(ENABLE);
  EnterAutoShutdownMode_RTCAlarm(180);  //3 minutes for auto shutdown
  return;
#endif /* SHOW_MENU_ICONS */
}

/**
  * @brief  Display the STM32 introduction.
  * @param  None
  * @retval None
  */
void Menu_STM32Intro(void)
{
  /* Clear the LCD */
  LCD_Clear(LCD_COLOR_WHITE);

#ifdef SHOW_LOGO_IMAGE
  LCD_DisplayImage(Logo_Image, 0, 16, 128, 32); /* 128x32 Image Size */
  Delay(500);
#else
  LCD_DisplayStringLine(LCD_LINE_4, "      YANTRONIX     ");  
  Delay(100);
#endif /* SHOW_LOGO_IMAGE */
}

/**
  * @brief  Display the Help menu.
  * @param  None
  * @retval None
  */
void Menu_HelpFunc(void)
{
  LCD_Clear(LCD_COLOR_WHITE);
  IntExtOnOffConfig(DISABLE);

  /* Display Help Bitmap Image here */

  LCD_SetTextColor(LCD_COLOR_TEXT_REVERSE);
  LCD_DisplayStringLine(LCD_LINE_0, psMenuItem->pszTitle);
  LCD_SetTextColor(LCD_COLOR_TEXT_NORMAL);
  LCD_DisplayStringLine(LCD_LINE_1, "UP, DOWN, buttons   ");
  LCD_DisplayStringLine(LCD_LINE_2, "perform circular    ");
  LCD_DisplayStringLine(LCD_LINE_3, "navigation in the   ");
  LCD_DisplayStringLine(LCD_LINE_4, "main menu, current  ");
  LCD_DisplayStringLine(LCD_LINE_5, "menu items. SEL     ");
  LCD_DisplayStringLine(LCD_LINE_6, "push-button selects ");
  LCD_DisplayStringLine(LCD_LINE_7, "the current item.   ");

  while (Menu_ReadKey() == NOKEY)
  {
  }

  LCD_Clear(LCD_COLOR_WHITE);
  Menu_DisplayMenu();
  IntExtOnOffConfig(ENABLE);
}

/**
  * @brief  Display the About menu.
  * @param  None
  * @retval None
  */
void Menu_AboutFunc(void)
{
  LCD_Clear(LCD_COLOR_WHITE);
  IntExtOnOffConfig(DISABLE);

  // while (Menu_ReadKey() != NOKEY)
  // {
  // }

  LCD_SetTextColor(LCD_COLOR_TEXT_REVERSE);
  LCD_DisplayStringLine(LCD_LINE_0, psMenuItem->pszTitle);
  LCD_SetTextColor(LCD_COLOR_TEXT_NORMAL);
  LCD_DisplayStringLine(LCD_LINE_1, "   ECG Prototype    ");
  LCD_DisplayStringLine(LCD_LINE_2, "   Demonstration    ");
  LCD_DisplayStringLine(LCD_LINE_3, "   Version 1.0.0    ");
  LCD_ClearLine(LCD_LINE_4);
  LCD_DisplayStringLine(LCD_LINE_5, "   COPYRIGHT 2011   ");
  LCD_DisplayStringLine(LCD_LINE_6, "      YANTRONIX     ");
  LCD_DisplayStringLine(LCD_LINE_7, "  www.yantronix.com ");    

  while (Menu_ReadKey() == NOKEY)
  {
  }

  LCD_Clear(LCD_COLOR_WHITE);
  Menu_DisplayMenu();
  IntExtOnOffConfig(ENABLE);
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

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
