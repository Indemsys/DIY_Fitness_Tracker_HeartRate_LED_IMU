// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 2019.09.22
// 11:50:10
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include   "App.h"
#include   "Net.h"
#include   "WEB_server_certificate.h"

extern const NX_SECURE_TLS_CRYPTO   nx_crypto_tls_ciphers;
extern const NX_SECURE_TLS_CRYPTO   nx_crypto_tls_ciphers_synergys7;

#ifdef APP_WEB
extern uint32_t App_get_handler(NX_WEB_HTTP_SERVER *server_ptr, CHAR *resource, NX_PACKET *packet_ptr);
extern uint32_t App_pos_handler(NX_WEB_HTTP_SERVER *server_ptr, CHAR *resource, NX_PACKET *packet_ptr);
#endif

#ifdef USE_HARDWARE_CRIPTO_ENGINE
  #define CRIPTO_TLS_CYPHERS  nx_crypto_tls_ciphers_synergys7
#else
  #define CRIPTO_TLS_CYPHERS  nx_crypto_tls_ciphers
#endif

static NX_IP              *http_ip_ptr;



#define    WEB_SERVER_STACK_SIZE     (2048*2)


static   uint8_t   server_stack[WEB_SERVER_STACK_SIZE];

#ifdef ENABLE_SDRAM

NX_WEB_HTTP_SERVER          http_server  @ ".sdram";
static NX_SECURE_X509_CERT  certificate @ ".sdram";
static CHAR                 crypto_metadata_server[20000 * NX_WEB_HTTP_SERVER_SESSION_MAX] @ ".sdram"; // В этой области будет размещена таблица с управляющими структурами криптоалгоритмов
                                                                                                       // Размер зависит от набора подключенных алгоритмов криптографии.
// В миниамальном варианте требуется не меньше 17596 байт на сессию
// С TLS 1.3  не менее 20000 байт
static UCHAR                tls_packet_buffer[40000] @ ".sdram"; // Опытным путем установлено, что размер этого буфере не может быть меньше 40000 байт
#else

NX_WEB_HTTP_SERVER          http_server;

#endif


#define               RESPONSE_MAX_SIZE 512
static CHAR           response_buf[RESPONSE_MAX_SIZE];
static uint8_t        web_first_call;
static FX_LOCAL_PATH  web_local_path;

static const T_post_handler_rec post_handlers[] =
{
  {"/post_save_module_params"  , HTTP_POST_Save_module_params    },   //
  {"/post_get_module_params"   , HTTP_POST_Get_module_params     },   //
  {"/post_device_info"         , HTTP_POST_send_device_info      },   //
  {"/post_status"              , HTTP_POST_send_device_status    },   //
  {"/post_sd_csd"              , HTTP_POST_send_card_csd         },   //
  {"/post_sd_state"            , HTTP_POST_send_card_state       },   //
  {"/post_sd_card_control"     , HTTP_POST_card_control          },   //
  {"/upload_time"              , HTTP_POST_upload_time           },   //
  {"/firmware_upload"          , HTTP_POST_firmware_upload       },   //
  {"/clear_log"                , HTTP_POST_Creal_Gen_Log_File    },   //
  {"/clear_netlog"             , HTTP_POST_Creal_Net_Log_File    },   //
  {"/reset"                    , HTTP_POST_Reset_device          },   //
  {"/start_wifi_scan"          , HTTP_POST_Start_WIFI_scan       },   //
  {"/get_wifi_scan_results"    , HTTP_POST_Send_WIFI_scan_results},   //
  {"/goto_sleep"               , HTTP_POST_GoTo_Sleep            },   //
};

#define SIZE_OF_POST_HANDLERS   (sizeof(post_handlers)/sizeof(post_handlers[0]))


static uint32_t HTTP_send_file(NX_WEB_HTTP_SERVER *server_ptr, UINT request_type, char *file_name);
/*-----------------------------------------------------------------------------------------------------
   Define the application's authentication check.  This is called by the HTTP server whenever a new request is received.

  \param server_ptr
  \param request_type
  \param resource
  \param name
  \param password
  \param realm

  \return UINT
-----------------------------------------------------------------------------------------------------*/
UINT  _HTTP_authentication_check(NX_WEB_HTTP_SERVER *server_ptr, UINT request_type, CHAR *resource, CHAR **name, CHAR **password, CHAR **realm)
{

  // Just use a simple name, password, and realm for all requests and resources.
  *name =     AUTH_USER_NAME;
  *password = (CHAR *)ivar.HTTP_server_password;
  *realm =    REALM_NAME;

  /* Request basic authentication.  */
  return (NX_WEB_HTTP_BASIC_AUTHENTICATE);
}

