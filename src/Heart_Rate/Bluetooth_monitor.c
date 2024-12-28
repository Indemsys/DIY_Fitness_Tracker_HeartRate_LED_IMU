// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 2024-07-09
// 10:39:33
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include   "Test.h"

#define  BT_MAX_REMOTE_NAME_LEN    64
wiced_bt_dev_rssi_result_t         rssi_info;
char                               remote_name[BT_MAX_REMOTE_NAME_LEN+1];

// Переменные процесса обсервера

#define START_ROW                    9
#define BLE_DEV_STR_TYPE_LEN         32
#define BLE_DEV_DIG_TYPE_LEN         10
#define MAX_OSERVER_RECS_NUM         30
#define MAX_OSERVER_UUID_NUM         4

#define   FLG_BT_OBSRV_RES_NEW          BIT(0)
#define   FLG_BT_OBSRV_COMPL            BIT(1)
#define   FLG_BT_RSSI_COMPL             BIT(2)
#define   FLG_BT_NAME_COMPL             BIT(3)
#define   FLG_BT_BLE_DEV_CONNECTED      BIT(4)
#define   FLG_BT_BLE_DEV_DISCONNECTED   BIT(5)
#define   FLG_BT_GATT_OP_COMPLETED      BIT(6)
#define   FLG_BT_GATT_OP_NOTIFICATION   BIT(7)

static wiced_bt_ble_scan_results_t   last_scan_result;
static uint8_t                       last_scan_name[BLE_DEV_STR_TYPE_LEN+1];
static uint8_t                       last_scan_UUIDs_num;
static uint16_t                      last_scan_UUIDs[MAX_OSERVER_UUID_NUM];
static TX_EVENT_FLAGS_GROUP          bt_mon_flags;
static uint8_t                       observer_busy;


typedef struct
{
    uint8_t                         active;
    uint32_t                        cnt;
    wiced_bt_device_address_t       addr;
    uint8_t                         addr_type;
    uint8_t                         flag;
    int8_t                          rssi;
    uint8_t                         tx_power;
    uint8_t                         UUIDs_num;
    uint16_t                        UUIDs[MAX_OSERVER_UUID_NUM];
    uint8_t                         device_name[BLE_DEV_STR_TYPE_LEN+1];
    uint8_t                         model_num[BLE_DEV_STR_TYPE_LEN+1];
    uint8_t                         serial_num[BLE_DEV_STR_TYPE_LEN+1];
    uint8_t                         dig_attr[BLE_DEV_DIG_TYPE_LEN];

} T_oserver_rec;

T_oserver_rec           orecs[MAX_OSERVER_RECS_NUM];
static uint32_t         curr_dev_indx;
static int32_t          curr_conn_id;
static uint8_t         *curr_buff;
static uint32_t         curr_buff_len;
static uint8_t          curr_op_status;

// Переменные для поиска хэндла Client Characteristic Configuration Descriptor (CCCD) для включения нитификаций
static uint16_t         service_start_handle;
static uint16_t         service_end_handle;
static uint16_t         current_characteristic_handle;
static uint16_t         current_service_uuid;
static uint16_t         current_characteristic_uuid;
static uint16_t         current_characteristic_cccd_handle; // Искомый хэндл, по кторому включаются нотификации


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


  \param v

  \return const char*
-----------------------------------------------------------------------------------------------------*/
const char* To_Str_Yes_No(uint32_t v)
{
  if (v != 0)
  {
    return "Yes";
  }
  else
  {
    return "No ";
  }
}

/*-----------------------------------------------------------------------------------------------------


  \param v

  \return const char*
-----------------------------------------------------------------------------------------------------*/
const char* To_Str_Addr_type(uint8_t v)
{
  switch (v)
  {
  case BLE_ADDR_PUBLIC   : //  0x00
    return "PUBLIC  ";
  case BLE_ADDR_RANDOM   : //  0x01
    return "RANDOM   ";
  case BLE_ADDR_PUBLIC_ID: //  0x02
    return "PUBLIC_ID";
  case BLE_ADDR_RANDOM_ID: //  0x03
    return "RANDOM_ID";
  }
  return "UNKNOWN  ";
}

/*-----------------------------------------------------------------------------------------------------

  \param v

  \return const char*
-----------------------------------------------------------------------------------------------------*/
const char* To_Str_Link_Role(uint8_t v)
{
  switch (v)
  {
  case HCI_ROLE_CENTRAL   :
    return "CENTRAL   ";
  case HCI_ROLE_PERIPHERAL   :
    return "PERIPHERAL";
  case HCI_ROLE_UNKNOWN:
    return "UNKNOWN   ";
  }
  return "UNKNOWN  ";
}

/*-----------------------------------------------------------------------------------------------------
  BT_TRANSPORT_BR_EDR
  BT_TRANSPORT_LE
  \param v

  \return const char*
-----------------------------------------------------------------------------------------------------*/
const char* To_Str_Bluetooth_transport(uint8_t v)
{
  switch (v)
  {
  case BT_TRANSPORT_BR_EDR   :
    return "BR_EDR";
  case BT_TRANSPORT_LE   :
    return "LE    ";
  }
  return "UNKNOWN  ";
}

/*-----------------------------------------------------------------------------------------------------


  \param v

  \return const char*
-----------------------------------------------------------------------------------------------------*/
const char* To_Str_BLE_sec_level(uint8_t v)
{
  switch (v)
  {
  case SMP_SEC_NONE   :
    return "NONE          ";
  case SMP_SEC_UNAUTHENTICATE   :
    return "UNAUTHENTICATE";
  case SMP_SEC_AUTHENTICATED   :
    return "AUTHENTICATED ";
  }
  return "UNKNOWN  ";
}

