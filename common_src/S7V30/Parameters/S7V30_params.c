﻿#include "App.h"
#include "freemaster_tsa.h"

#define  IVAR_SIZE        81
#define  SELECTORS_NUM    10

IVAR_TYPE  ivar;


static const T_parmenu parmenu[18]=
{
{ S7V30BOOT_0                 , S7V30BOOT_main              , "Parameters and settings                 ", "PARAMETERS          ", -1   }, // Основная категория
{ S7V30BOOT_WIFI_STA          , S7V30BOOT_WIFI_STA1         , "WiFi Station Configuration 1            ", "                    ", -1   }, // 
{ S7V30BOOT_WIFI_STA          , S7V30BOOT_WIFI_STA2         , "WiFi Station Configuration 2            ", "                    ", -1   }, // 
{ S7V30BOOT_main              , S7V30BOOT_General           , "General settings                        ", "GENERAL_SETTINGS    ", -1   }, // 
{ S7V30BOOT_main              , S7V30BOOT_USB_Interface     , "USB Interface settings                  ", "                    ", -1   }, // 
{ S7V30BOOT_main              , S7V30BOOT_Network           , "Network settings                        ", "                    ", -1   }, // 
{ S7V30BOOT_main              , S7V30BOOT_BLE               , "Bluetooth settings                      ", "                    ", -1   }, // 
{ S7V30BOOT_main              , S7V30BOOT_WIFI_STA          , "WiFi Station interface settings         ", "                    ", -1   }, // 
{ S7V30BOOT_main              , S7V30BOOT_WIFI_AP           , "WiFi Access Point interface settings    ", "                    ", -1   }, // 
{ S7V30BOOT_main              , S7V30BOOT_Telnet            , "Telnet settings                         ", "                    ", -1   }, // 
{ S7V30BOOT_main              , S7V30BOOT_WEB               , "HTTP server settings                    ", "                    ", -1   }, // 
{ S7V30BOOT_main              , S7V30BOOT_FTP_server        , "FTP server settings                     ", "                    ", -1   }, // 
{ S7V30BOOT_main              , S7V30BOOT_MQTT              , "MQTT settings                           ", "                    ", -1   }, // 
{ S7V30BOOT_main              , S7V30BOOT_SNTP              , "Net time protocol setting               ", "                    ", -1   }, // 
{ S7V30BOOT_main              , S7V30BOOT_FreeMaster        , "FreeMaster communication settings       ", "                    ", -1   }, // 
{ S7V30BOOT_main              , S7V30BOOT_Audio             , "Audio settings                          ", "                    ", -1   }, // 
{ S7V30BOOT_main              , S7V30BOOT_Charger           , "Embedded charger settings               ", "                    ", -1   }, // 
{ S7V30BOOT_main              , S7V30BOOT_IMU               , "IMU module settings                     ", "                    ", -1   }, // 
};


