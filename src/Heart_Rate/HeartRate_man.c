// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 2024-07-18
// 10:28:50
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include   "Test.h"

typedef enum
{
  INAVALID, START, DISCONNECTED, CONNECTING, ONLINE, WAIT_OPERATING, OPERATING, DISCONNECTING

} T_hr_state;

#define CASE_RETURN_STR(enum_val)          case enum_val: return #enum_val;



#define HR_RESP_BUF_SZ 16
typedef struct
{
    T_hr_state                  state;
    T_sys_timestump             timestump;
    wiced_bt_device_address_t   addr;
    uint8_t                     addr_valid;
    uint16_t                    conn_id;
    uint8_t                     hr_resp_buf[HR_RESP_BUF_SZ];
    T_HR_data                   hr_data;

} T_hr_cbl;

static T_hr_cbl        hr_cbl;
static TX_MUTEX        hr_mutex;
static void _cb_gatt_connect(wiced_bt_gatt_connection_status_t *p_conn_status);
static void _cb_gatt_disconnect(wiced_bt_gatt_connection_status_t *p_conn_status);
static void _cb_gatt_op_complete(wiced_bt_gatt_event_data_t *p_event_data);
static void _cb_gatt_discovery_result(wiced_bt_gatt_event_data_t *p_event_data);
static void _cb_gatt_discovery_cplt(wiced_bt_gatt_event_data_t *p_event_data);

static T_gatt_callbacks gatt_cb =
{
  .connect          = _cb_gatt_connect,
  .disconnect       = _cb_gatt_disconnect,
  .op_complete      = _cb_gatt_op_complete,
  .discovery_result = _cb_gatt_discovery_result,
  .discovery_cplt   = _cb_gatt_discovery_cplt
};


/*-----------------------------------------------------------------------------------------------------


  \param mode

  \return const char*
-----------------------------------------------------------------------------------------------------*/
const char* _get_HRman_state_name(T_hr_state st)
{
  switch (st)
  {
    CASE_RETURN_STR(INAVALID      );
    CASE_RETURN_STR(START         );
    CASE_RETURN_STR(DISCONNECTED  );
    CASE_RETURN_STR(CONNECTING    );
    CASE_RETURN_STR(ONLINE        );
    CASE_RETURN_STR(WAIT_OPERATING);
    CASE_RETURN_STR(OPERATING     );
    CASE_RETURN_STR(DISCONNECTING );
  }
  return "UNKNOWN";
}

/*-----------------------------------------------------------------------------------------------------


  \param state
-----------------------------------------------------------------------------------------------------*/
static void _set_state(T_hr_state state)
{
  hr_cbl.state = state;
  Get_hw_timestump(&hr_cbl.timestump);
  BLELOG("BLE: Heart Rate State Mashine state: %s (%d)", _get_HRman_state_name(state), state);
}

/*-----------------------------------------------------------------------------------------------------


  \param void

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
static uint32_t _state_elapsed_sec(void)
{
  return Time_elapsed_sec(&hr_cbl.timestump);
}

/*-----------------------------------------------------------------------------------------------------


  \param void

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
static uint32_t _Get_sensor_addr(void)
{
  int32_t res;
  uint32_t num1, num2, num3, num4, num5, num6;
  res = sscanf((char *)wvar.ble_heart_rate_sensor_addr, "%x %x %x %x %x %x",&num1,&num2,&num3,&num4,&num5,&num6);
  if (res == 6)
  {
    hr_cbl.addr[0] = (uint8_t)num1;
    hr_cbl.addr[1] = (uint8_t)num2;
    hr_cbl.addr[2] = (uint8_t)num3;
    hr_cbl.addr[3] = (uint8_t)num4;
    hr_cbl.addr[4] = (uint8_t)num5;
    hr_cbl.addr[5] = (uint8_t)num6;

    if (Is_All_Zeros(hr_cbl.addr, 6)==0)
    {
      hr_cbl.addr_valid = 0;
      return RES_ERROR;
    }
    hr_cbl.addr_valid = 1;
    return RES_OK;
  }
  else
  {
    hr_cbl.addr_valid = 0;
    return RES_ERROR;
  }
}

/*-----------------------------------------------------------------------------------------------------


  \param p_conn_status
-----------------------------------------------------------------------------------------------------*/
static void _cb_gatt_connect(wiced_bt_gatt_connection_status_t *p_conn_status)
{
  if (memcmp(p_conn_status->bd_addr, hr_cbl.addr, 6) == 0)
  {
    if (tx_mutex_get(&hr_mutex, MS_TO_TICKS(100)) == TX_SUCCESS)
    {
      hr_cbl.conn_id = p_conn_status->conn_id;
      _set_state(ONLINE);
      tx_mutex_put(&hr_mutex);
    }
  }
}

