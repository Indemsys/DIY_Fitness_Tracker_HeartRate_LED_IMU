#ifndef __FREEMASTER_LOOP
  #define __FREEMASTER_LOOP

  #define FMCMD_RESET_DEVICE                   0x01
  #define FMCMD_CHECK_LOG_PIPE                 0x07
  #define FMCMD_SAVE_APP_PARAMS                0x08
  #define FMCMD_SAVE_MODULE_PARAMS             0x09


  #define FREEMASTER_NO_INTERFACE              0
  #define FREEMASTER_ON_NET                    1
  #define FREEMASTER_ON_SERIAL                 2


#include "FreeMaster_USB_drv.h"

uint32_t Thread_FreeMaster_create(void);
void     Thread_FreeMaster_delete(void);
void     Task_FreeMaster(uint32_t initial_data);
void     Determine_FreeMaster_interface_type(void);

#endif

