#ifndef HEARTRATE_MAN_H
  #define HEARTRATE_MAN_H


typedef struct
{
    uint8_t           has_data;
    uint16_t          heart_rate;           // bpm
    uint16_t          energy_expended;      // J
    uint16_t          rr_interval;          // ms
    uint8_t           heart_rate_format;    // 0: Heart Rate Value Format is UINT8, 1: Heart Rate Value Format is UINT16
    uint8_t           sensor_contact_st;    // 0: Sensor Contact feature is not supported in the current connection, 1: Sensor Contact feature is supported in the current connection
    uint8_t           sensor_contact_dt;    // 0: Sensor Contact is not detected, 1: Sensor Contact is detected
    uint8_t           energy_expend_sts;    // 0: Energy Expended field is not present, 1: Energy Expended field is present
    uint8_t           rr_interval_prest;    // 0: RR-Interval values are not present, 1: One or more RR-Interval values are present
    T_sys_timestump   timestump;
} T_HR_data;

void     App_ble_init(void);
void     HRman_init(void);
void     HRman_state_machine(void);
void     HR_Decode(uint8_t *attr_buf, T_HR_data *hr_ptr);
void     HR_Get_heart_rate(int32_t *hrate_ptr);


#endif