/*-----------------------------------------------------------------------------------------------------


  \param p_conn_status
-----------------------------------------------------------------------------------------------------*/
static void _cb_gatt_disconnect(wiced_bt_gatt_connection_status_t *p_conn_status)
{
  if (memcmp(p_conn_status->bd_addr, hr_cbl.addr, 6) == 0)
  {
    if (tx_mutex_get(&hr_mutex, MS_TO_TICKS(100)) == TX_SUCCESS)
    {
      hr_cbl.conn_id = 0;
      _set_state(DISCONNECTED);
      tx_mutex_put(&hr_mutex);
    }
  }
}

/*-----------------------------------------------------------------------------------------------------


  \param p_event_data
-----------------------------------------------------------------------------------------------------*/
static void _cb_gatt_op_complete(wiced_bt_gatt_event_data_t *p_event_data)
{
  if ((hr_cbl.state == ONLINE) || (hr_cbl.state == WAIT_OPERATING) || (hr_cbl.state == OPERATING))
  {
    wiced_bt_gatt_operation_complete_t *data_ptr = &(p_event_data->operation_complete);

    if (hr_cbl.conn_id == data_ptr->conn_id)
    {
      if (tx_mutex_get(&hr_mutex, MS_TO_TICKS(100)) == TX_SUCCESS)
      {
        if ((data_ptr->op == GATTC_OPTYPE_NOTIFICATION)
            && (data_ptr->response_data.handle == (wvar.ble_heart_rate_value_handle+1))) // Значение характеристики имеет хэндл на единицу больше чем хэндл характеристики
        {
          HR_Decode(data_ptr->response_data.att_value.p_data,&hr_cbl.hr_data);
        }
        else if (data_ptr->op == GATTC_OPTYPE_WRITE_WITH_RSP)
        {
          if (data_ptr->response_data.handle == wvar.ble_heart_rate_cccd_handle)
          {
            _set_state(OPERATING);
          }
        }
        tx_mutex_put(&hr_mutex);
      }
    }
  }
}

/*-----------------------------------------------------------------------------------------------------


  \param p_event_data
-----------------------------------------------------------------------------------------------------*/
static void _cb_gatt_discovery_result(wiced_bt_gatt_event_data_t *p_event_data)
{
  if (hr_cbl.state == ONLINE)
  {
    if (tx_mutex_get(&hr_mutex, MS_TO_TICKS(100)) == TX_SUCCESS)
    {


      tx_mutex_put(&hr_mutex);
    }
  }
}

/*-----------------------------------------------------------------------------------------------------


  \param p_event_data
-----------------------------------------------------------------------------------------------------*/
static void _cb_gatt_discovery_cplt(wiced_bt_gatt_event_data_t *p_event_data)
{
  if (hr_cbl.state == ONLINE)
  {
    if (tx_mutex_get(&hr_mutex, MS_TO_TICKS(100)) == TX_SUCCESS)
    {


      tx_mutex_put(&hr_mutex);
    }
  }
}

/*-----------------------------------------------------------------------------------------------------


  \param attr_buf
-----------------------------------------------------------------------------------------------------*/
void HR_Decode(uint8_t *attr_buf, T_HR_data *hr_ptr)
{
  // Heartbeat decoder
  uint8_t  *ptr;
  uint8_t   flag = attr_buf[0];

  hr_ptr->heart_rate        = 0;
  hr_ptr->energy_expended   = 0;
  hr_ptr->rr_interval       = 0;
  hr_ptr->heart_rate_format = 0; // 0: Heart Rate Value Format is UINT8, 1: Heart Rate Value Format is UINT16
  hr_ptr->sensor_contact_st = 0; // 0: Sensor Contact feature is not supported in the current connection, 1: Sensor Contact feature is supported in the current connection
  hr_ptr->sensor_contact_dt = 0; // 0: Sensor Contact is not detected, 1: Sensor Contact is detected
  hr_ptr->energy_expend_sts = 0; // 0: Energy Expended field is not present, 1: Energy Expended field is present
  hr_ptr->rr_interval_prest = 0; // 0: RR-Interval values are not present, 1: One or more RR-Interval values are present
  ptr = &attr_buf[1];
  if (flag & BIT(0)) hr_ptr->heart_rate_format = 1;
  if (flag & BIT(1)) hr_ptr->sensor_contact_st = 1;
  if (flag & BIT(2)) hr_ptr->sensor_contact_dt = 1;
  if (flag & BIT(3)) hr_ptr->energy_expend_sts = 1;
  if (flag & BIT(4)) hr_ptr->rr_interval_prest = 1;
  if (hr_ptr->heart_rate_format)
  {
    memcpy(&(hr_ptr->heart_rate), ptr, 2);
    ptr += 2;
  }
  else
  {
    memcpy(&(hr_ptr->heart_rate), ptr, 1);
    ptr += 1;
  }
  if (hr_ptr->energy_expend_sts)
  {
    memcpy(&(hr_ptr->energy_expended), ptr, 2);
    ptr += 2;
  }
  if (hr_ptr->rr_interval_prest)
  {
    memcpy(&(hr_ptr->rr_interval), ptr, 2);
    ptr += 2;
  }

  hr_ptr->has_data = 1;
  Get_hw_timestump(&(hr_ptr->timestump));
  BLELOG("BLE: Heart Rate = %d", hr_ptr->heart_rate);
}