/*-----------------------------------------------------------------------------------------------------


  \param v

  \return const char*
-----------------------------------------------------------------------------------------------------*/
const char* To_Str_SMP_status(uint8_t v)
{
  switch (v)
  {
  case SMP_SUCCESS                 :
    return "SUCCESS                ";
  case SMP_PASSKEY_ENTRY_FAIL      :
    return "PASSKEY_ENTRY_FAIL     ";
  case SMP_OOB_FAIL                :
    return "OOB_FAIL               ";
  case SMP_PAIR_AUTH_FAIL          :
    return "PAIR_AUTH_FAIL         ";
  case SMP_CONFIRM_VALUE_ERR       :
    return "CONFIRM_VALUE_ERR      ";
  case SMP_PAIR_NOT_SUPPORT        :
    return "PAIR_NOT_SUPPORT       ";
  case SMP_ENC_KEY_SIZE            :
    return "ENC_KEY_SIZE           ";
  case SMP_INVALID_CMD             :
    return "INVALID_CMD            ";
  case SMP_PAIR_FAIL_UNKNOWN       :
    return "PAIR_FAIL_UNKNOWN      ";
  case SMP_REPEATED_ATTEMPTS       :
    return "REPEATED_ATTEMPTS      ";
  case SMP_INVALID_PARAMETERS      :
    return "INVALID_PARAMETERS     ";
  case SMP_DHKEY_CHK_FAIL          :
    return "DHKEY_CHK_FAIL         ";
  case SMP_NUMERIC_COMPAR_FAIL     :
    return "NUMERIC_COMPAR_FAIL    ";
  case SMP_BR_PAIRING_IN_PROGR     :
    return "BR_PAIRING_IN_PROGR    ";
  case SMP_XTRANS_DERIVE_NOT_ALLOW :
    return "XTRANS_DERIVE_NOT_ALLOW";
  case SMP_PAIR_INTERNAL_ERR       :
    return "PAIR_INTERNAL_ERR      ";
  case SMP_UNKNOWN_IO_CAP          :
    return "UNKNOWN_IO_CAP         ";
  case SMP_INIT_FAIL               :
    return "INIT_FAIL              ";
  case SMP_CONFIRM_FAIL            :
    return "CONFIRM_FAIL           ";
  case SMP_BUSY                    :
    return "BUSY                   ";
  case SMP_ENC_FAIL                :
    return "ENC_FAIL               ";
  case SMP_STARTED                 :
    return "STARTED                ";
  case SMP_RSP_TIMEOUT             :
    return "RSP_TIMEOUT            ";
  case SMP_FAIL                    :
    return "FAIL                   ";
  case SMP_CONN_TOUT               :
    return "CONN_TOUT              ";
  }
  return "UNKNOWN  ";
}

/*-----------------------------------------------------------------------------------------------------


  \param p_data
-----------------------------------------------------------------------------------------------------*/
static void  _Accept_RSSI_callback(void *p_data)
{
  memcpy(&rssi_info,  (wiced_bt_dev_rssi_result_t *)p_data, sizeof(wiced_bt_dev_rssi_result_t));
  tx_event_flags_set(&bt_mon_flags, FLG_BT_RSSI_COMPL, TX_OR);
}

/*-----------------------------------------------------------------------------------------------------


  \param p_remote_name_result
-----------------------------------------------------------------------------------------------------*/
static void _Accept_remote_name(wiced_bt_dev_remote_name_result_t *p_remote_name_result)
{
  uint32_t n;
  if (p_remote_name_result->status == 0)
  {
    n = BT_MAX_REMOTE_NAME_LEN;
    if (p_remote_name_result->length < BT_MAX_REMOTE_NAME_LEN) n = p_remote_name_result->length;
    memcpy(remote_name, p_remote_name_result->remote_bd_name, n);
    remote_name[n] = 0;
  }
  else
  {
    remote_name[0] = 0;
  }
  tx_event_flags_set(&bt_mon_flags, FLG_BT_NAME_COMPL, TX_OR);

}


/*-----------------------------------------------------------------------------------------------------
  Функция вызывается в контексе задачи cybt_hci_rx_task

  \param p_scan_result
  \param p_adv_data
-----------------------------------------------------------------------------------------------------*/
static void ble_scan_result_callback(wiced_bt_ble_scan_results_t *p_scan_result, uint8_t *p_adv_data)
{
  uint8_t *ptr;
  uint8_t length;
  if (p_scan_result)
  {
    if (observer_busy == 0)
    {
      last_scan_name[0] = 0;
      memcpy(&last_scan_result, p_scan_result, sizeof(wiced_bt_ble_scan_results_t));
      ptr = wiced_bt_ble_check_advertising_data(p_adv_data, BTM_BLE_ADVERT_TYPE_NAME_COMPLETE,&length);
      if (ptr != 0)
      {
        if (length > BLE_DEV_STR_TYPE_LEN) length = BLE_DEV_STR_TYPE_LEN;
        memcpy(last_scan_name, ptr, length);
        last_scan_name[length]  = 0;
      }
      else
      {
        ptr = wiced_bt_ble_check_advertising_data(p_adv_data, BTM_BLE_ADVERT_TYPE_NAME_SHORT,&length);
        if (ptr != 0)
        {
          if (length > BLE_DEV_STR_TYPE_LEN) length = BLE_DEV_STR_TYPE_LEN;
          memcpy(last_scan_name, ptr, length);
          last_scan_name[length]  = 0;
        }
      }
      ptr = wiced_bt_ble_check_advertising_data(p_adv_data, BTM_BLE_ADVERT_TYPE_16SRV_PARTIAL,&length);
      if (ptr != 0)
      {
        last_scan_UUIDs_num = length / 2;
        if (last_scan_UUIDs_num > MAX_OSERVER_UUID_NUM) last_scan_UUIDs_num  = MAX_OSERVER_UUID_NUM;
        for (uint32_t i = 0; i < last_scan_UUIDs_num; i++)
        {
          memcpy(&last_scan_UUIDs[i],ptr + 2 * i,2);
        }
      }
      else
      {
        ptr = wiced_bt_ble_check_advertising_data(p_adv_data, BTM_BLE_ADVERT_TYPE_16SRV_COMPLETE,&length);
        if (ptr != 0)
        {
          last_scan_UUIDs_num = length / 2;
          if (last_scan_UUIDs_num > MAX_OSERVER_UUID_NUM) last_scan_UUIDs_num  = MAX_OSERVER_UUID_NUM;
          for (uint32_t i = 0; i < last_scan_UUIDs_num; i++)
          {
            memcpy(&last_scan_UUIDs[i],ptr + 2 * i,2);
          }
        }
        else
        {
          last_scan_UUIDs_num = 0;
        }
      }


      tx_event_flags_set(&bt_mon_flags, FLG_BT_OBSRV_RES_NEW, TX_OR);
    }
  }
  else
  {
    tx_event_flags_set(&bt_mon_flags, FLG_BT_OBSRV_COMPL, TX_OR);
  }
}


