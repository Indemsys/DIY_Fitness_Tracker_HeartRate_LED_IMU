// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 2024-08-23
// 22:27:45
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include   "Test.h"


static uint32_t HTTP_POST_Send_app_status(NX_WEB_HTTP_SERVER *server_ptr, CHAR *url, NX_PACKET *packet_ptr);
static uint32_t HTTP_POST_Get_app_params(NX_WEB_HTTP_SERVER *server_ptr, CHAR *url, NX_PACKET *packet_ptr);
static uint32_t HTTP_POST_Save_app_params(NX_WEB_HTTP_SERVER *server_ptr, CHAR *url, NX_PACKET *packet_ptr);
static uint32_t HTTP_POST_App_command(NX_WEB_HTTP_SERVER *server_ptr, CHAR *url, NX_PACKET *packet_ptr);

static const T_post_handler_rec post_handlers[] =
{
  {"/post_get_app_status"       , HTTP_POST_Send_app_status     },
  {"/post_get_app_params"       , HTTP_POST_Get_app_params      },
  {"/post_save_app_params"      , HTTP_POST_Save_app_params     },
  {"/post_app_command"          , HTTP_POST_App_command         },
};

#define SIZE_OF_POST_HANDLERS   (sizeof(post_handlers)/sizeof(post_handlers[0]))

#define    LEFT_ARM_LOW_POS     1
#define    LEFT_ARM_UP_POS      2
#define    LEFT_ARM_MID_POS     3
#define    RIGHT_ARM_LOW_POS    4
#define    RIGHT_ARM_UP_POS     5
#define    RIGHT_ARM_MID_POS    6
#define    PLATF_UPPER_POS      7
#define    PLATF_DOWN_POS       8
#define    PLATF_OVERLOAD_POS   9


typedef struct
{
    const char *command_name;
    uint32_t  (*command_func)(int32_t arg);

} T_post_func_rec;

static uint32_t _do_flash_bootloader(int32_t arg);
static uint32_t _do_reset_nv_counters(int32_t arg);
static uint32_t _do_start_imu_stream(int32_t arg);
static uint32_t _do_stop_imu_stream(int32_t arg);
static uint32_t _do_goto_sleep(int32_t arg);


// Функции вызываемые по запросу POST - post_app_command
// Пример тела запроса  {"command":"remote_up", "arg":0}

static const T_post_func_rec post_funcs[] =
{
  {"start_imu_rec"             , _do_start_imu_stream  },
  {"stop_imu_rec"              , _do_stop_imu_stream   },
  {"flash_bootloader"          , _do_flash_bootloader  },
  {"reset_nv_counters"         , _do_reset_nv_counters },
  {"goto_sleep"                , _do_goto_sleep        }
};

