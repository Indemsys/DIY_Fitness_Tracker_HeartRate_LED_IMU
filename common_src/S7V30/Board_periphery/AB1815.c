// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 2020.04.30
// 9:03:01
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include   "App.h"


const  T_AB1815_reg_descr AB1815_regs[] =
{
  {  AB1815_HUNDREDTHS          , "HUNDREDTHS       "  },
  {  AB1815_SECONDS             , "SECONDS          "  },
  {  AB1815_MINUTES             , "MINUTES          "  },
  {  AB1815_HOURS               , "HOURS            "  },
  {  AB1815_DATE                , "DATE             "  },
  {  AB1815_MONTHS              , "MONTHS           "  },
  {  AB1815_YEARS               , "YEARS            "  },
  {  AB1815_WEEKDAYS            , "WEEKDAYS         "  },
  {  AB1815_HUNDREDTHS_ALARM    , "HUNDREDTHS_ALARM "  },
  {  AB1815_SECONDS_ALARM       , "SECONDS_ALARM    "  },
  {  AB1815_MINUTES_ALARM       , "MINUTES_ALARM    "  },
  {  AB1815_HOURS_ALARM_24      , "HOURS_ALARM_24   "  },
  {  AB1815_HOURS_ALARM_12      , "HOURS_ALARM_12   "  },
  {  AB1815_DATE_ALARM          , "DATE_ALARM       "  },
  {  AB1815_MONTHS_ALARM        , "MONTHS_ALARM     "  },
  {  AB1815_WEEKDAYS_ALARM      , "WEEKDAYS_ALARM   "  },
  {  AB1815_STATUS              , "STATUS           "  },
  {  AB1815_CONTROL1            , "CONTROL1         "  },
  {  AB1815_CONTROL2            , "CONTROL2         "  },
  {  AB1815_INTMASK             , "INTMASK          "  },
  {  AB1815_SQW                 , "SQW              "  },
  {  AB1815_CAL_XT              , "CAL_XT           "  },
  {  AB1815_CAL_RC_             , "CAL_RC_          "  },
  {  AB1815_CAL_RC_LOW          , "CAL_RC_LOW       "  },
  {  AB1815_SLEEP_CONTROL       , "SLEEP_CONTROL    "  },
  {  AB1815_TIMER_CONTROL       , "TIMER_CONTROL    "  },
  {  AB1815_TIMER               , "TIMER            "  },
  {  AB1815_TIMER_INITIAL       , "TIMER_INITIAL    "  },
  {  AB1815_WDT                 , "WDT              "  },
  {  AB1815_OSC_CONTROL         , "OSC_CONTROL      "  },
  {  AB1815_OSC_STATUS          , "OSC_STATUS       "  },
  {  AB1815_RESERVED1           , "RESERVED1        "  },
  {  AB1815_CONFIGURATION_KEY   , "CONFIGURATION_KEY"  },
  {  AB1815_TRICKLE             , "TRICKLE          "  },
  {  AB1815_BREF_CONTROL        , "BREF_CONTROL     "  },
  {  AB1815_RESERVED2           , "RESERVED2        "  },
  {  AB1815_RESERVED3           , "RESERVED3        "  },
  {  AB1815_RESERVED4           , "RESERVED4        "  },
  {  AB1815_RESERVED5           , "RESERVED5        "  },
  {  AB1815_AFCTRL              , "AFCTRL           "  },
  {  AB1815_BATMODE_I_O         , "BATMODE_I_O      "  },
  {  AB1815_ID0                 , "ID0              "  },
  {  AB1815_ID1                 , "ID1              "  },
  {  AB1815_ID2                 , "ID2              "  },
  {  AB1815_ID3                 , "ID3              "  },
  {  AB1815_ID4                 , "ID4              "  },
  {  AB1815_ID5                 , "ID5              "  },
  {  AB1815_ID6                 , "ID6              "  },
  {  AB1815_ASTAT               , "ASTAT            "  },
  {  AB1815_OCTRL               , "OCTRL            "  },
  {  AB1815_EXTENSION_ADDRESS   , "EXTENSION_ADDRESS"  },
  {  AB1815_RAM_NORMAL          , "RAM_NORMAL       "  },
  {  AB1815_RAM_ALTERNATE       , "RAM_ALTERNATE    "  },
};