/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
static uint32_t _Filter_dev_list_by_UUID(uint16_t UUID)
{
  uint8_t n = 0;

  // Ищем среди активных записей те кторые имеют UUID = UUID_SERVICE_HEART_RATE
  for (uint32_t i = 0; i < MAX_OSERVER_RECS_NUM; i++)
  {
    if (orecs[i].active)
    {
      if (orecs[i].UUIDs_num > 0)
      {
        for (uint32_t k = 0; k < orecs[i].UUIDs_num; k++)
        {
          if (orecs[i].UUIDs[k] == UUID)
          {
            // Найден нужный UUID
            if (n != i)
            {
              // Копируем в начало списка обнаруженную запись
              memcpy(&orecs[n],&orecs[i], sizeof(orecs[0]));
            }
            n++;
            break;
          }
        }
      }
    }
  }
  return n;
}

/*-----------------------------------------------------------------------------------------------------


-----------------------------------------------------------------------------------------------------*/
static void _cb_gatt_connect(wiced_bt_gatt_connection_status_t *p_conn_status)
{
  if (memcmp(orecs[curr_dev_indx].addr, p_conn_status->bd_addr, sizeof(wiced_bt_device_address_t)) == 0)
  {
    tx_event_flags_set(&bt_mon_flags, FLG_BT_BLE_DEV_CONNECTED, TX_OR);
  }
}

/*-----------------------------------------------------------------------------------------------------


-----------------------------------------------------------------------------------------------------*/
static void _cb_gatt_disconnect(wiced_bt_gatt_connection_status_t *p_conn_status)
{
  if (memcmp(orecs[curr_dev_indx].addr, p_conn_status->bd_addr, sizeof(wiced_bt_device_address_t)) == 0)
  {
    tx_event_flags_set(&bt_mon_flags, FLG_BT_BLE_DEV_DISCONNECTED, TX_OR);
  }
}

/*-----------------------------------------------------------------------------------------------------


-----------------------------------------------------------------------------------------------------*/
static void _cb_gatt_op_complete(wiced_bt_gatt_event_data_t *p_event_data)
{
  wiced_bt_gatt_operation_complete_t *data_ptr =&(p_event_data->operation_complete);
  uint32_t len;
  if (curr_conn_id == data_ptr->conn_id)
  {
    if ((data_ptr->op == GATTC_OPTYPE_READ_BY_TYPE) || (data_ptr->op == GATTC_OPTYPE_READ_HANDLE))
    {
      curr_op_status = data_ptr->status;
      if (curr_op_status == WICED_BT_GATT_SUCCESS)
      {
        len = data_ptr->response_data.att_value.len;
        if (len > curr_buff_len) len = curr_buff_len;
        memcpy(curr_buff, data_ptr->response_data.att_value.p_data,len);
      }
      tx_event_flags_set(&bt_mon_flags, FLG_BT_GATT_OP_COMPLETED, TX_OR);

    }
    else if (data_ptr->op == GATTC_OPTYPE_NOTIFICATION)
    {
      len = data_ptr->response_data.att_value.len;
      if (len > curr_buff_len) len = curr_buff_len;
      memcpy(curr_buff, data_ptr->response_data.att_value.p_data,len);
      tx_event_flags_set(&bt_mon_flags, FLG_BT_GATT_OP_NOTIFICATION, TX_OR);
    }
  }
}


/*-----------------------------------------------------------------------------------------------------
  Запускаем процедуру считывания сервисаов в поисках хэндла CCCD заданной характеристики в заданном сервисе

  \param conn_id
-----------------------------------------------------------------------------------------------------*/
static void _Discover_services(uint16_t conn_id, uint16_t service_uuid, uint16_t characteristic_uuid)
{
  volatile wiced_bt_gatt_status_t res;
  current_service_uuid               = service_uuid;
  current_characteristic_uuid        = characteristic_uuid;
  service_start_handle               = 0;
  service_end_handle                 = 0;
  current_characteristic_handle      = 0;
  current_characteristic_cccd_handle = 0;

  wiced_bt_gatt_discovery_param_t discovery_param;
  discovery_param.s_handle = 0x0001;
  discovery_param.e_handle = 0xFFFF;
  res = wiced_bt_gatt_client_send_discover(conn_id, GATT_DISCOVER_SERVICES_ALL,&discovery_param);
  BLELOG("BLE: _Discover_services resul= %04X", res);
}

/*-----------------------------------------------------------------------------------------------------
   Поиск заданной по UUID характеристики в сервисе

  \param conn_id
  \param start_handle
  \param end_handle
-----------------------------------------------------------------------------------------------------*/
static void _Discover_characterictic(uint16_t conn_id, uint16_t start_handle, uint16_t end_handle)
{
  volatile wiced_bt_gatt_status_t res;
  wiced_bt_gatt_discovery_param_t discovery_param;

  discovery_param.uuid.len       = LEN_UUID_16;
  discovery_param.uuid.uu.uuid16 = current_characteristic_uuid;
  discovery_param.s_handle       = start_handle;
  discovery_param.e_handle       = end_handle;
  BLELOG("BLE: _Discover_characterictic start from %04X to %04X", start_handle, end_handle);
  res = wiced_bt_gatt_client_send_discover(conn_id, GATT_DISCOVER_CHARACTERISTICS,&discovery_param);
  BLELOG("BLE: _Discover_characterictic resul= %04X", res);

}

/*-----------------------------------------------------------------------------------------------------
  Поиск дескриптора конфигурации характеристики

  \param conn_id
  \param start_handle
  \param end_handle
-----------------------------------------------------------------------------------------------------*/
static void _Discover_characteristic_descriptor(uint16_t conn_id, uint16_t start_handle, uint16_t end_handle)
{
  volatile wiced_bt_gatt_status_t res;
  wiced_bt_gatt_discovery_param_t discovery_param;
  discovery_param.uuid.len       = LEN_UUID_16;
  discovery_param.uuid.uu.uuid16 = GATT_UUID_CHAR_CLIENT_CONFIG;
  discovery_param.s_handle = start_handle;
  discovery_param.e_handle = end_handle;
  BLELOG("BLE: _Discover_characteristic_descriptor start from %04X to %04X", start_handle, end_handle);
  res = wiced_bt_gatt_client_send_discover(conn_id, GATT_DISCOVER_CHARACTERISTIC_DESCRIPTORS,&discovery_param);
  BLELOG("BLE: _Discover_characteristic_descriptor resul= %04X", res);
}

