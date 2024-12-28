// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 2023-02-06
// 17:23:49
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include   "App.h"

/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
void System_start_report(void)
{
  uint32_t err;

  fmi_unique_id_t uid;
  APPLOG("##############################################################");


  err = g_fmi.p_api->uniqueIdGet(&uid);
  if (err == SSP_SUCCESS)
  {
    APPLOG("Chip ID: %08X %08X %08X %08X", uid.unique_id[0], uid.unique_id[1], uid.unique_id[2], uid.unique_id[3]);
  }
  else
  {
    APPLOG("Failed to read Chip ID");
  }

  APPLOG("RSTSR0 = %02X, RSTSR1 = %04X, RSTSR3 = %02X",loader_info.rstsr0, loader_info.rstsr1, loader_info.rstsr2);

  if (loader_info.rstsr0 & BIT(0))
  {
    APPLOG("Power-on reset detected");
  }
  if (loader_info.rstsr0 & BIT(1))
  {
    APPLOG("Voltage monitor 0 reset detected");
  }
  if (loader_info.rstsr0 & BIT(2))
  {
    APPLOG("Voltage monitor 1 reset detected")
  }
  if (loader_info.rstsr0 & BIT(3))
  {
    APPLOG("Voltage monitor 2 reset detected");
  }
  if (loader_info.rstsr0 & BIT(7))
  {
    APPLOG("Deep Software Standby mode cancellation requested by an interrupt");
  }
  if (loader_info.rstsr1 & BIT(0))
  {
    APPLOG("Independent Watchdog Timer reset detected");
  }
  if (loader_info.rstsr1 & BIT(1))
  {
    APPLOG("Watchdog Timer reset detected");
  }
  if (loader_info.rstsr1 & BIT(2))
  {
    APPLOG("Software reset detected");
  }
  if (loader_info.rstsr1 & BIT(8))
  {
    APPLOG("SRAM parity error reset detected");
  }
  if (loader_info.rstsr1 & BIT(9))
  {
    APPLOG("SRAM DED error reset detected");
  }
  if (loader_info.rstsr1 & BIT(10))
  {
    APPLOG("Bus slave MPU error reset detected");
  }
  if (loader_info.rstsr1 & BIT(11))
  {
    APPLOG("Bus master MPU error reset detected");
  }
  if (loader_info.rstsr1 & BIT(12))
  {
    APPLOG("SP error reset detected");
  }

  if (loader_info.rstsr2 & BIT(0))
  {
    APPLOG("Warm start");
  }
  else
  {
    APPLOG("Cold start");
  }

  if (g_file_system_ready==0)
  {
    APPLOG("File system on SD card missing.");
  }
  else
  {
    APPLOG("File system on SD card ready.");
  }


  T_settings_restore_results* r = Get_Setting_restoring_res(MODULE_PARAMS);

  if (r->dataflash_restoring_error != 0 )
  {
    APPLOG("Internal flash memory error.");
  }

  switch (r->settings_source)
  {
  case RESTORED_DEFAULT_SETTINGS:
     APPLOG("Restored module default parameters.");
     break;
  case RESTORED_SETTINGS_FROM_DATAFLASH:
     APPLOG("Module parameters restored from internal Flash memory.");
     break;
  case RESTORED_SETTINGS_FROM_JSON_FILE:
     APPLOG("Module parameters restored from JSON file on SD card.");
     break;
  case RESTORED_SETTINGS_FROM_INI_FILE:
     APPLOG("Module parameters restored from INI file on SD card.");
     break;
  }

  switch (r->dataflash_saving_error)
  {
  case SAVED_TO_DATAFLASH_NONE:
    break;
  case SAVED_TO_DATAFLASH_OK:
    APPLOG("Module parameters saved to internal flash memory.");
    break;
  case SAVED_TO_DATAFLASH_ERROR:
    APPLOG("Module parameters saving to internal flash memory error.");
    break;
  }

  APPLOG("Compile time    : %s",__DATE__" "__TIME__);

  rtc_time_t  curr_time;
  rtc_cbl.p_api->calendarTimeGet(rtc_cbl.p_ctrl,&curr_time);
  curr_time.tm_mon++; //
  APPLOG("Date Time: %04d.%02d.%02d  %02d:%02d:%02d", curr_time.tm_year+1900, curr_time.tm_mon, curr_time.tm_mday, curr_time.tm_hour, curr_time.tm_min, curr_time.tm_sec);
  if (rtc_init_res.RTC_status == ACTIVE_INTERNAL_RTC)
  {
    APPLOG("RTC valid. Internal MCU RTC is used.");
  }
  else if (rtc_init_res.RTC_status == ACTIVE_EXTERNAL_RTC)
  {
    APPLOG("RTC valid. External chip AB1815 is used.");
  }

  if (g_nv_ram_couners_valid)
  {
     APPLOG("Counters in NV RAM is valid.");
  }
  else
  {
    APPLOG("Counters in NV RAM is invalid.");
  }

  if (g_dataflash_couners_valid)
  {
     APPLOG("Counters in Data Flash is valid.");
  }
  else
  {
    APPLOG("Counters in Data Flash is invalid.");
  }
}

/*-----------------------------------------------------------------------------------------------------


  \param void

  \return uint8_t
-----------------------------------------------------------------------------------------------------*/
uint8_t Is_IWDT_reset_detected(void)
{
  if (loader_info.rstsr1 & BIT(0))
  {
    loader_info.rstsr1 &= ~BIT(0);
    return RES_YES;
  }
  return RES_NO;
}

/*-----------------------------------------------------------------------------------------------------


  \param void

  \return uint8_t
-----------------------------------------------------------------------------------------------------*/
uint8_t Is_software_reset_detected(void)
{
  if (loader_info.rstsr1 & BIT(2))
  {
    loader_info.rstsr1 &= ~BIT(2);
    return RES_YES;
  }
  return RES_NO;
}

/*-----------------------------------------------------------------------------------------------------


  \param void

  \return uint8_t
-----------------------------------------------------------------------------------------------------*/
uint8_t Is_power_on_reset_detected(void)
{
  if (loader_info.rstsr0 & BIT(0))
  {
    loader_info.rstsr0 &= ~BIT(0);
    return RES_YES;
  }
  return RES_NO;
}

