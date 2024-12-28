#ifndef WIFI_AP_NETWORK_H
  #define WIFI_AP_NETWORK_H

#define MAX_SSID_LEN  32

extern char   g_wifi_ap_ssid[MAX_SSID_LEN+1];

uint32_t      WIFI_AP_accept_STA_connection(void);
uint32_t      WIFI_AP_accept_STA_disconnection(void);
uint32_t      WIFI_AP_start(void);
NX_INTERFACE *WIFI_AP_link_state(void);
void          WIFI_AP_save_remote_sta_addr(whd_mac_t *addr_ptr);
whd_mac_t    *WIFI_AP_get_remote_sta_addr(void);
#endif