static const T_NV_parameters arr_ivar[IVAR_SIZE]=
{
// N: 0
  {
    "audio_volume",
    "Volume (0...10)",
    "-",
    (void*)&ivar.audio_volume,
    tint32u,
    8,
    0,
    10,
    0,
    S7V30BOOT_Audio,
    "",
    "%d",
    0,
    sizeof(ivar.audio_volume),
    0,
    0,
  },
// N: 1
  {
    "en_ble",
    "Enable Bluetooth",
    "-",
    (void*)&ivar.en_ble,
    tint8u,
    1,
    0,
    1,
    0,
    S7V30BOOT_BLE,
    "",
    "%d",
    0,
    sizeof(ivar.en_ble),
    0,
    1,
  },
// N: 2
  {
    "bt_device_name",
    "Bluetooth Classic device name",
    "-",
    (void*)&ivar.bt_device_name,
    tstring,
    0,
    0,
    0,
    0,
    S7V30BOOT_BLE,
    "BT_S7V30",
    "%s",
    0,
    sizeof(ivar.bt_device_name)-1,
    1,
    0,
  },
// N: 3
  {
    "ble_device_name",
    "Bluetooth LE device name",
    "-",
    (void*)&ivar.ble_device_name,
    tstring,
    0,
    0,
    0,
    0,
    S7V30BOOT_BLE,
    "BLE_S7V30",
    "%s",
    0,
    sizeof(ivar.ble_device_name)-1,
    2,
    0,
  },
// N: 4
  {
    "en_embedded_charger",
    "Enable embedded Li-Po charger",
    "-",
    (void*)&ivar.en_embedded_charger,
    tint8u,
    0,
    0,
    1,
    0,
    S7V30BOOT_Charger,
    "",
    "%d",
    0,
    sizeof(ivar.en_embedded_charger),
    0,
    1,
  },
// N: 5
  {
    "accum_nominal_capacity",
    "Accumulator capacity, mAh",
    "-",
    (void*)&ivar.accum_nominal_capacity,
    tfloat,
    500.00,
    100.00,
    6000.00,
    0,
    S7V30BOOT_Charger,
    "",
    "%0.0f",
    0,
    sizeof(ivar.accum_nominal_capacity),
    1,
    0,
  },
// N: 6
  {
    "emb_charger_max_current",
    "Max charging current, mAh",
    "-",
    (void*)&ivar.emb_charger_max_current,
    tint32u,
    500,
    100,
    1000,
    0,
    S7V30BOOT_Charger,
    "",
    "%d",
    0,
    sizeof(ivar.emb_charger_max_current),
    2,
    0,
  },
// N: 7
  {
    "en_freemaster",
    "Enable FreeMaster protocol",
    "-",
    (void*)&ivar.en_freemaster,
    tint8u,
    0,
    0,
    1,
    0,
    S7V30BOOT_FreeMaster,
    "",
    "%d",
    0,
    sizeof(ivar.en_freemaster),
    0,
    1,
  },
// N: 8
  {
    "en_log_to_freemaster",
    "Enable logging to FreeMaster pipe",
    "-",
    (void*)&ivar.en_log_to_freemaster,
    tint8u,
    0,
    0,
    1,
    0,
    S7V30BOOT_FreeMaster,
    "",
    "%d",
    0,
    sizeof(ivar.en_log_to_freemaster),
    1,
    1,
  },
// N: 9
  {
    "enable_ftp_server",
    "Enable FTP server",
    "-",
    (void*)&ivar.enable_ftp_server,
    tint8u,
    1,
    0,
    1,
    0,
    S7V30BOOT_FTP_server,
    "",
    "%d",
    0,
    sizeof(ivar.enable_ftp_server),
    1,
    1,
  },
// N: 10
  {
    "ftp_serv_login",
    "Login",
    "-",
    (void*)&ivar.ftp_serv_login,
    tstring,
    0,
    0,
    0,
    0,
    S7V30BOOT_FTP_server,
    "ftp_login",
    "%s",
    0,
    sizeof(ivar.ftp_serv_login)-1,
    2,
    0,
  },
// N: 11
  {
    "ftp_serv_password",
    "Password ",
    "-",
    (void*)&ivar.ftp_serv_password,
    tstring,
    0,
    0,
    0,
    4,
    S7V30BOOT_FTP_server,
    "ftp_pass",
    "%s",
    0,
    sizeof(ivar.ftp_serv_password)-1,
    3,
    0,
  },
// N: 12
  {
    "product_name",
    "Product  name",
    "SYSNAM",
    (void*)&ivar.product_name,
    tstring,
    0,
    0,
    0,
    0,
    S7V30BOOT_General,
    "S7V30",
    "%s",
    0,
    sizeof(ivar.product_name)-1,
    1,
    0,
  },
// N: 13
  {
    "software_version",
    "Software version",
    "-",
    (void*)&ivar.software_version,
    tstring,
    0,
    0,
    0,
    0,
    S7V30BOOT_General,
    "S7V30",
    "%s",
    0,
    sizeof(ivar.software_version)-1,
    2,
    0,
  },
// N: 14
  {
    "hardware_version",
    "Hardware version",
    "-",
    (void*)&ivar.hardware_version,
    tstring,
    0,
    0,
    0,
    0,
    S7V30BOOT_General,
    "S7V30V2 ",
    "%s",
    0,
    sizeof(ivar.hardware_version)-1,
    3,
    0,
  },
// N: 15
  {
    "en_wifi_module",
    "Enable  Wi-Fi Bluetooth module ",
    "-",
    (void*)&ivar.en_wifi_module,
    tint8u,
    1,
    0,
    1,
    0,
    S7V30BOOT_General,
    "",
    "%d",
    0,
    sizeof(ivar.en_wifi_module),
    4,
    1,
  },
// N: 16
  {
    "en_compress_settins",
    "Enable compress settings file",
    "-",
    (void*)&ivar.en_compress_settins,
    tint8u,
    1,
    0,
    1,
    0,
    S7V30BOOT_General,
    "",
    "%d",
    0,
    sizeof(ivar.en_compress_settins),
    5,
    1,
  },
// N: 17
  {
    "en_formated_settings",
    "Enable formating in  settings file",
    "-",
    (void*)&ivar.en_formated_settings,
    tint8u,
    1,
    0,
    1,
    0,
    S7V30BOOT_General,
    "",
    "%d",
    0,
    sizeof(ivar.en_formated_settings),
    6,
    1,
  },
// N: 18
  {
    "sd_card_password",
    "SD card password",
    "-",
    (void*)&ivar.sd_card_password,
    tstring,
    0,
    0,
    0,
    4,
    S7V30BOOT_General,
    "1234",
    "%s",
    0,
    sizeof(ivar.sd_card_password)-1,
    7,
    0,
  },
// N: 19
  {
    "en_log_to_file",
    "Enable logging to file",
    "-",
    (void*)&ivar.en_log_to_file,
    tint8u,
    0,
    0,
    1,
    0,
    S7V30BOOT_General,
    "",
    "%d",
    0,
    sizeof(ivar.en_log_to_file),
    8,
    1,
  },
// N: 20
  {
    "en_engineering_log",
    "Enable engineering log",
    "-",
    (void*)&ivar.en_engineering_log,
    tint8u,
    0,
    0,
    1,
    0,
    S7V30BOOT_General,
    "",
    "%d",
    0,
    sizeof(ivar.en_engineering_log),
    9,
    1,
  },
// N: 21
  {
    "motion_logfile_name",
    "Motion logfile name",
    "-",
    (void*)&ivar.motion_logfile_name,
    tstring,
    0,
    0,
    0,
    0,
    S7V30BOOT_IMU,
    "motion_log",
    "%s",
    0,
    sizeof(ivar.motion_logfile_name)-1,
    0,
    0,
  },
// N: 22
  {
    "accelerometer_scale",
    "Accelerometer full-scale selection",
    "-",
    (void*)&ivar.accelerometer_scale,
    tint8u,
    0,
    0,
    3,
    0,
    S7V30BOOT_IMU,
    "",
    "%d",
    0,
    sizeof(ivar.accelerometer_scale),
    1,
    7,
  },
// N: 23
  {
    "gyroscope_scale",
    "Gyroscope full-scale selection",
    "-",
    (void*)&ivar.gyroscope_scale,
    tint8u,
    0,
    0,
    3,
    0,
    S7V30BOOT_IMU,
    "",
    "%d",
    0,
    sizeof(ivar.gyroscope_scale),
    2,
    8,
  },
// N: 24
  {
    "imu_output_data_rate",
    "Output Data Rate selection",
    "-",
    (void*)&ivar.imu_output_data_rate,
    tint8u,
    0,
    0,
    8,
    0,
    S7V30BOOT_IMU,
    "",
    "%d",
    0,
    sizeof(ivar.imu_output_data_rate),
    3,
    9,
  },
// N: 25
  {
    "imu_fifo_rd_period",
    "Period of reading FIFO (ms)",
    "-",
    (void*)&ivar.imu_fifo_rd_period,
    tint32u,
    50,
    10,
    1000,
    0,
    S7V30BOOT_IMU,
    "",
    "%d",
    0,
    sizeof(ivar.imu_fifo_rd_period),
    4,
    0,
  },
// N: 26
  {
    "imu_tap_threshold",
    "Tap recognition threshold (0..31) ",
    "-",
    (void*)&ivar.imu_tap_threshold,
    tint8u,
    1,
    0,
    31,
    0,
    S7V30BOOT_IMU,
    "",
    "%d",
    0,
    sizeof(ivar.imu_tap_threshold),
    5,
    0,
  },
// N: 27
  {
    "imu_tap_shock_window",
    "Maximum duration of overthreshold event for tap(ms)",
    "-",
    (void*)&ivar.imu_tap_shock_window,
    tint32u,
    10,
    10,
    1000,
    0,
    S7V30BOOT_IMU,
    "",
    "%d",
    0,
    sizeof(ivar.imu_tap_shock_window),
    6,
    0,
  },
// N: 28
  {
    "imu_tap_quiet_time",
    "Expected quiet time after a tap detection (ms)",
    "-",
    (void*)&ivar.imu_tap_quiet_time,
    tint32u,
    50,
    10,
    1000,
    0,
    S7V30BOOT_IMU,
    "",
    "%d",
    0,
    sizeof(ivar.imu_tap_quiet_time),
    7,
    0,
  },
// N: 29
  {
    "imu_max_tap_time_gap",
    "Maximum time gap for double tap recognition (ms)",
    "-",
    (void*)&ivar.imu_max_tap_time_gap,
    tint32u,
    200,
    10,
    1000,
    0,
    S7V30BOOT_IMU,
    "",
    "%d",
    0,
    sizeof(ivar.imu_max_tap_time_gap),
    8,
    0,
  },
// N: 30
  {
    "imu_wakeup_threshold",
    "Wake Up threshold (0..63)",
    "-",
    (void*)&ivar.imu_wakeup_threshold,
    tint8u,
    1,
    0,
    63,
    0,
    S7V30BOOT_IMU,
    "",
    "%d",
    0,
    sizeof(ivar.imu_wakeup_threshold),
    9,
    0,
  },
// N: 31
  {
    "imu_wakeup_duration",
    "Wake up duration (ms)",
    "-",
    (void*)&ivar.imu_wakeup_duration,
    tint32u,
    10,
    1,
    1000,
    0,
    S7V30BOOT_IMU,
    "",
    "%d",
    0,
    sizeof(ivar.imu_wakeup_duration),
    10,
    0,
  },
// N: 32
  {
    "imu_free_fall_threshold",
    "Threshold for free-fall function",
    "-",
    (void*)&ivar.imu_free_fall_threshold,
    tint32u,
    0,
    0,
    7,
    0,
    S7V30BOOT_IMU,
    "",
    "%d",
    0,
    sizeof(ivar.imu_free_fall_threshold),
    11,
    0,
  },
// N: 33
  {
    "imu_free_fall_duration",
    "Free fall duration duration (ms)",
    "-",
    (void*)&ivar.imu_free_fall_duration,
    tint32u,
    10,
    10,
    1000,
    0,
    S7V30BOOT_IMU,
    "",
    "%d",
    0,
    sizeof(ivar.imu_free_fall_duration),
    12,
    0,
  },
// N: 34
  {
    "imu_6d_threshold",
    "Threshold for 4D/6D function",
    "-",
    (void*)&ivar.imu_6d_threshold,
    tint32u,
    0,
    0,
    3,
    0,
    S7V30BOOT_IMU,
    "",
    "%d",
    0,
    sizeof(ivar.imu_6d_threshold),
    13,
    0,
  },
// N: 35
  {
    "mqtt_enable",
    "Enable MQTT client ",
    "-",
    (void*)&ivar.mqtt_enable,
    tint8u,
    0,
    0,
    1,
    0,
    S7V30BOOT_MQTT,
    "",
    "%d",
    0,
    sizeof(ivar.mqtt_enable),
    1,
    1,
  },
// N: 36
  {
    "mqtt_client_id",
    "Client ID",
    "-",
    (void*)&ivar.mqtt_client_id,
    tstring,
    0,
    0,
    0,
    0,
    S7V30BOOT_MQTT,
    "Client1",
    "%s",
    0,
    sizeof(ivar.mqtt_client_id)-1,
    2,
    0,
  },
// N: 37
  {
    "mqtt_server_ip",
    "MQTT server IP address",
    "-",
    (void*)&ivar.mqtt_server_ip,
    tstring,
    0,
    0,
    0,
    0,
    S7V30BOOT_MQTT,
    "192.168.3.2",
    "%s",
    0,
    sizeof(ivar.mqtt_server_ip)-1,
    3,
    0,
  },
// N: 38
  {
    "mqtt_server_port",
    "MQTT server port number",
    "-",
    (void*)&ivar.mqtt_server_port,
    tint32u,
    1883,
    0,
    65535,
    0,
    S7V30BOOT_MQTT,
    "",
    "%d",
    0,
    sizeof(ivar.mqtt_server_port),
    4,
    0,
  },
// N: 39
  {
    "mqtt_user_name",
    "User name",
    "-",
    (void*)&ivar.mqtt_user_name,
    tstring,
    0,
    0,
    0,
    0,
    S7V30BOOT_MQTT,
    "user",
    "%s",
    0,
    sizeof(ivar.mqtt_user_name)-1,
    5,
    0,
  },
// N: 40
  {
    "mqtt_password",
    "User password",
    "-",
    (void*)&ivar.mqtt_password,
    tstring,
    0,
    0,
    0,
    4,
    S7V30BOOT_MQTT,
    "pass",
    "%s",
    0,
    sizeof(ivar.mqtt_password)-1,
    6,
    0,
  },
// N: 41
  {
    "this_host_name",
    "This device host name",
    "HOSTNAM",
    (void*)&ivar.this_host_name,
    tstring,
    0,
    0,
    0,
    0,
    S7V30BOOT_Network,
    "S7V30",
    "%s",
    0,
    sizeof(ivar.this_host_name)-1,
    0,
    0,
  },
// N: 42
  {
    "en_net_log",
    "Enable log",
    "-",
    (void*)&ivar.en_net_log,
    tint8u,
    1,
    0,
    1,
    0,
    S7V30BOOT_Network,
    "",
    "%d",
    0,
    sizeof(ivar.en_net_log),
    1,
    1,
  },
// N: 43
  {
    "en_sntp",
    "Enable SNTP client",
    "-",
    (void*)&ivar.en_sntp,
    tint8u,
    1,
    0,
    1,
    0,
    S7V30BOOT_SNTP,
    "",
    "%d",
    0,
    sizeof(ivar.en_sntp),
    1,
    1,
  },
// N: 44
  {
    "en_sntp_time_receiving",
    "Allow to receive time from time servers",
    "-",
    (void*)&ivar.en_sntp_time_receiving,
    tint8u,
    1,
    0,
    1,
    0,
    S7V30BOOT_SNTP,
    "",
    "%d",
    0,
    sizeof(ivar.en_sntp_time_receiving),
    2,
    1,
  },
// N: 45
  {
    "utc_offset",
    "UTC offset (difference in hours +-)",
    "-",
    (void*)&ivar.utc_offset,
    tfloat,
    3.00,
    -12.00,
    12.00,
    0,
    S7V30BOOT_SNTP,
    "",
    "%0.0f",
    0,
    sizeof(ivar.utc_offset),
    3,
    0,
  },
// N: 46
  {
    "time_server_1",
    "Time server 1 URL",
    "-",
    (void*)&ivar.time_server_1,
    tstring,
    0,
    0,
    0,
    0,
    S7V30BOOT_SNTP,
    "pool.ntp.org",
    "%s",
    0,
    sizeof(ivar.time_server_1)-1,
    4,
    0,
  },
// N: 47
  {
    "time_server_2",
    "Time server 2 URL",
    "-",
    (void*)&ivar.time_server_2,
    tstring,
    0,
    0,
    0,
    0,
    S7V30BOOT_SNTP,
    "129.6.15.28",
    "%s",
    0,
    sizeof(ivar.time_server_2)-1,
    5,
    0,
  },
// N: 48
  {
    "time_server_3",
    "Time serber 3 URL",
    "-",
    (void*)&ivar.time_server_3,
    tstring,
    0,
    0,
    0,
    0,
    S7V30BOOT_SNTP,
    "time.nist.gov",
    "%s",
    0,
    sizeof(ivar.time_server_3)-1,
    6,
    0,
  },
// N: 49
  {
    "sntp_poll_interval",
    "Poll interval (s)",
    "-",
    (void*)&ivar.sntp_poll_interval,
    tint32u,
    10,
    1,
    1000000,
    0,
    S7V30BOOT_SNTP,
    "",
    "%d",
    0,
    sizeof(ivar.sntp_poll_interval),
    7,
    0,
  },
// N: 50
  {
    "en_telnet",
    "Enable Telnet",
    "-",
    (void*)&ivar.en_telnet,
    tint8u,
    1,
    0,
    1,
    0,
    S7V30BOOT_Telnet,
    "",
    "%d",
    0,
    sizeof(ivar.en_telnet),
    0,
    1,
  },
// N: 51
  {
    "usb_mode",
    "USB interface mode",
    "USBIMOD",
    (void*)&ivar.usb_mode,
    tint32u,
    1,
    0,
    7,
    0,
    S7V30BOOT_USB_Interface,
    "",
    "%d",
    0,
    sizeof(ivar.usb_mode),
    1,
    4,
  },
// N: 52
  {
    "usd_dev_interface",
    "USB device interface (HS/FS)",
    "-",
    (void*)&ivar.usd_dev_interface,
    tint8u,
    0,
    0,
    1,
    0,
    S7V30BOOT_USB_Interface,
    "",
    "%d",
    0,
    sizeof(ivar.usd_dev_interface),
    2,
    6,
  },
// N: 53
  {
    "en_rndis_dhcp_server",
    "Enable DHCP server on RNDIS interface ",
    "RNDSCFG",
    (void*)&ivar.en_rndis_dhcp_server,
    tint8u,
    1,
    0,
    1,
    0,
    S7V30BOOT_USB_Interface,
    "",
    "%d",
    0,
    sizeof(ivar.en_rndis_dhcp_server),
    3,
    1,
  },
// N: 54
  {
    "en_ecm_host_dhcp_client",
    "Enable DHCP client on ECM host interface ",
    "-",
    (void*)&ivar.en_ecm_host_dhcp_client,
    tint8u,
    1,
    0,
    1,
    0,
    S7V30BOOT_USB_Interface,
    "",
    "%d",
    0,
    sizeof(ivar.en_ecm_host_dhcp_client),
    4,
    1,
  },
// N: 55
  {
    "usb_default_ip_addr",
    "Default IP address",
    "DEFIPAD",
    (void*)&ivar.usb_default_ip_addr,
    tstring,
    0,
    0,
    0,
    0,
    S7V30BOOT_USB_Interface,
    "192.168.13.1",
    "%s",
    0,
    sizeof(ivar.usb_default_ip_addr)-1,
    5,
    2,
  },
// N: 56
  {
    "usb_default_net_mask",
    "Default network mask ",
    "DEFNTMS",
    (void*)&ivar.usb_default_net_mask,
    tstring,
    0,
    0,
    0,
    0,
    S7V30BOOT_USB_Interface,
    "255.255.255.0",
    "%s",
    0,
    sizeof(ivar.usb_default_net_mask)-1,
    6,
    2,
  },
// N: 57
  {
    "enable_HTTP_server",
    "Enable HTTP server",
    "-",
    (void*)&ivar.enable_HTTP_server,
    tint8u,
    1,
    0,
    1,
    0,
    S7V30BOOT_WEB,
    "",
    "%d",
    0,
    sizeof(ivar.enable_HTTP_server),
    0,
    1,
  },
// N: 58
  {
    "enable_HTTPS",
    "Enable TLS",
    "-",
    (void*)&ivar.enable_HTTPS,
    tint8u,
    1,
    0,
    1,
    0,
    S7V30BOOT_WEB,
    "",
    "%d",
    0,
    sizeof(ivar.enable_HTTPS),
    1,
    1,
  },
// N: 59
  {
    "HTTP_server_password",
    "HTTP server password",
    "-",
    (void*)&ivar.HTTP_server_password,
    tstring,
    0,
    0,
    0,
    4,
    S7V30BOOT_WEB,
    "123456789",
    "%d",
    0,
    sizeof(ivar.HTTP_server_password)-1,
    2,
    0,
  },
// N: 60
  {
    "en_wifi_ap",
    "Enable Acces Point mode",
    "-",
    (void*)&ivar.en_wifi_ap,
    tint8u,
    1,
    0,
    1,
    0,
    S7V30BOOT_WIFI_AP,
    "",
    "%d",
    0,
    sizeof(ivar.en_wifi_ap),
    0,
    1,
  },
// N: 61
  {
    "wifi_ap_ssid",
    "Access Point SSID",
    "-",
    (void*)&ivar.wifi_ap_ssid,
    tstring,
    0,
    0,
    0,
    0,
    S7V30BOOT_WIFI_AP,
    "S7V30",
    "%s",
    0,
    sizeof(ivar.wifi_ap_ssid)-1,
    1,
    0,
  },
// N: 62
  {
    "wifi_ap_key",
    "Access Point password (>=8 symbols)",
    "-",
    (void*)&ivar.wifi_ap_key,
    tstring,
    0,
    0,
    0,
    4,
    S7V30BOOT_WIFI_AP,
    "12345678",
    "%s",
    0,
    sizeof(ivar.wifi_ap_key)-1,
    2,
    0,
  },
// N: 63
  {
    "ap_default_ip_addr",
    "Default IP address",
    "-",
    (void*)&ivar.ap_default_ip_addr,
    tstring,
    0,
    0,
    0,
    0,
    S7V30BOOT_WIFI_AP,
    "192.168.10.1",
    "%s",
    0,
    sizeof(ivar.ap_default_ip_addr)-1,
    3,
    0,
  },
// N: 64
  {
    "ap_default_net_mask",
    "Default network mask ",
    "-",
    (void*)&ivar.ap_default_net_mask,
    tstring,
    0,
    0,
    0,
    0,
    S7V30BOOT_WIFI_AP,
    "255.255.255.0",
    "%s",
    0,
    sizeof(ivar.ap_default_net_mask)-1,
    4,
    0,
  },
// N: 65
  {
    "ap_en_dhcp_server",
    "Enable DHCP server",
    "-",
    (void*)&ivar.ap_en_dhcp_server,
    tint8u,
    1,
    0,
    1,
    0,
    S7V30BOOT_WIFI_AP,
    "",
    "%d",
    0,
    sizeof(ivar.ap_en_dhcp_server),
    5,
    1,
  },
// N: 66
  {
    "wifi_ap_channel",
    "WIFI channell",
    "-",
    (void*)&ivar.wifi_ap_channel,
    tint8u,
    1,
    0,
    255,
    0,
    S7V30BOOT_WIFI_AP,
    "",
    "%d",
    0,
    sizeof(ivar.wifi_ap_channel),
    6,
    0,
  },
// N: 67
  {
    "wifi_sta_cfg1_en",
    "Enable configuration",
    "-",
    (void*)&ivar.wifi_sta_cfg1_en,
    tint8u,
    0,
    0,
    1,
    0,
    S7V30BOOT_WIFI_STA1,
    "",
    "%d",
    0,
    sizeof(ivar.wifi_sta_cfg1_en),
    0,
    1,
  },
// N: 68
  {
    "wifi_sta_cfg1_pass",
    "Password ",
    "-",
    (void*)&ivar.wifi_sta_cfg1_pass,
    tstring,
    0,
    0,
    0,
    4,
    S7V30BOOT_WIFI_STA1,
    "wifi_pass",
    "%s",
    0,
    sizeof(ivar.wifi_sta_cfg1_pass)-1,
    1,
    0,
  },
// N: 69
  {
    "wifi_sta_cfg1_ssid",
    "SSID",
    "-",
    (void*)&ivar.wifi_sta_cfg1_ssid,
    tstring,
    0,
    0,
    0,
    0,
    S7V30BOOT_WIFI_STA1,
    "wifi_login",
    "%d",
    0,
    sizeof(ivar.wifi_sta_cfg1_ssid)-1,
    2,
    0,
  },
// N: 70
  {
    "wifi_sta_cfg1_en_dhcp",
    "Enable DHCP client",
    "-",
    (void*)&ivar.wifi_sta_cfg1_en_dhcp,
    tint8u,
    1,
    0,
    1,
    0,
    S7V30BOOT_WIFI_STA1,
    "",
    "%d",
    0,
    sizeof(ivar.wifi_sta_cfg1_en_dhcp),
    3,
    1,
  },
// N: 71
  {
    "wifi_sta_cfg1_default_ip_addr",
    "Default IP address",
    "-",
    (void*)&ivar.wifi_sta_cfg1_default_ip_addr,
    tstring,
    0,
    0,
    0,
    0,
    S7V30BOOT_WIFI_STA1,
    "192.168.11.1",
    "%s",
    0,
    sizeof(ivar.wifi_sta_cfg1_default_ip_addr)-1,
    4,
    2,
  },
// N: 72
  {
    "wifi_sta_cfg1_default_net_mask",
    "Default network mask ",
    "-",
    (void*)&ivar.wifi_sta_cfg1_default_net_mask,
    tstring,
    0,
    0,
    0,
    0,
    S7V30BOOT_WIFI_STA1,
    "255.255.255.0",
    "%s",
    0,
    sizeof(ivar.wifi_sta_cfg1_default_net_mask)-1,
    5,
    2,
  },
// N: 73
  {
    "wifi_sta_cfg1_default_gate_addr",
    "Default gateway address",
    "-",
    (void*)&ivar.wifi_sta_cfg1_default_gate_addr,
    tstring,
    0,
    0,
    0,
    0,
    S7V30BOOT_WIFI_STA1,
    "192.168.11.1",
    "%s",
    0,
    sizeof(ivar.wifi_sta_cfg1_default_gate_addr)-1,
    6,
    2,
  },
// N: 74
  {
    "wifi_sta_cfg2_en",
    "Enable configuration",
    "-",
    (void*)&ivar.wifi_sta_cfg2_en,
    tint8u,
    0,
    0,
    1,
    0,
    S7V30BOOT_WIFI_STA2,
    "",
    "%d",
    0,
    sizeof(ivar.wifi_sta_cfg2_en),
    0,
    1,
  },
// N: 75
  {
    "wifi_sta_cfg2_pass",
    "Password ",
    "-",
    (void*)&ivar.wifi_sta_cfg2_pass,
    tstring,
    0,
    0,
    0,
    4,
    S7V30BOOT_WIFI_STA2,
    "wifi_pass",
    "%s",
    0,
    sizeof(ivar.wifi_sta_cfg2_pass)-1,
    1,
    0,
  },
// N: 76
  {
    "wifi_sta_cfg2_ssid",
    "SSID",
    "-",
    (void*)&ivar.wifi_sta_cfg2_ssid,
    tstring,
    0,
    0,
    0,
    0,
    S7V30BOOT_WIFI_STA2,
    "wifi_login",
    "%s",
    0,
    sizeof(ivar.wifi_sta_cfg2_ssid)-1,
    2,
    0,
  },
// N: 77
  {
    "wifi_sta_cfg2_en_dhcp",
    "Enable DHCP client",
    "-",
    (void*)&ivar.wifi_sta_cfg2_en_dhcp,
    tint8u,
    1,
    0,
    1,
    0,
    S7V30BOOT_WIFI_STA2,
    "",
    "%d",
    0,
    sizeof(ivar.wifi_sta_cfg2_en_dhcp),
    3,
    1,
  },
// N: 78
  {
    "wifi_sta_cfg2_default_ip_addr",
    "Default IP address",
    "-",
    (void*)&ivar.wifi_sta_cfg2_default_ip_addr,
    tstring,
    0,
    0,
    0,
    0,
    S7V30BOOT_WIFI_STA2,
    "192.168.12.1",
    "%s",
    0,
    sizeof(ivar.wifi_sta_cfg2_default_ip_addr)-1,
    4,
    0,
  },
// N: 79
  {
    "wifi_sta_cfg2_default_net_mask",
    "Default network mask ",
    "-",
    (void*)&ivar.wifi_sta_cfg2_default_net_mask,
    tstring,
    0,
    0,
    0,
    0,
    S7V30BOOT_WIFI_STA2,
    "255.255.255.0",
    "%s",
    0,
    sizeof(ivar.wifi_sta_cfg2_default_net_mask)-1,
    5,
    0,
  },
// N: 80
  {
    "wifi_sta_cfg2_default_gate_addr",
    "Default gateway address",
    "-",
    (void*)&ivar.wifi_sta_cfg2_default_gate_addr,
    tstring,
    0,
    0,
    0,
    0,
    S7V30BOOT_WIFI_STA2,
    "192.168.12.1",
    "%s",
    0,
    sizeof(ivar.wifi_sta_cfg2_default_gate_addr)-1,
    6,
    0,
  },
};
 
 
// Selector description:  Выбор между Yes и No
static const T_selector_items selector_1[2] = 
{
  { 0 , "No                                          " , 0},
  { 1 , "Yes                                         " , 1},
};
 
