﻿// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 2024-07-26
// 16:08:05
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include   "App.h"



/*-----------------------------------------------------------------------------------------------------


  \param server_ptr
  \param request_type

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t HTTP_POST_send_device_info(NX_WEB_HTTP_SERVER *server_ptr, CHAR *url, NX_PACKET *packet_ptr)
{
  uint32_t    status;
  char       *buffer  = NULL;
  uint32_t    buff_sz = 0;

  buffer = DeviceInfo_serializer_to_buff(&buff_sz);
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
  \param request_type

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t HTTP_POST_send_device_status(NX_WEB_HTTP_SERVER *server_ptr, CHAR *url, NX_PACKET *packet_ptr)
{
  uint32_t    status;
  char       *buffer  = NULL;
  uint32_t    buff_sz = 0;

  buffer = DeviceStatus_serializer_to_buff(&buff_sz);
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
  \param packet_ptr
  \param request_type
  \param url

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t HTTP_POST_upload_time(NX_WEB_HTTP_SERVER *server_ptr, CHAR *url, NX_PACKET *packet_ptr)
{
  uint32_t             res;
  uint8_t             *input_buffer = NULL;
  UINT                 actual_size;

  json_t              *root = 0;
  json_error_t         error;

  uint32_t             year = 0;
  uint32_t             month = 0;
  uint32_t             day  = 0;
  uint32_t             hours = 0;
  uint32_t             mins = 0;
  uint32_t             secs = 0;
  rtc_time_t           rt_time = {0};
  char                 str[64];

  ULONG content_length;
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

    json_unpack(root, "{s:i, s:i, s:i, s:i, s:i, s:i}", "year",&year, "month",&month, "day",&day, "hours",&hours, "mins",&mins, "secs",&secs);
    rt_time.tm_year  = year - 1900;
    rt_time.tm_mon   = month;
    rt_time.tm_mday  = day;
    rt_time.tm_hour  = hours;
    rt_time.tm_min   = mins;
    rt_time.tm_sec   = secs;

    RTC_set_system_DateTime(&rt_time);
  }

  json_decref(root);
  App_free(input_buffer);
  strcpy(str,"{\"status\":0}");
  HTTP_send_data(server_ptr,str, strlen(str),NX_WEB_HTTP_SERVER_POST_REQUEST,"application/json");
  return res;

err_:
  json_decref(root);
  App_free(input_buffer);
  strcpy(str,"{\"status\":1}");
  HTTP_send_data(server_ptr,str, strlen(str),NX_WEB_HTTP_SERVER_POST_REQUEST,"application/json");
  return RES_ERROR;
}

/*-----------------------------------------------------------------------------------------------------


  \param server_ptr
  \param url
  \param packet_ptr

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t HTTP_POST_Start_WIFI_scan(NX_WEB_HTTP_SERVER *server_ptr, CHAR *url, NX_PACKET *packet_ptr)
{
  Send_wifi_event(NET_EVT_START_WIFI_SCAN);
  _nx_web_http_server_response_send(server_ptr, NX_WEB_HTTP_STATUS_OK, sizeof(NX_WEB_HTTP_STATUS_OK) - 1, "Request Accepted", sizeof("Request Accepted") - 1, NX_NULL, 0);
  return RES_OK;
}

/*-----------------------------------------------------------------------------------------------------


  \param server_ptr
  \param url
  \param packet_ptr

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t HTTP_POST_Send_WIFI_scan_results(NX_WEB_HTTP_SERVER *server_ptr, CHAR *url, NX_PACKET *packet_ptr)
{
  uint32_t    status;
  char       *buffer  = NULL;
  uint32_t    buff_sz = 0;

  buffer = WiFi_scan_results_serializer_to_buff(&buff_sz);
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
uint32_t HTTP_POST_GoTo_Sleep(NX_WEB_HTTP_SERVER *server_ptr, CHAR *url, NX_PACKET *packet_ptr)
{
  Set_app_event(EVENT_GOTO_SLEEP);
  _nx_web_http_server_response_send(server_ptr, NX_WEB_HTTP_STATUS_OK, sizeof(NX_WEB_HTTP_STATUS_OK) - 1, "Request Accepted", sizeof("Request Accepted") - 1, NX_NULL, 0);
  return RES_OK;
}

/*-----------------------------------------------------------------------------------------------------


  \param server_ptr
  \param request_type

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t HTTP_POST_Reset_device(NX_WEB_HTTP_SERVER *server_ptr, CHAR *url, NX_PACKET *packet_ptr)
{
  _nx_web_http_server_response_send(server_ptr, NX_WEB_HTTP_STATUS_OK, sizeof(NX_WEB_HTTP_STATUS_OK) - 1, "Request Accepted", sizeof("Request Accepted") - 1, NX_NULL, 0);
  Wait_ms(300);
  Reset_SoC();
  return RES_OK;
}

/*-----------------------------------------------------------------------------------------------------


  \param server_ptr
  \param resource
  \param packet_ptr

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t  HTTP_POST_Creal_Gen_Log_File(NX_WEB_HTTP_SERVER *server_ptr, CHAR *url, NX_PACKET *packet_ptr)
{
  _nx_web_http_server_response_send(server_ptr, NX_WEB_HTTP_STATUS_OK, sizeof(NX_WEB_HTTP_STATUS_OK) - 1, "Request Accepted", sizeof("Request Accepted") - 1, NX_NULL, 0);
  Req_to_reset_log_file();
  return RES_OK;
}

/*-----------------------------------------------------------------------------------------------------


  \param server_ptr
  \param resource
  \param packet_ptr

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t  HTTP_POST_Creal_Net_Log_File(NX_WEB_HTTP_SERVER *server_ptr, CHAR *url, NX_PACKET *packet_ptr)
{
  _nx_web_http_server_response_send(server_ptr, NX_WEB_HTTP_STATUS_OK, sizeof(NX_WEB_HTTP_STATUS_OK) - 1, "Request Accepted", sizeof("Request Accepted") - 1, NX_NULL, 0);
  Req_to_reset_netlog_file();
  return RES_OK;
}

/*-----------------------------------------------------------------------------------------------------


  \param server_ptr
  \param url
  \param packet_ptr

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t HTTP_POST_Get_module_params(NX_WEB_HTTP_SERVER *server_ptr, CHAR *url, NX_PACKET *packet_ptr)
{
  return HTTP_POST_Get_params(server_ptr,url,packet_ptr,MODULE_PARAMS);
}


/*-----------------------------------------------------------------------------------------------------


  \param server_ptr
  \param url
  \param packet_ptr

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t HTTP_POST_Save_module_params(NX_WEB_HTTP_SERVER *server_ptr, CHAR *url, NX_PACKET *packet_ptr)
{
  return HTTP_POST_Save_params(server_ptr,url,packet_ptr,MODULE_PARAMS);
}


