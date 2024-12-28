#ifndef __FREEMASTER_WVAR_H
  #define __FREEMASTER_WVAR_H
#include "Test.h"
#include "freemaster_tsa.h"

FMSTR_TSA_TABLE_BEGIN(wvar_tbl)
FMSTR_TSA_RW_VAR( wvar.automatic_start_motion_log      ,FMSTR_TSA_UINT8     ) // Enable automatic start motion log | def.val.= 0
FMSTR_TSA_RW_VAR( wvar.ble_heart_rate_cccd_handle      ,FMSTR_TSA_UINT16    ) // Heart Rate Monitor CCCD handle | def.val.= 0
FMSTR_TSA_RW_VAR( wvar.ble_heart_rate_sensor_addr_type  ,FMSTR_TSA_UINT8     ) // Heart Rate Monitor address type | def.val.= 0
FMSTR_TSA_RW_VAR( wvar.ble_heart_rate_value_handle     ,FMSTR_TSA_UINT16    ) // Heart Rate Monitor measurement handle | def.val.= 0
FMSTR_TSA_RW_VAR( wvar.en_iperf                        ,FMSTR_TSA_UINT32    ) // Enable IPerf | def.val.= 0
FMSTR_TSA_RW_VAR( wvar.led_matrix_intensity            ,FMSTR_TSA_UINT8     ) // LED matrix intensity (0..31) | def.val.= 1
FMSTR_TSA_RW_VAR( wvar.sleep_condition_code            ,FMSTR_TSA_UINT8     ) // Condition for entering sleep state | def.val.= 4
FMSTR_TSA_RW_VAR( wvar.wake_сondition_сode             ,FMSTR_TSA_UINT8     ) // Condition for exiting sleep state | def.val.= 3
FMSTR_TSA_TABLE_END();


#endif
