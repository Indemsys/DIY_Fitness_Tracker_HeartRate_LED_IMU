// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 2024-10-19
// 13:17:53
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include   "Test.h"

static void External_irq_int2_isr(void);

SSP_VECTOR_DEFINE(External_irq_int2_isr, ICU, IRQ13);

static IRQn_Type        imu_int2_isr_num;

TX_THREAD motion_thread;
#define MOTION_THREAD_STACK_SIZE (1024*3)
static uint8_t motion_thread_stack[MOTION_THREAD_STACK_SIZE] BSP_PLACE_IN_SECTION_V2(".stack.motion_thread")BSP_ALIGN_VARIABLE_V2(BSP_STACK_ALIGNMENT);

#define  FLAG_ISM330_INTERRUPT              BIT(0)
#define  FLAG_ISM330_START_STREAM           BIT(1)
#define  FLAG_ISM330_STREAM_STARTED         BIT(2)
#define  FLAG_ISM330_STOP_STREAM            BIT(3)
#define  FLAG_ISM330_TASK_SUSPEND           BIT(4)
#define  FLAG_ISM330_TASK_SUSPENDED         BIT(5)


TX_EVENT_FLAGS_GROUP   ism330_flags;

#define MAX_FIFO_SIZE      4096

#define HALF_BUFFER_SIZE (7680)                   // Размер половины буфера. Делится и на 512 для быстроты работы файловой системы и на 12 для корректности считывания FIFO ISM330
#define BUFFER_SIZE      (HALF_BUFFER_SIZE*2)     //


// Глобальный буфер
uint8_t            g_ism330_buf[BUFFER_SIZE] BSP_PLACE_IN_SECTION_V2(".stack.motion_buffer")BSP_ALIGN_VARIABLE_V2(BSP_STACK_ALIGNMENT);


// Глобальные переменные для управления буфером
static uint32_t    buffer_index = 0;  // Текущая позиция в g_ism330_buf
static uint8_t     current_half = 0;  // 0 для первой половины, 1 для второй половины

FX_FILE            motion_data_file;
#define            MOTION_FILE_NAME_MAX_SZ  64
char               motion_file_name[MOTION_FILE_NAME_MAX_SZ+1];
uint32_t           ism330_err;
uint8_t            fifo_status1;
uint8_t            fifo_status2;


T_motion_smpl      motion_smpl;

uint32_t           motion_task_active;
uint32_t           motion_stream_active;
uint32_t           motion_log_active;

#define            NO_USE_IMU_FIFO_STATUS  // Этот макрос предотвращает чтение статуса FIFO для сокращения времени обмена данными с ISM330

static uint32_t    g_fifo_threshold;
static uint32_t    g_fifo_read_buf_sz;

#ifndef            NO_USE_IMU_FIFO_STATUS
static uint16_t    g_fifo_max_fill;
static uint16_t    g_fifo_min_fill;
#endif

uint8_t            imu_src_regs_saved;

T_SM_controller    xh_xl_action_controller;

static void IMU_check_to_sleep_condition(void);
static void IMU_check_mode_changer(T_SM_controller *controller, uint8_t xh, uint8_t xl);
/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
static void External_irq_int2_isr(void)
{
  // Отправляем флаг готовности данных
  tx_event_flags_set(&ism330_flags, FLAG_ISM330_INTERRUPT,TX_OR);
  R_ICU->IELSRn_b[imu_int2_isr_num].IR = 0;  // Сбрасываем IR флаг в ICU
}

/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
static void IMU_INT2_Clear_Interrupts(void)
{
  imu_int2_isr_num    = (IRQn_Type)Find_IRQ_number_by_evt(ELC_EVENT_ICU_IRQ13);
  NVIC_DisableIRQ(imu_int2_isr_num);
  NVIC_ClearPendingIRQ(imu_int2_isr_num);
  R_ICU->IELSRn_b[imu_int2_isr_num].IR = 0;  // Сбрасываем IR флаг в ICU
}


/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
void IMU_INT2_enable_interrupts(void)
{
  IMU_INT2_Clear_Interrupts();

  R_ICU->IRQCRn_b[13].IRQMD = 1;  // 0 1: Rising edge, Прерывание по нарастанию
  NVIC_SetPriority(imu_int2_isr_num,  IMU_INT13_PRIO);
  NVIC_EnableIRQ(imu_int2_isr_num);
}