#define AB1815_REGISTERS_COUNT  (sizeof(AB1815_regs)/sizeof(AB1815_regs[0]))

static T_AB1815_init_res AB1815_init_res;


static uint32_t  _AB1815_open(void);
static uint32_t  _AB1815_read_byte(uint8_t addr, uint8_t *val);
static uint32_t  _AB1815_write_byte(uint8_t addr, uint8_t val);

#define AB1815_VERIFY_RESULT( x )  { uint32_t vres; vres = (x); AB1815_init_res.err_code = (uint32_t)vres; if ( vres != RES_OK ) { goto EXIT_ON_ERROR; } }

/*-----------------------------------------------------------------------------------------------------


  \param void

  \return T_AB1815_init_res*
-----------------------------------------------------------------------------------------------------*/
T_AB1815_init_res* AB1815_get_init_res(void)
{
  return &AB1815_init_res;
}

/*-----------------------------------------------------------------------------------------------------


  \param addr

  \return char const*
-----------------------------------------------------------------------------------------------------*/
char  const* AB1815_get_register_name_by_addr(uint8_t addr)
{
  for (uint32_t i = 0; i < AB1815_REGISTERS_COUNT; i++)
  {
    if (addr == AB1815_regs[i].reg_addr)
    {
      return AB1815_regs[i].reg_name;
    }
  }
  return  "register don't exist";
}
/*-----------------------------------------------------------------------------------------------------


  \param void

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t AB1815_get_registers_count(void)
{
  return AB1815_REGISTERS_COUNT;
}

/*-----------------------------------------------------------------------------------------------------

  \param indx

  \return T_AB1815_reg_descr*
-----------------------------------------------------------------------------------------------------*/
const T_AB1815_reg_descr* AB1815_get_register_descr(uint32_t indx)
{
  if (indx >= AB1815_REGISTERS_COUNT)
  {
    return &AB1815_regs[0];
  }
  return &AB1815_regs[indx];
}

/*-----------------------------------------------------------------------------------------------------


  \param v
-----------------------------------------------------------------------------------------------------*/
static void AB1815_Set_CS(uint8_t v)
{
  R_PFS->P414PFS_b.PODR = v;
}

/*-----------------------------------------------------------------------------------------------------
  Открытие коммуникационного канала с чипом AB1815

  Функция использует глобальный объект  g_sf_i2c_ab1815
  Быть внимательным при вызове из разных задач!!!


  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
static uint32_t  _AB1815_open(void)
{
  SCI8_SPI_init();
  return RES_OK;
}



/*-----------------------------------------------------------------------------------------------------


  \param addr
  \param val

  \return ssp_err_t
-----------------------------------------------------------------------------------------------------*/
static uint32_t _AB1815_read_byte(uint8_t addr, uint8_t *val)
{
  uint32_t          res;
  uint8_t           buf[2];
  buf[0] = addr;
  buf[1] = 0;

  AB1815_Set_CS(0);
  res = SCI8_SPI_tx_rx_buf(buf,buf, 2);
  AB1815_Set_CS(1);

  *val = buf[1];
  return res;
}

/*-----------------------------------------------------------------------------------------------------


  \param addr
  \param val

  \return ssp_err_t
-----------------------------------------------------------------------------------------------------*/
static uint32_t _AB1815_write_byte(uint8_t addr, uint8_t val)
{
  uint32_t res;
  uint8_t  buf[2];
  buf[0] = addr | 0x80;
  buf[1] = val;

  AB1815_Set_CS(0);
  res = SCI8_SPI_tx_rx_buf(buf, 0, 2);
  AB1815_Set_CS(1);

  return res;
}

