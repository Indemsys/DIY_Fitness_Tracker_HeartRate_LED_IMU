#ifndef __FREEMASTER_IVAR_H
  #define __FREEMASTER_IVAR_H
#include "App.h"
#include "freemaster_tsa.h"

FMSTR_TSA_TABLE_BEGIN(ivar_tbl)
FMSTR_TSA_RW_VAR( ivar.accelerometer_scale             ,FMSTR_TSA_UINT8     ) // Accelerometer full-scale selection | def.val.= 0
FMSTR_TSA_RW_VAR( ivar.accum_nominal_capacity          ,FMSTR_TSA_FLOAT     ) // Accumulator capacity, mAh | def.val.= 500
FMSTR_TSA_RW_VAR( ivar.ap_en_dhcp_server               ,FMSTR_TSA_UINT8     ) // Enable DHCP server | def.val.= 1
FMSTR_TSA_RW_VAR( ivar.audio_volume                    ,FMSTR_TSA_UINT32    ) // Volume (0...10) | def.val.= 8
FMSTR_TSA_RW_VAR( ivar.emb_charger_max_current         ,FMSTR_TSA_UINT32    ) // Max charging current, mAh | def.val.= 500
FMSTR_TSA_RW_VAR( ivar.en_ble                          ,FMSTR_TSA_UINT8     ) // Enable Bluetooth | def.val.= 1
FMSTR_TSA_RW_VAR( ivar.en_compress_settins             ,FMSTR_TSA_UINT8     ) // Enable compress settings file | def.val.= 1
FMSTR_TSA_RW_VAR( ivar.en_ecm_host_dhcp_client         ,FMSTR_TSA_UINT8     ) // Enable DHCP client on ECM host interface  | def.val.= 1
FMSTR_TSA_RW_VAR( ivar.en_embedded_charger             ,FMSTR_TSA_UINT8     ) // Enable embedded Li-Po charger | def.val.= 0
FMSTR_TSA_RW_VAR( ivar.en_engineering_log              ,FMSTR_TSA_UINT8     ) // Enable engineering log | def.val.= 0
FMSTR_TSA_RW_VAR( ivar.en_formated_settings            ,FMSTR_TSA_UINT8     ) // Enable formating in  settings file | def.val.= 1
FMSTR_TSA_RW_VAR( ivar.en_freemaster                   ,FMSTR_TSA_UINT8     ) // Enable FreeMaster protocol | def.val.= 0
FMSTR_TSA_RW_VAR( ivar.en_log_to_file                  ,FMSTR_TSA_UINT8     ) // Enable logging to file | def.val.= 0
FMSTR_TSA_RW_VAR( ivar.en_log_to_freemaster            ,FMSTR_TSA_UINT8     ) // Enable logging to FreeMaster pipe | def.val.= 0
FMSTR_TSA_RW_VAR( ivar.en_net_log                      ,FMSTR_TSA_UINT8     ) // Enable log | def.val.= 1
FMSTR_TSA_RW_VAR( ivar.en_rndis_dhcp_server            ,FMSTR_TSA_UINT8     ) // Enable DHCP server on RNDIS interface  | def.val.= 1
FMSTR_TSA_RW_VAR( ivar.en_sntp                         ,FMSTR_TSA_UINT8     ) // Enable SNTP client | def.val.= 1
FMSTR_TSA_RW_VAR( ivar.en_sntp_time_receiving          ,FMSTR_TSA_UINT8     ) // Allow to receive time from time servers | def.val.= 1
FMSTR_TSA_RW_VAR( ivar.en_telnet                       ,FMSTR_TSA_UINT8     ) // Enable Telnet | def.val.= 1
FMSTR_TSA_RW_VAR( ivar.en_wifi_ap                      ,FMSTR_TSA_UINT8     ) // Enable Acces Point mode | def.val.= 1
FMSTR_TSA_RW_VAR( ivar.en_wifi_module                  ,FMSTR_TSA_UINT8     ) // Enable  Wi-Fi Bluetooth module  | def.val.= 1
FMSTR_TSA_RW_VAR( ivar.enable_ftp_server               ,FMSTR_TSA_UINT8     ) // Enable FTP server | def.val.= 1
FMSTR_TSA_RW_VAR( ivar.enable_HTTP_server              ,FMSTR_TSA_UINT8     ) // Enable HTTP server | def.val.= 1
FMSTR_TSA_RW_VAR( ivar.enable_HTTPS                    ,FMSTR_TSA_UINT8     ) // Enable TLS | def.val.= 1
FMSTR_TSA_RW_VAR( ivar.gyroscope_scale                 ,FMSTR_TSA_UINT8     ) // Gyroscope full-scale selection | def.val.= 0
FMSTR_TSA_RW_VAR( ivar.imu_6d_threshold                ,FMSTR_TSA_UINT32    ) // Threshold for 4D/6D function | def.val.= 0
FMSTR_TSA_RW_VAR( ivar.imu_fifo_rd_period              ,FMSTR_TSA_UINT32    ) // Period of reading FIFO (ms) | def.val.= 50
FMSTR_TSA_RW_VAR( ivar.imu_free_fall_duration          ,FMSTR_TSA_UINT32    ) // Free fall duration duration (ms) | def.val.= 10
FMSTR_TSA_RW_VAR( ivar.imu_free_fall_threshold         ,FMSTR_TSA_UINT32    ) // Threshold for free-fall function | def.val.= 0
FMSTR_TSA_RW_VAR( ivar.imu_max_tap_time_gap            ,FMSTR_TSA_UINT32    ) // Maximum time gap for double tap recognition (ms) | def.val.= 200
FMSTR_TSA_RW_VAR( ivar.imu_output_data_rate            ,FMSTR_TSA_UINT8     ) // Output Data Rate selection | def.val.= 0
FMSTR_TSA_RW_VAR( ivar.imu_tap_quiet_time              ,FMSTR_TSA_UINT32    ) // Expected quiet time after a tap detection (ms) | def.val.= 50
FMSTR_TSA_RW_VAR( ivar.imu_tap_shock_window            ,FMSTR_TSA_UINT32    ) // Maximum duration of overthreshold event for tap(ms) | def.val.= 10
FMSTR_TSA_RW_VAR( ivar.imu_tap_threshold               ,FMSTR_TSA_UINT8     ) // Tap recognition threshold (0..31)  | def.val.= 1
FMSTR_TSA_RW_VAR( ivar.imu_wakeup_duration             ,FMSTR_TSA_UINT32    ) // Wake up duration (ms) | def.val.= 10
FMSTR_TSA_RW_VAR( ivar.imu_wakeup_threshold            ,FMSTR_TSA_UINT8     ) // Wake Up threshold (0..63) | def.val.= 1
FMSTR_TSA_RW_VAR( ivar.mqtt_enable                     ,FMSTR_TSA_UINT8     ) // Enable MQTT client  | def.val.= 0
FMSTR_TSA_RW_VAR( ivar.mqtt_server_port                ,FMSTR_TSA_UINT32    ) // MQTT server port number | def.val.= 1883
FMSTR_TSA_RW_VAR( ivar.sntp_poll_interval              ,FMSTR_TSA_UINT32    ) // Poll interval (s) | def.val.= 10
FMSTR_TSA_RW_VAR( ivar.usb_mode                        ,FMSTR_TSA_UINT32    ) // USB interface mode | def.val.= 1
FMSTR_TSA_RW_VAR( ivar.usd_dev_interface               ,FMSTR_TSA_UINT8     ) // USB device interface (HS/FS) | def.val.= 0
FMSTR_TSA_RW_VAR( ivar.utc_offset                      ,FMSTR_TSA_FLOAT     ) // UTC offset (difference in hours +-) | def.val.= 3
FMSTR_TSA_RW_VAR( ivar.wifi_ap_channel                 ,FMSTR_TSA_UINT8     ) // WIFI channell | def.val.= 1
FMSTR_TSA_RW_VAR( ivar.wifi_sta_cfg1_en                ,FMSTR_TSA_UINT8     ) // Enable configuration | def.val.= 0
FMSTR_TSA_RW_VAR( ivar.wifi_sta_cfg1_en_dhcp           ,FMSTR_TSA_UINT8     ) // Enable DHCP client | def.val.= 1
FMSTR_TSA_RW_VAR( ivar.wifi_sta_cfg2_en                ,FMSTR_TSA_UINT8     ) // Enable configuration | def.val.= 0
FMSTR_TSA_RW_VAR( ivar.wifi_sta_cfg2_en_dhcp           ,FMSTR_TSA_UINT8     ) // Enable DHCP client | def.val.= 1
FMSTR_TSA_TABLE_END();


#endif