/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
void IMU_INT2_disable_interrupts(void)
{
  NVIC_DisableIRQ(imu_int2_isr_num);
  IMU_INT2_Clear_Interrupts();
}

/*-----------------------------------------------------------------------------------------------------


  \param buf_ptr
  \param bytes_to_copy

  \return __weak void
-----------------------------------------------------------------------------------------------------*/
__weak void MATLAB_post_data_to_tx_queue(uint8_t *buf_ptr, uint16_t sz)
{

}

/*-----------------------------------------------------------------------------------------------------


  \param num_bytes_to_read
-----------------------------------------------------------------------------------------------------*/
static ssp_err_t Read_and_buffer_fifo_data(uint16_t num_bytes_to_read)
{
  ssp_err_t err;
  uint16_t bytes_remaining = num_bytes_to_read;
  uint16_t bytes_to_copy;
  uint8_t *buf_ptr;

  while (bytes_remaining > 0)
  {
    // Вычислить оставшееся пространство в текущей половине
    uint32_t half_buffer_offset = current_half * HALF_BUFFER_SIZE;
    uint32_t half_buffer_end = half_buffer_offset + HALF_BUFFER_SIZE;
    uint32_t space_in_current_half = half_buffer_end - buffer_index;

    // Проверить, заполнена ли текущая половина буфера
    if (space_in_current_half == 0)
    {
      // Текущая половина заполнена, установить соответствующий флаг события
      if (motion_log_active)
      {
        if (current_half == 0)
        {
          Set_file_logger_event(EVT_MOTION_BUF_READY_FIRST_HALF);
        }
        else
        {
          Set_file_logger_event(EVT_MOTION_BUF_READY_SECOND_HALF);
        }
      }

      // Переключиться на другую половину
      current_half = (current_half + 1) % 2;

      // Обновить buffer_index на начало новой половины
      buffer_index = current_half * HALF_BUFFER_SIZE;
      space_in_current_half = HALF_BUFFER_SIZE;
    }

    // Определить количество байт для чтения в этой итерации
    if (bytes_remaining < space_in_current_half)
    {
      bytes_to_copy = bytes_remaining;
    }
    else
    {
      bytes_to_copy = space_in_current_half;
    }

    // Установить указатель на буфер
    buf_ptr = &g_ism330_buf[buffer_index];

    // Прочитать данные из FIFO ISM330 в g_ism330_buf
    err = ISM330_read_buf(ISM330_FIFO_DATA_OUT_L, buf_ptr, bytes_to_copy);
    if (err != SSP_SUCCESS)
    {
      // Обработать ошибку
      return err;
    }

    // Записываем данные в стуктуру используемум для мониторинга через FreeMaster
    memcpy(&motion_smpl, buf_ptr, sizeof(motion_smpl));
    // Высылаем данные в сеть для приема в MATLAB
    MATLAB_post_data_to_tx_queue(buf_ptr, bytes_to_copy);

    // Обновить счетчики
    buffer_index += bytes_to_copy;
    bytes_remaining -= bytes_to_copy;

    // Проверить, заполнена ли текущая половина после чтения
    if (buffer_index == half_buffer_end)
    {
      // Текущая половина заполнена, установить соответствующий флаг события
      if (motion_log_active)
      {
        if (current_half == 0)
        {
          Set_file_logger_event(EVT_MOTION_BUF_READY_FIRST_HALF);
        }
        else
        {
          Set_file_logger_event(EVT_MOTION_BUF_READY_SECOND_HALF);
        }
      }

      // Переключиться на другую половину
      current_half = (current_half + 1) % 2;

      // Обновить buffer_index на начало новой половины
      buffer_index = current_half * HALF_BUFFER_SIZE;
    }
  }

  // Если все прошло успешно, вернуть RES_OK
  return SSP_SUCCESS;
}

/*-----------------------------------------------------------------------------------------------------


  \param void

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t Start_IMU_stream(void)
{
  tx_event_flags_set(&ism330_flags, FLAG_ISM330_START_STREAM,TX_OR);
  return RES_OK;
}

/*-----------------------------------------------------------------------------------------------------


  \param void

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t Stop_IMU_stream(void)
{
  tx_event_flags_set(&ism330_flags, FLAG_ISM330_STOP_STREAM,TX_OR);
  return RES_OK;
}

/*-----------------------------------------------------------------------------------------------------
  Функция создания нового файла лога IMU и начала записи в него

  \param void
-----------------------------------------------------------------------------------------------------*/
uint32_t Start_motion_logfile(void)
{
  if (motion_log_active == 0)
  {
    if (Create_motion_file() == RES_OK)
    {
      motion_log_active = 1;
      return RES_OK;
    }
  }
  return RES_ERROR;
}

