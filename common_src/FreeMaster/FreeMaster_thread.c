#include "App.h"
#include "Net.h"
#include "freemaster_cfg.h"
#include "freemaster.h"
#include "freemaster_tsa.h"


#define FM_PIPE_RX_BUF_SIZE 64
#define FM_PIPE_TX_BUF_SIZE (1024*8)
#define FM_PIPE_MAX_STR_LEN 512

#define FM_PIPE_PORT_NUM    0
#define FM_PIPE_CALLBACK    0
#define FM_PIPE_TYPE        FMSTR_PIPE_TYPE_ANSI_TERMINAL
FMSTR_ADDR                  pipeRxBuff;
FMSTR_PIPE_SIZE             pipeRxSize;
FMSTR_ADDR                  pipeTxBuff;
FMSTR_PIPE_SIZE             pipeTxSize;
FMSTR_U8                    *fmstr_pOwnRecBuffer;

FMSTR_HPIPE                 fm_pipe = NULL;
char                        *log_str;
uint8_t                     f_unsent_record;
uint32_t                    g_freemaster_interface_type;

#define FREEMASTER_TASK_STACK_SIZE (1024*2)
static uint8_t freemaster_stack[FREEMASTER_TASK_STACK_SIZE] BSP_PLACE_IN_SECTION_V2(".stack.freemaster")BSP_ALIGN_VARIABLE_V2(BSP_STACK_ALIGNMENT);
static TX_THREAD       freemaster_thread;

extern const T_NV_parameters_instance* Get_app_params_instance(void);

static void Thread_FreeMaster(ULONG initial_data);

#ifdef ENABLE_SDRAM
  #define FM_malloc SDRAM_malloc
  #define FM_free   SDRAM_free
#else
  #define FM_malloc App_malloc
  #define FM_free   App_free
#endif

extern   T_freemaster_serial_driver *frm_serial_drv;

/*-----------------------------------------------------------------------------------------------------


  \param app_command
  \param len
  \param dbuf

  \return __weak
-----------------------------------------------------------------------------------------------------*/
__weak uint8_t App_Freemaster_cmd_man(uint16_t app_command, uint32_t len, uint8_t  *dbuf)
{
  return 0;
}
/*-----------------------------------------------------------------------------------------------------
  Обработка пользовательских комманд поступающих движку FreeMaster

 \param app_command

 \return uint8_t
-----------------------------------------------------------------------------------------------------*/
static uint8_t Freemaster_Command_Manager(uint16_t app_command)
{
  uint8_t  res;
  res = 0;
  uint32_t len;
  uint8_t  *dbuf;

  // Получаем указатель на буфер с данными команды
  dbuf = FMSTR_GetAppCmdData(&len);

  switch (app_command)
  {
  case FMCMD_CHECK_LOG_PIPE:
    FMSTR_PipePuts(fm_pipe, "Log pipe checked.\r\n");
    break;

  case FMCMD_SAVE_APP_PARAMS:
    Request_save_nv_parameters(APPLICATION_PARAMS);
    break;
  case FMCMD_SAVE_MODULE_PARAMS:
    Request_save_nv_parameters(MODULE_PARAMS);
    break;
  case FMCMD_RESET_DEVICE:
    APPLOG("System reset caused by FreeMaster command.");
    Wait_ms(200);
    Reset_SoC();
    break;
  default:
    res = App_Freemaster_cmd_man(app_command, len, dbuf);
    break;
  }


  return res;
}


/*-----------------------------------------------------------------------------------------------------
  Пересылаем данные из лога приложения в канал FreeMaster

  \param void
-----------------------------------------------------------------------------------------------------*/
void Freemaster_send_log_to_pipe(void)
{
  if (f_unsent_record != 0)
  {
    if (FMSTR_PipePuts(fm_pipe, log_str) != FMSTR_TRUE) return;
    f_unsent_record = 0;
  }

  while (FreeMaster_get_app_log_string(log_str, FM_PIPE_MAX_STR_LEN) == RES_OK)
  {
    if (FMSTR_PipePuts(fm_pipe, log_str) != FMSTR_TRUE)
    {
      f_unsent_record = 1;
      return;
    }
  }
}