/*-----------------------------------------------------------------------------------------------------


  \param p_event_data
-----------------------------------------------------------------------------------------------------*/
static void _cb_gatt_discovery_result(wiced_bt_gatt_event_data_t *p_event_data)
{
  if (p_event_data->discovery_result.discovery_type == GATT_DISCOVER_SERVICES_ALL)
  {
    wiced_bt_gatt_group_value_t *p_service =&p_event_data->discovery_result.discovery_data.group_value;
    BLELOG("BLE: Discovery result GATT_DISCOVER_SERVICES_ALL. UUID=%04X (%04X...%04X)", p_service->service_type.uu.uuid16, p_service->s_handle, p_service->e_handle);
    if (p_service->service_type.uu.uuid16 == current_service_uuid)
    {
      // Найден нужный сервис и диапазон его хэндлов
      service_start_handle = p_service->s_handle;
      service_end_handle = p_service->e_handle;
    }
  }
  else if (p_event_data->discovery_result.discovery_type == GATT_DISCOVER_CHARACTERISTICS)
  {
    wiced_bt_gatt_char_declaration_t *p_char =&p_event_data->discovery_result.discovery_data.characteristic_declaration;
    BLELOG("BLE: Discovery result GATT_DISCOVER_CHARACTERISTICS. UUID=%04X, (%04X...%04X)", p_char->char_uuid.uu.uuid16, p_char->handle, service_end_handle);
    if (p_char->char_uuid.uu.uuid16 == current_characteristic_uuid)
    {
      // Найден хэндл нужной характеристики
      // Дальше остается найти хэндл относящейся к ней характеристики Client Characteristic Configuration Descriptor (CCCD)
      current_characteristic_handle = p_char->handle;
    }
  }
  else if (p_event_data->discovery_result.discovery_type == GATT_DISCOVER_CHARACTERISTIC_DESCRIPTORS)
  {
    BLELOG("BLE: Discovery result GATT_DISCOVER_CHARACTERISTIC_DESCRIPTORS. UUID=%04X, Handle=%04X",p_event_data->discovery_result.discovery_data.char_descr_info.type.uu.uuid16,  p_event_data->discovery_result.discovery_data.char_descr_info.handle);
    if (p_event_data->discovery_result.discovery_data.char_descr_info.type.uu.uuid16 == GATT_UUID_CHAR_CLIENT_CONFIG)
    {
      // Хэндл Client Characteristic Configuration Descriptor (CCCD) найден.
      // Он будет использован для включения нотификации
      current_characteristic_cccd_handle = p_event_data->discovery_result.discovery_data.char_descr_info.handle;
    }
  }
}

/*-----------------------------------------------------------------------------------------------------


  \param p_event_data
-----------------------------------------------------------------------------------------------------*/
static void _cb_gatt_discovery_cplt(wiced_bt_gatt_event_data_t *p_event_data)
{
  volatile wiced_bt_gatt_status_t res;
  if (p_event_data->discovery_complete.discovery_type == GATT_DISCOVER_SERVICES_ALL)
  {
    // Завершено обнаружение заданного сервиса,
    // Если хэнлы сервиса обнаружены, то начать обнаружение характеристик в сервисе
    if (service_start_handle && service_end_handle)
    {
      _Discover_characterictic(p_event_data->discovery_complete.conn_id, service_start_handle, service_end_handle);
    }
  }
  else if (p_event_data->discovery_complete.discovery_type == GATT_DISCOVER_CHARACTERISTICS)
  {
    if (current_characteristic_handle)
    {
      _Discover_characteristic_descriptor(p_event_data->discovery_result.conn_id, current_characteristic_handle, service_end_handle);
    }
  }
  else if (p_event_data->discovery_complete.discovery_type == GATT_DISCOVER_CHARACTERISTIC_DESCRIPTORS)
  {
    if (current_characteristic_cccd_handle)
    {
      uint8_t notification_enable[2] = {0x01, 0x00}; // Значение для включения уведомлений

      wiced_bt_gatt_write_hdr_t write_value;
      write_value.handle = current_characteristic_cccd_handle;
      write_value.offset = 0;
      write_value.len = sizeof(notification_enable);
      write_value.auth_req = GATT_AUTH_REQ_NONE;
      BLELOG("BLE: wiced_bt_gatt_client_send_write start. Handle=%04X ", write_value.handle);
      res = wiced_bt_gatt_client_send_write(p_event_data->discovery_result.conn_id, GATT_REQ_WRITE,&write_value,  notification_enable, NULL);
      BLELOG("BLE: wiced_bt_gatt_client_send_write resul= %04X", res);
    }
  }
}

/*-----------------------------------------------------------------------------------------------------


  \param uuid
  \param buf
  \param buf_len
-----------------------------------------------------------------------------------------------------*/
uint32_t _Read_attr_by_16bit_uuid(uint16_t uuid, uint8_t *buf, uint16_t buf_len)
{
  GET_MCBL;
  ULONG            actual_flags;
  wiced_bt_uuid_t  uuid_cbl;

  uuid_cbl.len = LEN_UUID_16;
  uuid_cbl.uu.uuid16 = uuid;

  curr_buff = buf;
  curr_buff_len = buf_len;

  // После вызова этой функции будет получено событие GATT_OPERATION_CPLT_EVT с идентификатором операции = GATTC_OPTYPE_READ_HANDLE
  wiced_bt_gatt_client_send_read_by_type(curr_conn_id, 0x0001, 0xFFFF,&uuid_cbl,buf, buf_len, GATT_AUTH_REQ_NONE);
  if (tx_event_flags_get(&bt_mon_flags, FLG_BT_GATT_OP_COMPLETED, TX_OR_CLEAR,&actual_flags, MS_TO_TICKS(10000)) == TX_SUCCESS)
  {
    if (curr_op_status != WICED_BT_GATT_SUCCESS)
    {
      MPRINTF(" UUID read %04X error= %04X",uuid, curr_op_status);
      return RES_ERROR;
    }
  }
  else
  {
    MPRINTF(" UUID %04X read timeout", uuid);
    return RES_ERROR;
  }
  return RES_OK;
}

