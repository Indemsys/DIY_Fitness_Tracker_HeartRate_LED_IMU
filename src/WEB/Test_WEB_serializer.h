#ifndef TEST_WEB_SERIALIZER_H
  #define TEST_WEB_SERIALIZER_H


typedef struct
{
    char            cpu_id_str[CPU_ID_STR_LEN];
    uint32_t        up_time;
    uint8_t         app_control_enable;
    uint32_t        left_arm_low_pos;
    uint32_t        left_arm_up_pos;
    uint32_t        left_arm_mid_pos;
    uint32_t        right_arm_low_pos;
    uint32_t        right_arm_up_pos;
    uint32_t        right_arm_mid_pos;
    uint32_t        platf_upper_pos;
    uint32_t        platf_down_pos;
    uint32_t        platf_overload_pos;
    float           accum_capacity;
    float           accum_idle_voltage;
    float           accum_max_voltage;
    float           accum_max_current;
    float           aver_SYS_V;
    float           aver_PWR_I;
    float           aver_LOAD_I;
    float           aver_CHARG_I;
    float           aver_DISCHRG_I;
    float           aver_ACC_V;
    double          charge_balance;
    float           charge_level;
    int8_t          X01;
    int8_t          X02;
    int8_t          X03;
    int8_t          X04;
    int8_t          X05;
    int8_t          X06;
    int8_t          X07;
    int8_t          X08;
    int8_t          X09;
    int8_t          X10;
    int8_t          X11;
    int8_t          X12;
    int8_t          X13;
    int8_t          X31__S5;
    int8_t          X32__S6;
    int8_t          X33__S7;
    int8_t          X34__S21;
    int8_t          X35__S23;
    int8_t          X36_BOT_SW;
    int8_t          X31_S5_P90;
    int8_t          X32_S6_P45;
    int8_t          X33_S7_P0;
    int8_t          X34_S21_LLS;
    int8_t          X35_S23_ULS;
    int8_t          esc_end;
    int8_t          access_key;
    int8_t          btn_alarm;
    int8_t          btn_up;
    int8_t          btn_down;
    int8_t          btn_fold;
    int8_t          btn_unfold;
    int8_t          welding_check;
    uint16_t        left_arm_pos_sens;
    uint16_t        right_arm_pos_sens;
    uint16_t        inclinometer;

    const char     *PLATFMode_str;
    const char     *MOTORMode_str;
    const char     *ChargerMode_str;

    const char     *left_arm_motor_state;
    const char     *right_arm_motor_state;
    const char     *platform_motor_state;
    const char     *traction_motor_state;

} T_app_snapshot;

uint32_t  Print_json_system_status(char *buff, uint32_t sz);

#endif



