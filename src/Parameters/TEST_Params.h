#ifndef __PARAMS_WVAR_H
  #define __PARAMS_WVAR_H

#define  APP_0                          0
#define  APP_main                       1
#define  APP_General                    2
#define  APP_HeartRate                  3
#define  APP_IMU                        4
#define  APP_Display                    5

typedef struct
{
  uint8_t        automatic_start_motion_log;    // Enable automatic start motion log | def.val.= 0
  uint16_t       ble_heart_rate_cccd_handle;    // Heart Rate Monitor CCCD handle | def.val.= 0
  uint8_t        ble_heart_rate_sensor_addr[18+1]; // Heart Rate Monitor address | def.val.= 00 00 00 00 00 00
  uint8_t        ble_heart_rate_sensor_addr_type; // Heart Rate Monitor address type | def.val.= 0
  uint16_t       ble_heart_rate_value_handle;   // Heart Rate Monitor measurement handle | def.val.= 0
  uint32_t       en_iperf;                      // Enable IPerf | def.val.= 0
  uint8_t        led_matrix_demo_text[32+1];    // Demo text  | def.val.= HELLO
  uint8_t        led_matrix_intensity;          // LED matrix intensity (0..31) | def.val.= 1
  uint8_t        led_matrix_text_color[12+1];   // Display color (R G B) | def.val.= 64 00 00 
  uint8_t        manuf_date[64+1];              // Manufacturing date | def.val.= 2023 02 28 
  uint8_t        name[64+1];                    // Product  name | def.val.= S7V30
  uint8_t        sleep_condition_code;          // Condition for entering sleep state | def.val.= 4
  uint8_t        wake_сondition_сode;           // Condition for exiting sleep state | def.val.= 3
} WVAR_TYPE;



// Selector description:  Выбор между Yes и No
#define BINARY_NO                                 0
#define BINARY_YES                                1

// Selector description:  Условя приводящие к переключению состояния системы
#define IMU_CODITION_WAKE_UP_CONDITION            0
#define IMU_CODITION_CHANGE_ORIENTATION           1
#define IMU_CODITION_SINGLE_TAP                   2
#define IMU_CODITION_DOUBLE_TAP                   3
#define IMU_CODITION_FREE_FALL                    4
#define IMU_CODITION_NONE                         5


extern WVAR_TYPE  wvar;
extern const T_NV_parameters_instance wvar_inst;

#endif