// Selector description:  LED mode
static const T_selector_items selector_3[2] = 
{
  { 0 , "Always OFF                                  " , -1},
  { 1 , "Normal work                                 " , -1},
};
 
// Selector description:  USB mode
static const T_selector_items selector_4[8] = 
{
  { 0 , "None                                        " , -1},
  { 1 , "VCOM port                                   " , -1},
  { 2 , "Mass storage                                " , -1},
  { 3 , "VCOM and Mass storage                       " , -1},
  { 5 , "VCOM and FreeMaster port                    " , -1},
  { 6 , "RNDIS                                       " , -1},
  { 7 , "Host ECM                                    " , -1},
  { 4 , "VCOM and VCOM                               " , -1},
};
 
// Selector description:  IP address assignment method
static const T_selector_items selector_5[2] = 
{
  { 0 , "Static adress                               " , -1},
  { 1 , "DHCP server                                 " , -1},
};
 
// Selector description:  Выбор интерфейса для работы USB device
static const T_selector_items selector_6[2] = 
{
  { 0 , "High speed interface                        " , -1},
  { 1 , "Full speed interface                        " , -1},
};
 
// Selector description:  Accelerometer full-scale selection
static const T_selector_items selector_7[4] = 
{
  { 0 , "2g                                          " , -1},
  { 1 , "4g                                          " , -1},
  { 2 , "8g                                          " , -1},
  { 3 , "16g                                         " , -1},
};
 
