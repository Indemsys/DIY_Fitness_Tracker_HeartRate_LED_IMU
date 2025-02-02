﻿// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 2023-01-28
// 13:19:14 
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include   "App.h"


/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
static void   _Print_MAX17262_test_header(void)
{
  GET_MCBL;
  MPRINTF(VT100_CLEAR_AND_HOME);
  MPRINTF(" ===  MAX17262 test ===\n\r");
  MPRINTF("ESC - exit, [E] - edit register (format: <HEX_ADDR> <HEX_VAL>), [C] - clear MinMax  \n\r");
  MPRINTF("----------------------------------------------------------------------\n\r");

}

/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
static void _Print_MAX17262_registers(uint32_t start_row, uint32_t last_row)
{
  GET_MCBL;
  uint32_t row;
  uint32_t col;
  T_MAX17262_reg_descr const *rds;
  uint16_t b;
  char     str[64];

  row = start_row;
  col = 1;

  for (uint32_t i = 0; i < MAX17262_reg_count(); i++)
  {
    rds = MAX17262_get_reg_descr(i);
    MAX17262_read_reg(rds->reg_addr,rds->reg_val);

    b = *rds->reg_val;
    Get_16bit_str(str, b);

    if (((i % (last_row - start_row + 1)) == 0) && (i != 0))
    {
      row = start_row;
      col += 60;
    }
    MPRINTF(VT100_CURSOR_SET,row,col);
    MPRINTF("[0x%02X] %s = 0x%04X ( %s )",rds->reg_addr, rds->reg_name, b, str);
    MPRINTF("\n\r");
    row++;
  }
  MPRINTF(VT100_CURSOR_SET,last_row + 1,1);
  uint16_t *rv = MAX17262_get_rv();

  float vc    = rv[MAX17262_VCELL] * VOLTAGE_MEASUREMENT_STEP;
  float avc   = rv[MAX17262_AVGVCELL] * VOLTAGE_MEASUREMENT_STEP;
  float minvc = (rv[MAX17262_MAXMINVOLT] & 0xFF) * 20.0f;
  float maxvc = ((rv[MAX17262_MAXMINVOLT] >> 8) & 0xFF) * 20.0f;

  MPRINTF(VT100_CLL_FM_CRSR"Accum. voltage = %6.0f mV , Accum. aver. voltage = %6.0f mV, Min v. = %6.0f mV , Max v. = %6.0f mV\n\r",(double)vc, (double)avc, (double)minvc, (double)maxvc);

  float ic    = (int16_t)(rv[MAX17262_CURRENT]) * CURRENT_MEASUREMENT_STEP;
  float aic   = (int16_t)(rv[MAX17262_AVGCURRENT]) * CURRENT_MEASUREMENT_STEP;
  float minic = (int8_t)(rv[MAX17262_MAXMINCURR] & 0xFF) * 160.0f;
  float maxic = (int8_t)((rv[MAX17262_MAXMINCURR] >> 8) & 0xFF) * 160.0f;

  MPRINTF(VT100_CLL_FM_CRSR"Accum. current = %6.1f mA , Accum. aver. current = %6.1f mA, Min i. = %6.0f mA , Max i. = %6.0f mA\n\r",(double)ic, (double)aic, (double)minic, (double)maxic);

  float tc    = (int16_t)rv[MAX17262_TEMP] * TEMPER_MEASUREMENT_STEP;
  float atc   = (int16_t)rv[MAX17262_AVGTA] * TEMPER_MEASUREMENT_STEP;
  float mintc = (int8_t)(rv[MAX17262_MAXMINTEMP] & 0xFF) * 1.0f;
  float maxtc = (int8_t)((rv[MAX17262_MAXMINTEMP] >> 8) & 0xFF) * 1.0f;

  MPRINTF(VT100_CLL_FM_CRSR"Accum. temper. = %6.1f C  , Accum. aver. temper. = %6.1f C , Min t. = %6.0f C  , Max t. = %6.0f C \n\r",(double)tc, (double)atc, (double)mintc, (double)maxtc);

  float cap     = rv[MAX17262_REPCAP] * CAPACITY_MEASUREMENT_STEP;
  float capp    = rv[MAX17262_REPSOC] * PERCENTAGE_MEASUREMENT_STEP;
  float full_cap= rv[MAX17262_FULLCAPREP] * CAPACITY_MEASUREMENT_STEP;

  MPRINTF(VT100_CLL_FM_CRSR"Accum. capacity= %6.1f mAh, Accum. capacity perc.= %6.1f %% , Full C.= %6.0f mAh\n\r",(double)cap, (double)capp, (double)full_cap);

}

/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
static void _Clear_min_max(void)
{
  MAX17262_write_reg(MAX17262_MAXMINCURR,0x807F);
  MAX17262_write_reg(MAX17262_MAXMINVOLT,0x00FF);
  MAX17262_write_reg(MAX17262_MAXMINTEMP,0x807F);
}


/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
void Do_MAX17262_test(uint8_t keycode)
{
  uint8_t             b;
  char                str[16];
  GET_MCBL;

  if (Is_MAX17262_opened() == 0)
  {
    MPRINTF(VT100_CLEAR_AND_HOME);
    MPRINTF("MAX17262 is not opened. \n\r");
    Wait_ms(1000);
    return;
  }

  // Останавливаем работу задачи зарядника
  Charger_service_off_request();
  Wait_ms(100);

  _Print_MAX17262_test_header();

  do
  {
    if (WAIT_CHAR(&b,  ms_to_ticks(500)) == RES_OK)
    {
      if (b > 0)
      {
        switch (b)
        {
        case 'E':
        case 'e':
          if (VT100_edit_string_in_pos(str, sizeof(str), 36, 0) == RES_OK)
          {
            uint32_t reg_addr;
            uint32_t reg_val;
            if (sscanf(str, "%02X %04X",&reg_addr,&reg_val) == 2)
            {
              if (MAX17262_write_reg(reg_addr,reg_val) != SSP_SUCCESS)
              {
                MPRINTF("\n\r Write error!");
                Wait_ms(1000);
              }
            }
            else
            {
              MPRINTF("\n\r String error!");
              Wait_ms(1000);
            }
          }
          _Print_MAX17262_test_header();
          break;
        case 'C':
        case 'c':
          _Clear_min_max();
          _Print_MAX17262_test_header();
          break;

        case VT100_ESC:
          Charger_service_on_request();
          return;

        default:
          _Print_MAX17262_test_header();
          break;
        }
      }
    }
    else
    {
      _Print_MAX17262_registers(4, 34);
    }
  } while (1);
}