/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
uint32_t Stop_motion_logfile(void)
{
  uint32_t res = RES_ERROR;
  if (motion_log_active != 0)
  {
    if (Close_motion_file() == RES_OK) res = RES_OK;
    motion_log_active = 0;
    return res;
  }
  return res;
}


/*-----------------------------------------------------------------------------------------------------


  \param void

  \return uint8_t
-----------------------------------------------------------------------------------------------------*/
uint8_t Get_motion_stream_state(void)
{
  return motion_stream_active;
}

/*-----------------------------------------------------------------------------------------------------


  \param curr
  \param prev
  \param hold
  \param size
-----------------------------------------------------------------------------------------------------*/
static void _Calculate_rising_flags(const void *curr, const void *prev, void *hold, size_t size)
{
  const uint8_t *pCurr = (const uint8_t *)curr;
  const uint8_t *pPrev = (const uint8_t *)prev;
  uint8_t *pHold = (uint8_t *)hold;

  for (size_t i = 0; i < size; ++i)
  {
    pHold[i] = (~pPrev[i] & pCurr[i]) | pHold[i];    // Вычисляем изменение битов с 0 на 1 c удержанием 1
  }
}

/*-----------------------------------------------------------------------------------------------------


  \param void

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
static ssp_err_t ISM330_set_sampling_rate(void)
{
  ssp_err_t  err = SSP_SUCCESS;

  ISM330_set_sample_rate(ivar.imu_output_data_rate);
  err |= ISM330_Set_accelerometer_scale_rate(ivar.accelerometer_scale, ivar.imu_output_data_rate);
  err |= ISM330_Set_gyro_scale_rate(ivar.gyroscope_scale, ivar.imu_output_data_rate);

  if (err == SSP_SUCCESS)
  {
    APPLOG("ISM330 initialised. Sampling Rate=%0.1f", (double)ISM330_get_sample_rate());
  }
  else
  {
    APPLOG("ISM330 sampling rate initialisation error 0x%04X", (uint32_t)err);
  }
  return err;
}

/*-----------------------------------------------------------------------------------------------------


  \param void

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
static ssp_err_t ISM330_init_stream(void)
{
  uint8_t    value;
  ssp_err_t  err = SSP_SUCCESS;

  #ifndef NO_USE_IMU_FIFO_STATUS
  g_fifo_max_fill = 0;
  g_fifo_min_fill = 0xFFFF;
  #endif


  xh_xl_action_controller.action = Toggle_display_mode;
  Get_hw_timestump(&xh_xl_action_controller.timestamp);
  xh_xl_action_controller.state = STATE_IDLE;


  ISM330_lock(500);

  ISM330_config_xyz_tap_recognition(1);
  ISM330_config_wake_up_recognition();
  ISM330_config_tilt_recognition();
  ISM330_config_free_fall_recognition();
  ISM330_config_6d_recognition();
  ISM330_INT2_activity_interrupt_control(0x00); // Запрещаем сигнал внешнего прерывания от активностей
  ISM330_activity_interrupts_control(1);


  // Сбрасываем FIFO
  value = 0x00; // FIFO_MODE = 000 (Bypass mode. FIFO disabled.)
  err |= ISM330_write_register(ISM330_FIFO_CTRL5,value); // FIFO_CTRL5

  // Устанавливаем коэффициенты децимации данных.
  value = 0x09; // DEC_FIFO_GYRO = 001, DEC_FIFO_XL = 001 - без децимации
  err |= ISM330_write_register(ISM330_FIFO_CTRL3,value); // FIFO_CTRL3

  g_fifo_threshold   = (uint32_t)roundf((ivar.imu_fifo_rd_period * ISM330_get_sample_rate()) / 1000) * 6; // Это значение должно быть строго кратно количеству 2-байтных сэмплов за одно измерение
  // Иначе прерывания по FIFO будут идти нестабильно.
  if (g_fifo_threshold == 0) g_fifo_threshold = 6;

  g_fifo_read_buf_sz = g_fifo_threshold * 2;

  // Установить уровень заполнения FIFO для генерации прерывания
  value = g_fifo_threshold & 0xFF; // FIFO_THRESHOLD
  err |= ISM330_write_register(ISM330_FIFO_CTRL1,value); // FIFO_CTRL1

  value = (g_fifo_threshold >> 8) & 0x07; // Дополнительные биты FIFO_THRESHOLD
  err |= ISM330_write_register(ISM330_FIFO_CTRL2,value); // FIFO_CTRL2

  // Включить прерывание по достижению уровня FIFO_THRESHOLD на INT2
  value = 0x08; // FIFO_TH = 1
  err |= ISM330_write_register(ISM330_INT2_CTRL,value); // INT2_CTRL

  // Gyroscope sleep mode disabled, Interrupt signals divided between INT1 and INT2 pads,  Both I²C and SPI enabled
  value = 0x00;
  err |= ISM330_write_register(ISM330_CTRL4_C,value); // CTRL4_C

  // Запускаем FIFO
  // Установить режим работы FIFO и частоту дискретизации FIFO
  value = 0x01; // FIFO_MODE = 001 (FIFO mode. Stops collecting data when FIFO is full.)
  switch (ivar.imu_output_data_rate)
  {
  case IMU_OUTPUT_DATA_RATE_1666HZ :
    value |= LSHIFT(8,3);
    break;
  case IMU_OUTPUT_DATA_RATE_833HZ  :
    value |= LSHIFT(7,3);
    break;
  case IMU_OUTPUT_DATA_RATE_416HZ  :
    value |= LSHIFT(6,3);
    break;
  case IMU_OUTPUT_DATA_RATE_208HZ  :
    value |= LSHIFT(5,3);
    break;
  case IMU_OUTPUT_DATA_RATE_104HZ  :
    value |= LSHIFT(4,3);
    break;
  }
  err |= ISM330_write_register(ISM330_FIFO_CTRL5,value); // FIFO_CTRL5

  ISM330_enable_source_register_rounding();

  ISM330_unlock();

  imu_src_regs_saved = 0;

  if (err == 0)
  {
    APPLOG("ISM330 FIFO initialised. Period = %d ms, FIFO threshold = %d", ivar.imu_fifo_rd_period, g_fifo_read_buf_sz);
  }
  else
  {
    APPLOG("ISM330 FIFO initialition error 0x%04X", (uint32_t)err);
  }

  return err;
}

/*-----------------------------------------------------------------------------------------------------
   Функция для чтения данных из FIFO

   На частоте сэмплироания 1666 Гц с периодом выборки FIFO 10 мс, процедура считывает 204 байта за 5 мс
-----------------------------------------------------------------------------------------------------*/
void ISM330_proccess_stream(void)
{
  ssp_err_t  err = SSP_SUCCESS;

  // О необходимости запрета прерываний от IMU
  // -------------------------------------------------------------
  // 300 мкс уходит на чтение статуса и количества данных в FIFO
  // 50 мкс уходит на устновку адреса чтения из FIFO
  // Около 40 мкс на чтение 2-байтного слова
  // Итого 590 мкс. такая задержка приводит к тому что при частоте сэмплирования 1666 в течении чтения FIFO происходит новое прерывание
  // Поэтому на время чтения FIFO временно запрещает прерывания от сигнала IMU_INT2
  // -------------------------------------------------------------
  NVIC_DisableIRQ(imu_int2_isr_num);

  ISM330_lock(500);


  memcpy(&g_ism330_src_regs_prev,&g_ism330_src_regs, sizeof(g_ism330_src_regs));
  // Читаем статусы источников прерываний
  ISM330_read_buf(ISM330_WAKE_UP_SRC, (uint8_t *)&g_ism330_src_regs, sizeof(g_ism330_src_regs));

  _Calculate_rising_flags(&g_ism330_src_regs,&g_ism330_src_regs_prev,&g_ism330_src_regs_hold , sizeof(g_ism330_src_regs));

  if (imu_src_regs_saved == 0)
  {
    memset(&g_ism330_src_regs     , 0, sizeof(g_ism330_src_regs));
    memset(&g_ism330_src_regs_hold, 0, sizeof(g_ism330_src_regs_hold));
    imu_src_regs_saved  = 1;
  }

  IMU_check_to_sleep_condition();
  IMU_check_mode_changer(&xh_xl_action_controller, g_ism330_src_regs.d6d_src.XH, g_ism330_src_regs.d6d_src.XL);

  #ifndef NO_USE_IMU_FIFO_STATUS

  uint8_t    fifo_status1;
  uint8_t    fifo_status2;
  uint16_t   num_samples;
  uint16_t   num_bytes;

  // Получить количество доступных данных в FIFO
  // Каким то образом очередность чтения этих регистров влияет на периодичность чтения
  err |= ISM330_read_register(ISM330_FIFO_STATUS2,&fifo_status2);   // FIFO_STATUS2
  err |= ISM330_read_register(ISM330_FIFO_STATUS1,&fifo_status1);   // FIFO_STATUS1

  num_samples = ((fifo_status2 & 0x07) << 8) | fifo_status1; // 12-битное значение
  // Рассчитать количество байт для чтения
  num_bytes = num_samples * 2;
  // Количество читаемых байт должно быть кратными 12
  num_bytes = (num_bytes / 12) * 12;


  if (err == SSP_SUCCESS)
  {
    if (num_bytes > MAX_FIFO_SIZE)
    {
      ism330_err = (uint32_t)SSP_ERR_OVERFLOW;
      Request_to_close_motion_logfile();
    }
    else if (num_bytes > 0)
    {
      err = Read_and_buffer_fifo_data(num_bytes);
      if (err == SSP_SUCCESS)
      {
        if (num_bytes > g_fifo_max_fill)
        {
          APPLOG("ISM330 max read bytes = %d", num_bytes);
          g_fifo_max_fill = num_bytes;
        }
        if (num_bytes < g_fifo_min_fill)
        {
          APPLOG("ISM330 min read bytes = %d", num_bytes);
          g_fifo_min_fill = num_bytes;
        }
      }
      else
      {
        Request_to_close_motion_logfile();
      }
    }
    else
    {
      APPLOG("ISM330 read %d byte error 0x%04X", num_bytes, (uint32_t)err);
    }
  }
  else
  {
    APPLOG("ISM330 read status regs error 0x%04X",(uint32_t)err);
  }
  ism330_err =  (uint32_t)err;
  #else



  err = Read_and_buffer_fifo_data(g_fifo_read_buf_sz);
  if (err != SSP_SUCCESS)
  {
    Stop_motion_logfile();
  }
  #endif


  ISM330_unlock();

  // Очищаем и снова разрешаем прерывания
  R_ICU->IELSRn_b[imu_int2_isr_num].IR = 0;  // Сбрасываем IR флаг в ICU
  NVIC_ClearPendingIRQ(imu_int2_isr_num);
  NVIC_EnableIRQ(imu_int2_isr_num);

}

