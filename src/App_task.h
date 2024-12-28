#ifndef APP_TASK_H
  #define APP_TASK_H

typedef struct
{
  uint32_t    dummy_cnt;

} T_Test_NV_counters;


extern uint8_t                g_display_mode;

void                            App_first_function(void);
void                            Restore_app_settings(void);
void                            App_function(void);
const T_NV_parameters_instance* Get_app_params_instance(void);
char*                           Get_product_name(void);
void                            Perform_bootloader_flashing(void);
void                            Request_to_clear_system_nv_counters(void);
void                            Toggle_display_mode(void);


#endif