/*-----------------------------------------------------------------------------------------------------


  \param void

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t AB1815_reg_count(void)
{
  return sizeof(AB1815_regs) / sizeof(AB1815_regs[0]);
}

/*-----------------------------------------------------------------------------------------------------


  \param indx

  \return T_AB1815_reg_descr*
-----------------------------------------------------------------------------------------------------*/
T_AB1815_reg_descr const* AB1815_get_reg_descr(uint8_t indx)
{
  if (indx >= AB1815_reg_count()) indx = 0;
  return &AB1815_regs[indx];
}

/*-----------------------------------------------------------------------------------------------------
   Чтение значения времени из чипа часов реального времени. Резальтат сохраняется в формате структуры языка C

   Значения полей стуктуры rtc_time_t

   tm_sec   int   seconds after the minute  0-61*
   tm_min   int   minutes after the hour  0-59
   tm_hour  int   hours since midnight  0-23
   tm_mday  int   day of the month  1-31
   tm_mon   int   months since January  0-11        В чипе AB1815 счет номеров месяцев начинается с 1 и до 12
   tm_year  int   years since 1900                  В чипе AB1815 счет номера года начинается с 0 и до 99
   tm_wday  int   days since Sunday 0-6
   tm_yday  int   days since January 1  0-365
   tm_isdst int   Daylight Saving Time flag


  \param p_time

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t AB1815_read_date_time(rtc_time_t  *p_time)
{
  uint8_t b;
  SCI8_SPI_mutex_get(TX_WAIT_FOREVER);
  SET_AB1815_MODE;


  AB1815_VERIFY_RESULT(_AB1815_read_byte(AB1815_SECONDS ,&b));
  p_time->tm_sec   = BCD2ToBYTE(b & 0x7F);
  AB1815_VERIFY_RESULT(_AB1815_read_byte(AB1815_MINUTES ,&b));
  p_time->tm_min   = BCD2ToBYTE(b & 0x7F);
  AB1815_VERIFY_RESULT(_AB1815_read_byte(AB1815_HOURS   ,&b));
  p_time->tm_hour  = BCD2ToBYTE(b & 0x3F);
  AB1815_VERIFY_RESULT(_AB1815_read_byte(AB1815_DATE    ,&b));
  p_time->tm_mday  = BCD2ToBYTE(b & 0x3F);
  AB1815_VERIFY_RESULT(_AB1815_read_byte(AB1815_MONTHS  ,&b));
  p_time->tm_mon   = BCD2ToBYTE(b & 0x1F) - 1;
  AB1815_VERIFY_RESULT(_AB1815_read_byte(AB1815_YEARS   ,&b));
  p_time->tm_year  = BCD2ToBYTE(b) + 2000 - 1900;
  AB1815_VERIFY_RESULT(_AB1815_read_byte(AB1815_WEEKDAYS,&b));
  p_time->tm_wday  = BCD2ToBYTE(b & 0x07);
  p_time->tm_isdst = 1;

  mktime(p_time); // Функция исправит структуру времени если в ней сесть ошибки, скорректирует номер дня недели и добавит номер дня года

  SCI8_SPI_mutex_put();
  return RES_OK;
EXIT_ON_ERROR:
  SCI8_SPI_mutex_put();
  return RES_ERROR;
}


/*-----------------------------------------------------------------------------------------------------
  Установка времени в чипе AB1815
   Аргумент передается в формате структуры языка C

  \param p_rt_time
-----------------------------------------------------------------------------------------------------*/
uint32_t AB1815_write_date_time(rtc_time_t  *p_time)
{
  uint8_t  b;
  SCI8_SPI_mutex_get(TX_WAIT_FOREVER);

  SET_AB1815_MODE;
  b = BYTEToBCD2(p_time->tm_sec);
  AB1815_VERIFY_RESULT(_AB1815_write_byte(AB1815_SECONDS, b));
  b = BYTEToBCD2(p_time->tm_min);
  AB1815_VERIFY_RESULT(_AB1815_write_byte(AB1815_MINUTES, b));
  b = BYTEToBCD2(p_time->tm_hour);
  AB1815_VERIFY_RESULT(_AB1815_write_byte(AB1815_HOURS, b));
  b = BYTEToBCD2(p_time->tm_mday);
  AB1815_VERIFY_RESULT(_AB1815_write_byte(AB1815_DATE, b));
  b = BYTEToBCD2(p_time->tm_mon + 1);
  AB1815_VERIFY_RESULT(_AB1815_write_byte(AB1815_MONTHS, b));
  b = BYTEToBCD2(p_time->tm_year + 1900 - 2000);
  AB1815_VERIFY_RESULT(_AB1815_write_byte(AB1815_YEARS, b));
  b = BYTEToBCD2(p_time->tm_wday);
  AB1815_VERIFY_RESULT(_AB1815_write_byte(AB1815_WEEKDAYS, b));

  SCI8_SPI_mutex_put();
  return RES_OK;
EXIT_ON_ERROR:
  SCI8_SPI_mutex_put();
  return RES_ERROR;
}