/*-----------------------------------------------------------------------------------------------------


  \param hr_ptr
-----------------------------------------------------------------------------------------------------*/
static void _Print_heart_rate(T_HR_data *hr_ptr)
{
  GET_MCBL;

  MPRINTF(" Heart rate= %d bpm,",hr_ptr->heart_rate);
  if (hr_ptr->energy_expend_sts)
  {
    MPRINTF(" Energy exp.= %d J,",hr_ptr->energy_expended);
  }
  if (hr_ptr->rr_interval_prest)
  {
    MPRINTF(" RR interval= %d ms,",hr_ptr->rr_interval);
  }
  if (hr_ptr->sensor_contact_st)
  {
    if (hr_ptr->sensor_contact_dt)
    {
      MPRINTF(" Contact is detected,");
    }
    else
    {
      MPRINTF(" Contact is not detected,");
    }
  }
}
/*-----------------------------------------------------------------------------------------------------


  \param uuid
  \param buf
  \param buf_len
-----------------------------------------------------------------------------------------------------*/
uint32_t _Get_and_show_heart_rate(uint8_t *buf, uint16_t buf_len)
{
  GET_MCBL;
  ULONG            actual_flags;
  T_HR_data        hr;

  curr_buff = buf;
  curr_buff_len = buf_len;

  // Сначала запускаем процесс получения хэндла и включения нотификации от требуемой характеристики
  _Discover_services(curr_conn_id, UUID_SERVICE_HEART_RATE, UUID_CHARACTERISTIC_HEART_RATE_MEASUREMENT);

  // Ждем флага о событии прихода данных по факту нотификации
  if (tx_event_flags_get(&bt_mon_flags, FLG_BT_GATT_OP_NOTIFICATION, TX_OR_CLEAR,&actual_flags, MS_TO_TICKS(10000)) == TX_SUCCESS)
  {
    if (curr_op_status != WICED_BT_GATT_SUCCESS)
    {
      MPRINTF(" Heart rate (Handle=%04X) read error= %04X." ,current_characteristic_cccd_handle, curr_op_status);
      return RES_ERROR;
    }
    else
    {
      HR_Decode(curr_buff,&hr);
      _Print_heart_rate(&hr);
    }
  }
  else
  {
    MPRINTF(" Heart rate (Handle=%04X) read timeout.", current_characteristic_cccd_handle);
    return RES_ERROR;
  }
  return RES_OK;
}


/*-----------------------------------------------------------------------------------------------------


-----------------------------------------------------------------------------------------------------*/
static void _Heart_rate_monitor(uint16_t  UUID)
{
  GET_MCBL;
  char          str[64];
  uint32_t      row = START_ROW;
  uint32_t      recN;
  uint8_t       b = 0;
  ULONG         actual_flags;
  uint32_t      i;

  VT100_set_cursor_pos(row,0);
  MPRINTF(VT100_CLR_FM_CRSR); // Очищаем экран от текущей строки

  recN = _Filter_dev_list_by_UUID(UUID);
  if (recN == 0)
  {
    MPRINTF("Devices with heart rate monitor service (UUID=%04X) not found.\n\r",UUID);
    return;
  }

  MPRINTF("List of detected devices with heart rate monitor service. Press <S> and enter the number of the selected device, or ESC to skip",UUID);


  Set_gatt_callbacks(&gatt_cb);

  row += 2;
  // Выводим на дисплей все активные записи
  for (i = 0; i < recN; i++)
  {
    if (Get_gat_conn_id(orecs[i].addr) >= 0) continue; // Уже подключенные устройства не показываем
    VT100_set_cursor_pos(row++,0);
    MPRINTF(VT100_CLR_LINE"Nr:%02d  Address: %s, RSSI=%04d",i,Buf_to_hex_str(str,6,orecs[i].addr),orecs[i].rssi);

    curr_dev_indx = i;
    // Выдаем команду на соединение с устройством
    wiced_bt_gatt_le_connect(orecs[i].addr, orecs[i].addr_type, BLE_CONN_MODE_HIGH_DUTY, TRUE);
    if (tx_event_flags_get(&bt_mon_flags, FLG_BT_BLE_DEV_CONNECTED, TX_OR_CLEAR,&actual_flags, MS_TO_TICKS(10000)) == TX_SUCCESS)
    {

      curr_conn_id = Get_gat_conn_id(orecs[i].addr);
      if (curr_conn_id >= 0)
      {
        memset(orecs[i].device_name, 0, sizeof(orecs[i].device_name));
        memset(orecs[i].serial_num, 0, sizeof(orecs[i].serial_num));
        memset(orecs[i].model_num, 0, sizeof(orecs[i].model_num));
        memset(orecs[i].dig_attr, 0, sizeof(orecs[i].dig_attr));
        _Read_attr_by_16bit_uuid(UUID_CHARACTERISTIC_MANUFACTURER_NAME_STRING, orecs[i].device_name, sizeof(orecs[i].device_name)- 1);
        _Read_attr_by_16bit_uuid(UUID_CHARACTERISTIC_SERIAL_NUMBER_STRING, orecs[i].serial_num, sizeof(orecs[i].serial_num)- 1);
        _Read_attr_by_16bit_uuid(UUID_CHARACTERISTIC_MODEL_NUMBER_STRING, orecs[i].model_num, sizeof(orecs[i].model_num)- 1);
        _Get_and_show_heart_rate(orecs[i].dig_attr, sizeof(orecs[i].dig_attr));
      }
      wiced_bt_gatt_disconnect(curr_conn_id);
      if (tx_event_flags_get(&bt_mon_flags, FLG_BT_BLE_DEV_DISCONNECTED, TX_OR_CLEAR,&actual_flags, MS_TO_TICKS(10000)) != TX_SUCCESS)
      {
        MPRINTF(" Disconnection timeout.");
      }
    }
    else
    {
      MPRINTF(" No connection.");
    }

    MPRINTF(" %s, %s, %s",orecs[i].device_name, orecs[i].model_num, orecs[i].serial_num);
  }

  // Для каждой записи проводим чтение имени

  do
  {
    Wait_ms(100);
  }while (WAIT_CHAR(&b, MS_TO_TICKS(1000)) != RES_OK);

  row++;
  if ((b == 'S') || (b == 's'))
  {
    int32_t val = 0;

    // Предложить выбрать устройство в качестве рабочего датчика пульса
    VT100_edit_integer_val(row,&val, 0, i-1);

    VT100_set_cursor_pos(row,0);
    MPRINTF(VT100_CLR_FM_CRSR); // Очищаем экран от текущей строки

    MPRINTF("Selected device number is %d. Press Enter to confirm or ESC to skip.", val);
    do
    {
      if (WAIT_CHAR(&b, MS_TO_TICKS(10000)) == RES_OK)
      {
        if (b == 0x0D)
        {
          Buf_to_hex_str(str,6,orecs[val].addr);
          memcpy(wvar.ble_heart_rate_sensor_addr, str, strlen(str));
          wvar.ble_heart_rate_value_handle = current_characteristic_handle;
          wvar.ble_heart_rate_cccd_handle  = current_characteristic_cccd_handle;
          wvar.ble_heart_rate_sensor_addr_type = orecs[val].addr_type;
          Request_save_nv_parameters(APPLICATION_PARAMS);
        }
      }
      else
      {
        break;
      }
    } while (b==0);
  }

  Delete_gatt_callbacks(& gatt_cb);

}

