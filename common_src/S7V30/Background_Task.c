// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 2023-02-20
// 16:37:01
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include   "App.h"
#include   "BLE_main.h"

#define CPU_USAGE_FLTR_LEN  128

uint32_t               g_ref_time;             // Калибровочная константа предназначенная для измерения нагрузки микропроцессора
T_run_average_int32_N  filter_cpu_usage;
volatile uint32_t      g_aver_cpu_usage;
volatile uint32_t      g_cpu_usage;            // Процент загрузки процессора
int32_t                cpu_usage_arr[CPU_USAGE_FLTR_LEN];

#define BACKGROUND_TASK_STACK_SIZE (1024*3)
static uint8_t background_stacks[BACKGROUND_TASK_STACK_SIZE] BSP_PLACE_IN_SECTION_V2(".stack.background")BSP_ALIGN_VARIABLE_V2(BSP_STACK_ALIGNMENT);
static TX_THREAD       background_thread;

static TX_EVENT_FLAGS_GROUP   bkg_flags;
static TX_MUTEX               bkg_mutex;

#define CMD_TO_SAVE_APP_PARAMETERS               BIT(0)
#define CMD_TO_SAVE_MOD_PARAMETERS               BIT(1)
#define CMD_TO_SAVE_BT_NV_DATA                   BIT(2)
#define CMD_TO_SAVE_NV_COUNTERS_TO_DATAFLASH     BIT(3)
#define CMD_TO_SAVE_NV_COUNTERS_TO_NVRAM         BIT(4)

typedef struct
{
    uint8_t                         request_mask;
} T_params_saving_req;

T_params_saving_req ps_req;

/*-----------------------------------------------------------------------------------------------------
  Получаем  оценку калибровочного интервала времени предназначенного для измерения загрузки процессора

  Проводим несколько измерений и выбираем минимальный интервал
-----------------------------------------------------------------------------------------------------*/
void Get_reference_time(void)
{
  uint32_t i;
  uint32_t t;
  uint32_t tt = 0xFFFFFFFF;

  for (i = 0; i < 2; i++)
  {
    t = (uint32_t)Measure_reference_time_interval(REF_TIME_INTERVAL);
    if (t < tt) tt = t;
  }
  g_ref_time = tt;
}



/*-----------------------------------------------------------------------------------------------------
  Измеряем длительность интервала времени ti заданного в милисекундах
-----------------------------------------------------------------------------------------------------*/
uint64_t Measure_reference_time_interval(uint32_t time_delay_ms)
{
  T_sys_timestump   tickt1;
  T_sys_timestump   tickt2;
  uint64_t diff;


  Get_hw_timestump(&tickt1);
  DELAY_ms(time_delay_ms);
  Get_hw_timestump(&tickt2);

  diff = Hw_timestump_diff64_us(&tickt1,&tickt2);

  return diff;
}

/*-----------------------------------------------------------------------------------------------------
  Зарегистрировать запрос на сохранение параметров

 \param void
-----------------------------------------------------------------------------------------------------*/
void Request_save_nv_parameters(uint8_t ptype)
{
  if (tx_mutex_get(&bkg_mutex, MS_TO_TICKS(1000)) == TX_SUCCESS)
  {
    if (ptype == APPLICATION_PARAMS)
    {
      ps_req.request_mask     |= CMD_TO_SAVE_APP_PARAMETERS;
    }
    else if (ptype == MODULE_PARAMS)
    {
      ps_req.request_mask     |= CMD_TO_SAVE_MOD_PARAMETERS;
    }
    tx_mutex_put(&bkg_mutex);
  }
}

/*-----------------------------------------------------------------------------------------------------


  \param pinst
  \param ptype
-----------------------------------------------------------------------------------------------------*/
void Request_save_bt_nv_data(void)
{
  if (tx_mutex_get(&bkg_mutex, MS_TO_TICKS(1000)) == TX_SUCCESS)
  {
    ps_req.request_mask     |= CMD_TO_SAVE_BT_NV_DATA;
    tx_mutex_put(&bkg_mutex);
  }
}

/*-----------------------------------------------------------------------------------------------------
  Отправка запроса на процедуру сохранения NV данных в DataFlash микроконтролера
  Вызывается приблизительно каждый час, чтобы не зависеть от способности внешних часов реального времени удерживать данные


  \param void
-----------------------------------------------------------------------------------------------------*/
void Request_save_nv_counters_to_DataFlash(void)
{
  if (tx_mutex_get(&bkg_mutex, MS_TO_TICKS(1000)) == TX_SUCCESS)
  {
    ps_req.request_mask     |= CMD_TO_SAVE_NV_COUNTERS_TO_DATAFLASH;
    tx_mutex_put(&bkg_mutex);
  }
}