/*-----------------------------------------------------------------------------------------------------


  \param void

  \return ssp_err_t
-----------------------------------------------------------------------------------------------------*/
static ssp_err_t ISM330_deinit_stream(void)
{
  uint8_t    value;
  ssp_err_t  err = SSP_SUCCESS;

  // Сбрасываем FIFO
  value = 0x00; // FIFO_MODE = 000 (Bypass mode. FIFO disabled.)
  err |= ISM330_write_register(ISM330_FIFO_CTRL5,value); // FIFO_CTRL5

  // Выключаем прерывания от FIFO на INT2
  value = 0x00; //
  err |= ISM330_write_register(ISM330_INT2_CTRL,value); // INT2_CTRL


  ISM330_activity_interrupts_control(0);
  return err;

}


/*-----------------------------------------------------------------------------------------------------


  \param void

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
ssp_err_t Reset_ISM330(void)
{
  uint8_t    value;
  ssp_err_t  err = SSP_SUCCESS;


  ISM330_lock(500);

  // Выполнить программный сброс
  value = 0x81; // Установить биты SW_RESET и BOOT
  err |= ISM330_write_register(ISM330_CTRL3_C,value); // CTRL3_C

  // Подождать, пока сброс завершится
  do
  {
    err |= ISM330_read_register(ISM330_CTRL3_C,&value); // CTRL3_C
  } while (value & 0x01); // Пока бит SW_RESET установлен

  // Включить BDU (Block data update) и автоинкремент адреса,  interrupt output pads active high
  value = 0x44; // BDU = 1, IF_INC = 1
  err |= ISM330_write_register(ISM330_CTRL3_C,value); // CTRL3_C

  ISM330_unlock();

  if (err != SSP_SUCCESS)
  {
    APPLOG("ISM330 reset error 0x%04X", (uint32_t)err);
  }
  else
  {
    APPLOG("ISM330 reset");
  }


  return err;
}

/*-----------------------------------------------------------------------------------------------------


  \param void

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t Create_motion_file(void)
{
  uint32_t res;

  if (motion_data_file.fx_file_id != FX_FILE_ID)
  {
    rtc_time_t  curr_time;
    int32_t len;

    strncpy(motion_file_name,(char const *)ivar.motion_logfile_name , MOTION_FILE_NAME_MAX_SZ);
    len = strlen((char const *)ivar.motion_logfile_name);
    rtc_cbl.p_api->calendarTimeGet(rtc_cbl.p_ctrl,&curr_time);
    curr_time.tm_mon++;
    len +=  snprintf(&motion_file_name[len], MOTION_FILE_NAME_MAX_SZ - len, "_%0.0fHz_%04d_%02d_%02d__%02d_%02d_%02d",(double)ISM330_get_sample_rate(), curr_time.tm_year + 1900, curr_time.tm_mon, curr_time.tm_mday, curr_time.tm_hour, curr_time.tm_min, curr_time.tm_sec);
    snprintf(&motion_file_name[len],MOTION_FILE_NAME_MAX_SZ - len, ".bin");

    res = Recreate_file_for_write(&motion_data_file, motion_file_name);
    if (res == TX_SUCCESS)
    {
      APPLOG("Created log file %s",motion_file_name);
      return RES_OK;
    }
    else
    {
      APPLOG("Creation log file %s error %d",motion_file_name, res);
    }
  }
  return RES_ERROR;
}

/*-----------------------------------------------------------------------------------------------------


  \param void

  \return int32_t
-----------------------------------------------------------------------------------------------------*/
int32_t Get_recorded_log_file_size(void)
{
  if (motion_data_file.fx_file_id == FX_FILE_ID)
  {
    return motion_data_file.fx_file_current_file_size;
  }
  else return -1;
}

