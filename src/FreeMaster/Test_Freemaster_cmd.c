// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 2024-10-28
// 12:02:12
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "Test.h"
#include "freemaster_cfg.h"
#include "freemaster.h"
#include "freemaster_tsa.h"



/*-----------------------------------------------------------------------------------------------------


  \param app_command
  \param len
  \param dbuf

  \return uint8_t
-----------------------------------------------------------------------------------------------------*/
uint8_t App_Freemaster_cmd_man(uint16_t app_command, uint32_t len, uint8_t  *dbuf)
{
  uint8_t res = 0;
  switch (app_command)
  {
  case FMCMD_START_IMU_STREAM :
    Start_IMU_stream();
    break;
  case FMCMD_STOP_IMU_STREAM  :
    Stop_IMU_stream();
    break;
  case FMCMD_START_MOTION_LOG:
    Start_motion_logfile();
    break;
  case FMCMD_STOP_MOTION_LOG :
    Stop_motion_logfile();
    break;
  default:
    break;
  }

  return res;
}