/*-----------------------------------------------------------------------------------------------------
  Запись в чип времени пробуждения
  Аргумент передается в формате структуры языка C

  \param p_time

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t AB1815_write_alarm_date_time(rtc_time_t  *p_time)
{
  uint8_t   b;
  uint8_t   reg;

  SCI8_SPI_mutex_get(TX_WAIT_FOREVER);
  SET_AB1815_MODE;


  // Читаем регистр STATUS и таким образом сбрасываем его флаги. Для такой функциональности требуется чтобы флаг ARST был установлен в 1
  reg = 0;
  AB1815_VERIFY_RESULT(_AB1815_read_byte(AB1815_STATUS,&reg)); // Сбрасываем биты статуса чтением, чтобы дать возможность возникнуть новому прерыванию

  b = 0; // Доли секунды
  AB1815_VERIFY_RESULT(_AB1815_write_byte(AB1815_HUNDREDTHS, b));
  b = BYTEToBCD2(p_time->tm_sec);
  AB1815_VERIFY_RESULT(_AB1815_write_byte(AB1815_SECONDS, b));
  b = BYTEToBCD2(p_time->tm_min);
  AB1815_VERIFY_RESULT(_AB1815_write_byte(AB1815_MINUTES, b));
  b = BYTEToBCD2(p_time->tm_hour);
  AB1815_VERIFY_RESULT(_AB1815_write_byte(AB1815_HOURS, b));
  b = BYTEToBCD2(p_time->tm_mday);
  AB1815_VERIFY_RESULT(_AB1815_write_byte(AB1815_DATE, b));
  b = BYTEToBCD2(p_time->tm_mon + 1);
  AB1815_VERIFY_RESULT(_AB1815_write_byte(AB1815_MONTHS, b));
  b = BYTEToBCD2(p_time->tm_year + 1900 - 2000);
  AB1815_VERIFY_RESULT(_AB1815_write_byte(AB1815_YEARS, b));
  b = BYTEToBCD2(p_time->tm_wday);
  AB1815_VERIFY_RESULT(_AB1815_write_byte(AB1815_WEEKDAYS, b));

  //
  reg = 0
        + LSHIFT(0, 7) // TE   | Timer Enable. When 1, the Countdown Timer will count down.
        + LSHIFT(0, 6) // TM   | Timer Interrupt Mode.
        + LSHIFT(0, 5) // TRPT | Along with TM, this controls the repeat function of the Countdown Timer.
        + LSHIFT(0, 4) // RPT  | These bits enable the Alarm Interrupt repeat function
        + LSHIFT(0, 3) // RPT  | 1 -> Hundredths, seconds, minutes, hours, date and month match (once per year)
        + LSHIFT(1, 2) // RPT  |
        + LSHIFT(0, 1) // TFS  | Select the clock frequency and interrupt pulse width of the Countdown Timer
        + LSHIFT(0, 0) // TFS  |
  ;

  AB1815_VERIFY_RESULT(_AB1815_write_byte(AB1815_TIMER_CONTROL,reg));

  SCI8_SPI_mutex_put();
  return RES_OK;
EXIT_ON_ERROR:
  SCI8_SPI_mutex_put();
  return RES_ERROR;
}


/*-----------------------------------------------------------------------------------------------------



  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t AB1815_read_alarm_flag(uint8_t *p_flag)
{
  uint8_t   reg;

  SCI8_SPI_mutex_get(TX_WAIT_FOREVER);
  SET_AB1815_MODE;


  AB1815_VERIFY_RESULT(_AB1815_read_byte(AB1815_STATUS,&reg));
  *p_flag = (reg >> 2) & 1;

  SCI8_SPI_mutex_put();
  return RES_OK;
EXIT_ON_ERROR:
  SCI8_SPI_mutex_put();
  return RES_ERROR;
}

/*-----------------------------------------------------------------------------------------------------


  \param reg_addr
  \param p_reg

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t AB1815_read_register(uint8_t reg_addr, uint8_t *p_reg)
{
  uint8_t   reg;

  SCI8_SPI_mutex_get(TX_WAIT_FOREVER);
  SET_AB1815_MODE;

  AB1815_VERIFY_RESULT(_AB1815_read_byte(reg_addr,&reg));

  if (p_reg) *p_reg = reg;

  SCI8_SPI_mutex_put();
  return RES_OK;
EXIT_ON_ERROR:
  SCI8_SPI_mutex_put();
  return RES_ERROR;
}

/*-----------------------------------------------------------------------------------------------------


  \param reg_addr
  \param reg

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t AB1815_write_register(uint8_t reg_addr, uint8_t reg)
{
  SCI8_SPI_mutex_get(TX_WAIT_FOREVER);
  SET_AB1815_MODE;

  AB1815_VERIFY_RESULT(_AB1815_write_byte(reg_addr,reg));

  SCI8_SPI_mutex_put();
  return RES_OK;
EXIT_ON_ERROR:
  SCI8_SPI_mutex_put();
  return RES_ERROR;
}


/*-----------------------------------------------------------------------------------------------------
  Чтение внутренней RAM чипа.
  Начальный адрес RAM для этой функции стартует c 0

  Функция использует пересылки по одному байту чтобы не занимать надолго шину по кторой идет обмен в внешними микросхемами АЦП

  \param void

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t AB1815_read_chip_RAM(uint8_t start_addr, uint8_t *buf, uint32_t sz)
{
  uint8_t xads      = 0;
  uint8_t prev_xads = 0xFF;
  uint8_t addr;

  for (uint32_t i = 0; i < sz; i++)
  {
    xads = ((i + start_addr)/ AB1815_STANDARD_RAM_SZ) & 0x03;
    if (xads != prev_xads)
    {
      if (AB1815_write_register(AB1815_EXTENSION_ADDRESS, xads) != RES_OK) return RES_ERROR;
    }
    addr = AB1815_RAM_NORMAL + ((start_addr + i) & 0x3F);

    if (AB1815_read_register(addr,&buf[i]) != RES_OK) return RES_ERROR;
    prev_xads = xads;
  }

  return RES_OK;
}

/*-----------------------------------------------------------------------------------------------------
  Запись во внутреннюю RAM чипа.
  Начальный адрес RAM для этой функции стартует c 0

  Функция использует пересылки по одному байту чтобы не занимать надолго шину по кторой идет обмен в внешними микросхемами АЦП


  \param start_addr
  \param buf
  \param sz

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t AB1815_write_chip_RAM(uint8_t start_addr, uint8_t *buf, uint32_t sz)
{
  uint8_t xads      = 0;
  uint8_t prev_xads = 0xFF;
  uint8_t addr;

  for (uint32_t i = 0; i < sz; i++)
  {
    xads = ((i + start_addr)/ AB1815_STANDARD_RAM_SZ) & 0x03;
    if (xads != prev_xads)
    {
      if (AB1815_write_register(AB1815_EXTENSION_ADDRESS, xads) != RES_OK) return RES_ERROR;
    }
    addr = AB1815_RAM_NORMAL + ((start_addr + i) & 0x3F);
    if (AB1815_write_register(addr, buf[i]) != RES_OK) return RES_ERROR;
    prev_xads = xads;
  }
  return RES_OK;
}

/*-----------------------------------------------------------------------------------------------------


  \param start_addr
  \param buf
  \param sz

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t AB1815_clear_chip_RAM(void)
{
  uint8_t xads      = 0;
  uint8_t prev_xads = 0xFF;
  uint8_t addr;

  for (uint32_t i = 0; i < AB1815_NVRAM_SZ; i++)
  {
    xads = (i/ AB1815_STANDARD_RAM_SZ) & 0x03;
    if (xads != prev_xads)
    {
      if (AB1815_write_register(AB1815_EXTENSION_ADDRESS, xads) != RES_OK) return RES_ERROR;
    }
    addr = AB1815_RAM_NORMAL + (i & 0x3F);
    if (AB1815_write_register(addr, 0xFF) != RES_OK) return RES_ERROR;
    prev_xads = xads;
  }
  return RES_OK;
}

/*-----------------------------------------------------------------------------------------------------
  Запись в биты XADS в чипе часов.
  Эти быты устанвливают старшие 8 и 7 биты адреса для доступа к внутренней RAM чипа

  \param xads

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t AB1815_write_xads(uint8_t xads)
{
   return AB1815_write_register(AB1815_EXTENSION_ADDRESS, xads);
}
/*-----------------------------------------------------------------------------------------------------


  \param p_id

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t AB1815_read_ID(uint16_t *p_id)
{
  uint8_t   reg;
  uint16_t  chip_id;

  SCI8_SPI_mutex_get(TX_WAIT_FOREVER);
  SET_AB1815_MODE;

  AB1815_VERIFY_RESULT(_AB1815_read_byte(AB1815_ID0,&reg));
  chip_id = reg << 8;
  AB1815_VERIFY_RESULT(_AB1815_read_byte(AB1815_ID1,&reg));
  chip_id |= reg;

  if (p_id) *p_id = chip_id;

  SCI8_SPI_mutex_put();
  return RES_OK;
EXIT_ON_ERROR:
  SCI8_SPI_mutex_put();
  return RES_ERROR;

}

/*-----------------------------------------------------------------------------------------------------
  Инициализация чипа AB1815 на старте программы

  \param void

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t AB1815_init(void)
{
  uint8_t   reg;
  uint8_t   id3;
  uint8_t   id4;
  uint8_t   id5;
  uint8_t   id6;
  uint8_t   b;



  if (_AB1815_open() != SSP_SUCCESS) return RES_ERROR;

  SCI8_SPI_mutex_get(TX_WAIT_FOREVER);
  SET_AB1815_MODE;

  // Читаем идентификатор чипа
  AB1815_VERIFY_RESULT(_AB1815_read_byte(AB1815_ID0,&b));
  AB1815_init_res.chip_id = b << 8;
  AB1815_VERIFY_RESULT(_AB1815_read_byte(AB1815_ID1,&b));
  AB1815_init_res.chip_id |= b;
  if (AB1815_init_res.chip_id != AB1815_CHIP_ID) goto EXIT_ON_ERROR;

  AB1815_VERIFY_RESULT(_AB1815_read_byte(AB1815_ID3,&id3));
  AB1815_VERIFY_RESULT(_AB1815_read_byte(AB1815_ID4,&id4));
  AB1815_VERIFY_RESULT(_AB1815_read_byte(AB1815_ID5,&id5));
  AB1815_VERIFY_RESULT(_AB1815_read_byte(AB1815_ID6,&id6));

  AB1815_init_res.rtc_idn = (((uint32_t)id4 & 0x7F) << 8) | (uint32_t)id5;
  AB1815_init_res.rtc_lotn = (uint32_t)id3 | (((uint32_t)id4 & 0x80) << 2) | (((uint32_t)id6 & 0x80) << 1);

  AB1815_VERIFY_RESULT(_AB1815_read_byte(AB1815_STATUS,&reg));
  AB1815_init_res.status = reg;
  AB1815_init_res.alarm_flag = (reg >> 2) & 1;
  reg = 0;
  AB1815_VERIFY_RESULT(_AB1815_write_byte(AB1815_STATUS,reg)); // Сбрасываем биты статуса

  AB1815_VERIFY_RESULT(_AB1815_read_byte(AB1815_OSC_STATUS,&reg));
  AB1815_init_res.osc_status = reg;

  //
  reg = 0
        + LSHIFT(0, 7) // XTCAL    | Extended Crystal Calibration. This field defines a value by which the Crystal Oscillator is adjusted to compensate for low capacitance crystals
        + LSHIFT(0, 6) // XTCAL    |
        + LSHIFT(0, 5) // LKO2     | Lock OUT2. If this bit is a 1, the OUTB register bit (see Section 7.3.2) cannot be set to 1
        + LSHIFT(0, 4) // OMODE    | (read only) – Oscillator Mode. This bit is a 1 if the RC Oscillator is selected to drive the internal clocks, and a 0 if the Crystal Oscillator is selected.
        + LSHIFT(0, 3) // RESERVED |
        + LSHIFT(0, 2) // RESERVED |
        + LSHIFT(0, 1) // OF       | Oscillator Failure. This bit is set on a power on reset, when both the system and battery voltages have dropped below acceptable levels.
        + LSHIFT(0, 0) // ACF      | Set when an Autocalibration Failure occurs
  ;
  AB1815_VERIFY_RESULT(_AB1815_write_byte(AB1815_OSC_STATUS,reg));


  // Выставить высокий уровень на выводе IRQ2 который подключен к входу QON чипа зарядника.
  // Это предотвращает отключение питания чипом зарядника
  reg = 0
        + LSHIFT(0, 7) // STOP  | When 1, stops the clocking system.
        + LSHIFT(0, 6) // 12/24 | When 0, the Hours register operates in 24 hour mode. When 1, the Hours register operates in 12 hour mode.
        + LSHIFT(1, 5) // OUTB  | A static value which may be driven on the nIRQ2 pin. The OUTB bit cannot be set to 1 if the LKO2 bit is 1.
        + LSHIFT(1, 4) // OUT   | A static value which may be driven on the FOUT/nIRQ pin.
        + LSHIFT(0, 3) // RSP   | Reset Polarity. When 1, the nRST pin is asserted high. When 0, the nRST pin is asserted low.
        + LSHIFT(1, 2) // ARST  | Auto reset enable. When 1, a read of the Status register will cause any interrupt bits (TIM, BL,ALM, WDT, XT1, XT2) to be cleared.
        + LSHIFT(0, 1) // PWR2  | When 1, the PSW/nIRQ2 pin is driven by an approximately 1 Ohm pull-down
        + LSHIFT(1, 0) // WRTC  | Write RTC. This bit must be set in order to write any of the Counter registers (Hundredths, Seconds, Minutes, Hours, Date, Months, Years or Weekdays)
  ;
  AB1815_VERIFY_RESULT(_AB1815_write_byte(AB1815_CONTROL1,reg));

  //
  reg = 0
        + LSHIFT(0, 7) // RESERVED |
        + LSHIFT(0, 6) // RESERVED |
        + LSHIFT(1, 5) // RESERVED |
        + LSHIFT(0, 4) // OUT2S    | 011 -> nAIRQ if AIE is set, else OUTB. Выводим сигнал прерывания по событию ALARM на пин IRQ2 (сигнал PWR_ON на схеме)
        + LSHIFT(1, 3) // OUT2S    |
        + LSHIFT(1, 2) // OUT2S    |
        + LSHIFT(1, 1) // OUT1S    | 11 -> nAIRQ if AIE is set, else OUT. Выводим сигнал прерывания по событию ALARM на пин IRQ (сигнал RTC_IRQ на схеме)
        + LSHIFT(1, 0) // OUT1S    |
  ;
  AB1815_VERIFY_RESULT(_AB1815_write_byte(AB1815_CONTROL2,reg));



  //
  reg = 0
        + LSHIFT(0, 7) // OSEL | When 1, request the RC Oscillator to generate a 128 Hz clock for the timer circuits.
        + LSHIFT(0, 6) // ACAL | Controls the automatic calibration function
        + LSHIFT(0, 5) // ACAL |
        + LSHIFT(0, 4) // AOS  | When 1, the oscillator will automatically switch to RC oscillator mode when the system is powered from the battery. When 0, no automatic switching occurs.
        + LSHIFT(1, 3) // FOS  | When 1, the oscillator will automatically switch to RC oscillator mode when an oscillator failure is detected. When 0, no automatic switching occurs.
        + LSHIFT(0, 2) // PWGT | When 1, the I/O interface will be disabled when the power switch is active and disabled
        + LSHIFT(0, 1) // OFIE | Oscillator Fail interrupt enable. When 1, an Oscillator Failure will generate an IRQ signal.
        + LSHIFT(0, 0) // ACIE | When 1, an Autocalibration Failure will generate an interrupt
  ;
  AB1815_VERIFY_RESULT(_AB1815_write_byte(AB1815_OSC_CONTROL,reg));


  // Разрешаем прерывание только от будильника
  reg = 0
        + LSHIFT(0, 7) // CEB  | 1: The CB bit will toggle when the Years register rolls over from 99 to 00.
        + LSHIFT(0, 6) // IM   | Interrupt Mode. 01: 1/8192 seconds for XT mode. 1/64 seconds for RC mode. 00: Level (static) for both XT mode and RC mode.
        + LSHIFT(0, 5) // IM   |
        + LSHIFT(0, 4) // BLIE | Battery Low Interrupt Enable
        + LSHIFT(0, 3) // TIE  | Timer Interrupt Enable.
        + LSHIFT(1, 2) // AIE  | Alarm Interrupt Enable.
        + LSHIFT(0, 1) // EX2E | XT2 Interrupt Enable. 1: The WDI input pin will generate the XT2 interrupt when the edge specified by EX2P occurs
        + LSHIFT(0, 0) // EX1E | XT1 Interrupt Enable. 1: The EXTI input pin will generate the XT1 interrupt when the edge specified by EX1P occurs
  ;
  AB1815_VERIFY_RESULT(_AB1815_write_byte(AB1815_INTMASK,reg));



  // Выводим внешний сигнал 32768 Гц на пин IRQ3
  reg = 0
        + LSHIFT(1, 7) // SQWE     | When 1, the square wave output is enabled. When 0, the square wave output is held at the value of OUT.
        + LSHIFT(0, 6) // RESERVED |
        + LSHIFT(1, 5) // RESERVED |
        + LSHIFT(0, 4) // SQFS     | 00001  -> 32.768 kHz
        + LSHIFT(0, 3) // SQFS     |
        + LSHIFT(0, 2) // SQFS     |
        + LSHIFT(0, 1) // SQFS     |
        + LSHIFT(1, 0) // SQFS     |
  ;
  AB1815_VERIFY_RESULT(_AB1815_write_byte(AB1815_SQW,reg));


  // Включить цепь заряда резервного аккумулятора


  reg = 0x9D; // Открываем доступ к записи регистра Trickle
  AB1815_VERIFY_RESULT(_AB1815_write_byte(AB1815_CONFIGURATION_KEY,reg));

  //
  reg = 0
        + LSHIFT(1, 7) // TCS   | A value of 1010 enables the trickle charge function. All other values disable the Trickle Charger.
        + LSHIFT(0, 6) // TCS   |
        + LSHIFT(1, 5) // TCS   |
        + LSHIFT(0, 4) // TCS   |
        + LSHIFT(0, 3) // DIODE | Diode Select. A value of 01 inserts a schottky diode into the trickle charge circuit, with a voltage drop of 0.3V.
        + LSHIFT(1, 2) // DIODE |
        + LSHIFT(0, 1) // ROUT  | Output Resistor. 01 -> 3 Kohm
        + LSHIFT(1, 0) // ROUT  |
  ;
  AB1815_VERIFY_RESULT(_AB1815_write_byte(AB1815_TRICKLE,reg));

  // Читаем регистр статуса чтобы сбросить флаги прерываний
  AB1815_VERIFY_RESULT(_AB1815_read_byte(AB1815_STATUS,&reg));

  SCI8_SPI_mutex_put();
  return RES_OK;
EXIT_ON_ERROR:

  SCI8_SPI_mutex_put();
  return RES_ERROR;
}

