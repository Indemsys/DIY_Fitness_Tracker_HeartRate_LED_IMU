// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 2021-03-05
// 12:07:22
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include   "App.h"
#include   "Net.h"

extern VOID  _nx_sntp_client_utility_fraction_to_usecs(ULONG fraction, ULONG *usecs);

#define   EVENT_SNTP_UPDATE  BIT(0)
static UINT _Leap_second_handler(NX_SNTP_CLIENT *client_ptr, UINT leap_indicator);
static UINT _Kiss_of_death_handler(NX_SNTP_CLIENT *client_ptr, UINT KOD_code);
static VOID _SNTP_time_update_callback(NX_SNTP_TIME_MESSAGE *time_update_ptr, NX_SNTP_TIME *local_time);

NX_SNTP_CLIENT          sntp_client;


TX_EVENT_FLAGS_GROUP    sntp_flags;
static NX_IP           *snmp_ip_ptr;


typedef struct
{
    char const *address;
    uint8_t     disabled;
    uint32_t    nameresolve_fails_count;
    uint32_t    timeout_fails_count;
} T_sntp_servers;

T_sntp_servers sntp_servers[] =
{
  {(char *)ivar.time_server_1 , 0},
  {(char *)ivar.time_server_2 , 0},
  {(char *)ivar.time_server_3 , 0},
};

#define SNTP_SERVERS_NUM (sizeof(sntp_servers)/sizeof(sntp_servers[0]))

static uint32_t            g_current_sntp_serv_indx;
static T_sys_timestump     sntp_server_ack_ts;
static T_sys_timestump     sntp_last_log_ts;
static uint8_t             sntp_need_log;
static ULONG               sntp_address;
static uint32_t            sntp_KOD_code;
static uint8_t             has_internet_connection;

/*-----------------------------------------------------------------------------------------------------
  Перемещаем индекс на следующий SNTP сервер

  \param void

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
static uint32_t SNTP_select_next_server(void)
{
  uint32_t start_indx;
  start_indx = g_current_sntp_serv_indx;
  do
  {
    g_current_sntp_serv_indx++;
    if (g_current_sntp_serv_indx >= SNTP_SERVERS_NUM) g_current_sntp_serv_indx = 0;
    if (start_indx == g_current_sntp_serv_indx) return RES_ERROR;
    if (sntp_servers[g_current_sntp_serv_indx].disabled == 0) return RES_OK;
  }while (1);
}

/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
static void SNTP_server_name_resolve_fail(void)
{
  sntp_servers[g_current_sntp_serv_indx].nameresolve_fails_count++;
}

/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
static void SNTP_server_count_timeout_errors(void)
{
  sntp_servers[g_current_sntp_serv_indx].timeout_fails_count++;
}


/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
static void SNTP_reenable_all_server(void)
{
  for (uint32_t i = 0; i < SNTP_SERVERS_NUM; i++)
  {
    sntp_servers[i].disabled = 0;
  }
}

/*-----------------------------------------------------------------------------------------------------
  Получить IP адрес сервера по индексу в массиве записей с URL

  \param indx

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
static uint32_t SNTP_get_server_address(uint8_t *ip_address_ptr)
{
  UINT    status;
  ULONG   host_ip_address;

  do
  {
    if (Str_to_IP_v4(sntp_servers[g_current_sntp_serv_indx].address,(uint8_t *)&host_ip_address) != RES_OK)
    {
      // Используем DNS для определения IP адреса
      status =  DNS_get_host_address((UCHAR *)sntp_servers[g_current_sntp_serv_indx].address,&host_ip_address, MS_TO_TICKS(2000));
      if (status == NX_SUCCESS)
      {
        NETLOG("SNTP. Selected server %s (%03d.%03d.%03d.%03d)",sntp_servers[g_current_sntp_serv_indx].address, IPADDR(host_ip_address));
        memcpy(ip_address_ptr,&host_ip_address, 4);
        if (host_ip_address != 0)   return RES_OK;
      }
      else
      {
        NETLOG("SNTP. DNS request of %s failed. Error %04X",sntp_servers[g_current_sntp_serv_indx].address, status);
      }

      // Если неудача, то переходим на следующий адрес, а текущий исключаем из списка кандидатов
      SNTP_server_name_resolve_fail();
      if (SNTP_select_next_server() != RES_OK) return RES_ERROR;
      Wait_ms(5000);
      continue;
    }
    else
    {
      NETLOG("SNTP. Selected server %s",sntp_servers[g_current_sntp_serv_indx].address);
      memcpy(ip_address_ptr,&host_ip_address, 4);
      return RES_OK;
    }
  } while (1);
}


/*-----------------------------------------------------------------------------------------------------
  Callback for application response to impending leap second

   This application defined handler for handling an impending leap second is not required by the SNTP Client.
   The default handler below only logs the event for every time stamp received with the leap indicator set.

  \param client_ptr
  \param indicator

  \return UINT
-----------------------------------------------------------------------------------------------------*/
static UINT _Leap_second_handler(NX_SNTP_CLIENT *client_ptr, UINT indicator)
{

  return NX_SUCCESS;
}

