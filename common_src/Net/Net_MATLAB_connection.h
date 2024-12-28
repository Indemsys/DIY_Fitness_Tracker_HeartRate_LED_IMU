#ifndef MATLAB_CONNECTION_H
  #define MATLAB_CONNECTION_H


  #define MATLAB_CONN_PORT 3333

  #define MTLB_CUSTOM_CMD_1                       1
  #define MTLB_CUSTOM_CMD_2                       2

  #define MTLB_CMD_STOP                           0xFF
  #define MTLB_ANSWER_OK                          0
  #define MTLB_ANSWER_ERROR                       1

__packed typedef struct
{
  uint32_t cmd_id;
  uint32_t data;
  uint16_t crc;
}
T_matlab_cmd1;


__packed typedef struct
{
  uint32_t cmd_id;
  uint32_t data;
  uint16_t crc;
}
T_matlab_cmd2;

typedef struct
{
  uint32_t packet_num;
  uint32_t mutex_fails_cnt;
  uint32_t oversize_drops_cnt;
  uint32_t alloc_fail_cnt;
  uint32_t queue_fail_cnt;
  uint32_t lost_bytes_cnt;
  uint32_t sending_fail_cnt;


} T_matlab_communication_status;


extern T_matlab_communication_status  mc_stat;

uint32_t MATLAB_server_create(NX_IP *ip_ptr);
int      MATLAB_packet_send(const void *buf, unsigned int len);
void     MATLAB_post_data_to_tx_queue(uint8_t *buf_ptr, uint16_t sz);
uint32_t MATLAB_server_delete(void);


#endif // MATLAB_CONNECTION_H



