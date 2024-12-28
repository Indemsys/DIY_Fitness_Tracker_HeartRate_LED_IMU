// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 2024-12-26
// 20:59:10
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "Test.h"


const T_VT100_Menu_item MENU_BASE_ITEMS[] =
{
  { '1', Do_Bluetooth_control              , 0 },
  { 'R', 0                                 , 0 },
  { 'M', 0,                 (void *)&MENU_MAIN },
  { 0 }
};

const T_VT100_Menu      MENU_APPLICATION       =
{
  "Application diagnostic menu",
  "\033[5C <1> - Heart Rate Bluetooth control\r\n"
  "\033[5C <R> - Display previous menu\r\n"
  "\033[5C <M> - Display main menu\r\n",
  MENU_BASE_ITEMS,
};