// Selector description:  Gyroscope full-scale selection
static const T_selector_items selector_8[4] = 
{
  { 0 , "250dps                                      " , -1},
  { 1 , "500dps                                      " , -1},
  { 2 , "1000dps                                     " , -1},
  { 3 , "2000dps                                     " , -1},
};
 
// Selector description:  Output Data Rate
static const T_selector_items selector_9[9] = 
{
  { 0 , "1666Hz                                      " , -1},
  { 1 , "833Hz                                       " , -1},
  { 2 , "416Hz                                       " , -1},
  { 3 , "208Hz                                       " , -1},
  { 4 , "104Hz                                       " , -1},
  { 5 , "52                                          " , -1},
  { 6 , "26                                          " , -1},
  { 7 , "12.5                                        " , -1},
  { 8 , "1.6                                         " , -1},
};
 
static const T_selectors_list selectors_list[SELECTORS_NUM] = 
{
  {"string"                      , 0    , 0             },
  {"binary"                      , 2    , selector_1    },
  {"ip_addr"                     , 0    , 0             },
  {"leds_mode"                   , 2    , selector_3    },
  {"usb_mode"                    , 8    , selector_4    },
  {"IP_address_assignment_method", 2    , selector_5    },
  {"usb_dev_interface"           , 2    , selector_6    },
  {"accelerometer_scale"         , 4    , selector_7    },
  {"gyroscope_scale"             , 4    , selector_8    },
  {"imu_output_data_rate"        , 9    , selector_9    },
};
 
const T_NV_parameters_instance ivar_inst =
{
  IVAR_SIZE,
  arr_ivar,
  18,
  parmenu,
  SELECTORS_NUM,
  selectors_list
};
