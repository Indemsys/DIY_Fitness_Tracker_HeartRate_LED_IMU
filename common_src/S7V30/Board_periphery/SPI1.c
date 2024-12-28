// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 2023-04-11
// 11:50:40
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define     S7V30_spi1
#include   "App.h"

static void  Transfer_callback(transfer_callback_args_t *p_args);

extern transfer_info_t                DMA_CH6_transfer_info;
extern const transfer_instance_t      DMA_CH6_transfer_instance;


dmac_instance_ctrl_t DMA_CH6_control_block;

transfer_info_t DMA_CH6_transfer_info =
{
  .dest_addr_mode      = TRANSFER_ADDR_MODE_FIXED,
  .repeat_area         = TRANSFER_REPEAT_AREA_SOURCE,
  .irq                 = TRANSFER_IRQ_EACH,
  .chain_mode          = TRANSFER_CHAIN_MODE_DISABLED,
  .src_addr_mode       = TRANSFER_ADDR_MODE_INCREMENTED,
  .size                = TRANSFER_SIZE_2_BYTE,
  .mode                = TRANSFER_MODE_NORMAL,
  .p_dest              = (void *)&R_RSPI1->SPDR_HA,
  .p_src               = (void const *) NULL,
  .num_blocks          = 0,
  .length              = 0,
};

const transfer_on_dmac_cfg_t DMA_CH6_extended_configuration =
{
  .channel             = 6,
  .offset_byte         = 0,
};

const transfer_cfg_t DMA_CH6_configuration =
{
  .p_info              = &DMA_CH6_transfer_info,
  .activation_source   = ELC_EVENT_SPI1_TXI,
  .auto_enable         = false,
  .p_callback          = Transfer_callback,
  .p_context           = &DMA_CH6_transfer_instance,
  .irq_ipl             = (2),
  .p_extend            = &DMA_CH6_extended_configuration,
};

const transfer_instance_t DMA_CH6_transfer_instance =
{
  .p_ctrl        = &DMA_CH6_control_block,
  .p_cfg         = &DMA_CH6_configuration,
  .p_api         = &g_transfer_on_dmac
};


static uint32_t                spi1_initialized;
static IRQn_Type               spi1_rxi_int_num;
static IRQn_Type               spi1_txi_int_num;
static IRQn_Type               spi1_tei_int_num;
static IRQn_Type               spi1_eri_int_num;
static IRQn_Type               spi1_idle_int_num;


TX_EVENT_FLAGS_GROUP           spi1_flags;
static TX_MUTEX                spi1_mutex;

static uint8_t                 g_spbr =  2;  // Установка для содержимого регистра SPBR. Значение 2 по умолчанию будет означать чатотоц тактирования в 20 МГц

/*-----------------------------------------------------------------------------------------------------
  Не объявляем вектор события ELC_EVENT_SPI1_TXI чтобы не было конфликтов с DMA

  \param void
-----------------------------------------------------------------------------------------------------*/
void spi1_init_interrupts(void)
{
  spi1_rxi_int_num    = (IRQn_Type)Find_IRQ_number_by_evt(ELC_EVENT_SPI1_RXI);
  spi1_txi_int_num    = (IRQn_Type)Find_IRQ_number_by_evt(ELC_EVENT_SPI1_TXI);
  spi1_tei_int_num    = (IRQn_Type)Find_IRQ_number_by_evt(ELC_EVENT_SPI1_TEI);
  spi1_eri_int_num    = (IRQn_Type)Find_IRQ_number_by_evt(ELC_EVENT_SPI1_ERI);
  spi1_idle_int_num   = (IRQn_Type)Find_IRQ_number_by_evt(ELC_EVENT_SPI1_IDLE);

  NVIC_DisableIRQ(spi1_rxi_int_num);
  NVIC_DisableIRQ(spi1_tei_int_num);
  NVIC_DisableIRQ(spi1_eri_int_num);
  NVIC_DisableIRQ(spi1_idle_int_num);

  NVIC_ClearPendingIRQ(spi1_rxi_int_num);
  NVIC_ClearPendingIRQ(spi1_tei_int_num);
  NVIC_ClearPendingIRQ(spi1_eri_int_num);
  NVIC_ClearPendingIRQ(spi1_idle_int_num);

  NVIC_SetPriority(spi1_rxi_int_num ,  SPI1_PRIO);
  NVIC_SetPriority(spi1_tei_int_num ,  SPI1_PRIO);
  NVIC_SetPriority(spi1_eri_int_num ,  SPI1_PRIO);
  NVIC_SetPriority(spi1_idle_int_num,  SPI1_PRIO);

  R_ICU->IELSRn_b[spi1_rxi_int_num].IR  = 0;  // Сбрасываем IR флаг в ICU
  R_ICU->IELSRn_b[spi1_txi_int_num].IR  = 0;  // Сбрасываем IR флаг в ICU
  R_ICU->IELSRn_b[spi1_tei_int_num].IR  = 0;  // Сбрасываем IR флаг в ICU
  R_ICU->IELSRn_b[spi1_eri_int_num].IR  = 0;  // Сбрасываем IR флаг в ICU
  R_ICU->IELSRn_b[spi1_idle_int_num].IR = 0;  // Сбрасываем IR флаг в ICU
}


