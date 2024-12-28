// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 2019.07.11
// 23:40:58
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include   "App.h"
#include   "BLE_main.h"

extern void  App_function(void);

TX_EVENT_FLAGS_GROUP   app_flags;

char                   g_cpu_id_str[CPU_ID_STR_LEN];
uint8_t                g_cpu_id[CPU_ID_LEN];

extern ssp_err_t       sce_initialize(void);

/*-----------------------------------------------------------------------------------------------------


  \param eventmask
-----------------------------------------------------------------------------------------------------*/
void Set_app_event(uint32_t events_mask)
{
  tx_event_flags_set(&app_flags, events_mask, TX_OR);
}

/*-----------------------------------------------------------------------------------------------------


-----------------------------------------------------------------------------------------------------*/
uint32_t Wait_app_event(uint32_t events_mask, uint32_t opt, uint32_t wait)
{
  ULONG flags;
  if (tx_event_flags_get(&app_flags, events_mask, opt,&flags, wait)==TX_SUCCESS)
  {
     return RES_OK;
  }
  return RES_ERROR;
}

/*-----------------------------------------------------------------------------------------------------



  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t Clear_app_event(void)
{
  ULONG flags;
  return tx_event_flags_get(&app_flags, 0xFFFFFFFF, TX_AND_CLEAR, &flags, TX_NO_WAIT);
}


/*-----------------------------------------------------------------------------------------------------


  \param void

  \return T_NV_parameters_instance*
-----------------------------------------------------------------------------------------------------*/
const T_NV_parameters_instance* Get_mod_params_instance(void)
{
  return &ivar_inst;
}

/*-----------------------------------------------------------------------------------------------------


  \param events_mask
  \param p_flags
  \param opt
  \param wait

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t Get_app_events(uint32_t events_mask, ULONG *p_flags,  uint32_t opt, uint32_t wait)
{
  return tx_event_flags_get(&app_flags, events_mask, opt, p_flags, wait);
}

/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
void Public_names_modifier(void)
{
  char str[17];

  // Имя хоста не изменяем чтобы всегда работал URL по константному имени хоста
  // memset(str, 0, sizeof(str));
  // strncpy(str, (char*)ivar.this_host_name, 5); // От оригинального имени из параметров берем только первые 5 симвлов
  // strncat(str, g_cpu_id_str, 8); // Добавляем 8 первых символов от уникального номера чипа
  // strcpy((char*)ivar.this_host_name, str);

  memset(g_wifi_ap_ssid, 0, sizeof(str));
  strncpy(g_wifi_ap_ssid, (char*)ivar.wifi_ap_ssid, 5);
  strncat(g_wifi_ap_ssid, g_cpu_id_str, 8);

}

/*-----------------------------------------------------------------------------------------------------


  \param void

  \return __weak void
-----------------------------------------------------------------------------------------------------*/
__weak void App_first_function(void)
{

}

/*-----------------------------------------------------------------------------------------------------
  
  
  \param void  
  
  \return __weak void 
-----------------------------------------------------------------------------------------------------*/
__weak void Restore_app_settings(void)
{

}
/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
void Main_thread(ULONG arg)
{
  IWDT_refreshing_start();
  ELS_soft_event1_set_and_enable_interrupts();

  // Разрешаем запись в регистры пинов, проскольку ранее она была запрещена драйвером
  R_PMISC->PWPR_b.BOWI  = 0;
  R_PMISC->PWPR_b.PFSWE = 1;

  RED_LED   = 1;
  GREEN_LED = 1;
  BLUE_LED  = 1;

  g_fmi.p_api->init();                                                 // 9.2   us
  tx_event_flags_create(&app_flags, "APP");                            // 1.28  us
  SCI8_SPI_init();                                                     // 34    us SPI8 используется для управления часами реального времени и чипами ЦАП
  sce_initialize();                                                    // 53    us Инициализация движка шифрования и генерации случайных чисел. Применяется неявно в DNS
  g_sf_crypto.p_api->open(g_sf_crypto.p_ctrl, g_sf_crypto.p_cfg);      // 6     us
  Flash_driver_bgo_init();                                             // 24    us

  App_first_function();

  Init_RTC();                                                          // 880   us
  if  (Init_SD_card_file_system() == RES_OK) g_file_system_ready = 1;  // 71000 us
  Restore_settings(MODULE_PARAMS);                                     // 57000 us
  Restore_NV_counters_from_DataFlash();                                // 9500  us
  Restore_app_settings(); 
  Get_CPU_UID(g_cpu_id_str, g_cpu_id, CPU_ID_STR_LEN);                 // 24    us
  Public_names_modifier();                                             //          Модификация имени хоста, чтобы все имена были уникальными
  Generate_CRC32_table();                                              // 116   us Таблица CRC32 нужна для загрузчика образа прошивки

  // Фиксируем время завершения старта основного цикла
  Get_hw_timestump(&g_main_thread_start_timestump);

  Get_reference_time();                     // 10 ms

  VT100_task_manager_initialization();      // Инициализируем движок VT100 терминала до того как каналы коммуникации cмогут создать задачи VT100

  WIFi_control_init();
  Charger_thread_create();

  Init_app_logger();
  System_start_report();


  Create_Backgroung_task();
  Accept_certificates_from_file();

  Init_USB_stack();

  Create_File_Logger_task();
  Thread_Net_create();
  BLE_init();

  Thread_FreeMaster_create();

  App_function();
}