/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
static void _Accept_observer_result(uint32_t start_row)
{
  GET_MCBL;
  char          str[64];
  uint32_t      i;
  uint32_t      new_rec;
  uint32_t      row;

  observer_busy = 1;

  new_rec = 1;
  for (i = 0; i < MAX_OSERVER_RECS_NUM; i++)
  {
    if (orecs[i].active)
    {
      if (memcmp(orecs[i].addr, last_scan_result.remote_bd_addr, 6) == 0)
      {
        // Обнаружена уже существующая запись об этом адресе
        orecs[i].cnt++;
        orecs[i].addr_type = last_scan_result.ble_addr_type;
        orecs[i].flag      = last_scan_result.flag;
        orecs[i].rssi      = last_scan_result.rssi;
        orecs[i].tx_power  = last_scan_result.tx_power;
        if (last_scan_UUIDs_num != 0)
        {
          orecs[i].UUIDs_num = last_scan_UUIDs_num;
          memcpy(orecs[i].UUIDs, last_scan_UUIDs, sizeof(last_scan_UUIDs));
        }
        if (last_scan_name[0] != 0)
        {
          memcpy(orecs[i].device_name, last_scan_name, sizeof(last_scan_name));
        }
        new_rec = 0;
        break;
      }
    }
    else break;
  }

  if (new_rec == 1)
  {
    orecs[i].active = 1;
    memcpy(orecs[i].addr, last_scan_result.remote_bd_addr,6);
    orecs[i].addr_type = last_scan_result.ble_addr_type;
    orecs[i].flag      = last_scan_result.flag;
    orecs[i].rssi      = last_scan_result.rssi;
    orecs[i].tx_power  = last_scan_result.tx_power;
    orecs[i].UUIDs_num = last_scan_UUIDs_num;
    memcpy(orecs[i].UUIDs, last_scan_UUIDs, sizeof(last_scan_UUIDs));
    memcpy(orecs[i].device_name, last_scan_name, sizeof(last_scan_name));
  }

  row = start_row;

  // Выводим на дисплей все активные записи
  for (i = 0; i < MAX_OSERVER_RECS_NUM; i++)
  {
    VT100_set_cursor_pos(row,0);
    if (orecs[i].active)
    {
      MPRINTF(VT100_CLR_LINE"<%02d> Cnt=%06d, Type=%02X, Flag=%02X,  %s, RSSI=%04d",
        i,
        orecs[i].cnt,
        orecs[i].addr_type,
        orecs[i].flag,
        Buf_to_hex_str(str,6,orecs[i].addr),
        orecs[i].rssi);

      if (orecs[i].UUIDs_num != 0)
      {
        MPRINTF(" UUIDs = ");
        for (uint32_t n = 0; n < orecs[i].UUIDs_num; n++)
        {
          MPRINTF("%04X ",orecs[i].UUIDs[n]);
        }
      }
      MPRINTF(" %s",orecs[i].device_name);
    }
    else
    {
      MPRINTF(VT100_CLR_LINE);
    }
    row++;
  }

  observer_busy = 0;
}

/*-----------------------------------------------------------------------------------------------------


  \param void

  \return void _Print_connection
-----------------------------------------------------------------------------------------------------*/
static void _Observer_mode(void)
{
  GET_MCBL;
  ULONG         actual_flags;
  uint8_t       b;
  uint32_t      row;

  memset(orecs, 0, sizeof(orecs));

  row = START_ROW;
  VT100_set_cursor_pos(row,0);
  MPRINTF(VT100_CLR_FM_CRSR); // Очищаем экран от текущей строки
  MPRINTF("BLE Device list. Press <H> to filter out Heart Rate sensors.");


  wiced_bt_ble_observe(WICED_TRUE, 0, ble_scan_result_callback);
  do
  {
    if (tx_event_flags_get(&bt_mon_flags, 0xFFFFFFFF, TX_OR_CLEAR,&actual_flags, 100) == TX_SUCCESS)
    {
      if (actual_flags & FLG_BT_OBSRV_RES_NEW)
      {
        _Accept_observer_result(row + 2);
      }
      if (actual_flags & FLG_BT_OBSRV_COMPL)
      {
        break;
      }
    }
    if (WAIT_CHAR(&b, 100) == RES_OK)
    {
      wiced_bt_ble_observe(WICED_FALSE, 0, ble_scan_result_callback);
    }
  } while (1);


  Wait_ms(100);
  if ((b == 'H') || (b == 'h'))
  {
    _Heart_rate_monitor(UUID_SERVICE_HEART_RATE);
  }
}

