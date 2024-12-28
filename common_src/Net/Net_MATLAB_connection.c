// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 2022-08-19
// 11:37:03
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include   "App.h"
#include   "Net.h"
#include   "tx_thread.h"

#ifdef ENABLE_MATLAB_CONNECTION


NET_TCP_SERVER    *matlab_tcp_server_ptr;
T_tcpsrv_setup     matlab_tcp_setup;


static void TCP_new_connection_callback(NET_TCP_SERVER *server_ptr);
static void TCP_receive_data_callback(NET_TCP_SERVER *server_ptr, NX_PACKET *packet_ptr);
static void TCP_connection_end_callback(NET_TCP_SERVER *server_ptr);

uint8_t                        matlab_server_created;
uint8_t                        matlab_connected;
uint32_t                       matlab_cmd_id;
T_matlab_cmd1                  matlab_cmd1;
T_matlab_cmd2                  matlab_cmd2;
T_matlab_communication_status  mc_stat;

static TX_MUTEX                matlab_mutex;
static TX_THREAD               matlab_thread;

typedef struct
{
    uint32_t     pack_num;
    NX_PACKET   *packet;
} T_matlab_msg;

  #define  MATLAB_QUEUE_BUF_LEN  10
static T_matlab_msg           matlab_queue_buf[MATLAB_QUEUE_BUF_LEN];
static TX_QUEUE               matlab_queue;


  #define THREAD_MATLAB_STACK_SIZE 1024
uint8_t     thread_matlab_stack[THREAD_MATLAB_STACK_SIZE] BSP_PLACE_IN_SECTION_V2(".stack.matlab_thread")BSP_ALIGN_VARIABLE_V2(BSP_STACK_ALIGNMENT);


static uint32_t Thread_MATLAB_create(void);
static void     Thread_MATLAB(ULONG initial_input);
static uint32_t Thread_MATLAB_delete(void);

/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
uint32_t  MATLAB_server_create(NX_IP *ip_ptr)
{
  uint32_t status;

  if (tx_mutex_create(&matlab_mutex, "MATLAB", TX_INHERIT) != TX_SUCCESS) return RES_ERROR;


  if (matlab_server_created != 0) return RES_OK;

  matlab_tcp_setup.ip_ptr                 = ip_ptr;
  matlab_tcp_setup.server_port            = MATLAB_CONN_PORT;
  matlab_tcp_setup.stack_size             = 2048;
  matlab_tcp_setup.en_options_negotiation = 0;
  matlab_tcp_setup.activity_timeout_val   = 0xFFFFFFFF; // Не контролируем таймаут поступления данных в канал от клиента
  matlab_tcp_setup.packet_pool_ptr        = &net_packet_pool;
  matlab_tcp_setup.cbl_New_connection     = TCP_new_connection_callback;
  matlab_tcp_setup.cbl_Receive_data       = TCP_receive_data_callback;
  matlab_tcp_setup.cbl_Connection_end     = TCP_connection_end_callback;

  status = Net_tcp_server_create(&matlab_tcp_server_ptr,&matlab_tcp_setup, "MATLAB srv");
  if (status == NX_SUCCESS)
  {
    NETLOG("MATLAB server creating result: %04X", status);
    matlab_server_created = 1;
    status = Net_tcp_server_start(matlab_tcp_server_ptr);
    NETLOG("MATLAB server start result: %04X", status);
    return  RES_OK;
  }
  else
  {
    NETLOG("MATLAB server creating result: %04X", status);
    return RES_ERROR;
  }
}

/*-----------------------------------------------------------------------------------------------------


  \param buf
  \param len

  \return int
-----------------------------------------------------------------------------------------------------*/
int MATLAB_packet_send(const void *buf, unsigned int len)
{
  UINT                     res;
  NX_PACKET                *packet;

  res = nx_packet_allocate(&net_packet_pool,&packet, NX_TCP_PACKET, MS_TO_TICKS(10));
  if (res != NX_SUCCESS) return RES_ERROR;

  res = nx_packet_data_append(packet, (void *)buf, len,&net_packet_pool, MS_TO_TICKS(10));
  if (res != NX_SUCCESS)
  {
    nx_packet_release(packet);
    return RES_ERROR;
  }

  res = Net_tcp_server_packet_send(matlab_tcp_server_ptr,packet, MS_TO_TICKS(1000));
  if (res != NX_SUCCESS)
  {
    nx_packet_release(packet);
    return RES_ERROR;
  }

  return RES_OK;
}
/*-----------------------------------------------------------------------------------------------------


  \param telnet_server_ptr
-----------------------------------------------------------------------------------------------------*/
static void TCP_new_connection_callback(NET_TCP_SERVER *server_ptr)
{
  // Создаваться может только одно соедиенение, поэтому если соединение уже существует то отклонить новое
  if (matlab_connected)
  {
    nx_tcp_server_socket_unaccept(&(server_ptr->socket));
    NETLOG("MATLAB connection rejected");
  }
  else
  {
    if (Thread_MATLAB_create() != RES_OK)
    {
      nx_tcp_server_socket_unaccept(&(server_ptr->socket));
      NETLOG("MATLAB connection rejected");
      return;
    }
    NETLOG("MATLAB connected");
    matlab_connected = 1;
  }
}


