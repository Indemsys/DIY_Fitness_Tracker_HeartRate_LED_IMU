// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 2023-04-27
// 12:31:19
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "Test.h"
#include "freemaster_cfg.h"
#include "freemaster.h"
#include "freemaster_tsa.h"


FMSTR_TSA_TABLE_BEGIN(app_freemaster_tbl)

FMSTR_TSA_RW_VAR(g_cpu_usage                               ,FMSTR_TSA_UINT32)
FMSTR_TSA_RW_VAR(g_aver_cpu_usage                          ,FMSTR_TSA_UINT32)

#ifdef ENABLE_MATLAB_CONNECTION
FMSTR_TSA_RW_VAR(mc_stat.packet_num                        ,FMSTR_TSA_UINT32)
FMSTR_TSA_RW_VAR(mc_stat.oversize_drops_cnt                ,FMSTR_TSA_UINT32)
FMSTR_TSA_RW_VAR(mc_stat.alloc_fail_cnt                    ,FMSTR_TSA_UINT32)
FMSTR_TSA_RW_VAR(mc_stat.queue_fail_cnt                    ,FMSTR_TSA_UINT32)
FMSTR_TSA_RW_VAR(mc_stat.lost_bytes_cnt                    ,FMSTR_TSA_UINT32)
FMSTR_TSA_RW_VAR(mc_stat.sending_fail_cnt                  ,FMSTR_TSA_UINT32)

#endif

FMSTR_TSA_RW_VAR( motion_task_active                       ,FMSTR_TSA_UINT32)
FMSTR_TSA_RW_VAR( motion_stream_active                     ,FMSTR_TSA_UINT32)
FMSTR_TSA_RW_VAR( motion_log_active                        ,FMSTR_TSA_UINT32)
FMSTR_TSA_RW_VAR( motion_smpl.acc_x                        ,FMSTR_TSA_SINT16)
FMSTR_TSA_RW_VAR( motion_smpl.acc_y                        ,FMSTR_TSA_SINT16)
FMSTR_TSA_RW_VAR( motion_smpl.acc_z                        ,FMSTR_TSA_SINT16)
FMSTR_TSA_RW_VAR( motion_smpl.gyro_x                       ,FMSTR_TSA_SINT16)
FMSTR_TSA_RW_VAR( motion_smpl.gyro_y                       ,FMSTR_TSA_SINT16)
FMSTR_TSA_RW_VAR( motion_smpl.gyro_z                       ,FMSTR_TSA_SINT16)

FMSTR_TSA_RW_VAR( imu_src_regs_saved                       ,FMSTR_TSA_UINT8)
FMSTR_TSA_RW_VAR( g_ism330_src_regs.wake_up_src            ,FMSTR_TSA_UINT8)
FMSTR_TSA_RW_VAR( g_ism330_src_regs.tap_src                ,FMSTR_TSA_UINT8)
FMSTR_TSA_RW_VAR( g_ism330_src_regs.d6d_src                ,FMSTR_TSA_UINT8)
FMSTR_TSA_RW_VAR( g_ism330_src_regs.status_reg             ,FMSTR_TSA_UINT8)
FMSTR_TSA_RW_VAR( g_ism330_src_regs.func_src1              ,FMSTR_TSA_UINT8)

FMSTR_TSA_RW_VAR( g_ism330_src_regs_hold.wake_up_src       ,FMSTR_TSA_UINT8)
FMSTR_TSA_RW_VAR( g_ism330_src_regs_hold.tap_src           ,FMSTR_TSA_UINT8)
FMSTR_TSA_RW_VAR( g_ism330_src_regs_hold.d6d_src           ,FMSTR_TSA_UINT8)
FMSTR_TSA_RW_VAR( g_ism330_src_regs_hold.status_reg        ,FMSTR_TSA_UINT8)
FMSTR_TSA_RW_VAR( g_ism330_src_regs_hold.func_src1         ,FMSTR_TSA_UINT8)


FMSTR_TSA_TABLE_END();