/*-----------------------------------------------------------------------------------------------------


  \param server_ptr
  \param data_buf
  \param data_buf_size
  \param request_type
  \param mime_type_str

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t HTTP_send_data(NX_WEB_HTTP_SERVER *server_ptr, char *data_buf, uint32_t data_buf_size, UINT request_type, char *mime_type_str)
{
  uint32_t       status;
  char          *ptr;
  NX_PACKET     *new_packet_ptr;
  ULONG          block_sz;
  NX_TCP_SOCKET *socket_ptr =&server_ptr->nx_web_http_server_current_session_ptr->nx_tcp_session_socket;


  // Формируем заголовок отправляемого файла
  status = _nx_web_http_server_generate_response_header(
          server_ptr,                         //  Pointer to HTTP server
          &new_packet_ptr,                    //  Pointer to packet
          NX_WEB_HTTP_STATUS_OK,              //  Status-code and reason-phrase
          sizeof(NX_WEB_HTTP_STATUS_OK)- 1,   //  Length of status-code
          data_buf_size,                      //  Length of content
          mime_type_str,                      //  Type of content
          strlen(mime_type_str),              //  Length of content type
          NX_NULL,                            //  Other HTTP headers
          0);
  if (status != NX_SUCCESS)
  {
    _nx_web_http_server_response_send(server_ptr, NX_WEB_HTTP_STATUS_INTERNAL_ERROR,
      sizeof(NX_WEB_HTTP_STATUS_INTERNAL_ERROR)- 1,
      "NetX HTTP Request Aborted",
      sizeof("NetX HTTP Request Aborted")- 1, NX_NULL, 0);

    return RES_ERROR;
  }

  if (request_type == NX_WEB_HTTP_SERVER_HEAD_REQUEST)
  {
    status = _nx_web_http_server_send(server_ptr, new_packet_ptr, NX_WEB_HTTP_SERVER_TIMEOUT_SEND);
    if (status != NX_SUCCESS)
    {
      nx_packet_release(new_packet_ptr);
      return RES_ERROR;
    }
    return RES_OK;
  }

  // Получаем длину пакета
  block_sz = new_packet_ptr->nx_packet_length;
  ptr = data_buf;

  while (data_buf_size)
  {
    // Определим нужно ли выделять новый пакет
    if (block_sz == 0)
    {
      status =  _nx_web_http_server_response_packet_allocate(server_ptr,&new_packet_ptr, NX_WAIT_FOREVER);
      if (status != NX_SUCCESS)
      {
        server_ptr->nx_web_http_server_allocation_errors++; // Считаем ошибки
        break;
      }
    }

    // Вычисляем свободную длину пакета
    block_sz =((ULONG)(new_packet_ptr->nx_packet_data_end - new_packet_ptr->nx_packet_append_ptr))- NX_PHYSICAL_TRAILER;

    // Корректируем длину чтобы не превысить допустимое значение
    if (block_sz > socket_ptr->nx_tcp_socket_connect_mss) block_sz =  socket_ptr->nx_tcp_socket_connect_mss;

    if (block_sz > data_buf_size)  block_sz = data_buf_size;

    // Читаем данные в пакет
    memcpy(new_packet_ptr->nx_packet_append_ptr, ptr, block_sz);
    ptr += block_sz;

    // Обновим информацию пакета
    new_packet_ptr->nx_packet_length =  new_packet_ptr->nx_packet_length + block_sz;
    new_packet_ptr->nx_packet_append_ptr =  new_packet_ptr->nx_packet_append_ptr + block_sz;

    // Вышлем пакет
    status =  _nx_web_http_server_send(server_ptr, new_packet_ptr, NX_WEB_HTTP_SERVER_TIMEOUT_SEND);

    if (status != NX_SUCCESS)
    {
      nx_packet_release(new_packet_ptr);
      return RES_ERROR;
    }

    // Считаем отправленные байты
    server_ptr->nx_web_http_server_total_bytes_sent =  server_ptr->nx_web_http_server_total_bytes_sent + block_sz;

    // Вычислим сколько еще осталось отправить
    data_buf_size =  data_buf_size - block_sz;

    block_sz = 0; // Отметим необходимость в выделении нового пакета
  }
  return RES_OK;

}

/*-----------------------------------------------------------------------------------------------------


  \param server_ptr
  \param file_name

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
static uint32_t HTTP_send_file(NX_WEB_HTTP_SERVER *server_ptr, UINT request_type, char *file_name)
{
  uint32_t       status;
  CHAR           mime_type_str[48];
  UINT           file_name_len;
  UINT           mime_type_str_len;
  uint32_t       content_length =  0;
  NX_PACKET      *new_packet_ptr;
  ULONG          packet_size;
  ULONG          actual_read;
  NX_TCP_SOCKET *socket_ptr =&server_ptr->nx_web_http_server_current_session_ptr->nx_tcp_session_socket;

  // Проверяем длину строки ресурса
  if (_nx_utility_string_length_check(file_name,&file_name_len, NX_WEB_HTTP_MAX_RESOURCE) != NX_SUCCESS) return RES_ERROR;

  // Открываем файл на чтение
  status =  fx_file_open(server_ptr->nx_web_http_server_media_ptr,&(server_ptr->nx_web_http_server_file), file_name, FX_OPEN_FOR_READ);
  if (status != FX_SUCCESS)
  {
    HTTPLOG("File opening error %d.", status);
    // Возвращаем ответ о неудаче если файл не открылся
    _nx_web_http_server_response_send(server_ptr, NX_WEB_HTTP_STATUS_NOT_FOUND,sizeof(NX_WEB_HTTP_STATUS_NOT_FOUND)- 1,"Unable to find file: ",sizeof("Unable to find file: ")- 1,file_name,file_name_len);
    return RES_ERROR;
  }

  // Получаем длину файла
  content_length = server_ptr->nx_web_http_server_file.fx_file_current_file_size;

  // Получаем тип MIME
  _nx_web_http_server_type_get_extended(server_ptr, file_name, file_name_len, mime_type_str, sizeof(mime_type_str),&mime_type_str_len);


  mime_type_str[mime_type_str_len] = 0;

  // Формируем заголовок отправляемого файла
  status = _nx_web_http_server_generate_response_header(
          server_ptr,                         //  Pointer to HTTP server
          &new_packet_ptr,                    //  Pointer to packet
          NX_WEB_HTTP_STATUS_OK,              //  Status-code and reason-phrase
          sizeof(NX_WEB_HTTP_STATUS_OK)- 1,  //  Length of status-code
          content_length,                     //  Length of content
          mime_type_str,                      //  Type of content
          mime_type_str_len,                  //  Length of content type
          0,                                  //  Other HTTP headers
          0);                                 //  Length of other HTTP headers


  if (status != NX_SUCCESS)
  {
    HTTPLOG("Response generating error %d.", status);
    _nx_web_http_server_response_send(server_ptr, NX_WEB_HTTP_STATUS_INTERNAL_ERROR,sizeof(NX_WEB_HTTP_STATUS_INTERNAL_ERROR)- 1,"Request Aborted",sizeof("Request Aborted")- 1, NX_NULL, 0);
    fx_file_close(&(server_ptr->nx_web_http_server_file));
    return RES_ERROR;
  }

  // Если здесь был запрос только на чтение заголовка, то высылаем заголовок и заканчиваем
  if ((!content_length) || (request_type == NX_WEB_HTTP_SERVER_HEAD_REQUEST))
  {
    fx_file_close(&(server_ptr->nx_web_http_server_file));
    status = _nx_web_http_server_send(server_ptr, new_packet_ptr, NX_WEB_HTTP_SERVER_TIMEOUT_SEND);
    if (status != NX_SUCCESS)
    {
      HTTPLOG("Server send error %d.", status);
      nx_packet_release(new_packet_ptr);
      return RES_ERROR;
    }
    return RES_OK;
  }

  status = _nx_web_http_server_send(server_ptr, new_packet_ptr, NX_WEB_HTTP_SERVER_TIMEOUT_SEND);
  if (status != NX_SUCCESS)
  {
    HTTPLOG("Server send error %d.", status);
    nx_packet_release(new_packet_ptr);
    return RES_ERROR;
  }


  while (content_length)
  {
    // Выделяем новый пакет
    status =  _nx_web_http_server_response_packet_allocate(server_ptr,&new_packet_ptr, NX_WAIT_FOREVER);
    if (status != NX_SUCCESS)
    {
      server_ptr->nx_web_http_server_allocation_errors++; // Считаем ошибки
      break;
    }

    // Вычисляем свободную длину пакета
    packet_size =((ULONG)(new_packet_ptr->nx_packet_data_end - new_packet_ptr->nx_packet_append_ptr))- NX_PHYSICAL_TRAILER;

    // Корректируем длину чтобы не превысить допустимое значение
    if (packet_size > socket_ptr->nx_tcp_socket_connect_mss) packet_size =  socket_ptr->nx_tcp_socket_connect_mss;

    // Читаем данные в пакет
    status =  fx_file_read(&(server_ptr->nx_web_http_server_file), new_packet_ptr->nx_packet_append_ptr,packet_size ,&actual_read);

    if ((status != FX_SUCCESS) && (status != FX_END_OF_FILE))
    {
      HTTPLOG("File read error %d.", status);
      nx_packet_release(new_packet_ptr);
      fx_file_close(&(server_ptr->nx_web_http_server_file));
      return RES_ERROR;
    }

    if (actual_read == 0)
    {
      nx_packet_release(new_packet_ptr);
      break;
    }

    // Обновим информацию пакета
    new_packet_ptr->nx_packet_length =  new_packet_ptr->nx_packet_length + actual_read;
    new_packet_ptr->nx_packet_append_ptr =  new_packet_ptr->nx_packet_append_ptr + actual_read;

    // Вышлем пакет
    status =  _nx_web_http_server_send(server_ptr, new_packet_ptr, NX_WEB_HTTP_SERVER_TIMEOUT_SEND);

    if (status != NX_SUCCESS)
    {
      HTTPLOG("Server send error %d.", status);
      nx_packet_release(new_packet_ptr);
      fx_file_close(&(server_ptr->nx_web_http_server_file));
      return RES_ERROR;
    }

    // Считаем отправленные байты
    server_ptr->nx_web_http_server_total_bytes_sent =  server_ptr->nx_web_http_server_total_bytes_sent + actual_read;

    // Вычислим сколько еще осталось отправить
    content_length =  content_length - actual_read;

  }

  fx_file_close(&(server_ptr->nx_web_http_server_file));
  return RES_OK;
}

/*-----------------------------------------------------------------------------------------------------


  \param server_ptr
  \param request_type
  \param file_indx

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t  HTTP_send_compressed_flash_file(NX_WEB_HTTP_SERVER *server_ptr, UINT request_type,char const *file_name, const uint8_t *file_buf, uint32_t file_size)
{
  UINT           file_name_len;
  uint32_t       decompessed_data_sz;
  uint8_t       *decompessed_data_ptr = NULL;
  CHAR           mime_type_str[48];
  UINT           mime_type_str_len;
  uint32_t       res;

  // Проверяем длину строки ресурса
  if (_nx_utility_string_length_check((CHAR *)file_name,&file_name_len, NX_WEB_HTTP_MAX_RESOURCE) != NX_SUCCESS) return RES_ERROR;

  decompessed_data_sz = file_buf[0] +(file_buf[1] << 8)+(file_buf[2] << 16)+(file_buf[3] << 24);
  decompessed_data_ptr = SDRAM_malloc_pending(decompessed_data_sz + 1,10);
  if (decompessed_data_ptr == NULL) return RES_ERROR;
  if (Decompress_mem_to_mem(COMPR_ALG_SIXPACK, (void*)file_buf, file_size, decompessed_data_ptr, decompessed_data_sz) != decompessed_data_sz)
  {
    SDRAM_free(decompessed_data_ptr);
    return RES_ERROR;
  }


  // Получаем тип MIME
  _nx_web_http_server_type_get_extended(server_ptr, (CHAR *)file_name, file_name_len, mime_type_str, sizeof(mime_type_str),&mime_type_str_len);

  res = HTTP_send_data(server_ptr,(char *)decompessed_data_ptr,decompessed_data_sz,request_type,mime_type_str);

  SDRAM_free(decompessed_data_ptr);
  return res;
}


/*-----------------------------------------------------------------------------------------------------


  \param server_ptr
  \param url
  \param packet_ptr

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t HTTP_POST_Get_params(NX_WEB_HTTP_SERVER *server_ptr, CHAR *url, NX_PACKET *packet_ptr, uint8_t ptype)
{
  uint32_t   status;
  char            *buffer   = NULL;
  uint32_t         buff_sz;

  buffer = ParamsSchema_serialize_to_buff(ptype, &buff_sz);
  if (buffer == NULL)
  {
    _nx_web_http_server_response_send(server_ptr, NX_WEB_HTTP_STATUS_INTERNAL_ERROR,sizeof(NX_WEB_HTTP_STATUS_INTERNAL_ERROR) - 1,NULL,0,NULL,0);
    return RES_ERROR;
  }
  status = HTTP_send_data(server_ptr, buffer, buff_sz,NX_WEB_HTTP_SERVER_POST_REQUEST,"application/json");
  App_free(buffer);
  return status;
}


/*-----------------------------------------------------------------------------------------------------


  \param server_ptr
  \param url
  \param packet_ptr

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t HTTP_POST_Save_params(NX_WEB_HTTP_SERVER *server_ptr, CHAR *url, NX_PACKET *packet_ptr, uint8_t ptype)
{
  uint32_t   status;
  uint8_t   *input_buffer = NULL;
  uint32_t   received_size = 0;
  NX_PACKET *recv_packet_ptr;

  // Принимаем из браузера json файл с отредактированными параметрами для сохранения

  ULONG content_length;
  status = nx_web_http_server_content_length_get(packet_ptr,&content_length);
  if (status != NX_SUCCESS) goto error_;
  if (content_length > 0)
  {
    uint32_t actual_size;

    // Слишком большие блоки данных принять не можем
    if (content_length > MAX_PUT_DATA_SIZE) goto error_;

    // Выделяем память для поступающих данных
    input_buffer = App_malloc_pending(content_length + 1,10);
    if (input_buffer == NULL) goto error_;

    // Получаем данные из первого пакета
    status = nx_web_http_server_content_get(server_ptr, packet_ptr, 0, (CHAR *)input_buffer, content_length,&actual_size);
    if (status != NX_SUCCESS) goto error_;

    received_size = actual_size;

    // Принимаем остальные пакеты с данными
    while (received_size < content_length)
    {
      ULONG packet_length;
      status = nx_web_http_server_packet_get(server_ptr,&recv_packet_ptr);
      if (status != NX_SUCCESS) goto error_;
      nx_packet_length_get(recv_packet_ptr,&packet_length);

      uint32_t remain = content_length - received_size;
      if (packet_length > remain) packet_length = remain;
      nx_packet_data_retrieve(recv_packet_ptr,&input_buffer[received_size],&packet_length);
      nx_packet_release(recv_packet_ptr);

      received_size += packet_length;

    }

  }
  else goto error_;

  input_buffer[received_size] = 0; // Завершаем нулем json блок данных.

  if (JSON_Deser_settings(ptype,(char *)input_buffer) != RES_OK) goto error_;

  App_free(input_buffer);
  Request_save_nv_parameters(ptype);
  _nx_web_http_server_response_send(server_ptr, NX_WEB_HTTP_STATUS_OK, sizeof(NX_WEB_HTTP_STATUS_OK) - 1, NULL, 0, NULL, 0);
  return RES_OK;

error_:
  App_free(input_buffer);
  _nx_web_http_server_response_send(server_ptr, NX_WEB_HTTP_STATUS_INTERNAL_ERROR, sizeof(NX_WEB_HTTP_STATUS_INTERNAL_ERROR) - 1, "NetX HTTP Request Aborted", sizeof("NetX HTTP Request Aborted") - 1, NX_NULL, 0);
  return RES_ERROR;
}

/*-----------------------------------------------------------------------------------------------------


  \param server_ptr
  \param resource
  \param packet_ptr

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
static uint32_t HTTP_GET_handler(NX_WEB_HTTP_SERVER *server_ptr, CHAR *resource, NX_PACKET *packet_ptr)
{
  char *file_name;

#ifdef APP_WEB

  if (App_get_handler(server_ptr, resource, packet_ptr) == RES_OK)
  {
    return (NX_WEB_HTTP_CALLBACK_COMPLETED);
  }
#endif

  file_name =&resource[1];
  // Ищем файл в таблице файлов содержащихся во Flash памяти
  for (uint32_t i=0; i < COMPRESSED_WEB_FILES_NUM; i++)
  {
    if (strcmp(file_name,  compressed_web_files[i].name) == 0)
    {
      // Файл найден, отправляем его
      HTTP_send_compressed_flash_file(server_ptr, NX_WEB_HTTP_SERVER_GET_REQUEST, compressed_web_files[i].name, compressed_web_files[i].data, compressed_web_files[i].size);
      return (NX_WEB_HTTP_CALLBACK_COMPLETED);
    }
  }

  if (strcmp(resource, GET_GEN_LOG_FILE) == 0)
  {
    HTTP_send_file(server_ptr, NX_WEB_HTTP_SERVER_GET_REQUEST, APP_LOG_FILE_PATH);
    return (NX_WEB_HTTP_CALLBACK_COMPLETED);
  }
  else if (strcmp(resource, GET_GEN_NETLOG_FILE) == 0)
  {
    HTTP_send_file(server_ptr, NX_WEB_HTTP_SERVER_GET_REQUEST, NET_LOG_FILE_PATH);
    return (NX_WEB_HTTP_CALLBACK_COMPLETED);
  }



  // Символ '/' или '\' означает что путь к файлу абсолютный и поэтому не будет работать  назначение fx_directory_local_path_set
  // В этом случае надо откорректировать содержание запроса чтобы в нем отсутствовал некорректный первый символ
  server_ptr->nx_web_http_server_request_resource[0] = ' ';

  // Возвращаем статус = NX_SUCCESS о том что запрос не обработан и требуется дальнейшая обработка
  return NX_SUCCESS;
}

/*-----------------------------------------------------------------------------------------------------


  \param server_ptr

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
static uint32_t HTTP_POST_handler(NX_WEB_HTTP_SERVER *server_ptr, CHAR *resource, NX_PACKET *packet_ptr)
{
#ifdef APP_WEB

  if (App_pos_handler(server_ptr, resource, packet_ptr) == RES_OK)
  {
    return (NX_WEB_HTTP_CALLBACK_COMPLETED);
  }
#endif

  for (uint32_t i = 0; i < SIZE_OF_POST_HANDLERS; i++)
  {
    if (strcmp(post_handlers[i].url,resource) == 0)
    {
      post_handlers[i].handler_func(server_ptr, resource, packet_ptr);
      return NX_WEB_HTTP_CALLBACK_COMPLETED;
    }
  }
  _nx_web_http_server_response_send(server_ptr, NX_WEB_HTTP_STATUS_INTERNAL_ERROR, sizeof(NX_WEB_HTTP_STATUS_INTERNAL_ERROR)- 1, "NetX HTTP Request Aborted", sizeof("NetX HTTP Request Aborted")- 1, NX_NULL, 0);
  return NX_WEB_HTTP_ERROR;
}

/*-----------------------------------------------------------------------------------------------------


  \param server_ptr

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t HTTP_send_not_found(NX_WEB_HTTP_SERVER *server_ptr)
{
  uint32_t    status;
  NX_PACKET   *response_pkt;
  int32_t      response_len;

  response_len = sprintf(response_buf,"<!DOCTYPE html><html xmlns=\"http://www.w3.org/1999/xhtml\"><head><title>404 Not found</title></head><body><h1>Not found</h1></body></html>\r\n");

  status = nx_web_http_server_callback_generate_response_header_extended(
          server_ptr,
          &response_pkt,
          NX_WEB_HTTP_STATUS_OK,         // status_code.              Indicate status of resource.
          strlen(NX_WEB_HTTP_STATUS_OK), // status_code_length.
          response_len,                  // content_length.           Size of content in bytes
          "text/html",                   // content_type.             Type of HTTP e.g. "text/plain"
          strlen("text/html"),           // content_type_length.      String length of content type
          NX_NULL,                       // additional_header.        Pointer to additional header text
          0                              // additional_header_length. Length of additional header text
          );
  if (status != NX_SUCCESS) return (status);

  nx_packet_data_append(response_pkt, response_buf, response_len, server_ptr->nx_web_http_server_packet_pool_ptr, NX_WAIT_FOREVER);

  status = nx_web_http_server_callback_packet_send(server_ptr, response_pkt);
  if (status != NX_SUCCESS)
  {
    nx_packet_release(response_pkt);
    return (status);
  }

  return (NX_WEB_HTTP_CALLBACK_COMPLETED);

}

/*-----------------------------------------------------------------------------------------------------


  \param resource
  \param max_age
  \param last_modified

  \return UINT
-----------------------------------------------------------------------------------------------------*/
UINT cache_info_callback(CHAR *resource, UINT *max_age, NX_WEB_HTTP_SERVER_DATE *last_modified)
{
  rtc_time_t rt_time;
  *max_age = 1;

  RTC_get_system_DateTime(&rt_time);
  last_modified->nx_web_http_server_year   = rt_time.tm_year + 1900;
  last_modified->nx_web_http_server_month  = rt_time.tm_mon + 1;
  last_modified->nx_web_http_server_day    = rt_time.tm_mday;
  last_modified->nx_web_http_server_hour   = rt_time.tm_hour;
  last_modified->nx_web_http_server_minute = rt_time.tm_min;
  last_modified->nx_web_http_server_second = rt_time.tm_sec;
  last_modified->nx_web_http_server_weekday = rt_time.tm_wday;
  return NX_TRUE;
}

