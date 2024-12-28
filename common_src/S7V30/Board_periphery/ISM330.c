// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 2019.06.01
// 20:23:00
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include   "App.h"

const i2c_cfg_t g_sf_i2c_ISM330_i2c_cfg =
{
  .channel             = 2,
  .rate                = I2C_RATE_FAST,
  .slave               = 0x6B,
  .addr_mode           = I2C_ADDR_MODE_7BIT,
  .sda_delay           = (300),
  .p_transfer_tx       = &DTC_I2C2_TX_transfer_instance,
  .p_transfer_rx       = &DTC_I2C2_RX_transfer_instance,
  .p_callback          = (NULL),
  .p_context           = &I2C2_interface_instance,
  .rxi_ipl             = 3,
  .txi_ipl             = 3,
  .tei_ipl             = 3,
  .eri_ipl             = 3,
  .p_extend            = &I2C2_extended_configuration,
};

sf_i2c_instance_ctrl_t g_sf_i2c_ISM330_ctrl =
{
  .p_lower_lvl_ctrl = &I2C2_control_block,
};

const sf_i2c_cfg_t g_sf_i2c_ISM330_cfg =
{
  .p_bus                    = (sf_i2c_bus_t *)&I2C2_bus,
  .p_lower_lvl_cfg          = &g_sf_i2c_ISM330_i2c_cfg,
};


/* Instance structure to use this module. */
const sf_i2c_instance_t g_sf_i2c_ISM330 =
{
  .p_ctrl        = &g_sf_i2c_ISM330_ctrl,
  .p_cfg         = &g_sf_i2c_ISM330_cfg,
  .p_api         = &g_sf_i2c_on_sf_i2c
};


static uint32_t ISM330_opened = 0;

static uint8_t  rv[0x75];