/*-----------------------------------------------------------------------------------------------------
  Callback for application response to receiving Kiss of Death packet

   This application defined handler for handling a Kiss of Death packet is not required by the SNTP Client.
   A KOD handler should determine if the Client task should continue vs. abort sending/receiving time data
   from its current time server, and if aborting if it should remove the server from its active server list.

  \param client_ptr
  \param code

  \return UINT
-----------------------------------------------------------------------------------------------------*/
static UINT _Kiss_of_death_handler(NX_SNTP_CLIENT *client_ptr, UINT code)
{
  UINT    status = NX_SUCCESS;

  NX_PARAMETER_NOT_USED(client_ptr);

  sntp_KOD_code = code;

  return status;
}

/*-----------------------------------------------------------------------------------------------------
   Функция вызываемая при получении ответа от сервера времени

  \param time_update_ptr
  \param local_time
-----------------------------------------------------------------------------------------------------*/
static void _SNTP_time_update_callback(NX_SNTP_TIME_MESSAGE *time_update_ptr, NX_SNTP_TIME *local_time)
{
  tx_event_flags_set(&sntp_flags, EVENT_SNTP_UPDATE, TX_OR);
}


/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
uint32_t Net_SNTP_client_create(NX_IP  *ip_ptr)
{
  UINT     status;

  if (snmp_ip_ptr != NULL)    return RES_OK;
  if (ivar.en_sntp == 0)      return RES_ERROR;
  if (Is_DNS_created() == 0)  return RES_ERROR;


  status = SNTP_get_server_address((uint8_t *)&sntp_address);
  if (status != RES_OK)
  {
    NETLOG("SNTP. Unable to get server address: %d", status);
    return RES_ERROR;
  }



  status = tx_event_flags_create(&sntp_flags, "SNTP flags");
  if (status != TX_SUCCESS)
  {
    NETLOG("SNTP. Client event creation error %d",status);
    goto err1;
  }
  status =  nx_sntp_client_create(&sntp_client, ip_ptr, 0,&net_packet_pool,  _Leap_second_handler, _Kiss_of_death_handler, NULL); /* no random_number_generator callback */
  if (status != NX_SUCCESS)
  {
    NETLOG("SNTP. Client creation error %d",status);
    goto err;
  }

  nx_sntp_client_set_time_update_notify(&sntp_client, _SNTP_time_update_callback);

  status = nx_sntp_client_set_local_time(&sntp_client, 0,0);
  if (status != NX_SUCCESS)
  {
    NETLOG("SNTP. set local time error: %d", status);
    goto err;
  }

  status = nx_sntp_client_initialize_unicast(&sntp_client, sntp_address, ivar.sntp_poll_interval);
  if (status != NX_SUCCESS)
  {
    NETLOG("SNTP. inialising error: %d", status);
    goto err;
  }

  status = nx_sntp_client_run_unicast(&sntp_client);
  if (status != NX_SUCCESS)
  {
    NETLOG("SNTP. Run unicast error: %d", status);
    goto err;
  }

  NETLOG("SNTP. Client created");
  Get_hw_timestump(&sntp_server_ack_ts);
  Get_hw_timestump(&sntp_last_log_ts);
  sntp_need_log           = 1;
  has_internet_connection = 0;
  snmp_ip_ptr             = ip_ptr;
  return RES_OK;

err:
  nx_sntp_client_delete(&sntp_client);
  tx_event_flags_delete(&sntp_flags);
err1:
  snmp_ip_ptr = NULL;
  return RES_ERROR;
}

