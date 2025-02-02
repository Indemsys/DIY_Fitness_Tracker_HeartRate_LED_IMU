﻿// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 2023-01-27
// 13:16:41 
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include   "App.h"

/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
static void _Print_SX1262_test_header(void)
{
  GET_MCBL;
  MPRINTF(VT100_CLEAR_AND_HOME);
  MPRINTF(" ===  SX1262 test ===\n\r");
  MPRINTF("ESC - exit, [S] - run test\n\r");
  MPRINTF("----------------------------------------------------------------------\n\r");
}

/*-----------------------------------------------------------------------------------------------------


  \param void

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
static uint32_t _SX1262_Test(void)
{
  uint16_t  word;
  uint8_t   b;

  GET_MCBL;

  LORA_RESET  = 0;
  LORA_PWR_EN = 1;
  Wait_ms(20);
  LORA_RESET  = 1;
  Wait_ms(10);

  if (SX1262_open() != SSP_SUCCESS)
  {
    MPRINTF("Connection to SX1262 error.\n\r");
    return RES_ERROR;
  }

  if (SX1262_ReadRegister(SX_1262_LORA_SYNC_WORD_MSB, &b)!= RES_OK)
  {
    MPRINTF("Read SX_1262_LORA_SYNC_WORD_MSB error.\n\r");
    goto EXIT_ON_ERROR;
  }
  word = b << 8;

  if (SX1262_ReadRegister(SX_1262_LORA_SYNC_WORD_LSB, &b)!= RES_OK)
  {
    MPRINTF("Read SX_1262_LORA_SYNC_WORD_LSB error.\n\r");
    goto EXIT_ON_ERROR;
  }
  word |= b;

  MPRINTF("SX1262 SYNC WORD = %04X. ", word);
  if (word == 0x1424)
  {
    MPRINTF(" Correct!\n\r");
  }
  else
  {
    MPRINTF(" Incorrect!\n\r");
  }

  if (SX1262_ReadRegister(SX_1262_CRC_MSB_POLYNOMIAL_VALUE_0, &b)!= RES_OK)
  {
    MPRINTF("Read SX_1262_CRC_MSB_POLYNOMIAL_VALUE_0 error.\n\r");
    goto EXIT_ON_ERROR;
  }
  word = b ;
  if (SX1262_ReadRegister(SX_1262_CRC_LSB_POLYNOMIAL_VALUE_1, &b)!= RES_OK)
  {
    MPRINTF("Read SX_1262_CRC_LSB_POLYNOMIAL_VALUE_1 error.\n\r");
    goto EXIT_ON_ERROR;
  }
  word |= b << 8;
  MPRINTF("SX1262 POLYNOMIAL VALUE = %04X.\n\r", word);


  LORA_RESET  = 0;
  LORA_PWR_EN = 0;

  SX1262_close();
  return RES_OK;

EXIT_ON_ERROR:


  LORA_RESET  = 0;
  LORA_PWR_EN = 0;
  SX1262_close();
  return RES_ERROR;
}

/*-----------------------------------------------------------------------------------------------------


  \param keycode
-----------------------------------------------------------------------------------------------------*/
void Do_SX1262_test(uint8_t keycode)
{
  uint8_t             b;
  GET_MCBL;

  _Print_SX1262_test_header();

  do
  {
    if (WAIT_CHAR(&b,  ms_to_ticks(100)) == RES_OK)
    {
      if (b > 0)
      {
        switch (b)
        {
        case 'S':
        case 's':
          _SX1262_Test();
          MPRINTF("\r\nPress any key to continue.\r\n");
          WAIT_CHAR(&b,  ms_to_ticks(100000));
          _Print_SX1262_test_header();
          break;

        case VT100_ESC:
          return;

        default:
          _Print_SX1262_test_header();
          break;
        }
      }
    }
  } while (1);

}