/*-----------------------------------------------------------------------------------------------------
  Функция обработки принятого пакеты.
  Вызывается из контекста задачи примепа макетов сервера TCP


  \param telnet_server_ptr
  \param packet_ptr
-----------------------------------------------------------------------------------------------------*/
static void TCP_receive_data_callback(NET_TCP_SERVER *server_ptr, NX_PACKET *packet_ptr)
{
  uint16_t crc;
  uint16_t packet_crc;
  uint8_t *packet = packet_ptr->nx_packet_prepend_ptr;

  if (packet_ptr->nx_packet_length > (sizeof(matlab_cmd_id) + sizeof(crc)))
  {
    memcpy(&matlab_cmd_id, packet, sizeof(matlab_cmd_id));

    switch (matlab_cmd_id)
    {
    case MTLB_CUSTOM_CMD_1:
      if (packet_ptr->nx_packet_length == sizeof(T_matlab_cmd1))
      {
        memcpy(&packet_crc, packet + sizeof(T_matlab_cmd1) - 2, 2);
        crc = CRC16_matlab(packet, sizeof(T_matlab_cmd1) - 2);
        if (crc == packet_crc)
        {
          memcpy(&matlab_cmd1, packet, sizeof(T_matlab_cmd1));
        }
      }

      break;
    case MTLB_CUSTOM_CMD_2:
      if (packet_ptr->nx_packet_length == sizeof(T_matlab_cmd2))
      {
        memcpy(&packet_crc, packet + sizeof(T_matlab_cmd2) - 2, 2);
        crc = CRC16_matlab(packet, sizeof(T_matlab_cmd2) - 2);
        if (crc == packet_crc)
        {
          memcpy(&matlab_cmd2, packet, sizeof(T_matlab_cmd2));
        }
      }
      break;
    }

  }
  nx_packet_release(packet_ptr);
}

/*-----------------------------------------------------------------------------------------------------


  \param telnet_server_ptr
-----------------------------------------------------------------------------------------------------*/
static void TCP_connection_end_callback(NET_TCP_SERVER *server_ptr)
{
  if (tx_mutex_get(&matlab_mutex, MS_TO_TICKS(1000)) != TX_SUCCESS)
  {
    mc_stat.mutex_fails_cnt++;
    return;
  }

  if (matlab_connected)
  {
    matlab_connected = 0;
    Thread_MATLAB_delete();
    NETLOG("MATLAB disconnected.");
  }
  tx_mutex_put(&matlab_mutex);
}




/*-----------------------------------------------------------------------------------------------------
  Отсылка данных в MATLAB

  \param buf_ptr
  \param bytes_to_copy
-----------------------------------------------------------------------------------------------------*/
void MATLAB_post_data_to_tx_queue(uint8_t *buf_ptr, uint16_t sz)
{

  UINT               res;
  NX_PACKET         *packet;
  T_matlab_msg       msg;

  if (matlab_connected == 0) return;

  if (tx_mutex_get(&matlab_mutex, MS_TO_TICKS(100)) != TX_SUCCESS)
  {
    mc_stat.mutex_fails_cnt++;
    return;
  }

  mc_stat.packet_num++;

  if ((sz + 4) > matlab_tcp_setup.ip_ptr->nx_ip_interface->nx_interface_ip_mtu_size)
  {
    mc_stat.oversize_drops_cnt++;
    mc_stat.lost_bytes_cnt += sz;
    tx_mutex_put(&matlab_mutex);
    return;
  }

  // Выделяем пакет без ожидания. Функция должна быть максимально быстрой
  res = nx_packet_allocate(&net_packet_pool,&packet, NX_TCP_PACKET, NX_NO_WAIT);
  if (res != NX_SUCCESS)
  {
    mc_stat.alloc_fail_cnt++;
    mc_stat.lost_bytes_cnt += sz;
    tx_mutex_put(&matlab_mutex);
    return;
  }

  memcpy(packet->nx_packet_append_ptr, buf_ptr, sz);
  packet->nx_packet_length =  packet->nx_packet_length + sz;
  packet->nx_packet_append_ptr =  packet->nx_packet_append_ptr + sz;

  msg.pack_num = mc_stat.packet_num;
  msg.packet = packet;
  res = tx_queue_send(&matlab_queue,&msg, NX_NO_WAIT);
  if (res != TX_SUCCESS)
  {
    mc_stat.queue_fail_cnt++;
    mc_stat.lost_bytes_cnt += sz;
  }
  tx_mutex_put(&matlab_mutex);

}

/*-----------------------------------------------------------------------------------------------------

  \param void
-----------------------------------------------------------------------------------------------------*/
static uint32_t _Do_custom_cmd_1(void)
{

  // MATLAB_buf_send(str,sz); // Передача ответа на команду

  return RES_OK;
}