/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
static void _Print_identity_keys(void)
{
  GET_MCBL;
  char          str[64];
  uint8_t       b;
  uint32_t      row = START_ROW;

  VT100_set_cursor_pos(row,0);
  MPRINTF(VT100_CLR_FM_CRSR); // Очищаем экран от текущей строки
  MPRINTF("\n\r\n\r--- Identity_keys ----\n\r");
  MPRINTF("Key type mask               : %02X\n\r",bt_nv.identity_keys.key.key_type_mask);
  MPRINTF("Identity Root Key (IR)      : %s\n\r",Buf_to_hex_str(str,16, bt_nv.identity_keys.key.id_keys.ir));
  MPRINTF("Identity Resolving Key (IRK): %s\n\r",Buf_to_hex_str(str,16, bt_nv.identity_keys.key.id_keys.irk));
  MPRINTF("Diversifying Key (DHK)      : %s\n\r",Buf_to_hex_str(str,16, bt_nv.identity_keys.key.id_keys.dhk));
  MPRINTF("LE encryption key (ER)      : %s\n\r",Buf_to_hex_str(str,16, bt_nv.identity_keys.key.er));
  WAIT_CHAR(&b, 1000000);
}

/*-----------------------------------------------------------------------------------------------------


-----------------------------------------------------------------------------------------------------*/
static void _Print_pairing_keys(void)
{
  GET_MCBL;
  char          str[64];
  uint8_t       b;
  uint32_t      row = START_ROW;

  VT100_set_cursor_pos(row,0);
  MPRINTF(VT100_CLR_FM_CRSR); // Очищаем экран от текущей строки
  MPRINTF("\n\r\n\r--- Pairing_keys ----\n\r");
  for (uint32_t i = 0; i < PARING_KEYS_NUM; i++)
  {
    MPRINTF("\n\rPairing  %d\n\r",i);
    MPRINTF("Address type            : %s, mask: %02X\n\r",To_Str_Addr_type(bt_nv.paring_keys.link_keys[i].addr_type), bt_nv.paring_keys.link_keys[i].key_mask);
    MPRINTF("BD Address              : %s\n\r",Buf_to_hex_str(str,6, bt_nv.paring_keys.link_keys[i].key.bd_addr));
    MPRINTF("Connecton Address       : %s\n\r",Buf_to_hex_str(str,6, bt_nv.paring_keys.link_keys[i].key.conn_addr));
    MPRINTF("BLE address type        : %s\n\r",To_Str_Addr_type(bt_nv.paring_keys.link_keys[i].key.key_data.ble_addr_type));
    MPRINTF("LE keys available mask  : %02X\n\r",bt_nv.paring_keys.link_keys[i].key.key_data.le_keys_available_mask);
    MPRINTF("BR EDR key type         : %02X\n\r",bt_nv.paring_keys.link_keys[i].key.key_data.br_edr_key_type);
    MPRINTF("BR EDR key              : %s\n\r",Buf_to_hex_str(str,16, bt_nv.paring_keys.link_keys[i].key.key_data.br_edr_key));
    MPRINTF("BLE address type        : %s\n\r",To_Str_Addr_type(bt_nv.paring_keys.link_keys[i].key.key_data.ble_addr_type));
    MPRINTF("BLE keys data:\n\r");
    MPRINTF("  Identity Resolving Key (IRK)                    : %s\n\r", Buf_to_hex_str(str,16, bt_nv.paring_keys.link_keys[i].key.key_data.le_keys.irk));
    MPRINTF("  Peer long term key (PLTK)                       : %s\n\r", Buf_to_hex_str(str,16, bt_nv.paring_keys.link_keys[i].key.key_data.le_keys.pltk));
    MPRINTF("  Peer Connection Signature Resolving Key (PCSRK) : %s\n\r", Buf_to_hex_str(str,16, bt_nv.paring_keys.link_keys[i].key.key_data.le_keys.pcsrk));
    MPRINTF("  Local long term key (LLTK)                      : %s\n\r", Buf_to_hex_str(str,16, bt_nv.paring_keys.link_keys[i].key.key_data.le_keys.lltk));
    MPRINTF("  Local Connection Signature Resolving Key (LCSRK): %s\n\r", Buf_to_hex_str(str,16, bt_nv.paring_keys.link_keys[i].key.key_data.le_keys.lcsrk));
    MPRINTF("  Random vector for LTK generation (RAND)         : %s\n\r", Buf_to_hex_str(str,16, bt_nv.paring_keys.link_keys[i].key.key_data.le_keys.rand));
    MPRINTF("  Encrypted LTK Diversifier (EDIV)                : %04X\n\r", bt_nv.paring_keys.link_keys[i].key.key_data.le_keys.ediv);
    MPRINTF("  Local Div. to generate local LTK and CSRK (DIV) : %04X\n\r", bt_nv.paring_keys.link_keys[i].key.key_data.le_keys.div);
    MPRINTF("  Local pairing sec.level : %02X\n\r", bt_nv.paring_keys.link_keys[i].key.key_data.le_keys.sec_level);
    MPRINTF("  Key size                : %d\n\r",   bt_nv.paring_keys.link_keys[i].key.key_data.le_keys.key_size);
    MPRINTF("  SRK sec. level          : %02X\n\r", bt_nv.paring_keys.link_keys[i].key.key_data.le_keys.srk_sec_level);
    MPRINTF("  Local CSRK sec. level   : %02X\n\r", bt_nv.paring_keys.link_keys[i].key.key_data.le_keys.local_csrk_sec_level);
    MPRINTF("  Counter                 : %d\n\r",   bt_nv.paring_keys.link_keys[i].key.key_data.le_keys.counter);
    MPRINTF("  Local_counter           : %d\n\r",   bt_nv.paring_keys.link_keys[i].key.key_data.le_keys.local_counter);
  }
  WAIT_CHAR(&b, 1000000);

}