#define SIZE_OF_FUNCS_ARRAY   (sizeof(post_funcs)/sizeof(post_funcs[0]))
/*-----------------------------------------------------------------------------------------------------


  \param server_ptr
  \param url
  \param packet_ptr

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
static uint32_t HTTP_POST_Send_app_status(NX_WEB_HTTP_SERVER *server_ptr, CHAR *url, NX_PACKET *packet_ptr)
{
  uint32_t       status;
  char          *buffer  = NULL;
  uint32_t       buff_sz = 10000;

  // Приблизительный размер получаемого JSON файла 977 байт
  //  Print_app_snapshot(NULL,&buff_sz,&app_snapshot);

  buffer = SDRAM_malloc_pending(buff_sz, MS_TO_TICKS(100));  // Выделяем заведомо больший чем требуемый размер памяти
  if (buffer == NULL) goto err_;

  buff_sz = Print_json_system_status(buffer,buff_sz);

  status = HTTP_send_data(server_ptr, buffer, buff_sz,NX_WEB_HTTP_SERVER_POST_REQUEST,"application/json");
  App_free(buffer);
  return status;
err_:
  _nx_web_http_server_response_send(server_ptr, NX_WEB_HTTP_STATUS_INTERNAL_ERROR,sizeof(NX_WEB_HTTP_STATUS_INTERNAL_ERROR) - 1,NULL,0,NULL,0);
  return RES_ERROR;
}

/*-----------------------------------------------------------------------------------------------------


  \param server_ptr
  \param url
  \param packet_ptr

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
static uint32_t HTTP_POST_Get_app_params(NX_WEB_HTTP_SERVER *server_ptr, CHAR *url, NX_PACKET *packet_ptr)
{
  // Приблизительный размер получаемого JSON файла 6394 байт
  return HTTP_POST_Get_params(server_ptr,url,packet_ptr,APPLICATION_PARAMS);
}


/*-----------------------------------------------------------------------------------------------------


  \param server_ptr
  \param url
  \param packet_ptr

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
static uint32_t HTTP_POST_Save_app_params(NX_WEB_HTTP_SERVER *server_ptr, CHAR *url, NX_PACKET *packet_ptr)
{
  return HTTP_POST_Save_params(server_ptr,url,packet_ptr,APPLICATION_PARAMS);
}

/*-----------------------------------------------------------------------------------------------------


  \param server_ptr
  \param url
  \param packet_ptr

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
static uint32_t HTTP_POST_App_command(NX_WEB_HTTP_SERVER *server_ptr, CHAR *url, NX_PACKET *packet_ptr)
{
  uint32_t             res;
  uint8_t             *input_buffer = NULL;
  UINT                 actual_size;
  json_t              *root = 0;
  json_error_t         error;
  char                *cmd_str;
  int32_t              arg;
  char                 str[64];
  ULONG                content_length;

  res = nx_web_http_server_content_length_get(packet_ptr,&content_length);
  if ((res == NX_SUCCESS) && (content_length > 0))
  {
    // Выделяем память для поступающих данных
    input_buffer = App_malloc_pending(content_length + 1,10);
    if (input_buffer == NULL) goto err_;

    // Получаем данные из первого пакета.
    res = nx_web_http_server_content_get(server_ptr, packet_ptr, 0, (CHAR *)input_buffer, content_length,&actual_size);
    if (res != NX_SUCCESS) goto err_;

    //Данные должны быть в формате JSON

    root = json_loads((char const *)input_buffer, 0,&error);
    if (!root) goto err_;

    if (json_unpack(root, "{s:s, s:i}", "command",&cmd_str, "arg",&arg) != 0) goto err_;

    for (uint32_t i = 0; i < SIZE_OF_FUNCS_ARRAY; i++)
    {
      if (strcmp(cmd_str, post_funcs[i].command_name) == 0)
      {
        if (post_funcs[i].command_func(arg) != RES_OK) goto err_;
      }
    }
  }
  else goto err_;


  json_decref(root);
  App_free(input_buffer);
  strcpy(str,"Operation completed successfully");
  HTTP_send_data(server_ptr,str, strlen(str),NX_WEB_HTTP_SERVER_POST_REQUEST,"application/text");
  return res;
err_:
  json_decref(root);
  App_free(input_buffer);
  strcpy(str,"An operation error occurred");
  HTTP_send_data(server_ptr,str, strlen(str),NX_WEB_HTTP_SERVER_POST_REQUEST,"application/text");
  return RES_ERROR;
}

/*-----------------------------------------------------------------------------------------------------


  \param arg

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
static uint32_t _do_start_imu_stream(int32_t arg)
{
  Start_IMU_stream();
  return RES_OK;
}

/*-----------------------------------------------------------------------------------------------------


  \param arg

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
static uint32_t _do_stop_imu_stream(int32_t arg)
{
  Stop_IMU_stream();
  return RES_OK;
}

/*-----------------------------------------------------------------------------------------------------


  \param arg

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
static uint32_t _do_flash_bootloader(int32_t arg)
{
  Perform_bootloader_flashing();
  return RES_OK;
}


/*-----------------------------------------------------------------------------------------------------


  \param arg

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
static uint32_t _do_reset_nv_counters(int32_t arg)
{
  Request_to_clear_system_nv_counters();
  return RES_OK;
}

/*-----------------------------------------------------------------------------------------------------


  \param arg

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
static uint32_t _do_goto_sleep(int32_t arg)
{
  Set_app_event(EVENT_GOTO_SLEEP);
  return RES_OK;
}
/*-----------------------------------------------------------------------------------------------------


  \param server_ptr
  \param resource
  \param packet_ptr

  \return uint32_t - Если возвращаем RES_OK то HTTP сервер больше не выполняет никах действий
                     иначе сервер перелает управление своему обработчику
-----------------------------------------------------------------------------------------------------*/
uint32_t App_get_handler(NX_WEB_HTTP_SERVER *server_ptr, CHAR *resource, NX_PACKET *packet_ptr)
{

  char *file_name;
  if (strcmp(resource, "/") == 0) file_name = "index.html";
  else file_name = &resource[1];

  // Ищем файл в таблице файлов содержащихся во Flash памяти
  for (uint32_t i = 0; i < APP_COMPRESSED_WEB_FILES_NUM; i++)
  {
    if (strcmp(file_name,  app_compressed_web_files[i].name) == 0)
    {
      // Файл найден, отправляем его
      HTTP_send_compressed_flash_file(server_ptr, NX_WEB_HTTP_SERVER_GET_REQUEST, app_compressed_web_files[i].name, app_compressed_web_files[i].data, app_compressed_web_files[i].size);
      return (RES_OK);
    }
  }


  return RES_ERROR;
}

/*-----------------------------------------------------------------------------------------------------


  \param server_ptr
  \param resource
  \param packet_ptr

  \return uint32_t - Если возвращаем RES_OK то HTTP сервер больше не выполняет никах действий
                     иначе сервер перелает управление своему обработчику
-----------------------------------------------------------------------------------------------------*/
uint32_t App_pos_handler(NX_WEB_HTTP_SERVER *server_ptr, CHAR *resource, NX_PACKET *packet_ptr)
{
  for (uint32_t i = 0; i < SIZE_OF_POST_HANDLERS; i++)
  {
    if (strcmp(post_handlers[i].url,resource) == 0)
    {
      post_handlers[i].handler_func(server_ptr, resource, packet_ptr);
      return NX_WEB_HTTP_CALLBACK_COMPLETED;
    }
  }

  return RES_ERROR;
}