const T_ISM330_reg_descr ISM330_regs[] =
{
  { ISM330_FUNC_CFG_ACCESS           ,"ISM330_FUNC_CFG_ACCESS       ",&rv[ISM330_FUNC_CFG_ACCESS         ]},
  { ISM330_SENSOR_SYNC_TIME_FRAME    ,"ISM330_SENSOR_SYNC_TIME_FRAME",&rv[ISM330_SENSOR_SYNC_TIME_FRAME  ] },
  { ISM330_SENSOR_SYNC_RES_RATIO     ,"ISM330_SENSOR_SYNC_RES_RATIO ",&rv[ISM330_SENSOR_SYNC_RES_RATIO   ] },
  { ISM330_FIFO_CTRL1                ,"ISM330_FIFO_CTRL1            ",&rv[ISM330_FIFO_CTRL1              ] },
  { ISM330_FIFO_CTRL2                ,"ISM330_FIFO_CTRL2            ",&rv[ISM330_FIFO_CTRL2              ] },
  { ISM330_FIFO_CTRL3                ,"ISM330_FIFO_CTRL3            ",&rv[ISM330_FIFO_CTRL3              ] },
  { ISM330_FIFO_CTRL4                ,"ISM330_FIFO_CTRL4            ",&rv[ISM330_FIFO_CTRL4              ] },
  { ISM330_FIFO_CTRL5                ,"ISM330_FIFO_CTRL5            ",&rv[ISM330_FIFO_CTRL5              ] },
  { ISM330_DRDY_PULSE_CFG            ,"ISM330_DRDY_PULSE_CFG        ",&rv[ISM330_DRDY_PULSE_CFG          ] },
  { ISM330_INT1_CTRL                 ,"ISM330_INT1_CTRL             ",&rv[ISM330_INT1_CTRL               ] },
  { ISM330_INT2_CTRL                 ,"ISM330_INT2_CTRL             ",&rv[ISM330_INT2_CTRL               ] },
  { ISM330_WHO_AM_I                  ,"ISM330_WHO_AM_I              ",&rv[ISM330_WHO_AM_I                ] },
  { ISM330_CTRL1_XL                  ,"ISM330_CTRL1_XL              ",&rv[ISM330_CTRL1_XL                ] },
  { ISM330_CTRL2_G                   ,"ISM330_CTRL2_G               ",&rv[ISM330_CTRL2_G                 ] },
  { ISM330_CTRL3_C                   ,"ISM330_CTRL3_C               ",&rv[ISM330_CTRL3_C                 ] },
  { ISM330_CTRL4_C                   ,"ISM330_CTRL4_C               ",&rv[ISM330_CTRL4_C                 ] },
  { ISM330_CTRL5_C                   ,"ISM330_CTRL5_C               ",&rv[ISM330_CTRL5_C                 ] },
  { ISM330_CTRL6_C                   ,"ISM330_CTRL6_C               ",&rv[ISM330_CTRL6_C                 ] },
  { ISM330_CTRL7_G                   ,"ISM330_CTRL7_G               ",&rv[ISM330_CTRL7_G                 ] },
  { ISM330_CTRL8_XL                  ,"ISM330_CTRL8_XL              ",&rv[ISM330_CTRL8_XL                ] },
  { ISM330_CTRL9_XL                  ,"ISM330_CTRL9_XL              ",&rv[ISM330_CTRL9_XL                ] },
  { ISM330_CTRL10_C                  ,"ISM330_CTRL10_C              ",&rv[ISM330_CTRL10_C                ] },
  { ISM330_MASTER_CONFIG             ,"ISM330_MASTER_CONFIG         ",&rv[ISM330_MASTER_CONFIG           ] },
  { ISM330_WAKE_UP_SRC               ,"ISM330_WAKE_UP_SRC           ",&rv[ISM330_WAKE_UP_SRC             ] },
  { ISM330_TAP_SRC                   ,"ISM330_TAP_SRC               ",&rv[ISM330_TAP_SRC                 ] },
  { ISM330_D6D_SRC                   ,"ISM330_D6D_SRC               ",&rv[ISM330_D6D_SRC                 ] },
  { ISM330_STATUS_REG                ,"ISM330_STATUS_REG            ",&rv[ISM330_STATUS_REG              ] },
  { ISM330_OUT_TEMP_L                ,"ISM330_OUT_TEMP_L            ",&rv[ISM330_OUT_TEMP_L              ] },
  { ISM330_OUT_TEMP_H                ,"ISM330_OUT_TEMP_H            ",&rv[ISM330_OUT_TEMP_H              ] },
  { ISM330_OUTX_L_G                  ,"ISM330_OUTX_L_G              ",&rv[ISM330_OUTX_L_G                ] },
  { ISM330_OUTX_H_G                  ,"ISM330_OUTX_H_G              ",&rv[ISM330_OUTX_H_G                ] },
  { ISM330_OUTY_L_G                  ,"ISM330_OUTY_L_G              ",&rv[ISM330_OUTY_L_G                ] },
  { ISM330_OUTY_H_G                  ,"ISM330_OUTY_H_G              ",&rv[ISM330_OUTY_H_G                ] },
  { ISM330_OUTZ_L_G                  ,"ISM330_OUTZ_L_G              ",&rv[ISM330_OUTZ_L_G                ] },
  { ISM330_OUTZ_H_G                  ,"ISM330_OUTZ_H_G              ",&rv[ISM330_OUTZ_H_G                ] },
  { ISM330_OUTX_L_XL                 ,"ISM330_OUTX_L_XL             ",&rv[ISM330_OUTX_L_XL               ] },
  { ISM330_OUTX_H_XL                 ,"ISM330_OUTX_H_XL             ",&rv[ISM330_OUTX_H_XL               ] },
  { ISM330_OUTY_L_XL                 ,"ISM330_OUTY_L_XL             ",&rv[ISM330_OUTY_L_XL               ] },
  { ISM330_OUTY_H_XL                 ,"ISM330_OUTY_H_XL             ",&rv[ISM330_OUTY_H_XL               ] },
  { ISM330_OUTZ_L_XL                 ,"ISM330_OUTZ_L_XL             ",&rv[ISM330_OUTZ_L_XL               ] },
  { ISM330_OUTZ_H_XL                 ,"ISM330_OUTZ_H_XL             ",&rv[ISM330_OUTZ_H_XL               ] },
  { ISM330_SENSORHUB1_REG            ,"ISM330_SENSORHUB1_REG        ",&rv[ISM330_SENSORHUB1_REG          ] },
  { ISM330_SENSORHUB2_REG            ,"ISM330_SENSORHUB2_REG        ",&rv[ISM330_SENSORHUB2_REG          ] },
  { ISM330_SENSORHUB3_REG            ,"ISM330_SENSORHUB3_REG        ",&rv[ISM330_SENSORHUB3_REG          ] },
  { ISM330_SENSORHUB4_REG            ,"ISM330_SENSORHUB4_REG        ",&rv[ISM330_SENSORHUB4_REG          ] },
  { ISM330_SENSORHUB5_REG            ,"ISM330_SENSORHUB5_REG        ",&rv[ISM330_SENSORHUB5_REG          ] },
  { ISM330_SENSORHUB6_REG            ,"ISM330_SENSORHUB6_REG        ",&rv[ISM330_SENSORHUB6_REG          ] },
  { ISM330_SENSORHUB7_REG            ,"ISM330_SENSORHUB7_REG        ",&rv[ISM330_SENSORHUB7_REG          ] },
  { ISM330_SENSORHUB8_REG            ,"ISM330_SENSORHUB8_REG        ",&rv[ISM330_SENSORHUB8_REG          ] },
  { ISM330_SENSORHUB9_REG            ,"ISM330_SENSORHUB9_REG        ",&rv[ISM330_SENSORHUB9_REG          ] },
  { ISM330_SENSORHUB10_REG           ,"ISM330_SENSORHUB10_REG       ",&rv[ISM330_SENSORHUB10_REG         ] },
  { ISM330_SENSORHUB11_REG           ,"ISM330_SENSORHUB11_REG       ",&rv[ISM330_SENSORHUB11_REG         ] },
  { ISM330_SENSORHUB12_REG           ,"ISM330_SENSORHUB12_REG       ",&rv[ISM330_SENSORHUB12_REG         ] },
  { ISM330_FIFO_STATUS1              ,"ISM330_FIFO_STATUS1          ",&rv[ISM330_FIFO_STATUS1            ] },
  { ISM330_FIFO_STATUS2              ,"ISM330_FIFO_STATUS2          ",&rv[ISM330_FIFO_STATUS2            ] },
  { ISM330_FIFO_STATUS3              ,"ISM330_FIFO_STATUS3          ",&rv[ISM330_FIFO_STATUS3            ] },
  { ISM330_FIFO_STATUS4              ,"ISM330_FIFO_STATUS4          ",&rv[ISM330_FIFO_STATUS4            ] },
  { ISM330_FIFO_DATA_OUT_L           ,"ISM330_FIFO_DATA_OUT_L       ",&rv[ISM330_FIFO_DATA_OUT_L         ] },
  { ISM330_FIFO_DATA_OUT_H           ,"ISM330_FIFO_DATA_OUT_H       ",&rv[ISM330_FIFO_DATA_OUT_H         ] },
  { ISM330_TIMESTAMP0_REG            ,"ISM330_TIMESTAMP0_REG        ",&rv[ISM330_TIMESTAMP0_REG          ] },
  { ISM330_TIMESTAMP1_REG            ,"ISM330_TIMESTAMP1_REG        ",&rv[ISM330_TIMESTAMP1_REG          ] },
  { ISM330_TIMESTAMP2_REG            ,"ISM330_TIMESTAMP2_REG        ",&rv[ISM330_TIMESTAMP2_REG          ] },
  { ISM330_SENSORHUB13_REG           ,"ISM330_SENSORHUB13_REG       ",&rv[ISM330_SENSORHUB13_REG         ] },
  { ISM330_SENSORHUB14_REG           ,"ISM330_SENSORHUB14_REG       ",&rv[ISM330_SENSORHUB14_REG         ] },
  { ISM330_SENSORHUB15_REG           ,"ISM330_SENSORHUB15_REG       ",&rv[ISM330_SENSORHUB15_REG         ] },
  { ISM330_SENSORHUB16_REG           ,"ISM330_SENSORHUB16_REG       ",&rv[ISM330_SENSORHUB16_REG         ] },
  { ISM330_SENSORHUB17_REG           ,"ISM330_SENSORHUB17_REG       ",&rv[ISM330_SENSORHUB17_REG         ] },
  { ISM330_SENSORHUB18_REG           ,"ISM330_SENSORHUB18_REG       ",&rv[ISM330_SENSORHUB18_REG         ] },
  { ISM330_FUNC_SRC1                 ,"ISM330_FUNC_SRC1             ",&rv[ISM330_FUNC_SRC1               ] },
  { ISM330_FUNC_SRC2                 ,"ISM330_FUNC_SRC2             ",&rv[ISM330_FUNC_SRC2               ] },
  { ISM330_TAP_CFG                   ,"ISM330_TAP_CFG               ",&rv[ISM330_TAP_CFG                 ] },
  { ISM330_TAP_THS_6D                ,"ISM330_TAP_THS_6D            ",&rv[ISM330_TAP_THS_6D              ] },
  { ISM330_INT_DUR2                  ,"ISM330_INT_DUR2              ",&rv[ISM330_INT_DUR2                ] },
  { ISM330_WAKE_UP_THS               ,"ISM330_WAKE_UP_THS           ",&rv[ISM330_WAKE_UP_THS             ] },
  { ISM330_WAKE_UP_DUR               ,"ISM330_WAKE_UP_DUR           ",&rv[ISM330_WAKE_UP_DUR             ] },
  { ISM330_FREE_FALL                 ,"ISM330_FREE_FALL             ",&rv[ISM330_FREE_FALL               ] },
  { ISM330_MD1_CFG                   ,"ISM330_MD1_CFG               ",&rv[ISM330_MD1_CFG                 ] },
  { ISM330_MD2_CFG                   ,"ISM330_MD2_CFG               ",&rv[ISM330_MD2_CFG                 ] },
  { ISM330_MASTER_CMD_CODE           ,"ISM330_MASTER_CMD_CODE       ",&rv[ISM330_MASTER_CMD_CODE         ] },
  { ISM330_SENS_SYNC_SPI_ERROR_CODE  ,"ISM330_SENS_SYNC_SPI_ERR_CODE",&rv[ISM330_SENS_SYNC_SPI_ERROR_CODE] },
  { ISM330_OUT_MAG_RAW_X_L           ,"ISM330_OUT_MAG_RAW_X_L       ",&rv[ISM330_OUT_MAG_RAW_X_L         ] },
  { ISM330_OUT_MAG_RAW_X_H           ,"ISM330_OUT_MAG_RAW_X_H       ",&rv[ISM330_OUT_MAG_RAW_X_H         ] },
  { ISM330_OUT_MAG_RAW_Y_L           ,"ISM330_OUT_MAG_RAW_Y_L       ",&rv[ISM330_OUT_MAG_RAW_Y_L         ] },
  { ISM330_OUT_MAG_RAW_Y_H           ,"ISM330_OUT_MAG_RAW_Y_H       ",&rv[ISM330_OUT_MAG_RAW_Y_H         ] },
  { ISM330_OUT_MAG_RAW_Z_L           ,"ISM330_OUT_MAG_RAW_Z_L       ",&rv[ISM330_OUT_MAG_RAW_Z_L         ] },
  { ISM330_OUT_MAG_RAW_Z_H           ,"ISM330_OUT_MAG_RAW_Z_H       ",&rv[ISM330_OUT_MAG_RAW_Z_H         ] },
  { ISM330_INT_OIS                   ,"ISM330_INT_OIS               ",&rv[ISM330_INT_OIS                 ] },
  { ISM330_CTRL1_OIS                 ,"ISM330_CTRL1_OIS             ",&rv[ISM330_CTRL1_OIS               ] },
  { ISM330_CTRL2_OIS                 ,"ISM330_CTRL2_OIS             ",&rv[ISM330_CTRL2_OIS               ] },
  { ISM330_CTRL3_OIS                 ,"ISM330_CTRL3_OIS             ",&rv[ISM330_CTRL3_OIS               ] },
  { ISM330_X_OFS_USR                 ,"ISM330_X_OFS_USR             ",&rv[ISM330_X_OFS_USR               ] },
  { ISM330_Y_OFS_USR                 ,"ISM330_Y_OFS_USR             ",&rv[ISM330_Y_OFS_USR               ] },
  { ISM330_Z_OFS_USR                 ,"ISM330_Z_OFS_USR             ",&rv[ISM330_Z_OFS_USR               ] },
};

