#include "Test.h"
#include "MKR_RGB_Shield_resources.h"

extern uint8_t         usb_1_interface_type;
extern uint8_t         usb_2_interface_type;

uint32_t               g_main_task_max_duration_us;
uint32_t               g_main_task_max_period_us;


#define MAIN_TASK_STACK_SIZE 2048
static uint8_t main_stacks[MAIN_TASK_STACK_SIZE] BSP_PLACE_IN_SECTION_V2(".stack.main_thread")BSP_ALIGN_VARIABLE_V2(BSP_STACK_ALIGNMENT);
static TX_THREAD       main_thread;


static void     Main_task(ULONG arg);
static uint32_t Create_Main_task(void);

static uint8_t         f_clear_sys_nv_counters;
static uint8_t         flash_bootloder;
T_Test_NV_counters     sys_counters;

uint32_t               disable_IWDT_reset;

uint8_t                g_display_mode;
static uint8_t         g_display_mode_changed;

/*-----------------------------------------------------------------------------------------------------


  \param void

  \return T_NV_parameters_instance*
-----------------------------------------------------------------------------------------------------*/
const T_NV_parameters_instance* Get_app_params_instance(void)
{
  return &wvar_inst;
}

/*-----------------------------------------------------------------------------------------------------


  \param void

  \return char*
-----------------------------------------------------------------------------------------------------*/
char* Get_product_name(void)
{
  return (char *)wvar.name;
}

/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
void Perform_bootloader_flashing(void)
{
  flash_bootloder = 1;
}

/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
void Request_to_clear_system_nv_counters(void)
{
  f_clear_sys_nv_counters = 1;
}

/*-----------------------------------------------------------------------------------------------------
  Перевод модуля в спящий режим с сохранением реакции на прерывания

  \param void
-----------------------------------------------------------------------------------------------------*/
void Goto_Sleep(void)
{

  // Закрываем задачу Motion_thread
  Terminate_motion_task();

  Stop_main_screen();

  Reset_ISM330();
  IMU_Set_exit_low_power_condition();


  Goto_Deep_Software_Standby();
}

/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
void Toggle_display_mode(void)
{
  g_display_mode++;
  if (g_display_mode >= 2)
  {
    g_display_mode = 0;
  }
  g_display_mode_changed = 1;
}


int32_t                  heart_rate;
int32_t                  heart_rate_prev;
float                    cpu_usage_proc;
float                    cpu_usage_proc_prev;

/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
void Screen_process(void)
{
  static T_sys_timestump   ts;

  if (Time_elapsed_msec(&ts) > 100)
  {
    Get_hw_timestump(&ts);


    switch (g_display_mode)
    {

    case 0:
      {
        HR_Get_heart_rate(&heart_rate);

        if (g_display_mode_changed)
        {
          if (heart_rate >= 0)
          {
            MKR_RGB_Shield_print_str("%d",heart_rate);
          }
          else
          {
            MKR_RGB_Shield_print_str("......");
          }
          g_display_mode_changed  = 0;
        }
        else if (heart_rate_prev != heart_rate)
        {
          if (heart_rate >= 0)
          {
            MKR_RGB_Shield_print_str("%d",heart_rate);
          }
          else
          {
            MKR_RGB_Shield_print_str("......");
          }
        }

        heart_rate_prev = heart_rate;
      }
      break;
    case 1:
      if (g_display_mode_changed)
      {
        MKR_RGB_Shield_print_str("%s", wvar.led_matrix_demo_text);
        g_display_mode_changed  = 0;
      }
      break;
    case 2:
      // Вывод на дисплей процента использования CPU
      cpu_usage_proc = g_aver_cpu_usage / 10.0;
      if (g_display_mode_changed)
      {
        MKR_RGB_Shield_print_str("%0.0f", (double)cpu_usage_proc);
        g_display_mode_changed  = 0;
      }
      else if (cpu_usage_proc_prev != cpu_usage_proc)
      {
        MKR_RGB_Shield_print_str("%0.0f", (double)cpu_usage_proc);
      }
      cpu_usage_proc_prev = cpu_usage_proc;
      break;
    }
  }



  //  static uint8_t state = 0;
  //  static T_sys_timestump ts;
  //
  //  switch (state)
  //  {
  //  case 0:
  //    Get_hw_timestump(&ts);
  //    MKR_RGB_Shield_print_str(GX_COLOR_ID_RED, " CPU USAGE ");
  //    state++;
  //    break;
  //  case 1:
  //    if  (((Get_screen_scroll_num() < 0) && (Time_elapsed_msec(&ts) > 1000)) ||  (Get_screen_scroll_num() >= 2))
  //    {
  //      Get_hw_timestump(&ts);
  //      MKR_RGB_Shield_print_str(GX_COLOR_ID_GREEN, "%0.1f", g_aver_cpu_usage / 10.0);
  //      state++;
  //    }
  //    break;
  //  case 2:
  //    if  (((Get_screen_scroll_num() < 0) && (Time_elapsed_msec(&ts) > 1000)) ||  (Get_screen_scroll_num() >= 2))
  //    {
  //      state = 0;
  //    }
  //    break;
  //  }
}

