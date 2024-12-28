#ifndef TEST_FREEMASTER_CMD_H
  #define TEST_FREEMASTER_CMD_H


#define FMCMD_START_IMU_STREAM             0x10
#define FMCMD_STOP_IMU_STREAM              0x11
#define FMCMD_START_MOTION_LOG             0x12
#define FMCMD_STOP_MOTION_LOG              0x13


uint8_t App_Freemaster_cmd_man(uint16_t app_command, uint32_t len, uint8_t  *dbuf);


#endif // TEST_FREEMASTER_CMD_H



