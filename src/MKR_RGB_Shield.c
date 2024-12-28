// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 2024-11-23
// 16:38:08
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include   "Test.h"

static void  Transfer_callback(transfer_callback_args_t *p_args);

extern transfer_info_t                DMA_CH6_Shield_transfer_info;
extern const transfer_instance_t      DMA_CH6_Shield_transfer_instance;

dmac_instance_ctrl_t DMA_CH6_Shield_control_block;

transfer_info_t DMA_CH6_Shield_transfer_info =
{
  .dest_addr_mode      = TRANSFER_ADDR_MODE_FIXED,
  .repeat_area         = TRANSFER_REPEAT_AREA_SOURCE,
  .irq                 = TRANSFER_IRQ_EACH,
  .chain_mode          = TRANSFER_CHAIN_MODE_DISABLED,
  .src_addr_mode       = TRANSFER_ADDR_MODE_INCREMENTED,
  .size                = TRANSFER_SIZE_4_BYTE,
  .mode                = TRANSFER_MODE_NORMAL,
  .p_dest              = (void *)&R_RSPI1->SPDR,
  .p_src               = (void const *) NULL,
  .num_blocks          = 0,
  .length              = 0,
};

const transfer_on_dmac_cfg_t DMA_CH6_Shield_extended_configuration =
{
  .channel             = 6,
  .offset_byte         = 0,
};

const transfer_cfg_t DMA_CH6_Shield_configuration =
{
  .p_info              = &DMA_CH6_Shield_transfer_info,
  .activation_source   = ELC_EVENT_SPI1_TXI,
  .auto_enable         = false,
  .p_callback          = Transfer_callback,
  .p_context           = &DMA_CH6_Shield_transfer_instance,
  .irq_ipl             = (2),
  .p_extend            = &DMA_CH6_Shield_extended_configuration,
};

const transfer_instance_t DMA_CH6_Shield_transfer_instance =
{
  .p_ctrl        = &DMA_CH6_Shield_control_block,
  .p_cfg         = &DMA_CH6_Shield_configuration,
  .p_api         = &g_transfer_on_dmac
};




#define  SCREEN_HANDLE             0x12345679



T_mkr_rgb_shield_buf mkr_rgb_shield_buf;

/*-----------------------------------------------------------------------------------------------------


  \param p_args
-----------------------------------------------------------------------------------------------------*/
static void  Transfer_callback(transfer_callback_args_t *p_args)
{
  tx_event_flags_set(&spi1_flags, FLAG_SPI1_TRANSFER_END, TX_OR);
}