/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
void     spi1_rxi_isr(void)
{

  R_ICU->IELSRn_b[spi1_rxi_int_num].IR  = 0;
  __DSB();
}

/*-----------------------------------------------------------------------------------------------------
  Прерывание срабатывает сразу же как только будет разрешено, если еще не началась передача по SPI
  Поэтому его не используем для индикации окончания работы шины при использовании DMA

  \param void
-----------------------------------------------------------------------------------------------------*/
void     spi1_tei_isr(void)
{
  NVIC_DisableIRQ(spi1_tei_int_num);
  R_ICU->IELSRn_b[spi1_tei_int_num].IR  = 0;
  __DSB();
}
/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
void     spi1_eri_isr(void)
{

  R_ICU->IELSRn_b[spi1_eri_int_num].IR  = 0;
  __DSB();
}
/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
void     spi1_idle_isr(void)
{
  R_RSPI1->SPCR  = 0;
  R_RSPI1->SPCR2  = 0; // Прерывание по IDLE отключаем, чтобы не зациклится в этой процедуре обслуживания прерывания
  NVIC_DisableIRQ(spi1_idle_int_num);

  tx_event_flags_set(&spi1_flags, FLAG_SPI1_IDLE, TX_OR);
  R_ICU->IELSRn_b[spi1_idle_int_num].IR = 0;
  __DSB();
}

/*-----------------------------------------------------------------------------------------------------


  \param p_args
-----------------------------------------------------------------------------------------------------*/
static void  Transfer_callback(transfer_callback_args_t *p_args)
{
  tx_event_flags_set(&spi1_flags, FLAG_SPI1_TRANSFER_END, TX_OR);
}

/*-----------------------------------------------------------------------------------------------------


  \param val
-----------------------------------------------------------------------------------------------------*/
void SPI1_set_spbr(uint8_t val)
{
  g_spbr = val;
}