/*-----------------------------------------------------------------------------------------------------


  \param void

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t Close_motion_file(void)
{
  uint32_t res;
  if (motion_data_file.fx_file_id == FX_FILE_ID)
  {
    uint32_t sz = motion_data_file.fx_file_current_file_size;
    res = fx_file_close(&motion_data_file);
    if (res == TX_SUCCESS)
    {
      APPLOG("Closed log file %s. Size=%d",motion_file_name, sz);
      return RES_OK;
    }
    else
    {
      APPLOG("Closing log file %s error %d",motion_file_name, res);
    }
  }
  return RES_ERROR;
}

/*-----------------------------------------------------------------------------------------------------
  Сохранение буффера в файл
  Вызывается из задачи логера

  \param buf_indx

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t Save_motion_buffer(uint32_t buf_indx)
{
  uint8_t *buf_ptr;

  if (motion_data_file.fx_file_id != FX_FILE_ID) return RES_ERROR;

  if (buf_indx == 0)
  {
    buf_ptr = &g_ism330_buf[0];
  }
  else
  {
    buf_ptr = &g_ism330_buf[0 + HALF_BUFFER_SIZE];
  }

  return fx_file_write(&motion_data_file, buf_ptr, HALF_BUFFER_SIZE);

}

/*-----------------------------------------------------------------------------------------------------


  \param void

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t Get_motion_sens_error(void)
{
  return (uint32_t)ism330_err;
}



/*-----------------------------------------------------------------------------------------------------


  \param arg
-----------------------------------------------------------------------------------------------------*/
static void Motion_thread(ULONG arg)
{
  ULONG      actual_flags;

  fx_directory_default_set(&fat_fs_media, "/");

  if (wvar.automatic_start_motion_log != 0)
  {
    Start_motion_logfile();
  }

  if (Reset_ISM330() != SSP_SUCCESS) return;

  Start_IMU_stream();

  do
  {
    actual_flags = 0;
    tx_event_flags_get(&ism330_flags, 0xFFFFFFFF, TX_OR_CLEAR,&actual_flags,  MS_TO_TICK(6000));
    if (actual_flags & FLAG_ISM330_TASK_SUSPEND)
    {
      IMU_INT2_disable_interrupts();
      ISM330_deinit_stream();
      Stop_motion_logfile();
      tx_event_flags_set(&ism330_flags, FLAG_ISM330_TASK_SUSPENDED,TX_OR);
    }
    if (actual_flags & FLAG_ISM330_START_STREAM)
    {
      if (motion_stream_active == 0)
      {
        if (ISM330_set_sampling_rate() == SSP_SUCCESS)
        {
          if (ISM330_init_stream() == SSP_SUCCESS)
          {
            motion_stream_active = 1;
            tx_event_flags_set(&ism330_flags, FLAG_ISM330_STREAM_STARTED,TX_OR);
            IMU_INT2_enable_interrupts();
          }
        }
      }
    }
    if (actual_flags & FLAG_ISM330_STOP_STREAM)
    {
      if (motion_stream_active != 0)
      {
        IMU_INT2_disable_interrupts();
        ISM330_deinit_stream();
        motion_stream_active = 0;
        Stop_motion_logfile();
      }
    }

    if (actual_flags & FLAG_ISM330_INTERRUPT) // Флаг устанавливается в прерывании
    {
      ISM330_proccess_stream();
    }

  } while (1);
}


