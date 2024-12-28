#ifndef MOTION_SENSOR_H
  #define MOTION_SENSOR_H

typedef struct
{
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
    int16_t acc_x;
    int16_t acc_y;
    int16_t acc_z;
} T_motion_smpl;



typedef enum
{
    STATE_IDLE,
    STATE_XH_TRIGGERED
} T_SM_mode_enum;

typedef struct
{
    T_SM_mode_enum state;
    T_sys_timestump timestamp;
    void (*action)(void);
} T_SM_controller;



extern T_motion_smpl      motion_smpl;
extern uint8_t            imu_src_regs_saved;
extern uint32_t           motion_task_active;
extern uint32_t           motion_stream_active;
extern uint32_t           motion_log_active;




ssp_err_t  Reset_ISM330(void);
void       IMU_INT2_enable_interrupts(void);
void       IMU_INT2_disable_interrupts(void);
uint32_t   Motion_sensor_init(void);
uint32_t   Save_motion_buffer(uint32_t buf_indx);
uint32_t   Create_motion_file(void);
uint32_t   Close_motion_file(void);
int32_t    Get_recorded_log_file_size(void);
uint32_t   Get_motion_sens_error(void);
uint32_t   Start_IMU_stream(void);
uint32_t   Stop_IMU_stream(void);
uint32_t   Start_motion_logfile(void);
uint32_t   Stop_motion_logfile(void);
uint8_t    Get_motion_stream_state(void);
void       Terminate_motion_task(void);
ssp_err_t  IMU_Set_exit_low_power_condition(void);

#endif // MOTION_SENSOR_H



