#ifndef TEST_WEB_SERVER_H
  #define TEST_WEB_SERVER_H


uint32_t App_get_handler(NX_WEB_HTTP_SERVER *server_ptr, CHAR *resource, NX_PACKET *packet_ptr);
uint32_t App_pos_handler(NX_WEB_HTTP_SERVER *server_ptr, CHAR *resource, NX_PACKET *packet_ptr);


#endif