float              imu_sample_rate       = 104.0f;
float              g_accel_scale_factor  = 1.0f;
float              g_gyro_scale_factor   = 1.0f;

T_ism330_src_regs  g_ism330_src_regs;
T_ism330_src_regs  g_ism330_src_regs_prev;
T_ism330_src_regs  g_ism330_src_regs_hold;


/*-----------------------------------------------------------------------------------------------------


  \param void

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t ISM330_reg_count(void)
{
  return sizeof(ISM330_regs) / sizeof(ISM330_regs[0]);
}

/*-----------------------------------------------------------------------------------------------------


  \param indx

  \return T_da7217_reg_descr
-----------------------------------------------------------------------------------------------------*/
T_ISM330_reg_descr const* ISM330_get_reg_descr(uint8_t indx)
{
  if (indx >= ISM330_reg_count()) indx = 0;
  return &ISM330_regs[indx];
}

/*-----------------------------------------------------------------------------------------------------



  \return ssp_err_t
-----------------------------------------------------------------------------------------------------*/
ssp_err_t  ISM330_open(void)
{
  ssp_err_t           res = SSP_SUCCESS;
  if (ISM330_opened == 0)
  {
    res = g_sf_i2c_ISM330.p_api->open(g_sf_i2c_ISM330.p_ctrl, g_sf_i2c_ISM330.p_cfg);
    if (res == SSP_SUCCESS)
    {
      ISM330_opened = 1;
    }
  }
  return res;
}

