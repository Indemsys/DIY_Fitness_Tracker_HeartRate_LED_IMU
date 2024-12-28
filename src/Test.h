#ifndef TEST_H
  #define TEST_H

  #include "App.h"

  #define     SOFTWARE_VERSION         "APP"
  #define     HARDWARE_VERSION         "APP"


  #include "App_task.h"
  #include "IPerf_init.h"
  #include "TEST_Params.h"
  #include "Voice_announcer.h"
  #include "Motion_sensor.h"
  #include "Test_FreeMaster_tbl.h"
  #include "Test_Freemaster_cmd.h"
  #include "Test_WEB_server.h"
  #include "Test_WEB_files.h"
  #include "Test_WEB_serializer.h"
  #include "MKR_RGB_Shield.h"
  #include "MKR_RGB_Shield_HMI.h"
  #include "HeartRate_man.h"
  #include "Bluetooth_monitor.h"
  #include "App_terminal.h"

void Set_AUD_SHDN   (int32_t );
void Board_pins_init(void);

#endif