/*-----------------------------------------------------------------------------------------------------
  Отправка запроса на процедуру сохранения NV данных в RAM внешних часов реального времени
  Вызывается приблизительно каждую секунду, поскольку используется для учета времени наработки

  \param void
-----------------------------------------------------------------------------------------------------*/
void Request_save_nv_counters_to_NVRAM(void)
{
  if (tx_mutex_get(&bkg_mutex, MS_TO_TICKS(1000)) == TX_SUCCESS)
  {
    ps_req.request_mask     |= CMD_TO_SAVE_NV_COUNTERS_TO_NVRAM;
    tx_mutex_put(&bkg_mutex);
  }
}
/*-----------------------------------------------------------------------------------------------------
  Функция сохраняющая данные во внутреннюю память DataFlash
  Во избежание конфликтов это должно быть единственным местом вызывающим запись, стирание и чтение DataFlash
  после запуска многозадачности

  \param void

  \return uint8_t
-----------------------------------------------------------------------------------------------------*/
static void  NV_data_saver(void)
{
  if  (ps_req.request_mask != 0)
  {
    if (tx_mutex_get(&bkg_mutex, MS_TO_TICKS(1000)) == TX_SUCCESS)
    {

      if (ps_req.request_mask & CMD_TO_SAVE_APP_PARAMETERS)
      {
        if (Save_settings_to(APPLICATION_PARAMS, MEDIA_TYPE_DATAFLASH, 0) == RES_OK)
        {
          EAPPLOG("Application settings saved successfully");
        }
        else
        {
          EAPPLOG("Application settings saving error");
        }
        ps_req.request_mask &= ~CMD_TO_SAVE_APP_PARAMETERS;
      }

      if (ps_req.request_mask & CMD_TO_SAVE_MOD_PARAMETERS)
      {
        if (Save_settings_to(MODULE_PARAMS, MEDIA_TYPE_DATAFLASH, 0) == RES_OK)
        {
          EAPPLOG("Communication settings saved successfully");
        }
        else
        {
          EAPPLOG("Communication settings saving error");
        }
        ps_req.request_mask &= ~CMD_TO_SAVE_MOD_PARAMETERS;
      }

      if (ps_req.request_mask & CMD_TO_SAVE_BT_NV_DATA)
      {
        if (Save_buf_to_DataFlash(DATAFLASH_BLUETOOTH_DATA_ADDR, (uint8_t *)&bt_nv, sizeof(bt_nv)) == RES_OK)
        {
          EAPPLOG("Bluetooth NV data saved successfully");
        }
        else
        {
          EAPPLOG("Bluetooth NV data saving error");
        }
        ps_req.request_mask &= ~CMD_TO_SAVE_BT_NV_DATA;
      }

      if (ps_req.request_mask & CMD_TO_SAVE_NV_COUNTERS_TO_DATAFLASH)
      {
        if (Save_NV_counters_to_DataFlash()==RES_OK)
        {
          EAPPLOG("NV counters saved successfully");
        }
        else
        {
          EAPPLOG("NV counters saving error");
        }
        ps_req.request_mask &= ~CMD_TO_SAVE_NV_COUNTERS_TO_DATAFLASH;
      }

      if (ps_req.request_mask & CMD_TO_SAVE_NV_COUNTERS_TO_NVRAM)
      {
        Save_NV_counters_to_NVRAM();
        ps_req.request_mask &= ~CMD_TO_SAVE_NV_COUNTERS_TO_NVRAM;
      }
      tx_mutex_put(&bkg_mutex);
    }
    else
    {
      EAPPLOG("NV data saving error. ");
    }
  }
}

/*-----------------------------------------------------------------------------------------------------


  \param flag

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t Send_flag_to_background(uint32_t flag)
{
  return  tx_event_flags_set(&bkg_flags, flag, TX_OR);
}


/*-----------------------------------------------------------------------------------------------------


  \param arg
-----------------------------------------------------------------------------------------------------*/
static void Background_task(ULONG arg)
{
  uint32_t         t;
  uint32_t         dt;
  T_sys_timestump  ts1, ts2;
  uint32_t         tdelta;
  uint32_t         tdelta_sum = 0;

  g_cpu_usage      = 1000;
  g_aver_cpu_usage = 1000;

  Get_hw_timestump(&ts1);

  tx_event_flags_create(&bkg_flags, "bkg_flags");
  tx_mutex_create(&bkg_mutex, "bkg_mutex", TX_INHERIT);

  filter_cpu_usage.len = CPU_USAGE_FLTR_LEN;
  filter_cpu_usage.en  = 0;
  filter_cpu_usage.arr = cpu_usage_arr;
  g_aver_cpu_usage = RunAverageFilter_int32_N(g_cpu_usage,&filter_cpu_usage);

  for (;;)
  {
    t = Measure_reference_time_interval(REF_TIME_INTERVAL);

    if (t < g_ref_time)
    {
      dt = 0;
    }
    else
    {
      dt = t - g_ref_time;
    }
    g_cpu_usage = (1000ul * dt) / g_ref_time;
    g_aver_cpu_usage = RunAverageFilter_int32_N(g_cpu_usage,&filter_cpu_usage);

    NV_data_saver();

    // Подсчет времени работы системы
    Get_hw_timestump(&ts2);
    tdelta = Timestump_diff_to_msec(&ts1,&ts2);
    if (tdelta>=1000)
    {
      // Каждую секунду сохранияем счетчик времени наработки в RAM чипа часов
      ts1= ts2;
      g_nv_cnts.sys.accumulated_work_time += tdelta/1000;
      Request_save_nv_counters_to_NVRAM();
      tdelta_sum += tdelta;
      if (tdelta_sum > 60*60*1000)
      {
        // Каждый час сохранияем счетчик времени наработки в DataFlash
        tdelta_sum = 0;
        Request_save_nv_counters_to_DataFlash();
      }
    }

    // Периодически проверяем необходимость удаления отработавшей задачи инициализации стека BLE
    if (BLE_init_task_state() == BLE_INIT_TASK_FINISHED)
    {
      BLE_init_delete();
    }
  }
}

/*-----------------------------------------------------------------------------------------------------



  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t Create_Backgroung_task(void)
{
  UINT              res;

  res = tx_thread_create(
                         &background_thread,
                         (CHAR *)"Background",
                         Background_task,
                         0,
                         background_stacks,
                         BACKGROUND_TASK_STACK_SIZE,
                         BACKGROUND_TASK_PRIO,
                         BACKGROUND_TASK_PRIO,
                         1,
                         TX_AUTO_START
                        );

  if (res == TX_SUCCESS)
  {
    EAPPLOG("Background task created.");
    return RES_OK;
  }
  else
  {
    EAPPLOG("Background creating error %04X.", res);
    return RES_ERROR;
  }
}

