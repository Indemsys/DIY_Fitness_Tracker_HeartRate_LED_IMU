// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 2024-08-24
// 11:38:34
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include   "Test.h"



#define PR(A, B)      offset += Printn_to(buff, sz - offset, offset, A"," , B);
#define PRR(A, B, C)  offset += Printn_to(buff, sz - offset, offset, A"," , B, C);
#define PRV(A)        offset += Printn_to(buff, sz - offset, offset, A",");
#define PRN(A)        offset += Printn_to(buff, sz - offset, offset, A);


/*-----------------------------------------------------------------------------------------------------

  \param sz_ptr

  \return char*
-----------------------------------------------------------------------------------------------------*/
uint32_t  Print_json_system_status(char *buff, uint32_t sz)
{
  uint32_t offset = 0;

  PRN("{");
  PR("\"cpu_id\":\"%s\""                              ,g_cpu_id_str                                   );
  PR("\"up_time\":%d"                                 ,_tx_time_get() / TX_TIMER_TICKS_PER_SECOND     );

  PR("\"total_up_time\":%d"                           ,g_nv_cnts.sys.accumulated_work_time            );

  if (Get_motion_stream_state())
  {
    PRV("\"CMD_start_imu_recording\":1");
    PRV("\"CMD_stop_imu_recording\":0");
  }
  else
  {
    PRV("\"CMD_start_imu_recording\":0");
    PRV("\"CMD_stop_imu_recording\":1");
  }
  PR("\"imu_rec_file_size\":%d"                       ,Get_recorded_log_file_size()                   );

  PR("\"no_accum\":%d"                                ,emb_charger.no_accum                           );
  PR("\"charger_state_str\":\"%s\""                   ,emb_charger.charger_state_str                  );
  PR("\"accum_voltage\":%0.2f"                        ,(double)emb_charger.accum_voltage              );
  PR("\"accum_current\":%0.3f"                        ,(double)emb_charger.accum_current              );
  PR("\"accum_capacity\":%0.1f"                       ,(double)emb_charger.accum_capacity             );
  PR("\"pcb_temp\":%0.1f"                             ,(double)emb_charger.pcb_temp                   );

  PRN("\"E\":1}");

  return offset;
}

