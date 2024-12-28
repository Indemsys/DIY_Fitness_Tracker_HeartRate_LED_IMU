#ifndef NET_SNTP_CLIENT_H
  #define NET_SNTP_CLIENT_H


uint32_t Net_SNTP_client_create(NX_IP  *ip_ptr);
uint32_t SNTP_client_delete(void);
void     Net_SNTP_client_task(NX_IP   *ip_ptr);
uint8_t  Is_connected_to_internet(void);

#endif // NET_SNTP_CLIENT_H