/*-----------------------------------------------------------------------------------------------------


  \param void

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t SPI1_init(void)
{
  uint8_t   reg8;
  uint16_t  reg16;

  if (spi1_initialized) return RES_OK;

  R_MSTP->MSTPCRB_b.MSTPB18 = 0; // Разрешаем работу модуля Serial Peripheral Interface 1
  DELAY_1us;


  R_RSPI1->SPCR  = 0;      // Выключаем работу SPI

  R_RSPI1->SSLP  = 0;      // Set SSL0..SSL3 signal to active low
  R_RSPI1->SPPCR = BIT(5); // Set MOSI output value during MOSI idling to 0
  R_RSPI1->SPSCR = 0;      // SPI Sequence Length Specification. Используем только командный регистр 0

  R_RSPI1->SPBR  = g_spbr;      // Тактовая частота = PCLKA_FREQ / 2*(n+1) = 120/6 = 20

  //
  reg8 = 0
         + LSHIFT(0, 7) // -         |
         + LSHIFT(0, 6) // SPBYT     | SPI Byte Access Specification.                 0: SPDR is accessed in halfword or word (SPLW is valid). 1: SPDR is accessed in byte (SPLW is invalid).
         + LSHIFT(0, 5) // SPLW      | SPI Word Access/Halfword Access Specification. 0: Set SPDR_HA to valid for halfword access. 1: Set SPDR to valid for word access.
         + LSHIFT(0, 4) // SPRDTD    | SPI Receive/Transmit Data Select               0: Read SPDR/SPDR_HA values from receive buffer. 1: Read SPDR/SPDR_HA values from transmit buffer, but only if the transmit buffer is empty
         + LSHIFT(0, 3) // -         |
         + LSHIFT(0, 2) // -         |
         + LSHIFT(0, 0) // SPFC[1:0] | Number of Frames Specification. 0 0: 1 frame
  ;
  R_RSPI1->SPDCR = reg8;

  R_RSPI1->SPCKD = 2; // RSPCK Delay Setting. 0 1 0: 3 RSPCK. SPCKD specifies the RSPCK delay, the period from the beginning of SSLni signal assertion to RSPCK oscillation, when the SPCMDm.SCKDEN bit is 1

  R_RSPI1->SSLND = 0; // SSLND specifies the SSL negation delay, the period from the transmission of a final RSPCK edge to the negation of the SSLni signal during a serial transfer by the SPI in master mode.

  R_RSPI1->SPND  = 2; // SPI Next-Access Delay Setting. 0 1 0: 3 RSPCK + 2 PCLKA. SPND specifies the next-access delay, the non-active period of the SSLni signal after termination of a serial transfer, when the SPCMDm.SPNDEN bit is 1

  R_RSPI1->SPCR2 = 0; // Проверку четности не выполняем и прерывние по IDLE не вызываем


  //
  reg16 = 0
          + LSHIFT(0  , 15) // SCKDEN    | RSPCK Delay Setting Enable. 1: Select RSPCK delay equal to the setting in the SPI Clock Delay Register (SPCKD).
          + LSHIFT(0  , 14) // SLNDEN    | SSL Negation Delay Setting Enable. 1: Select SSL negation delay equal to the setting in the SPI Slave Select Negation Delay Register (SSLND).
          + LSHIFT(0  , 13) // SPNDEN    | SPI Next-Access Delay Enable. 1: Select next-access delay equal to the setting in the SPI NextAccess Delay Register (SPND).
          + LSHIFT(0  , 12) // LSBF      | SPI LSB First. 0: MSB first, 1: LSB first
          + LSHIFT(0xF,  8) // SPB[3:0]  | SPI Data Length Setting.  0111: 8 bits, 1111: 16 bits
          + LSHIFT(1  ,  7) // SSLKP     | SSL Signal Level Keeping.  0: Negate all SSL signals on completion of transfer
          + LSHIFT(0  ,  4) // SSLA[2:0] | SSL Signal Assertion Setting. 000: SSL0 ... 011: SSL3
          + LSHIFT(0  ,  2) // BRDV[1:0] | Bit Rate Division Setting. Снижаем частоту в 2 раза до 15 Мгц
          + LSHIFT(0  ,  1) // CPOL      | RSPCK Polarity Setting
          + LSHIFT(0  ,  0) // CPHA      | RSPCK Phase Setting
  ;
  R_RSPI1->SPCMDn[0] = reg16;


  R_RSPI1->SPDCR2 = 0; // Byte Swap Operating Mode Select. 0: Byte Swap OFF. 1: Byte Swap ON

  reg8 = 0
         + LSHIFT(0, 7) // SPRIE  | SPI Receive Buffer Full Interrupt Enable |
         + LSHIFT(1, 6) // SPE    | SPI Function Enable                      |
         + LSHIFT(0, 5) // SPTIE  | Transmit Buffer Empty Interrupt Enable   |
         + LSHIFT(0, 4) // SPEIE  | SPI Error Interrupt Enable               |
         + LSHIFT(1, 3) // MSTR   | SPI Master/Slave Mode Select             | 0: Select slave mode. 1: Select master mode.
         + LSHIFT(0, 2) // MODFEN | Mode Fault Error Detection Enable        |
         + LSHIFT(1, 1) // TXMD   | Communications Operating Mode Select     | 0: Select full-duplex synchronous serial communications. 1: Select serial communications with transmit-only
         + LSHIFT(0, 0) // SPMS   | SPI Mode Select                          | 0: Select SPI operation (4-wire method). 1: Select clock synchronous operation (3-wire method).
  ;
  R_RSPI1->SPCR = reg8;


  tx_event_flags_create(&spi1_flags, "spi1_flags");
  tx_mutex_create(&spi1_mutex, "spi1_mutex", TX_INHERIT);
  spi1_init_interrupts();

  spi1_initialized = 1;
  return RES_OK;
}


/*-----------------------------------------------------------------------------------------------------
  Дисплей может работать до частоты 15 МГц
  Послылаем однобайтные команды
  Для разрешения обмена используется сигнал SSL0
  CPOL = 0
  CPHA = 0

  \param void

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t SPI1_set_for_byte_transfer_to_display(void)
{
  uint8_t   reg8;
  uint16_t  reg16;

  // Проверяем флаг IDLNF. Если 1, то  SPI is in the transfer state.
  if (R_RSPI1->SPSR & BIT(1))
  {
    return RES_ERROR;
  }

  R_RSPI1->SPCR  = 0;      // Выключаем работу SPI

  R_RSPI1->SPBR  = g_spbr;      // Тактовая частота = PCLKA_FREQ / 2*(n+1) = 120/6 = 20

  // Делаем побайтовый вывод
  reg8 = 0
         + LSHIFT(0, 7) // -         |
         + LSHIFT(1, 6) // SPBYT     | SPI Byte Access Specification.                 0: SPDR is accessed in halfword or word (SPLW is valid). 1: SPDR is accessed in byte (SPLW is invalid).
         + LSHIFT(0, 5) // SPLW      | SPI Word Access/Halfword Access Specification. 0: Set SPDR_HA to valid for halfword access. 1: Set SPDR to valid for word access.
         + LSHIFT(0, 4) // SPRDTD    | SPI Receive/Transmit Data Select               0: Read SPDR/SPDR_HA values from receive buffer
         + LSHIFT(0, 3) // -         |
         + LSHIFT(0, 2) // -         |
         + LSHIFT(0, 0) // SPFC[1:0] | Number of Frames Specification. 0 0: 1 frame
  ;
  R_RSPI1->SPDCR = reg8;


  reg16 = 0
          + LSHIFT(1  , 15) // SCKDEN    | RSPCK Delay Setting Enable. 1: Select RSPCK delay equal to the setting in the SPI Clock Delay Register (SPCKD).
          + LSHIFT(1  , 14) // SLNDEN    | SSL Negation Delay Setting Enable. 1: Select SSL negation delay equal to the setting in the SPI Slave Select Negation Delay Register (SSLND).
          + LSHIFT(1  , 13) // SPNDEN    | SPI Next-Access Delay Enable. 1: Select next-access delay equal to the setting in the SPI NextAccess Delay Register (SPND).
          + LSHIFT(0  , 12) // LSBF      | SPI LSB First. 0: MSB first, 1: LSB first
          + LSHIFT(0x7,  8) // SPB[3:0]  | SPI Data Length Setting.  0111: 8 bits, 1111: 16 bits
          + LSHIFT(0  ,  7) // SSLKP     | SSL Signal Level Keeping.  0: Negate all SSL signals on completion of transfer
          + LSHIFT(0  ,  4) // SSLA[2:0] | SSL Signal Assertion Setting. 000: SSL0 ... 011: SSL3
          + LSHIFT(0  ,  2) // BRDV[1:0] | Bit Rate Division Setting. Не снижаем чатоту
          + LSHIFT(0  ,  1) // CPOL      | RSPCK Polarity Setting
          + LSHIFT(0  ,  0) // CPHA      | RSPCK Phase Setting
  ;
  R_RSPI1->SPCMDn[0] = reg16;


  R_RSPI1->SPDCR2 = 0;     // Byte Swap Operating Mode Select. 0: Byte Swap OFF. 1: Byte Swap ON


  reg8 = 0
         + LSHIFT(0, 7) // SPRIE  | SPI Receive Buffer Full Interrupt Enable |
         + LSHIFT(1, 6) // SPE    | SPI Function Enable                      |
         + LSHIFT(0, 5) // SPTIE  | Transmit Buffer Empty Interrupt Enable   |
         + LSHIFT(0, 4) // SPEIE  | SPI Error Interrupt Enable               |
         + LSHIFT(1, 3) // MSTR   | SPI Master/Slave Mode Select             | 0: Select slave mode. 1: Select master mode.
         + LSHIFT(0, 2) // MODFEN | Mode Fault Error Detection Enable        |
         + LSHIFT(1, 1) // TXMD   | Communications Operating Mode Select     | 0: Select full-duplex synchronous serial communications. 1: Select serial communications with transmit-only
         + LSHIFT(0, 0) // SPMS   | SPI Mode Select                          | 0: Select SPI operation (4-wire method). 1: Select clock synchronous operation (3-wire method).
  ;
  R_RSPI1->SPCR = reg8;

  NVIC_EnableIRQ(spi1_idle_int_num);

  return RES_OK;

}

/*-----------------------------------------------------------------------------------------------------
  Послылаем буффер данных с помощью DMA
  При пересылке видеобуфера требуется менять местами байты в двухбайтных словах

  \param void

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t SPI1_set_for_buffer_transfer_to_display(void)
{
  uint8_t   reg8;
  uint16_t  reg16;

  // Проверяем флаг IDLNF. Если 1, то  SPI is in the transfer state.
  if (R_RSPI1->SPSR & BIT(1))
  {
    return RES_ERROR;
  }

  R_RSPI1->SPCR  = 0;      // Выключаем работу SPI

  R_RSPI1->SPBR  = g_spbr;      // Тактовая частота = PCLKA_FREQ / 2*(n+1) = 120/6 = 20

  // Делаем побайтовый вывод
  reg8 = 0
         + LSHIFT(0, 7) // -         |
         + LSHIFT(0, 6) // SPBYT     | SPI Byte Access Specification.                 0: SPDR is accessed in halfword or word (SPLW is valid). 1: SPDR is accessed in byte (SPLW is invalid).
         + LSHIFT(0, 5) // SPLW      | SPI Word Access/Halfword Access Specification. 0: Set SPDR_HA to valid for halfword access. 1: Set SPDR to valid for word access.
         + LSHIFT(0, 4) // SPRDTD    | SPI Receive/Transmit Data Select               0: Read SPDR/SPDR_HA values from receive buffer
         + LSHIFT(0, 3) // -         |
         + LSHIFT(0, 2) // -         |
         + LSHIFT(0, 0) // SPFC[1:0] | Number of Frames Specification. 0 0: 1 frame
  ;
  R_RSPI1->SPDCR = reg8;


  reg16 = 0
          + LSHIFT(0  , 15) // SCKDEN    | RSPCK Delay Setting Enable. 1: Select RSPCK delay equal to the setting in the SPI Clock Delay Register (SPCKD).
          + LSHIFT(0  , 14) // SLNDEN    | SSL Negation Delay Setting Enable. 1: Select SSL negation delay equal to the setting in the SPI Slave Select Negation Delay Register (SSLND).
          + LSHIFT(0  , 13) // SPNDEN    | SPI Next-Access Delay Enable. 1: Select next-access delay equal to the setting in the SPI NextAccess Delay Register (SPND).
          + LSHIFT(0  , 12) // LSBF      | SPI LSB First. 0: MSB first, 1: LSB first
          + LSHIFT(0xF,  8) // SPB[3:0]  | SPI Data Length Setting.  0111: 8 bits, 1111: 16 bits
          + LSHIFT(1  ,  7) // SSLKP     | SSL Signal Level Keeping.  0: Negate all SSL signals on completion of transfer. 1: Keep SSL signal level from the end of transfer until the beginning of the next access.
          + LSHIFT(0  ,  4) // SSLA[2:0] | SSL Signal Assertion Setting. 000: SSL0 ... 011: SSL3
          + LSHIFT(0  ,  2) // BRDV[1:0] | Bit Rate Division Setting. Не снижаем чатоту
          + LSHIFT(0  ,  1) // CPOL      | RSPCK Polarity Setting
          + LSHIFT(0  ,  0) // CPHA      | RSPCK Phase Setting
  ;
  R_RSPI1->SPCMDn[0] = reg16;


  R_RSPI1->SPDCR2 = 0; // Byte Swap Operating Mode Select. 0: Byte Swap OFF. 1: Byte Swap ON

  R_RSPI1->SPCR2  = 0; // Прерывание по IDLE не используем

  return RES_OK;

}

/*-----------------------------------------------------------------------------------------------------



  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t SPI1_enable_function(void)
{
  uint8_t   reg8;

  R_ICU->IELSRn_b[spi1_txi_int_num].IR  = 0;

  reg8 = 0
         + LSHIFT(0, 7) // SPRIE  | SPI Receive Buffer Full Interrupt Enable |
         + LSHIFT(1, 6) // SPE    | SPI Function Enable                      |
         + LSHIFT(1, 5) // SPTIE  | Transmit Buffer Empty Interrupt Enable   | Этот бит надо установить чтобы работал запрос на DMA
         + LSHIFT(0, 4) // SPEIE  | SPI Error Interrupt Enable               |
         + LSHIFT(1, 3) // MSTR   | SPI Master/Slave Mode Select             | 0: Select slave mode. 1: Select master mode.
         + LSHIFT(0, 2) // MODFEN | Mode Fault Error Detection Enable        |
         + LSHIFT(1, 1) // TXMD   | Communications Operating Mode Select     | 0: Select full-duplex synchronous serial communications. 1: Select serial communications with transmit-only
         + LSHIFT(0, 0) // SPMS   | SPI Mode Select                          | 0: Select SPI operation (4-wire method). 1: Select clock synchronous operation (3-wire method).
  ;
  R_RSPI1->SPCR = reg8;

  return RES_OK;

}


/*-----------------------------------------------------------------------------------------------------


  \param b

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t SPI1_send_byte_to_display(uint8_t b)
{
  uint32_t     res = RES_ERROR;
  ULONG        actual_flags;

  if (tx_mutex_get(&spi1_mutex, MS_TO_TICKS(100)) == TX_SUCCESS)
  {
    Set_LCD_CS(0);
    if (SPI1_set_for_byte_transfer_to_display() != RES_OK)
    {
      Set_LCD_CS(1);
      return RES_ERROR;
    }
    R_RSPI1->SPDR_HA  = b;
    R_RSPI1->SPCR2 = BIT(2); // Разрешаем прерывания по IDLE
    if (tx_event_flags_get(&spi1_flags, FLAG_SPI1_IDLE, TX_OR_CLEAR,&actual_flags, MS_TO_TICKS(10)) == TX_SUCCESS) res = RES_OK;
    tx_mutex_put(&spi1_mutex);
    Set_LCD_CS(1);
  }
  return res;
}

/*-----------------------------------------------------------------------------------------------------
  На частоте SPI 20 МГц пересылка экрана 240x240 16 бит длиться 54 мс
  Эффективная скорость 17 мегабит в сек

  \param buf
  \param sz

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t SPI1_send_buff_to_display(uint16_t *buf, uint32_t sz)
{
  uint32_t  res = RES_ERROR;
  ULONG     actual_flags;

  if (tx_mutex_get(&spi1_mutex, MS_TO_TICKS(100)) == TX_SUCCESS)
  {
    Set_LCD_CS(0);
    DMA_CH6_transfer_info.p_src  = buf-1;      // Корректировка с учетом работы DMA.
                                               // Первое слово по DMA не высылается (причина не ясна). Поэтому сдвигаем указатель выше начала буфера
    DMA_CH6_transfer_info.length = (sz / 2)+1; //

    if (DMA_CH6_transfer_instance.p_api->open(DMA_CH6_transfer_instance.p_ctrl,DMA_CH6_transfer_instance.p_cfg) == SSP_SUCCESS)
    {
      if (DMA_CH6_transfer_instance.p_api->enable(DMA_CH6_transfer_instance.p_ctrl) == SSP_SUCCESS)
      {
        SPI1_enable_function();
        if (tx_event_flags_get(&spi1_flags, FLAG_SPI1_TRANSFER_END, TX_OR_CLEAR,&actual_flags, MS_TO_TICKS(200)) == TX_SUCCESS)
        {
          res = RES_OK;
        }
      }
      DMA_CH6_transfer_instance.p_api->close(DMA_CH6_transfer_instance.p_ctrl);
    }
    DELAY_1us;  // Даем время на окончание пересылки последнего слова
    R_RSPI1->SPCR  = 0;
    Set_LCD_CS(1);


    tx_mutex_put(&spi1_mutex);
  }
  return res;
}

