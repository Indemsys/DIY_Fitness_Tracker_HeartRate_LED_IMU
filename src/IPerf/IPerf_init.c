// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 2024-10-24
// 18:23:33
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include   "Test.h"

#ifdef IP_PERF

extern void            nx_iperf_entry(NX_PACKET_POOL *pool_ptr, NX_IP *ip_ptr, UCHAR *http_stack, ULONG http_stack_size, UCHAR *iperf_stack, ULONG iperf_stack_size);

  #define IPERF_TASK_STACK_SIZE (1024*2)
static uint8_t iperf_stack[IPERF_TASK_STACK_SIZE] BSP_PLACE_IN_SECTION_V2(".stack.iperf_thread")BSP_ALIGN_VARIABLE_V2(BSP_STACK_ALIGNMENT);

  #define HTTP_TASK_STACK_SIZE (1024*2)
static uint8_t http_stack[HTTP_TASK_STACK_SIZE] BSP_PLACE_IN_SECTION_V2(".stack.http_thread")BSP_ALIGN_VARIABLE_V2(BSP_STACK_ALIGNMENT);

/*-----------------------------------------------------------------------------------------------------


-----------------------------------------------------------------------------------------------------*/
void IPerf_init(void)
{
  if (wvar.en_iperf)
  {

    char ip_addr_str[32];
    char ip_mask_str[32];
    NX_IP *ip;

    APPLOG("Devices '%s' main task started", wvar.name);

    // Ждем инициализации сетевого стека


    ip = Net_get_ip_ptr();
    if (ip != NULL)
    {
      ULONG ip_address;
      ULONG network_mask;
      nx_ip_address_get(ip,&ip_address,&network_mask);
      snprintf(ip_addr_str, 31, "%03d.%03d.%03d.%03d", IPADDR(ip_address));
      snprintf(ip_mask_str, 31, "%03d.%03d.%03d.%03d", IPADDR(network_mask));

      APPLOG("Iperf task . IP address: %s , mask: %s", ip_addr_str, ip_mask_str);
      nx_iperf_entry(&net_packet_pool, ip, (UCHAR *)http_stack, HTTP_TASK_STACK_SIZE, (UCHAR *)iperf_stack, IPERF_TASK_STACK_SIZE);
    }
    else
    {
      APPLOG("Iperf task fail. No signal from Net stack");
    }
  }
}


#endif