/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
uint32_t Motion_sensor_init(void)
{
  uint8_t    value;


  tx_event_flags_create(&ism330_flags, "ISM330");

  // Инициализация ISM330
  if (ISM330_open() != SSP_SUCCESS)
  {
    APPLOG("Error while opening connection to ISM330");
    return RES_ERROR;
  }
  // Проверяем корректность идентификатора чипа
  if (ISM330_read_register(ISM330_WHO_AM_I,&value) != SSP_SUCCESS)
  {
    APPLOG("Read ISM330_WHO_AM_I error");
    return RES_ERROR;
  }
  if (value != ISM330_CHIP_ID)
  {
    APPLOG("Unexpected value read from ISM330_WHO_AM_I register");
    return RES_ERROR;
  }

  tx_thread_create(
                   &motion_thread,
                   (CHAR *)"Motion Thread",
                   Motion_thread,
                   (ULONG) NULL,
                   &motion_thread_stack,
                   MOTION_THREAD_STACK_SIZE,
                   MOTION_TASK_PRIO,
                   MOTION_TASK_PRIO,
                   1,
                   TX_AUTO_START
                  );

  motion_task_active = 1;
  return RES_OK;
}

/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
void Terminate_motion_task(void)
{
  ULONG actual_flags;
  if (motion_task_active)
  {
    tx_event_flags_set(&ism330_flags, FLAG_ISM330_TASK_SUSPEND,TX_OR);
    tx_event_flags_get(&ism330_flags, FLAG_ISM330_TASK_SUSPENDED, TX_OR_CLEAR,&actual_flags,  MS_TO_TICK(10000));
    tx_thread_terminate(&motion_thread);
    tx_thread_delete(&motion_thread);
    tx_event_flags_delete(&ism330_flags);
    motion_task_active = 0;
  }
  Stop_motion_logfile();
}