/*-----------------------------------------------------------------------------------------------------


  \param void

  \return ssp_err_t
-----------------------------------------------------------------------------------------------------*/
ssp_err_t  ISM330_close(void)
{
  ISM330_opened = 0;
  return g_sf_i2c_ISM330.p_api->close(g_sf_i2c_ISM330.p_ctrl);
}

/*-----------------------------------------------------------------------------------------------------


  \param timeout

  \return ssp_err_t
-----------------------------------------------------------------------------------------------------*/
ssp_err_t  ISM330_lock(uint32_t timeout)
{
  ssp_err_t           res = SSP_SUCCESS;
  res = g_sf_i2c_ISM330.p_api->lockWait(g_sf_i2c_ISM330.p_ctrl, timeout);
  return res;
}

/*-----------------------------------------------------------------------------------------------------



  \return ssp_err_t
-----------------------------------------------------------------------------------------------------*/
ssp_err_t  ISM330_unlock(void)
{
  ssp_err_t           res = SSP_SUCCESS;
  res = g_sf_i2c_ISM330.p_api->unlock(g_sf_i2c_ISM330.p_ctrl);
  return res;
}


/*-----------------------------------------------------------------------------------------------------
  Запись буфера с данными

-----------------------------------------------------------------------------------------------------*/
ssp_err_t ISM330_write_buf(uint8_t addr, uint8_t *buf, uint32_t sz)
{
  ssp_err_t           res;


  if (ISM330_opened == 1)
  {
    do
    {
      buf[0] = addr;
      res = g_sf_i2c_ISM330.p_api->write(g_sf_i2c_ISM330.p_ctrl, buf, sz + 1, false , 500); // Записываем адрес
      if (res == SSP_ERR_IN_USE)
      {
        res = g_sf_i2c_ISM330.p_api->reset(g_sf_i2c_ISM330.p_ctrl, 500);
      }
    } while (res != SSP_SUCCESS);
    return res;
  }
  return SSP_ERR_NOT_OPEN;
}