/*-----------------------------------------------------------------------------------------------------


  \param last_modified
-----------------------------------------------------------------------------------------------------*/
void gmt_get_callback(NX_WEB_HTTP_SERVER_DATE *last_modified)
{
  rtc_time_t rt_time;
  RTC_get_system_DateTime(&rt_time);
  last_modified->nx_web_http_server_year   = rt_time.tm_year + 1900 - 1;
  last_modified->nx_web_http_server_month  = rt_time.tm_mon + 1;
  last_modified->nx_web_http_server_day    = rt_time.tm_mday;
  last_modified->nx_web_http_server_hour   = rt_time.tm_hour;
  last_modified->nx_web_http_server_minute = rt_time.tm_min;
  last_modified->nx_web_http_server_second = rt_time.tm_sec;
  last_modified->nx_web_http_server_weekday = rt_time.tm_wday;
}
/*-----------------------------------------------------------------------------------------------------
  Вызывается в функции _nx_tcpserver_data_process -> _nx_web_http_server_receive_data -> nx_web_http_server_get_process
  путем вызова server_ptr -> nx_web_http_server_request_notify
  в контексте задачи TCPSERVER Thread создаваемой в функции _nx_tcpserver_create

  \param server_ptr
  \param request_type
  \param resource
  \param packet_ptr

  \return UINT
-----------------------------------------------------------------------------------------------------*/
UINT _HTTP_server_request_callback(NX_WEB_HTTP_SERVER *server_ptr, UINT request_type, CHAR *resource, NX_PACKET *packet_ptr)
{
  uint32_t res;
  // Создаем локальный контекст задачи если он еще не создан
  if (web_first_call)
  {
    // Устаноавливаем директорию по умолчанию для файлов WEB сервера
    fx_directory_local_path_set(server_ptr->nx_web_http_server_media_ptr,&web_local_path, WEB_FILES_DIR_NAME);
    web_first_call = 0;

    nx_web_http_server_cache_info_callback_set(server_ptr, cache_info_callback);
    nx_web_http_server_gmt_callback_set(server_ptr,gmt_get_callback);
  }

  HTTPLOG("WEB | Req.type %d | url = %s", request_type, resource)
  if (request_type == NX_WEB_HTTP_SERVER_HEAD_REQUEST)
  {
    return (NX_SUCCESS);
  }
  if (request_type == NX_WEB_HTTP_SERVER_GET_REQUEST)
  {
    res = HTTP_GET_handler(server_ptr, resource, packet_ptr);
    return (res);
  }
  else if (request_type == NX_WEB_HTTP_SERVER_POST_REQUEST)
  {
    res = HTTP_POST_handler(server_ptr, resource, packet_ptr);
    return res;
  }
  else
  {
    // Символ '/' или '\' означает что путь к файлу абсолютный и поэтому не будет работать  назначение fx_directory_local_path_set
    // В этом случае надо откорректировать содержание запроса чтобы в нем отсутствовал некорректный первый символ
    server_ptr->nx_web_http_server_request_resource[0] = ' ';
    // Возвращаем статус = NX_SUCCESS о том что запрос не обработан и требуется дальнейшая обработка
    return (NX_SUCCESS);
  }


}