/*-------------------------------------------------------------------------------------------------------------
  Цикл движка FreeMaster
-------------------------------------------------------------------------------------------------------------*/
static void Thread_FreeMaster(ULONG initial_data)
{
  uint32_t    actual_flags;
  uint16_t    app_command;
  uint8_t     res;

  if (initial_data == FREEMASTER_ON_NET)
  {
    if (Wait_net_event(NET_FLG_BSD_READY, &actual_flags, TX_WAIT_FOREVER)!=TX_SUCCESS) return;

    if (!FMSTR_Init((void *)&FMSTR_NET))
    {
      return;
    }
  }
  else if (initial_data == FREEMASTER_ON_SERIAL)
  {
    frm_serial_drv = Get_FreeMaster_usb_vcom1_driver();
    if (frm_serial_drv->_init(&frm_serial_drv->drv_cbl_ptr, frm_serial_drv) != RES_OK)
    {
      return;
    }
    tx_thread_identify()->driver =  (ULONG)(frm_serial_drv);
    if (!FMSTR_Init((void *)&FMSTR_SERIAL))
    {
      return;
    }
  }
  else return;


  EAPPLOG("FreeMaster task started");

  pipeRxSize = FM_PIPE_RX_BUF_SIZE;
  pipeRxBuff = FM_malloc(pipeRxSize);
  pipeTxSize = FM_PIPE_TX_BUF_SIZE;
  pipeTxBuff = FM_malloc(pipeTxSize);
  log_str    = FM_malloc(FM_PIPE_MAX_STR_LEN);
  if ((pipeRxBuff != NULL) && (pipeTxBuff != NULL) && (log_str != NULL))
  {
    fm_pipe = FMSTR_PipeOpen(FM_PIPE_PORT_NUM, FM_PIPE_CALLBACK,  pipeRxBuff, pipeRxSize, pipeTxBuff, pipeTxSize, FM_PIPE_TYPE, "SysLog");
  }

  while (1)
  {
    app_command = FMSTR_GetAppCmd();

    if (app_command != FMSTR_APPCMDRESULT_NOCMD)
    {
      res = Freemaster_Command_Manager(app_command);
      FMSTR_AppCmdAck(res);
    }
    FMSTR_Poll();
    if (ivar.en_log_to_freemaster)
    {
      if (fm_pipe != NULL)
      {
        Freemaster_send_log_to_pipe();
      }
    }
  }
}



/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
void Determine_FreeMaster_interface_type(void)
{

  if (ivar.usb_mode == USB_MODE_VCOM_AND_FREEMASTER_PORT)
  {
    g_freemaster_interface_type = FREEMASTER_ON_SERIAL;
  }
  else if (ivar.usb_mode == USB_MODE_RNDIS)
  {
    g_freemaster_interface_type = FREEMASTER_ON_NET;
  }
  else if (ivar.usb_mode == USB_MODE_HOST_ECM)
  {
    g_freemaster_interface_type = FREEMASTER_ON_NET;
  }
  else if (ivar.en_wifi_module)
  {
    g_freemaster_interface_type = FREEMASTER_ON_NET;
  }
  else
  {
    g_freemaster_interface_type = FREEMASTER_NO_INTERFACE;
  }
}

/*-----------------------------------------------------------------------------------------------------
  Вызывается из контекста удаляемой задачи после того как она выключена

  \param thread_ptr
  \param condition
-----------------------------------------------------------------------------------------------------*/
static void FreeMaster_entry_exit_notify(TX_THREAD *thread_ptr, UINT condition)
{
  if (condition == TX_THREAD_ENTRY)
  {

  }
  else if (condition == TX_THREAD_EXIT)
  {
    FM_free(pipeRxBuff);
    FM_free(pipeTxBuff);
    FM_free(log_str);
  }
}


/*-----------------------------------------------------------------------------------------------------


  \param void

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t Thread_FreeMaster_create(void)
{
  UINT                res;

  if (ivar.en_freemaster==0) return RES_ERROR;

  Determine_FreeMaster_interface_type();
  if (g_freemaster_interface_type == FREEMASTER_NO_INTERFACE)
  {
    EAPPLOG("No available interface for the FreeMaster task");
    return RES_ERROR;
  }

  // Выделяем память для рекордера
  fmstr_pOwnRecBuffer = FM_malloc(FMSTR_REC_BUFF_SIZE);
  if (fmstr_pOwnRecBuffer == NULL)
  {
    res = TX_NO_MEMORY;
    goto exit_;
  }

  res = tx_thread_create(
                         &freemaster_thread,
                         (CHAR *)"FreeMaster",
                         Thread_FreeMaster,
                         (ULONG)g_freemaster_interface_type,
                         freemaster_stack,
                         FREEMASTER_TASK_STACK_SIZE,
                         FREEMASTER_TASK_PRIO,
                         FREEMASTER_TASK_PRIO,
                         1,
                         TX_AUTO_START
                        );

  if (res != TX_SUCCESS)
  {
    goto exit_;
  }

  tx_thread_entry_exit_notify(&freemaster_thread, FreeMaster_entry_exit_notify);
  EAPPLOG("FreeMaster task created");

  return RES_OK;
exit_:
  EAPPLOG("FreeMaster task creation fault: %d", res);
  return RES_ERROR;

}


/*-----------------------------------------------------------------------------------------------------


  \param void

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
void  Thread_FreeMaster_delete(void)
{
  tx_thread_terminate(&freemaster_thread); // Эту функцию нельзя вызывать из сфддифсл функции thread_ptr -> tx_thread_entry_exit_notify
                                           // назаначенную функцией tx_thread_entry_exit_notify поскольку может привести к рекурсивному вызову
  tx_thread_delete(&freemaster_thread);    // Эта функция просто удаляет ссылку на управляющую структуру потока из списка потоков
  FM_free(fmstr_pOwnRecBuffer);
}