/*-----------------------------------------------------------------------------------------------------


  \param void

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
static uint32_t _Do_custom_cmd_2(void)
{
  // MATLAB_buf_send(str,sz); // Передача ответа на команду

  return RES_OK;
}

/*-----------------------------------------------------------------------------------------------------


  \param initial_input
-----------------------------------------------------------------------------------------------------*/
static void Thread_MATLAB(ULONG initial_input)
{
  uint32_t      res;
  T_matlab_msg  msg;

  NETLOG("MATLAB thread entered");

  do
  {
    res = tx_queue_receive(&matlab_queue,&msg, MS_TO_TICKS(10));
    if (res == TX_SUCCESS)
    {
      res = Net_tcp_server_packet_send(matlab_tcp_server_ptr,msg.packet, MS_TO_TICKS(1000));
      if (res != NX_SUCCESS)
      {
        nx_packet_release(msg.packet);
        mc_stat.sending_fail_cnt++;
      }
    }

    if (matlab_cmd_id != 0)
    {
      switch (matlab_cmd_id)
      {
      case MTLB_CUSTOM_CMD_1:
        _Do_custom_cmd_1();
        break;
      case MTLB_CUSTOM_CMD_2:
        _Do_custom_cmd_2();
        break;
      }
      matlab_cmd_id = 0;
    }
  } while (1);


}

/*-----------------------------------------------------------------------------------------------------
  Поток создаваемый и уничтожаемый динамически при появлении соединения
  Создается в единственном экземпляре

  \param void
-----------------------------------------------------------------------------------------------------*/
static uint32_t Thread_MATLAB_create(void)
{
  uint32_t res;

  if (matlab_thread.tx_thread_id == TX_THREAD_ID) return RES_ERROR;

  res = tx_queue_create(&matlab_queue, (CHAR *) "MATLAB_tx", sizeof(T_matlab_msg) / sizeof(uint32_t), matlab_queue_buf, sizeof(T_matlab_msg) * MATLAB_QUEUE_BUF_LEN);
  if (res != TX_SUCCESS)
  {
    NETLOG("MATLAB connection task creating error %04X.", res);
    return RES_ERROR;
  }
  mc_stat.packet_num         = 0;
  mc_stat.oversize_drops_cnt = 0;
  mc_stat.alloc_fail_cnt     = 0;
  mc_stat.queue_fail_cnt     = 0;
  mc_stat.lost_bytes_cnt     = 0;
  mc_stat.sending_fail_cnt   = 0;


  res = tx_thread_create(&matlab_thread,
                         "MATLAB",
                         Thread_MATLAB,
                         0,
                         (void *)thread_matlab_stack, // stack_start
                         THREAD_MATLAB_STACK_SIZE,    // stack_size
                         MATLAB_TASK_PRIO,      // priority. Numerical priority of thread. Legal values range from 0 through (TX_MAX_PRIORITES-1), where a value of 0 represents the highest priority.
                         MATLAB_TASK_PRIO,      // preempt_threshold. Highest priority level (0 through (TX_MAX_PRIORITIES-1)) of disabled preemption. Only priorities higher than this level are allowed to preempt this thread. This value must be less than or equal to the specified priority. A value equal to the thread priority disables preemption-threshold.
                         TX_NO_TIME_SLICE,
                         TX_DONT_START);

  if (res == TX_SUCCESS)
  {
    NETLOG("MATLAB connection  task created.");
  }
  else
  {
    tx_queue_delete(&matlab_queue);
    NETLOG("MATLAB connection task creating error %04X.", res);
    return RES_ERROR;
  }
  tx_thread_resume(&matlab_thread);
  return RES_OK;
}


/*-----------------------------------------------------------------------------------------------------


  \param void

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
static uint32_t Thread_MATLAB_delete(void)
{
  uint32_t status;

  T_matlab_msg  msg;

  if (matlab_thread.tx_thread_id == TX_THREAD_ID)
  {
    // Удаляем все пакеты из очереди и удаляем саму очередь
    while (tx_queue_receive(&matlab_queue,&msg, TX_NO_WAIT) == TX_SUCCESS)
    {
      nx_packet_release(msg.packet);
    }

    tx_thread_suspend(&matlab_thread);

    status =  tx_thread_terminate(&matlab_thread);
    if (status == TX_SUCCESS)
    {
      status = tx_thread_delete(&matlab_thread);
    }
    tx_queue_flush(&matlab_queue);
    tx_queue_delete(&matlab_queue);

    NETLOG("MATLAB thread terminating status=%d", status);
  }
  else
  {
    status =  TX_SUCCESS;
    NETLOG("MATLAB thread matlab_thread was not found.");
  }

  return status;
}

/*-----------------------------------------------------------------------------------------------------


  \param void

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t MATLAB_server_delete(void)
{
  if (matlab_server_created == 0) return RES_OK;

  Net_tcp_server_delete(matlab_tcp_server_ptr);
  tx_mutex_delete(&matlab_mutex);

  return RES_OK;
}

#endif