/*-----------------------------------------------------------------------------------------------------


  \param void

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t  Net_HTTP_server_delete(void)
{
  uint32_t res = NX_SUCCESS;
  if (http_ip_ptr != 0)
  {
    res = nx_web_http_server_stop(&http_server);
    if (res == NX_SUCCESS)
    {
      HTTPLOG("HTTP server stopped successfully");
    }
    else
    {
      HTTPLOG("Failed to stop HTTP server. Error %d", res);
    }

    res = nx_web_http_server_delete(&http_server);
    if (res == NX_SUCCESS)
    {
      HTTPLOG("HTTP server deleted successfully");
    }
    else
    {
      HTTPLOG("Failed to delete HTTP server. Error %d", res);
    }

    http_ip_ptr = 0;
  }
  return res;
}

/*-----------------------------------------------------------------------------------------------------


-----------------------------------------------------------------------------------------------------*/
uint32_t  HTTP_server_create(NX_IP *ip_ptr)
{
  UINT status;
  UINT port;

  if (ivar.enable_HTTP_server == 0) return RES_ERROR;
  if (http_ip_ptr != 0) return RES_ERROR;

#ifdef ENABLE_SDRAM
  if (ivar.enable_HTTPS != 0)
  {
    port = NX_WEB_HTTPS_SERVER_PORT;
  }
  else
#endif
  {
    port = NX_WEB_HTTP_SERVER_PORT;
  }

  status = nx_web_http_server_create(&http_server,
          "HTTP Server",
          ip_ptr,
          port,
          &fat_fs_media,
          server_stack,
          WEB_SERVER_STACK_SIZE,
          &net_packet_pool,
          _HTTP_authentication_check,
          _HTTP_server_request_callback);

  if (NX_SUCCESS != status)
  {
    HTTPLOG("HTTP server create error %d", status);
    return RES_ERROR;
  }
  HTTPLOG("HTTP server created successfully.");

#ifdef ENABLE_SDRAM
  if (ivar.enable_HTTPS)
  {
    memset(&certificate, 0, sizeof(certificate));
    memset(crypto_metadata_server, 0, sizeof(crypto_metadata_server));
    memset(tls_packet_buffer, 0, sizeof(tls_packet_buffer));
    nx_secure_x509_certificate_initialize(&certificate, (UCHAR *)SERVER_CERTIFICATE, sizeof(SERVER_CERTIFICATE), NX_NULL, 0, SERVER_KEY, sizeof(SERVER_KEY), NX_SECURE_X509_KEY_TYPE_EC_DER);

    // Setup TLS session data for the TCP server.
    status = nx_web_http_server_secure_configure(
            &http_server,
            &CRIPTO_TLS_CYPHERS,
            crypto_metadata_server,
            sizeof(crypto_metadata_server),
            tls_packet_buffer,
            sizeof(tls_packet_buffer),
            &certificate,
            NX_NULL, 0, NX_NULL, 0, NX_NULL, 0
            );
    if (status != NX_SUCCESS)
    {
      HTTPLOG("HTTP server secure configuration error %d", status);
    }

  }
#endif

  web_first_call = 1;
  status = nx_web_http_server_start(&http_server);
  if (status == NX_SUCCESS)
  {
    http_ip_ptr = ip_ptr;
    HTTPLOG("HTTP server started successfully.");
    return RES_OK;
  }
  else
  {
    HTTPLOG("HTTP server starting  error %d", status);
    nx_web_http_server_delete(&http_server);
    return RES_ERROR;
  }

}