/*-----------------------------------------------------------------------------------------------------


  \param void

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t SPI1_Shield_init(uint8_t spbr)
{
  uint8_t   reg8;
  uint16_t  reg16;

  tx_event_flags_create(&spi1_flags, "spi1_flags");

  R_MSTP->MSTPCRB_b.MSTPB18 = 0; // Разрешаем работу модуля Serial Peripheral Interface 1
  DELAY_1us;

  R_RSPI1->SPCR  = 0;            // Выключаем работу SPI
  R_RSPI1->SSLP  = 0;            // Set SSL0..SSL3 signal to active low
  R_RSPI1->SPPCR = BIT(5);       // Set MOSI output value during MOSI idling to 0
  R_RSPI1->SPSCR = 0;            // SPI Sequence Length Specification. Используем только командный регистр 0

  R_RSPI1->SPBR  = spbr;         // Тактовая частота = PCLKA_FREQ / 2*(n+1)

  //
  reg8 = 0
         + LSHIFT(0, 7) // -         |
         + LSHIFT(0, 6) // SPBYT     | SPI Byte Access Specification.                 0: SPDR is accessed in halfword or word (SPLW is valid). 1: SPDR is accessed in byte (SPLW is invalid).
         + LSHIFT(1, 5) // SPLW      | SPI Word Access/Halfword Access Specification. 0: Set SPDR_HA to valid for halfword access. 1: Set SPDR to valid for word access.
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


  reg16 = 0
          + LSHIFT(0  , 15) // SCKDEN    | RSPCK Delay Setting Enable. 1: Select RSPCK delay equal to the setting in the SPI Clock Delay Register (SPCKD).
          + LSHIFT(0  , 14) // SLNDEN    | SSL Negation Delay Setting Enable. 1: Select SSL negation delay equal to the setting in the SPI Slave Select Negation Delay Register (SSLND).
          + LSHIFT(0  , 13) // SPNDEN    | SPI Next-Access Delay Enable. 1: Select next-access delay equal to the setting in the SPI NextAccess Delay Register (SPND).
          + LSHIFT(0  , 12) // LSBF      | SPI LSB First. 0: MSB first, 1: LSB first
          + LSHIFT(0x3,  8) // SPB[3:0]  | SPI Data Length Setting.  0111: 8 bits, 1111: 16 bits, 0011: 32 bits
          + LSHIFT(1  ,  7) // SSLKP     | SSL Signal Level Keeping.  0: Negate all SSL signals on completion of transfer. 1: Keep SSL signal level from the end of transfer until the beginning of the next access.
          + LSHIFT(0  ,  4) // SSLA[2:0] | SSL Signal Assertion Setting. 000: SSL0 ... 011: SSL3
          + LSHIFT(0  ,  2) // BRDV[1:0] | Bit Rate Division Setting. Не снижаем чатоту
          + LSHIFT(0  ,  1) // CPOL      | RSPCK Polarity Setting
          + LSHIFT(0  ,  0) // CPHA      | RSPCK Phase Setting
  ;
  R_RSPI1->SPCMDn[0] = reg16;


  R_RSPI1->SPDCR2 = 0; // Byte Swap Operating Mode Select. 0: Byte Swap OFF. 1: Byte Swap ON

  spi1_init_interrupts();


  return RES_OK;
}

/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
void SPI1_Shield_enable_function(void)
{
  uint8_t   reg8;
  reg8 = 0
         + LSHIFT(0, 7) // SPRIE  | SPI Receive Buffer Full Interrupt Enable |
         + LSHIFT(1, 6) // SPE    | SPI Function Enable                      |
         + LSHIFT(1, 5) // SPTIE  | Transmit Buffer Empty Interrupt Enable   | Этот бит надо установить чтобы работал запрос на DMA
         + LSHIFT(0, 4) // SPEIE  | SPI Error Interrupt Enable               |
         + LSHIFT(1, 3) // MSTR   | SPI Master/Slave Mode Select             | 0: Select slave mode. 1: Select master mode.
         + LSHIFT(0, 2) // MODFEN | Mode Fault Error Detection Enable        |
         + LSHIFT(1, 1) // TXMD   | Communications Operating Mode Select     | 0: Select full-duplex synchronous serial communications. 1: Select serial communications with transmit-only
         + LSHIFT(1, 0) // SPMS   | SPI Mode Select                          | 0: Select SPI operation (4-wire method). 1: Select clock synchronous operation (3-wire method).
  ;
  R_RSPI1->SPCR = reg8;
}

/*-----------------------------------------------------------------------------------------------------


  \param buf
  \param sz

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t SPI1_MKR_RGB_Shield_wr_buff_to_display(uint32_t *buf, uint32_t sz)
{
  uint32_t  res = RES_ERROR;
  ULONG     actual_flags;

  DMA_CH6_Shield_transfer_info.p_src  = buf;      // Корректировка с учетом работы DMA.
                                                  // Первое слово по DMA не высылается (причина не ясна). Поэтому сдвигаем указатель выше начала буфера
  DMA_CH6_Shield_transfer_info.length = sz / 4;     //

  if (DMA_CH6_Shield_transfer_instance.p_api->open(DMA_CH6_Shield_transfer_instance.p_ctrl,DMA_CH6_Shield_transfer_instance.p_cfg) == SSP_SUCCESS)
  {
    if (DMA_CH6_Shield_transfer_instance.p_api->enable(DMA_CH6_Shield_transfer_instance.p_ctrl) == SSP_SUCCESS)
    {
      SPI1_Shield_enable_function();
      if (tx_event_flags_get(&spi1_flags, FLAG_SPI1_TRANSFER_END, TX_OR_CLEAR,&actual_flags, MS_TO_TICKS(200)) == TX_SUCCESS)
      {
        res = RES_OK;
      }
    }
    DMA_CH6_Shield_transfer_instance.p_api->close(DMA_CH6_Shield_transfer_instance.p_ctrl);
  }
  DELAY_8us;  // Даем время 11 us на окончание пересылки последнего слова
  DELAY_4us;
  R_RSPI1->SPCR  = 0;


  return res;
}


/*-----------------------------------------------------------------------------------------------------
  Подготовка видеобуфера с очисткой экрана

-----------------------------------------------------------------------------------------------------*/
static void Prepare_screen(void)
{
  mkr_rgb_shield_buf.header = 0;
  mkr_rgb_shield_buf.footer = 0xFFFFFFFF;

  for (uint32_t i = 0; i < MKR_RGB_SHIELD_LEDS_NUM; i++)
  {
    // Const   Global    BLUE       GREEN      RED
    // 111     00000     00000000   00000000   00000000
    mkr_rgb_shield_buf.data[i] = 0xE0000000;
  }
}


/*-----------------------------------------------------------------------------------------------------


  \param val
-----------------------------------------------------------------------------------------------------*/
void MKR_RGB_Fill_const(uint32_t val)
{
  for (uint32_t i = 0; i < MKR_RGB_SHIELD_LEDS_NUM; i++)
  {
    mkr_rgb_shield_buf.data[i] = 0xE0000000 | (val & 0x1FFFFFFF);
  }
  SPI1_MKR_RGB_Shield_wr_buff_to_display((uint32_t *)&mkr_rgb_shield_buf, sizeof(mkr_rgb_shield_buf));
}

/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
void MKR_RGB_clear_screen(void)
{
  Prepare_screen();
  // Пересылка всего буфера MKR RGB Shield длится 1 мс
  SPI1_MKR_RGB_Shield_wr_buff_to_display((uint32_t *)&mkr_rgb_shield_buf, sizeof(mkr_rgb_shield_buf));
}

/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
void MKR_RGB_Shield_init(void)
{
  // Здесь не можем использовать NV параметры поскольку они еще не прочитаны
  LED_MATRIX_EN = 1;
  SPI1_Shield_init(19); // Частоту шины SPI1 установливаем на 3 МГц
  MKR_RGB_clear_screen();
}


