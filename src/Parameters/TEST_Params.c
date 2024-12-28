#include "Test.h"
#include "freemaster_tsa.h"

#define  WVAR_SIZE        13
#define  SELECTORS_NUM    3

WVAR_TYPE  wvar;


static const T_parmenu parmenu[5]=
{
{ APP_0                       , APP_main                    , "Parameters and settings                 ", "PARAMETERS          ", -1   }, // Основная категория
{ APP_main                    , APP_General                 , "General settings                        ", "GENERAL_SETTINGS    ", -1   }, // 
{ APP_main                    , APP_IMU                     , "Motion log settings                     ", "                    ", -1   }, // 
{ APP_main                    , APP_HeartRate               , "Heart rate sensor params                ", "                    ", -1   }, // 
{ APP_main                    , APP_Display                 , "Display settings                        ", "                    ", -1   }, // 
};


static const T_NV_parameters arr_wvar[WVAR_SIZE]=
{
// N: 0
  {
    "led_matrix_intensity",
    "LED matrix intensity (0..31)",
    "-",
    (void*)&wvar.led_matrix_intensity,
    tint8u,
    1,
    0,
    31,
    0,
    APP_Display,
    "",
    "%d",
    0,
    sizeof(wvar.led_matrix_intensity),
    0,
    0,
  },
// N: 1
  {
    "led_matrix_text_color",
    "Display color (R G B)",
    "-",
    (void*)&wvar.led_matrix_text_color,
    tstring,
    0,
    0,
    0,
    0,
    APP_Display,
    "64 00 00 ",
    "%s",
    0,
    sizeof(wvar.led_matrix_text_color)-1,
    1,
    0,
  },
// N: 2
  {
    "led_matrix_demo_text",
    "Demo text ",
    "-",
    (void*)&wvar.led_matrix_demo_text,
    tstring,
    0,
    0,
    0,
    0,
    APP_Display,
    "HELLO",
    "%s",
    0,
    sizeof(wvar.led_matrix_demo_text)-1,
    2,
    0,
  },
// N: 3
  {
    "name",
    "Product  name",
    "SYSNAM",
    (void*)&wvar.name,
    tstring,
    0,
    0,
    0,
    0,
    APP_General,
    "S7V30",
    "%s",
    0,
    sizeof(wvar.name)-1,
    1,
    0,
  },
// N: 4
  {
    "manuf_date",
    "Manufacturing date",
    "FRMVER",
    (void*)&wvar.manuf_date,
    tstring,
    0,
    0,
    0,
    0,
    APP_General,
    "2023 02 28 ",
    "%s",
    0,
    sizeof(wvar.manuf_date)-1,
    2,
    0,
  },
// N: 5
  {
    "en_iperf",
    "Enable IPerf",
    "-",
    (void*)&wvar.en_iperf,
    tint32u,
    0,
    0,
    1,
    0,
    APP_General,
    "",
    "%d",
    0,
    sizeof(wvar.en_iperf),
    3,
    0,
  },
// N: 6
  {
    "wake_сondition_сode",
    "Condition for exiting sleep state",
    "-",
    (void*)&wvar.wake_сondition_сode,
    tint8u,
    3,
    0,
    5,
    0,
    APP_General,
    "",
    "%d",
    0,
    sizeof(wvar.wake_сondition_сode),
    4,
    2,
  },
// N: 7
  {
    "sleep_condition_code",
    "Condition for entering sleep state",
    "-",
    (void*)&wvar.sleep_condition_code,
    tint8u,
    4,
    0,
    5,
    0,
    APP_General,
    "",
    "%d",
    0,
    sizeof(wvar.sleep_condition_code),
    5,
    2,
  },
// N: 8
  {
    "ble_heart_rate_sensor_addr",
    "Heart Rate Monitor address",
    "-",
    (void*)&wvar.ble_heart_rate_sensor_addr,
    tstring,
    0,
    0,
    0,
    0,
    APP_HeartRate,
    "00 00 00 00 00 00",
    "%s",
    0,
    sizeof(wvar.ble_heart_rate_sensor_addr)-1,
    1,
    0,
  },
// N: 9
  {
    "ble_heart_rate_sensor_addr_type",
    "Heart Rate Monitor address type",
    "-",
    (void*)&wvar.ble_heart_rate_sensor_addr_type,
    tint8u,
    0,
    0,
    255,
    0,
    APP_HeartRate,
    "",
    "%d",
    0,
    sizeof(wvar.ble_heart_rate_sensor_addr_type),
    2,
    0,
  },
// N: 10
  {
    "ble_heart_rate_value_handle",
    "Heart Rate Monitor measurement handle",
    "-",
    (void*)&wvar.ble_heart_rate_value_handle,
    tint16u,
    0,
    0,
    65535,
    0,
    APP_HeartRate,
    "",
    "%d",
    0,
    sizeof(wvar.ble_heart_rate_value_handle),
    3,
    0,
  },
// N: 11
  {
    "ble_heart_rate_cccd_handle",
    "Heart Rate Monitor CCCD handle",
    "-",
    (void*)&wvar.ble_heart_rate_cccd_handle,
    tint16u,
    0,
    0,
    65535,
    0,
    APP_HeartRate,
    "",
    "%d",
    0,
    sizeof(wvar.ble_heart_rate_cccd_handle),
    4,
    0,
  },
// N: 12
  {
    "automatic_start_motion_log",
    "Enable automatic start motion log",
    "-",
    (void*)&wvar.automatic_start_motion_log,
    tint8u,
    0,
    0,
    1,
    0,
    APP_IMU,
    "",
    "%d",
    0,
    sizeof(wvar.automatic_start_motion_log),
    7,
    1,
  },
};
 
 
// Selector description:  Выбор между Yes и No
static const T_selector_items selector_1[2] = 
{
  { 0 , "No                                          " , 0},
  { 1 , "Yes                                         " , 1},
};
 
// Selector description:  Условя приводящие к переключению состояния системы
static const T_selector_items selector_2[6] = 
{
  { 0 , "Wake Up condition                           " , -1},
  { 1 , "Change orientation                          " , -1},
  { 2 , "Single tap                                  " , -1},
  { 3 , "Double tap                                  " , -1},
  { 4 , "Free fall                                   " , -1},
  { 5 , "None                                        " , -1},
};
 
static const T_selectors_list selectors_list[SELECTORS_NUM] = 
{
  {"string"                      , 0    , 0             },
  {"binary"                      , 2    , selector_1    },
  {"imu_codition"                , 6    , selector_2    },
};
 
const T_NV_parameters_instance wvar_inst =
{
  WVAR_SIZE,
  arr_wvar,
  5,
  parmenu,
  SELECTORS_NUM,
  selectors_list
};