/*-----------------------------------------------------------------------------------------------------


  \param ip_ptr

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t SNTP_client_delete(void)
{
  volatile UINT     status;

  if (snmp_ip_ptr == 0) return RES_OK;

  status = nx_sntp_client_stop(&sntp_client);
  NETLOG("SNTP. Stop result: 0x%08X", status);
  status = nx_sntp_client_delete(&sntp_client);
  NETLOG("SNTP. Delete result: 0x%08X", status);
  status = tx_event_flags_delete(&sntp_flags);
  NETLOG("SNTP. Delete flags result: 0x%08X", status);
  snmp_ip_ptr   = 0;
  sntp_need_log = 1;
  SNTP_reenable_all_server();
  return RES_OK;

}

/*-----------------------------------------------------------------------------------------------------


  \param void

  \return uint8_t
-----------------------------------------------------------------------------------------------------*/
uint8_t Is_connected_to_internet(void)
{
  return has_internet_connection;
}
/*-----------------------------------------------------------------------------------------------------


  \param void

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
void Net_SNTP_client_task(NX_IP   *ip_ptr)
{
  ULONG     status;
  ULONG     events = 0;
  char      time_str[128];

  if (tx_event_flags_get(&sntp_flags, EVENT_SNTP_UPDATE, TX_OR_CLEAR,&events, TX_NO_WAIT) == TX_SUCCESS)
  {
    UINT         server_status;
    ULONG        seconds;
    volatile ULONG        milliseconds;
    ULONG        microseconds;
    ULONG        fraction;
    rtc_time_t   rt_time;


    if (events & EVENT_SNTP_UPDATE)
    {
      // Здесь получили ответ от сервера времени и обрабатываем его
      Get_hw_timestump(&sntp_server_ack_ts);

      status = nx_sntp_client_receiving_updates(&sntp_client,&server_status);
      if ((status == NX_SUCCESS) && (server_status != NX_FALSE))
      {
        status = nx_sntp_client_get_local_time_extended(&sntp_client,&seconds,&fraction, NX_NULL, 0);
        if (status == NX_SUCCESS)
        {
          has_internet_connection = 1;
          _nx_sntp_client_utility_fraction_to_usecs(fraction,&microseconds);
          milliseconds = ((microseconds + 500) / 1000);


          Convert_NTP_to_UTC_time(seconds,&rt_time, ivar.utc_offset);

          // Выводим полученное значение времени в лог каждые 10 мин
          if ((Time_elapsed_sec(&sntp_last_log_ts) > (10 * 60)) || (sntp_need_log!=0))
          {
            Get_hw_timestump(&sntp_last_log_ts);
            sntp_need_log = 0;
            nx_sntp_client_utility_display_date_time(&sntp_client , time_str, 128); // Создать строку с датой временем
            NETLOG("SNTP sec, msec: %u %u" , seconds, milliseconds);
            NETLOG("SNTP resp: %s", time_str);
            NETLOG("Time : %04d.%02d.%02d %02d:%02d:%02d", rt_time.tm_year + 1970, rt_time.tm_mon + 1, rt_time.tm_mday, rt_time.tm_hour, rt_time.tm_min, rt_time.tm_sec);
          }

          if (ivar.en_sntp_time_receiving)
          {
            // Обновить время в RTC устройства
            rt_time.tm_year = rt_time.tm_year + 1970 - 1900;
            RTC_set_system_DateTime(&rt_time);
          }
        }
        else
        {
          NETLOG("SNTP. Get local time error %d", status);
        }
      }
      else
      {
        NETLOG("SNTP. Receiving updates error %d", status);
      }
    }
  }

  if (sntp_client.nx_sntp_client_started)
  {
    // Проверим как долго ждем ответа от сервера времени.
    // Если ждем слишком долго, то меняем сервер времени.

    if ((Time_elapsed_sec(&sntp_server_ack_ts) > (ivar.sntp_poll_interval + 10)) || (sntp_KOD_code != 0))
    {
      has_internet_connection = 0;
      sntp_need_log           = 1;
      sntp_KOD_code           = 0;
      Get_hw_timestump(&sntp_server_ack_ts);

      // Переходим на другой сервер
      SNTP_server_count_timeout_errors();
      SNTP_select_next_server();
      status = nx_sntp_client_stop(&sntp_client);
      if (status == NX_SUCCESS)
      {
        if (SNTP_get_server_address((uint8_t *)&sntp_address) == RES_OK)
        {
          nx_sntp_client_initialize_unicast(&sntp_client, sntp_address, ivar.sntp_poll_interval);
          nx_sntp_client_run_unicast(&sntp_client);
        }
      }
    }
  }


}