/*-----------------------------------------------------------------------------------------------------



  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
ssp_err_t IMU_Set_exit_low_power_condition(void)
{
  ssp_err_t  err = SSP_SUCCESS;

  ISM330_set_sample_rate(ivar.imu_output_data_rate);
  err |= ISM330_Set_accelerometer_scale_rate(ACCELEROMETER_SCALE_2G, IMU_OUTPUT_DATA_RATE_12_5);
  err |= ISM330_enable_low_power_accelerometer();

  switch (wvar.wake_сondition_сode)
  {
    case IMU_CODITION_NONE:
    return err;
    case IMU_CODITION_WAKE_UP_CONDITION   :
    err |= ISM330_config_wake_up_recognition();
    err |= ISM330_INT2_activity_interrupt_control(MD2_CFG_INT2_WU_ENABLED);
    break;
    case IMU_CODITION_CHANGE_ORIENTATION  :
    err |= ISM330_config_6d_recognition();
    err |= ISM330_INT2_activity_interrupt_control(MD2_CFG_INT2_6D_ENABLED);
    break;
    case IMU_CODITION_SINGLE_TAP          :
    err |= ISM330_config_xyz_tap_recognition(0);
    err |= ISM330_INT2_activity_interrupt_control(MD2_CFG_INT2_SINGLE_TAP_ENABLED);
    break;
    case IMU_CODITION_DOUBLE_TAP          :
    err |= ISM330_config_xyz_tap_recognition(1);
    err |= ISM330_INT2_activity_interrupt_control(MD2_CFG_INT2_SINGLE_TAP_ENABLED);
    break;
    case IMU_CODITION_FREE_FALL           :
    err |= ISM330_config_free_fall_recognition();
    err |= ISM330_INT2_activity_interrupt_control(MD2_CFG_INT2_FF_ENABLED);
    break;
  }

  err |= ISM330_activity_interrupts_control(1);

  R_ICU->IRQCRn_b[13].IRQMD = 1;  // 0 1: Rising edge, Прерывание по нарастанию

  Wait_ms(10);

  return err;
}

/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
static void IMU_check_to_sleep_condition(void)
{
  uint32_t elapsed_time;

  elapsed_time = tx_time_get() / TX_TIMER_TICKS_PER_SECOND;

  // Не переходим в сон раньше чем через 1 сек
  if (elapsed_time > 1)
  {
    switch (wvar.sleep_condition_code)
    {
    case IMU_CODITION_NONE:
      return;
    case IMU_CODITION_WAKE_UP_CONDITION   :
      if (g_ism330_src_regs.wake_up_src.WU_IA)
      {
        Set_app_event(EVENT_GOTO_SLEEP);
      }
      break;
    case IMU_CODITION_CHANGE_ORIENTATION  :
      if (g_ism330_src_regs.d6d_src.D6D_IA)
      {
        Set_app_event(EVENT_GOTO_SLEEP);
      }
      break;
    case IMU_CODITION_SINGLE_TAP          :
      if (g_ism330_src_regs.tap_src.TAP_IA)
      {
        Set_app_event(EVENT_GOTO_SLEEP);
      }
      break;
    case IMU_CODITION_DOUBLE_TAP          :
      if (g_ism330_src_regs.tap_src.TAP_IA)
      {
        Set_app_event(EVENT_GOTO_SLEEP);
      }
      break;
    case IMU_CODITION_FREE_FALL           :
      if (g_ism330_src_regs.wake_up_src.FF_IA)
      {
        Set_app_event(EVENT_GOTO_SLEEP);
      }
      break;

    }
  }

}

/*-----------------------------------------------------------------------------------------------------
  Следим за сменой режима работы дисплея
  Режим меняется если последовательно в течении секунды появился сигнал
  xh а за ним  xl

  \param void
-----------------------------------------------------------------------------------------------------*/
void IMU_check_mode_changer(T_SM_controller *controller, uint8_t xh, uint8_t xl)
{
  T_sys_timestump current_time;
  Get_hw_timestump(&current_time);

  switch (controller->state)
  {
  case STATE_IDLE:
    {
      // Проверка на недопустимое состояние
      if (xh == 1 && xl == 1)
      {
        // Недопустимое состояние, сброс автомата
        controller->state = STATE_IDLE;
        break;
      }

      // Переход xh из 0 в 1
      if (xh == 1)
      {
        controller->state = STATE_XH_TRIGGERED;
        Get_hw_timestump(&controller->timestamp);
      }
      break;
    }

  case STATE_XH_TRIGGERED:
    {
      // Проверка на недопустимое состояние
      if (xh == 1 && xl == 1)
      {
        // Недопустимое состояние, сброс автомата
        controller->state = STATE_IDLE;
        break;
      }

      uint32_t elapsed_time = Timestump_diff_to_msec(&controller->timestamp,&current_time);

      // Сброс автомата, если прошло более 1 секунды
      if (elapsed_time > 1000)
      {
        controller->state = STATE_IDLE;
        break;
      }

      // Проверка перехода xl из 0 в 1 после xh
      if (xl == 1 && xh == 0)
      {
        if (elapsed_time < 1000)
        {
          if (controller->action != NULL)
          {
            controller->action();
          }
        }

        // Сброс автомата в начальное состояние
        controller->state = STATE_IDLE;
      }

      break;
    }

  default:
    {
      // Восстановление начального состояния в случае ошибки
      controller->state = STATE_IDLE;
      break;
    }
  }
}