/*-----------------------------------------------------------------------------------------------------


-----------------------------------------------------------------------------------------------------*/
static void _Print_connections(void)
{
  GET_MCBL;
  char                     str[64];
  uint8_t                  b;
  ULONG                    actual_flags;
  uint32_t                 row = START_ROW;
  T_gatt_connection_cbl   *cons;
  T_bt_dev_pairing_info   *pinf;

  VT100_set_cursor_pos(row,0);
  MPRINTF(VT100_CLR_FM_CRSR); // Очищаем экран от текущей строки

  do
  {
    VT100_set_cursor_pos(row,0);

    MPRINTF("\n\r\n\r----------------------------- Last connections --------------------------------\n\r");
    for (uint32_t i = 0; i < MAX_BLE_CONNECTION_NUMBER; i++)
    {
      cons = Get_gatt_connection_status(i);
      if (cons->active)
      {
        wiced_bt_dev_get_remote_name(cons->addr,_Accept_remote_name);
        tx_event_flags_get(&bt_mon_flags, FLG_BT_NAME_COMPL, TX_OR_CLEAR,&actual_flags, MS_TO_TICKS(5000));

        wiced_bt_dev_read_rssi(cons->addr, cons->status.transport, _Accept_RSSI_callback);
        tx_event_flags_get(&bt_mon_flags, FLG_BT_RSSI_COMPL, TX_OR_CLEAR,&actual_flags, MS_TO_TICKS(5000));

        MPRINTF(VT100_CLR_LINE"Connection ID           : %d  \n\r",  cons->status.conn_id);
        MPRINTF(VT100_CLR_LINE"Connected device address: %s. Type: %s. Link Role: %s, Remote name : %s\n\r",  Buf_to_hex_str(str,6,cons->addr), To_Str_Addr_type(cons->status.addr_type), To_Str_Link_Role(cons->status.link_role), remote_name);
        MPRINTF(VT100_CLR_LINE"RSSI                    : %d dB, status: %d, HCI status: %d, Bluetooth transport : %s\n\r\n\r",rssi_info.rssi, rssi_info.status, rssi_info.hci_status, To_Str_Bluetooth_transport(cons->status.transport));
      }
      else
      {
        MPRINTF(VT100_CLR_LINE"Connection ID           : ---\n\r");
        MPRINTF(VT100_CLR_LINE"Connected device address: ---\n\r");
        MPRINTF(VT100_CLR_LINE"RSSI                    : ---\n\r\n\r");
      }
    }

    MPRINTF(VT100_CLR_LINE"\n\r----------------------------- Last pairing -----------------------------------\n\r");

    pinf = BLE_get_last_pairing_info();
    MPRINTF(VT100_CLR_LINE"Paired peer  address    : %s\n\r", Buf_to_hex_str(str,6,pinf->addr));
    MPRINTF(VT100_CLR_LINE"Bluetooth transport     : %s\n\r",To_Str_Bluetooth_transport(pinf->info.transport));
    MPRINTF(VT100_CLR_LINE"Bluetooth EDR status    : %d\n\r",pinf->info.pairing_complete_info.br_edr.status);
    MPRINTF(VT100_CLR_LINE"Bluetooth LE status     : %04X\n\r",pinf->info.pairing_complete_info.ble.status);
    MPRINTF(VT100_CLR_LINE"Bluetooth LE resolv.addr: %s Type: %s\n\r",Buf_to_hex_str(str,6,pinf->info.pairing_complete_info.ble.resolved_bd_addr), To_Str_Addr_type(pinf->info.pairing_complete_info.ble.resolved_bd_addr_type));
    MPRINTF(VT100_CLR_LINE"Bluetooth LE sec.level  : %s\n\r",To_Str_BLE_sec_level(pinf->info.pairing_complete_info.ble.sec_level));
    MPRINTF(VT100_CLR_LINE"Bluetooth LE is pair canceled : %s  Failure reason: %s\n\r",To_Str_Yes_No(pinf->info.pairing_complete_info.ble.is_pair_cancel), To_Str_SMP_status(pinf->info.pairing_complete_info.ble.reason));

    if (WAIT_CHAR(&b, 100) == RES_OK) return;

  }while (1);

}

/*-----------------------------------------------------------------------------------------------------


-----------------------------------------------------------------------------------------------------*/
static void _Print_header(void)
{
  GET_MCBL;
  char            str[64];

  wiced_bt_dev_local_addr_ext_t       ext_data;


  MPRINTF(VT100_CLEAR_AND_HOME);
  MPRINTF(" ===  Bluetooth monitor ===\n\r");
  MPRINTF("ESC - exit,  [L] - Print connections. [K]- Print identity keys, [B] - Print clients bindings, [O] - Observer mode, [D] - Erase bindings data\n\r");
  MPRINTF("----------------------------------------------------------------------\n\r");

  wiced_bt_dev_read_local_addr_ext(&ext_data);


  // Здесь наблюдаем подключения по BLE
  MPRINTF("Device local    address: %s\n\r", Buf_to_hex_str(str,6,ext_data.local_addr));
  MPRINTF("Device private  address: %s. Type:%s\n\r", Buf_to_hex_str(str,6,ext_data.private_addr), To_Str_Addr_type(ext_data.private_addr_type));
  MPRINTF("LE Privacy is enabled  : %s.  Static random address is used : %s\n\r", To_Str_Yes_No(ext_data.is_privacy_enabled), To_Str_Yes_No(ext_data.is_static_rand_addr_used));


}

/*-----------------------------------------------------------------------------------------------------


  \param keycode
-----------------------------------------------------------------------------------------------------*/
void Do_Bluetooth_control(uint8_t keycode)
{
  GET_MCBL;
  uint8_t                    b;

  if (BLE_is_initialised() == 0)
  {
    MPRINTF("Bluetooth stack not initialised. Abort!\n\r");
    Wait_ms(1000);
    return;
  }
  tx_event_flags_create(&bt_mon_flags, "bt_mon");
  _Print_header();

  do
  {
    if (WAIT_CHAR(&b, 500) == RES_OK)
    {
      switch (b)
      {
      case 'L':
      case 'l':
        _Print_connections();
        break;
      case 'K':
      case 'k':
        _Print_identity_keys();
        break;
      case 'B':
      case 'b':
        _Print_pairing_keys();
        break;
      case 'D':
      case 'd':
        memset(&bt_nv.paring_keys, 0, sizeof(bt_nv.paring_keys));
        bt_nv.oldest_paring_keys_indx = 0;
        Request_save_bt_nv_data();
        break;
      case 'O':
      case 'o':
        _Observer_mode();
        break;

      case VT100_ESC:
        tx_event_flags_delete(&bt_mon_flags);
        return;
      }
    }
    else
    {
      _Print_header();
    }

  }while (1);


}