/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
void App_first_function(void)
{
  GuiX_start();
  Screen_process();

}

/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
void Restore_app_settings(void)
{
  Restore_settings(APPLICATION_PARAMS);
}

/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
void App_function(void)
{

  Init_DAC();

  strcpy((char *)ivar.software_version, SOFTWARE_VERSION);
  strcpy((char *)ivar.hardware_version, HARDWARE_VERSION);
  APPLOG("Product  name   : %s", wvar.name);
  APPLOG("Software version: %s", ivar.software_version);
  APPLOG("Hardware version: %s", ivar.hardware_version);

  Create_Main_task();

  Output_SLOW_BLINK(S7V30_BLUE_LED);
  Output_OFF(S7V30_RED_LED);
  Output_OFF(S7V30_GREEN_LED);



  while (1)
  {
    if (Wait_app_event(EVENT_GOTO_SLEEP, TX_AND_CLEAR, MS_TO_TICKS(10)) == RES_OK)
    {
      Goto_Sleep();
    }

    if ((WIFI_AP_link_state() != NULL) || (WIFI_STA_link_state() != NULL) || (RNDIS_link_state() != NULL) || (ECM_host_link_state() != NULL))
    {
      Output_SLOW_BLINK(S7V30_BLUE_LED);
    }
    else
    {
      Output_FAST_BLINK(S7V30_BLUE_LED);
    }

    if (Get_motion_stream_state() != 0)
    {
      Output_FAST_BLINK(S7V30_RED_LED);
    }
    else
    {
      Output_OFF(S7V30_RED_LED);
    }
    if (disable_IWDT_reset == 1)
    {
      IWDT_refreshing_stop();
      disable_IWDT_reset = 0;
    }
    S7V30_Outputs_state_automat();
    Screen_process();
    HRman_state_machine();

    if (BT3_STATE == 0)
    {
      Goto_power_off();
    }
  }
}

/*-----------------------------------------------------------------------------------------------------

  Выполнение теста IPerf
  Результаты при включенной опции максимальной оптимизации по размеру
  Пересылка на сервер по TCP выполнялась на скорости около 40 Мбит в сек
  Пересылка на сервер по UDP выполнялась на скорости около 83 Мбит в сек

  \param arg
-----------------------------------------------------------------------------------------------------*/
static void Main_task(ULONG arg)
{

  Motion_sensor_init();

  #ifdef IP_PERF
  IPerf_init();
  #endif


  do
  {
    Wait_ms(10);

    if (flash_bootloder)
    {
      flash_bootloder = 0;
      Load_and_Flash_Image_File(BOOTLOADER_IMAGE_FILE_NAME);
    }

    if (f_clear_sys_nv_counters)
    {
      f_clear_sys_nv_counters = 0;
      memset(&sys_counters, 0, sizeof(sys_counters));
    }

  } while (1);

}


/*-----------------------------------------------------------------------------------------------------


  \param void

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
static uint32_t Create_Main_task(void)
{
  UINT              err;

  err = tx_thread_create(
                         &main_thread,
                         (CHAR *)"Main",
                         Main_task,
                         0,
                         main_stacks,
                         MAIN_TASK_STACK_SIZE,
                         MAIN_TASK_PRIO,
                         MAIN_TASK_PRIO,
                         1,
                         TX_AUTO_START
                        );

  if (err == TX_SUCCESS)
  {
    APPLOG("Main task created.");
    return RES_OK;
  }
  else
  {
    APPLOG("Main creating error %d.", err);
    return RES_ERROR;
  }
}