/*-----------------------------------------------------------------------------------------------------


  \param hrate

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
void HR_Get_heart_rate(int32_t *hrate_ptr)
{
  if (hrate_ptr == 0) return;
  if (hr_cbl.hr_data.has_data ==0)
  {
    *hrate_ptr = -1;
    return;
  }
  // Если пульс не получали дольше 10 сек то возвращать ошибку
  if (Time_elapsed_sec(&hr_cbl.hr_data.timestump) > 10)
  {
    *hrate_ptr = -1;
    return;
  }

  *hrate_ptr =  (int32_t)hr_cbl.hr_data.heart_rate;
  return;
}

/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
void  App_ble_init(void)
{
  HRman_init();
}


/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
void HRman_init(void)
{
  tx_mutex_create(&hr_mutex, "HeartRate", TX_INHERIT);
  Get_hw_timestump(&hr_cbl.hr_data.timestump);
  _set_state(INAVALID);
  if (_Get_sensor_addr() == RES_OK)
  {
    Set_gatt_callbacks(&gatt_cb);
    _set_state(START);
  }
}

/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
void HRman_state_machine(void)
{

  if (BLE_is_initialised())
  {
    if (tx_mutex_get(&hr_mutex, MS_TO_TICKS(100)) == TX_SUCCESS)
    {
      switch (hr_cbl.state)
      {
      case INAVALID:

        break;
      case START:
        wiced_bt_gatt_le_connect(hr_cbl.addr, wvar.ble_heart_rate_sensor_addr_type , BLE_CONN_MODE_HIGH_DUTY, TRUE);
        _set_state(CONNECTING);
        break;
      case DISCONNECTED:
        if (_state_elapsed_sec() > 10) // В состоянии DISCONNECTED находимся не мнеее 10 сек, и снова пытаемся соединиться
        {
          wiced_bt_gatt_le_connect(hr_cbl.addr, wvar.ble_heart_rate_sensor_addr_type , BLE_CONN_MODE_HIGH_DUTY, TRUE);
          _set_state(CONNECTING);
          break;
        }
      case CONNECTING:
        if (_state_elapsed_sec() > 30)  // Подключения ждем не дольше 30 сек, иначе разраваем процесс соединения
        {
          wiced_bt_gatt_cancel_connect(hr_cbl.addr, TRUE);
          _set_state(DISCONNECTED);
        }
        break;
      case ONLINE:
        {
          // При получении соединения отправляем команду на включения нотификаций со значением пульса
          uint8_t notification_enable[2] = {0x01, 0x00}; // Значение для включения уведомлений

          wiced_bt_gatt_write_hdr_t    write_value;
          write_value.handle   = wvar.ble_heart_rate_cccd_handle;
          write_value.offset   = 0;
          write_value.len      = sizeof(notification_enable);
          write_value.auth_req = GATT_AUTH_REQ_NONE;
          wiced_bt_gatt_client_send_write(hr_cbl.conn_id, GATT_REQ_WRITE,&write_value,  notification_enable, NULL);
          _set_state(WAIT_OPERATING);
        }
        break;
      case WAIT_OPERATING:
        if (_state_elapsed_sec() > 30)  // Подверждение записи в CCCD ждем не дольше 30 сек, требуем разъединения
        {
          wiced_bt_gatt_disconnect(hr_cbl.conn_id);
          _set_state(DISCONNECTING);
        }
        break;
        break;
      case OPERATING:

        break;
      case DISCONNECTING:
        if (_state_elapsed_sec() > 30)  // Разъединения ждем не дольше 30 сек, иначе разрываем процесс и считаем что разъединение произошло
        {
          wiced_bt_gatt_cancel_connect(hr_cbl.addr, TRUE);
          _set_state(DISCONNECTED);
        }
        break;

      }
      tx_mutex_put(&hr_mutex);
    }

  }

}