/*-----------------------------------------------------------------------------------------------------


  \param addr
  \param buf
  \param sz

  \return ssp_err_t
-----------------------------------------------------------------------------------------------------*/
ssp_err_t ISM330_read_buf(uint8_t addr, uint8_t *buf, uint32_t sz)
{
  ssp_err_t           res;

  if (ISM330_opened == 1)
  {
    do
    {
      res = g_sf_i2c_ISM330.p_api->write(g_sf_i2c_ISM330.p_ctrl,&addr, 1, true , 500); // Записываем адрес
      if (res == SSP_SUCCESS)
      {
        res = g_sf_i2c_ISM330.p_api->read(g_sf_i2c_ISM330.p_ctrl, buf, sz, false , 500); // Записываем адрес
      }
      if (res == SSP_ERR_IN_USE)
      {
        res = g_sf_i2c_ISM330.p_api->reset(g_sf_i2c_ISM330.p_ctrl, 500);
      }
    } while (res != SSP_SUCCESS);
    return res;
  }
  return SSP_ERR_NOT_OPEN;
}


/*-----------------------------------------------------------------------------------------------------


  \param addr
  \param val

  \return ssp_err_t
-----------------------------------------------------------------------------------------------------*/
ssp_err_t ISM330_read_register(uint8_t addr, uint8_t *val)
{
  return ISM330_read_buf(addr,val,1);
}


/*-----------------------------------------------------------------------------------------------------


  \param addr
  \param val

  \return ssp_err_t
-----------------------------------------------------------------------------------------------------*/
ssp_err_t ISM330_write_register(uint8_t addr, uint8_t val)
{
  uint8_t buf[2];
  buf[1] = val;
  return ISM330_write_buf(addr,buf,1);
}

/*-----------------------------------------------------------------------------------------------------


  \param rate

  \return float
-----------------------------------------------------------------------------------------------------*/
void ISM330_set_sample_rate(uint8_t rate)
{
  switch (rate)
  {
  case IMU_OUTPUT_DATA_RATE_1666HZ :
    imu_sample_rate =  1666.0f;
    return;
  case IMU_OUTPUT_DATA_RATE_833HZ  :
    imu_sample_rate =  833.0f;
    return;
  case IMU_OUTPUT_DATA_RATE_416HZ  :
    imu_sample_rate =  416.0f;
    return;
  case IMU_OUTPUT_DATA_RATE_208HZ  :
    imu_sample_rate =  208.0f;
    return;
  case IMU_OUTPUT_DATA_RATE_104HZ  :
    imu_sample_rate =  104.0f;
    return;
  case IMU_OUTPUT_DATA_RATE_52  :
    imu_sample_rate =   52.0f;
    return;
  case IMU_OUTPUT_DATA_RATE_26  :
    imu_sample_rate =   26.0f;
    return;
  case IMU_OUTPUT_DATA_RATE_12_5  :
    imu_sample_rate =   12.5f;
    return;
  case IMU_OUTPUT_DATA_RATE_1_6  :
    imu_sample_rate =   1.6f;
    return;
  default:
    imu_sample_rate =   104.0f;
    return;
  }
}

/*-----------------------------------------------------------------------------------------------------


  \param void

  \return float
-----------------------------------------------------------------------------------------------------*/
float ISM330_get_sample_rate(void)
{
  return  imu_sample_rate;
}

/*-----------------------------------------------------------------------------------------------------

  \param scale
  \param rate

  \return ssp_err_t
-----------------------------------------------------------------------------------------------------*/
ssp_err_t ISM330_Set_accelerometer_scale_rate(uint8_t scale, uint8_t rate)
{
  ssp_err_t err = SSP_SUCCESS;
  uint8_t value;

  value = CTRL1_XL_LPF1_BW_CTRL8 | CTRL1_XL_BW0_XL_1_5KHZ;
  switch (scale)
  {
  case ACCELEROMETER_SCALE_2G  :
    value |= CTRL1_XL_FS_XL_2G;
    g_accel_scale_factor = 2.0f / 32768.0f;
    break;
  case ACCELEROMETER_SCALE_4G  :
    value |= CTRL1_XL_FS_XL_4G;
    g_accel_scale_factor = 4.0f / 32768.0f;
    break;
  case ACCELEROMETER_SCALE_8G  :
    value |= LSHIFT(3,2);
    g_accel_scale_factor = 8.0f / 32768.0f;
    break;
  case ACCELEROMETER_SCALE_16G :
    value |= CTRL1_XL_FS_XL_16G;
    g_accel_scale_factor = 16.0f / 32768.0f;
    break;
  default:
    value |= CTRL1_XL_FS_XL_2G;
    g_accel_scale_factor = 2.0f / 32768.0f;
    break;
  }

  switch (rate)
  {
  case IMU_OUTPUT_DATA_RATE_1666HZ :
    value |= CTRL1_XL_ODR_XL_1_66KHZ;
    break;
  case IMU_OUTPUT_DATA_RATE_833HZ  :
    value |= CTRL1_XL_ODR_XL_833HZ;
    break;
  case IMU_OUTPUT_DATA_RATE_416HZ  :
    value |= CTRL1_XL_ODR_XL_416HZ;
    break;
  case IMU_OUTPUT_DATA_RATE_208HZ  :
    value |= CTRL1_XL_ODR_XL_208HZ;
    break;
  case IMU_OUTPUT_DATA_RATE_104HZ  :
    value |= CTRL1_XL_ODR_XL_104HZ;
    break;
  case IMU_OUTPUT_DATA_RATE_52  :
    value |= CTRL1_XL_ODR_XL_52HZ;
    break;
  case IMU_OUTPUT_DATA_RATE_26  :
    value |= CTRL1_XL_ODR_XL_26HZ;
    break;
  case IMU_OUTPUT_DATA_RATE_12_5  :
    value |= CTRL1_XL_ODR_XL_12_5HZ;
    break;
  case IMU_OUTPUT_DATA_RATE_1_6  :
    value |= CTRL1_XL_ODR_XL_1_6HZ;
    break;
  default:
    value |= CTRL1_XL_ODR_XL_104HZ;
    break;
  }

  err |= ISM330_write_register(ISM330_CTRL1_XL,value);

  return err;
}

/*-----------------------------------------------------------------------------------------------------

  \param scale
  \param rate

  \return ssp_err_t
-----------------------------------------------------------------------------------------------------*/
ssp_err_t ISM330_Set_gyro_scale_rate(uint8_t scale, uint8_t rate)
{
  ssp_err_t err = SSP_SUCCESS;
  uint8_t value;


  value = CTRL2_G_FS_125_DISABLED;
  switch (scale)
  {
  case GYROSCOPE_SCALE_250DPS  :
    value |= CTRL2_G_FS_G_250DPS;
    g_gyro_scale_factor = 250.0f / 32768.0f;
    break;
  case GYROSCOPE_SCALE_500DPS  :
    value |= CTRL2_G_FS_G_500DPS;
    g_gyro_scale_factor = 500.0f / 32768.0f;
    break;
  case GYROSCOPE_SCALE_1000DPS  :
    value |= CTRL2_G_FS_G_1000DPS;
    g_gyro_scale_factor = 1000.0f / 32768.0f;
    break;
  case GYROSCOPE_SCALE_2000DPS :
    value |= CTRL2_G_FS_G_2000DPS;
    g_gyro_scale_factor = 2000.0f / 32768.0f;
    break;
  }

  switch (rate)
  {
  case IMU_OUTPUT_DATA_RATE_1666HZ :
    value |= CTRL2_G_ODR_G_1_66KHZ;
    break;
  case IMU_OUTPUT_DATA_RATE_833HZ  :
    value |= CTRL2_G_ODR_G_833HZ;
    break;
  case IMU_OUTPUT_DATA_RATE_416HZ  :
    value |= CTRL2_G_ODR_G_416HZ;
    break;
  case IMU_OUTPUT_DATA_RATE_208HZ  :
    value |= CTRL2_G_ODR_G_208HZ;
    break;
  case IMU_OUTPUT_DATA_RATE_104HZ  :
    value |= CTRL2_G_ODR_G_104HZ;
    break;
  case IMU_OUTPUT_DATA_RATE_52  :
    value |= CTRL2_G_ODR_G_52HZ;
    break;
  case IMU_OUTPUT_DATA_RATE_26  :
    value |= CTRL2_G_ODR_G_26HZ;
    break;
  case IMU_OUTPUT_DATA_RATE_12_5  :
    value |= CTRL2_G_ODR_G_12_5HZ;
    break;
  case IMU_OUTPUT_DATA_RATE_1_6  :
    value |= CTRL2_G_ODR_G_POWER_DOWN;
    break;
  default:
    value |= CTRL2_G_ODR_G_104HZ;
    break;
  }

  err |= ISM330_write_register(ISM330_CTRL2_G,value); // CTRL2_G

  return err;
}

/*-----------------------------------------------------------------------------------------------------


  \param void

  \return ssp_err_t
-----------------------------------------------------------------------------------------------------*/
ssp_err_t ISM330_enable_source_register_rounding(void)
{
  ssp_err_t  err = SSP_SUCCESS;
  uint8_t    reg8;

  err  |= ISM330_read_register(ISM330_CTRL7_G,&reg8);
  reg8 |= BIT(2);
  err  |= ISM330_write_register(ISM330_CTRL7_G, reg8);
  return err;
}

/*-----------------------------------------------------------------------------------------------------

  \param void

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
ssp_err_t ISM330_enable_low_power_accelerometer(void)
{
  ssp_err_t  err = SSP_SUCCESS;
  uint8_t    reg8;

  err  |= ISM330_read_register(ISM330_CTRL6_C,&reg8);
  reg8 |= BIT(4);
  err  |= ISM330_write_register(ISM330_CTRL6_C, reg8);
  return err;
}

/*-----------------------------------------------------------------------------------------------------

  \param void

  \return ssp_err_t
-----------------------------------------------------------------------------------------------------*/
ssp_err_t ISM330_config_xyz_tap_recognition(uint8_t en_double_tap)
{
  ssp_err_t  err = SSP_SUCCESS;
  uint8_t    tap_cfg;
  uint8_t    tap_ths_6d;
  uint8_t    int_dur2;
  uint8_t    wake_up_ths;

  err  |= ISM330_read_register(ISM330_TAP_CFG,&tap_cfg);
  tap_cfg |= BIT(3) | BIT(2) | BIT(1); // Разрешаем определение тапа по всем 3 осям
  err  |= ISM330_write_register(ISM330_TAP_CFG, tap_cfg);

  err  |= ISM330_read_register(ISM330_TAP_THS_6D,&tap_ths_6d);
  tap_ths_6d &= ~0x1F;
  tap_ths_6d |= (ivar.imu_tap_threshold & 0x1F);
  err  |= ISM330_write_register(ISM330_TAP_THS_6D, tap_ths_6d);


  int_dur2 = 0;
  float odr_xl = 1000.0f / ISM330_get_sample_rate();
  uint32_t shock_value =  (uint32_t)(ivar.imu_tap_shock_window / odr_xl);
  if (shock_value <= 4)
  {
    int_dur2 |= 0x00;    // 00: 4 * ODR_XL time
  }
  else if (shock_value <= 8)
  {
    int_dur2 |= 0x01;     // 01: 8 * ODR_XL time
  }
  else if (shock_value <= 16)
  {
    int_dur2 |= 0x02;    // 10: 16 * ODR_XL time
  }
  else
  {
    int_dur2 |= 0x03;     // 11: 24 * ODR_XL time
  }
  uint32_t quiet_value =  (uint32_t)(ivar.imu_tap_quiet_time / odr_xl);
  if (quiet_value <= 2)
  {
    int_dur2 |= 0x00;    // 00: 2 * ODR_XL time
  }
  else if (quiet_value <= 4)
  {
    int_dur2 |= 0x04;     // 01: 4 * ODR_XL time
  }
  else if (quiet_value <= 8)
  {
    int_dur2 |= 0x08;    // 10: 8 * ODR_XL time
  }
  else
  {
    int_dur2 |= 0x0C;     // 11: 12 * ODR_XL time
  }
  uint32_t gap_value =  (uint32_t)(ivar.imu_max_tap_time_gap / odr_xl);
  if (gap_value <= 16)
  {
    int_dur2 |= 0x00;
  }
  else
  {
    if ((gap_value / 32) > 0xF)
    {
      int_dur2 |= 0xF0;
    }
    else
    {
      int_dur2 |= ((gap_value / 32) << 4);
    }
  }
  err  |= ISM330_write_register(ISM330_INT_DUR2, int_dur2);

  err  |= ISM330_read_register(ISM330_WAKE_UP_THS,&wake_up_ths);
  if (en_double_tap)
  {
    wake_up_ths |= BIT(7);
  }
  else
  {
    wake_up_ths &= ~BIT(7);
  }
  err  |= ISM330_write_register(ISM330_WAKE_UP_THS, wake_up_ths);

  return err;
}

/*-----------------------------------------------------------------------------------------------------


  \param void

  \return ssp_err_t
-----------------------------------------------------------------------------------------------------*/
ssp_err_t ISM330_config_tilt_recognition(void)
{
  ssp_err_t  err = SSP_SUCCESS;
  uint8_t    ctrl10_c;

  err  |= ISM330_read_register(ISM330_CTRL10_C,&ctrl10_c);
  ctrl10_c |= (BIT(2) | BIT(3));
  err  |= ISM330_write_register(ISM330_CTRL10_C, ctrl10_c);

  return err;
}

/*-----------------------------------------------------------------------------------------------------


  \param void

  \return ssp_err_t
-----------------------------------------------------------------------------------------------------*/
ssp_err_t  ISM330_config_free_fall_recognition(void)
{
  ssp_err_t  err = SSP_SUCCESS;
  uint8_t    free_fall;
  uint8_t    wake_up_dur;

  float odr_xl = 1000.0f / ISM330_get_sample_rate();
  uint32_t n = (uint32_t)roundf(ivar.imu_free_fall_duration / odr_xl);
  if (n > 0x3F) n = 0x3F;

  err  |= ISM330_read_register(ISM330_FREE_FALL,&free_fall);
  free_fall = ivar.imu_free_fall_threshold & 0x07;
  free_fall |= (n << 3) & 0xF8;
  err  |= ISM330_write_register(ISM330_FREE_FALL, free_fall);

  err  |= ISM330_read_register(ISM330_WAKE_UP_DUR,&wake_up_dur);
  wake_up_dur &= ~0x80;
  wake_up_dur |= (n << 3) & 0x80;
  err  |= ISM330_write_register(ISM330_WAKE_UP_DUR, wake_up_dur);

  return err;
}

/*-----------------------------------------------------------------------------------------------------


  \param void

  \return ssp_err_t
-----------------------------------------------------------------------------------------------------*/
ssp_err_t ISM330_config_wake_up_recognition(void)
{
  ssp_err_t  err = SSP_SUCCESS;
  uint8_t    wake_up_ths;
  uint8_t    wake_up_dur;

  float odr_xl = 1000.0f / ISM330_get_sample_rate();
  uint32_t n = (uint32_t)roundf(ivar.imu_wakeup_duration / odr_xl);
  if (n > 3) n = 3;

  err  |= ISM330_read_register(ISM330_WAKE_UP_THS,&wake_up_ths);
  wake_up_ths &= ~0x3F;
  wake_up_ths |= (ivar.imu_wakeup_threshold & 0x3F);
  err  |= ISM330_write_register(ISM330_WAKE_UP_THS, wake_up_ths);

  err  |= ISM330_read_register(ISM330_WAKE_UP_DUR,&wake_up_dur);
  wake_up_dur &= ~0x60;
  wake_up_dur |= n << 5;
  err  |= ISM330_write_register(ISM330_WAKE_UP_DUR, wake_up_dur);

  return err;
}

/*-----------------------------------------------------------------------------------------------------



  \return ssp_err_t
-----------------------------------------------------------------------------------------------------*/
ssp_err_t ISM330_config_6d_recognition(void)
{
  ssp_err_t  err = SSP_SUCCESS;
  uint8_t    tap_ths_6d;

  err  |= ISM330_read_register(ISM330_TAP_THS_6D,&tap_ths_6d);
  tap_ths_6d &= ~0x60;
  tap_ths_6d |= ((ivar.imu_6d_threshold & 0x03) << 5);
  err  |= ISM330_write_register(ISM330_TAP_THS_6D, tap_ths_6d);

  return err;
}
/*-----------------------------------------------------------------------------------------------------


  \param enable

  \return ssp_err_t
-----------------------------------------------------------------------------------------------------*/
ssp_err_t ISM330_activity_interrupts_control(uint8_t enable)
{
  ssp_err_t  err = SSP_SUCCESS;
  uint8_t    tap_cfg;

  if (enable == 0)
  {
    err  |= ISM330_read_register(ISM330_TAP_CFG,&tap_cfg);
    tap_cfg &= ~BIT(7); // Запрещаем прерывания
    err  |= ISM330_write_register(ISM330_TAP_CFG, tap_cfg);
  }
  else
  {
    err  |= ISM330_read_register(ISM330_TAP_CFG,&tap_cfg);
    tap_cfg |= BIT(7); // Разрешаем прерывания
    err  |= ISM330_write_register(ISM330_TAP_CFG, tap_cfg);
  }
  return err;
}


/*-----------------------------------------------------------------------------------------------------

  \param int_mask

  \return ssp_err_t
-----------------------------------------------------------------------------------------------------*/
ssp_err_t ISM330_INT2_activity_interrupt_control(uint8_t int_mask)
{
  ssp_err_t  err = SSP_SUCCESS;
  err |= ISM330_write_register(ISM330_MD2_CFG, int_mask);
  return err;
}


