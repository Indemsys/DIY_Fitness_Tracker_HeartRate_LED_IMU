// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 2024-12-10
// 17:30:39
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include   "App.h"


/*-----------------------------------------------------------------------------------------------------
  Переключение на тактирование системы из тактировнаия от кварца 24 МГц и PLL 240 МГц на MOCO с частотой 8 МГц
  Кварц 24 Мгц и PLL отключаем
  Прерывания в течении выполнения этой функции должны быть запрещены!!!

  Длительность выполнения процедуры - 200 мкс
  Потребление системы в этом режиме - 1.84...2.11 мА в зависимости от температуры
-----------------------------------------------------------------------------------------------------*/
void Switch_to_MOCO_clock(uint8_t mode)
{
  R_SYSTEM->PRCR = 0xA50B;       // Разрешаем запись в регистр управления тактированием и режимом потребления

  if (R_SYSTEM->MOCOCR_b.MCSTP == 1)
  {
    R_SYSTEM->MOCOCR_b.MCSTP = 0;  // Включаем MOCO 8 МГц
    DELAY_8us;                     // Ожидание стабилизации MOCO. Максимальное время стабилизации MOCO - 15 мкс
    DELAY_8us;
  }

  R_SYSTEM->SCKSCR_b.CKSEL = 1;    // Устанавливаем тактирование системы от MOCO.  0 0 1: MOCO

  switch (mode)
  {
  case 0:
    // Режим для работы кодека до частоты 24 КГц 16-бит
    R_SYSTEM->SCKDIVCR = 0
                         + LSHIFT(3, 28) // FCK[2:0]  | Flash Interface Clock (FCLK)| div 8 -> 1 MHz | 4 to  60  MHz. Flash interface
                         + LSHIFT(3, 24) // ICK[2:0]  | System Clock (ICLK)         | div 8 -> 1 MHz | Up to 240 MHz. CPU, DTC, DMAC, ROM, RAM
                         + LSHIFT(6, 16) // BCK[2:0]  | External Bus Clock (BCLK)   | div 8 -> 1 MHz | Up to 120 MHz. External bus
                         + LSHIFT(6, 12) // PCKA[2:0] | Peripheral Module Clock A   | div 8 -> 1 MHz | Up to 120 MHz. ETHERC,EDMAC, USBHS, QSPI, SPI,SCI, SCE7, Graphics LCD,SDHI, CRC, JPEG engine, DRW,IrDA, GPT bus-clock, Standby SRAM
                         + LSHIFT(6,  8) // PCKB[2:0] | Peripheral Module Clock B   | div 8 -> 1 MHz | Up to 60  MHz. IIC, SSI,SRC, DOC, CAC, CAN, DAC12, POEG, CTSU, AGT, ELC, I/OPorts, RTC, WDT, IWDT, ADC12, KINT, USBFS, ACMPHS, TSN, PDC
                         + LSHIFT(6,  4) // PCKC[2:0] | Peripheral Module Clock C   | div 8 -> 1 MHz | Up to 60  MHz. ADC12
                         + LSHIFT(6,  0) // PCKD[2:0] | Peripheral Module Clock D   | div 8 -> 1 MHz | Up to 120 MHz. GPT countclock
    ;
    break;
  case 1:
    // Режим для работы кодека до частоты 24 КГц 16-бит
    R_SYSTEM->SCKDIVCR = 0
                         + LSHIFT(3, 28) // FCK[2:0]  | Flash Interface Clock (FCLK)| div 8 -> 1 MHz | 4 to  60  MHz. Flash interface
                         + LSHIFT(3, 24) // ICK[2:0]  | System Clock (ICLK)         | div 8 -> 1 MHz | Up to 240 MHz. CPU, DTC, DMAC, ROM, RAM
                         + LSHIFT(6, 16) // BCK[2:0]  | External Bus Clock (BCLK)   | div 8 -> 1 MHz | Up to 120 MHz. External bus
                         + LSHIFT(5, 12) // PCKA[2:0] | Peripheral Module Clock A   | div 8 -> 1 MHz | Up to 120 MHz. ETHERC,EDMAC, USBHS, QSPI, SPI,SCI, SCE7, Graphics LCD,SDHI, CRC, JPEG engine, DRW,IrDA, GPT bus-clock, Standby SRAM
                         + LSHIFT(5,  8) // PCKB[2:0] | Peripheral Module Clock B   | div 8 -> 1 MHz | Up to 60  MHz. IIC, SSI,SRC, DOC, CAC, CAN, DAC12, POEG, CTSU, AGT, ELC, I/OPorts, RTC, WDT, IWDT, ADC12, KINT, USBFS, ACMPHS, TSN, PDC
                         + LSHIFT(6,  4) // PCKC[2:0] | Peripheral Module Clock C   | div 8 -> 1 MHz | Up to 60  MHz. ADC12
                         + LSHIFT(5,  0) // PCKD[2:0] | Peripheral Module Clock D   | div 8 -> 1 MHz | Up to 120 MHz. GPT countclock
    ;
    break;
  case 2:
    // Режим для работы кодека до частоты 48 КГц 16-бит
    R_SYSTEM->SCKDIVCR = 0
                         + LSHIFT(3, 28) // FCK[2:0]  | Flash Interface Clock (FCLK)| div 8 -> 1 MHz | 4 to  60  MHz. Flash interface
                         + LSHIFT(3, 24) // ICK[2:0]  | System Clock (ICLK)         | div 8 -> 1 MHz | Up to 240 MHz. CPU, DTC, DMAC, ROM, RAM
                         + LSHIFT(6, 16) // BCK[2:0]  | External Bus Clock (BCLK)   | div 8 -> 1 MHz | Up to 120 MHz. External bus
                         + LSHIFT(3, 12) // PCKA[2:0] | Peripheral Module Clock A   | div 8 -> 1 MHz | Up to 120 MHz. ETHERC,EDMAC, USBHS, QSPI, SPI,SCI, SCE7, Graphics LCD,SDHI, CRC, JPEG engine, DRW,IrDA, GPT bus-clock, Standby SRAM
                         + LSHIFT(3,  8) // PCKB[2:0] | Peripheral Module Clock B   | div 8 -> 1 MHz | Up to 60  MHz. IIC, SSI,SRC, DOC, CAC, CAN, DAC12, POEG, CTSU, AGT, ELC, I/OPorts, RTC, WDT, IWDT, ADC12, KINT, USBFS, ACMPHS, TSN, PDC
                         + LSHIFT(6,  4) // PCKC[2:0] | Peripheral Module Clock C   | div 8 -> 1 MHz | Up to 60  MHz. ADC12
                         + LSHIFT(3,  0) // PCKD[2:0] | Peripheral Module Clock D   | div 8 -> 1 MHz | Up to 120 MHz. GPT countclock
    ;
    break;
  case 3:
    // Режим для работы кодека до частоты 48 КГц 24-бит
    R_SYSTEM->SCKDIVCR = 0
                         + LSHIFT(2, 28) // FCK[2:0]  | Flash Interface Clock (FCLK)| div 8 -> 1 MHz | 4 to  60  MHz. Flash interface
                         + LSHIFT(2, 24) // ICK[2:0]  | System Clock (ICLK)         | div 8 -> 1 MHz | Up to 240 MHz. CPU, DTC, DMAC, ROM, RAM
                         + LSHIFT(6, 16) // BCK[2:0]  | External Bus Clock (BCLK)   | div 8 -> 1 MHz | Up to 120 MHz. External bus
                         + LSHIFT(2, 12) // PCKA[2:0] | Peripheral Module Clock A   | div 8 -> 1 MHz | Up to 120 MHz. ETHERC,EDMAC, USBHS, QSPI, SPI,SCI, SCE7, Graphics LCD,SDHI, CRC, JPEG engine, DRW,IrDA, GPT bus-clock, Standby SRAM
                         + LSHIFT(2,  8) // PCKB[2:0] | Peripheral Module Clock B   | div 8 -> 1 MHz | Up to 60  MHz. IIC, SSI,SRC, DOC, CAC, CAN, DAC12, POEG, CTSU, AGT, ELC, I/OPorts, RTC, WDT, IWDT, ADC12, KINT, USBFS, ACMPHS, TSN, PDC
                         + LSHIFT(6,  4) // PCKC[2:0] | Peripheral Module Clock C   | div 8 -> 1 MHz | Up to 60  MHz. ADC12
                         + LSHIFT(2,  0) // PCKD[2:0] | Peripheral Module Clock D   | div 8 -> 1 MHz | Up to 120 MHz. GPT countclock
    ;
    break;
  case 4:
    // Режим для работы кодека до частоты 96 КГц 24-бит
    R_SYSTEM->SCKDIVCR = 0
                         + LSHIFT(1, 28) // FCK[2:0]  | Flash Interface Clock (FCLK)| div 8 -> 1 MHz | 4 to  60  MHz. Flash interface
                         + LSHIFT(1, 24) // ICK[2:0]  | System Clock (ICLK)         | div 8 -> 1 MHz | Up to 240 MHz. CPU, DTC, DMAC, ROM, RAM
                         + LSHIFT(1, 16) // BCK[2:0]  | External Bus Clock (BCLK)   | div 8 -> 1 MHz | Up to 120 MHz. External bus
                         + LSHIFT(1, 12) // PCKA[2:0] | Peripheral Module Clock A   | div 8 -> 1 MHz | Up to 120 MHz. ETHERC,EDMAC, USBHS, QSPI, SPI,SCI, SCE7, Graphics LCD,SDHI, CRC, JPEG engine, DRW,IrDA, GPT bus-clock, Standby SRAM
                         + LSHIFT(1,  8) // PCKB[2:0] | Peripheral Module Clock B   | div 8 -> 1 MHz | Up to 60  MHz. IIC, SSI,SRC, DOC, CAC, CAN, DAC12, POEG, CTSU, AGT, ELC, I/OPorts, RTC, WDT, IWDT, ADC12, KINT, USBFS, ACMPHS, TSN, PDC
                         + LSHIFT(1,  4) // PCKC[2:0] | Peripheral Module Clock C   | div 8 -> 1 MHz | Up to 60  MHz. ADC12
                         + LSHIFT(1,  0) // PCKD[2:0] | Peripheral Module Clock D   | div 8 -> 1 MHz | Up to 120 MHz. GPT countclock
    ;
    break;
  }


  R_ROMC->ROMWT_b.ROMWT = 0;       // Установливаем 0 тактов ожидания при доступе к FLASH.

  R_SYSTEM->PLLCR_b.PLLSTP = 1;    // Выключаем PLL
  while (R_SYSTEM->OSCSF_b.PLLSF) __no_operation();   // Ожидаем остановки PLL

  R_SYSTEM->MOSCCR_b.MOSTP = 1;    // Выключаем осциллятор.
  while (R_SYSTEM->OSCSF_b.MOSCSF) __no_operation();  // Ожидаем остановки осциллятора

  R_SYSTEM->HOCOCR_b.HCSTP = 1;    // Выключаем HOCO
  while (R_SYSTEM->OSCSF_b.HOCOSF) __no_operation();  // Ожидаем остановки HOCO

  R_SYSTEM->LOCOCR_b.LCSTP = 1;    // Выключаем LOCO

  while (R_SYSTEM->OPCCR_b.OPCMTSF) __no_operation(); // Ожидать пока закончится переход на новый режим
  R_SYSTEM->OPCCR_b.OPCM = 3;      // 1 1: Low-speed mode.
  while (R_SYSTEM->OPCCR_b.OPCMTSF) __no_operation(); // Ожидать пока закончится переход на новый режим

}

/*-----------------------------------------------------------------------------------------------------
  Переключение на частоту 32768 Гц из режима тактирования от MOCO
  Прерывания в течении выполнения этой функции должны быть запрещены!!!

  Потребление системы в этом режиме - 1.11 мА в зависимости от температуры
-----------------------------------------------------------------------------------------------------*/
void Switch_to_SUBOSC_clock(void)
{
  R_SYSTEM->PRCR = 0xA50B;       // Разрешаем запись в регистр управления тактированием и режимом потребления

  // Установливаем делители в 1 поскольку в режиме SUBOSC другие запрещены
  R_SYSTEM->SCKDIVCR = 0
                       + LSHIFT(0, 28) // FCK[2:0]  | Flash Interface Clock (FCLK)| div 1 -> 1 MHz | 4 to  60  MHz. Flash interface
                       + LSHIFT(0, 24) // ICK[2:0]  | System Clock (ICLK)         | div 1 -> 1 MHz | Up to 240 MHz. CPU, DTC, DMAC, ROM, RAM
                       + LSHIFT(0, 16) // BCK[2:0]  | External Bus Clock (BCLK)   | div 1 -> 1 MHz | Up to 120 MHz. External bus
                       + LSHIFT(0, 12) // PCKA[2:0] | Peripheral Module Clock A   | div 1 -> 1 MHz | Up to 120 MHz. ETHERC,EDMAC, USBHS, QSPI, SPI,SCI, SCE7, Graphics LCD,SDHI, CRC, JPEG engine, DRW,IrDA, GPT bus-clock, Standby SRAM
                       + LSHIFT(0,  8) // PCKB[2:0] | Peripheral Module Clock B   | div 1 -> 1 MHz | Up to 60  MHz. IIC, SSI,SRC, DOC, CAC, CAN, DAC12, POEG, CTSU, AGT, ELC, I/OPorts, RTC, WDT, IWDT, ADC12, KINT, USBFS, ACMPHS, TSN, PDC
                       + LSHIFT(0,  4) // PCKC[2:0] | Peripheral Module Clock C   | div 1 -> 1 MHz | Up to 60  MHz. ADC12
                       + LSHIFT(0,  0) // PCKD[2:0] | Peripheral Module Clock D   | div 1 -> 1 MHz | Up to 120 MHz. GPT countclock
  ;

  R_ROMC->ROMCE_b.ROMCEN = 0;    // Выключаем кэширование FLASH.

  R_SYSTEM->SCKSCR_b.CKSEL = 4;    // Устанавливаем тактирование системы от SUBOSC.  1 0 0: Sub-clock oscillator

  R_SYSTEM->MOCOCR_b.MCSTP = 1;    // Выключаем MOCO осциллятор

  while (R_SYSTEM->SOPCCR_b.SOPCMTSF) __no_operation(); // Ожидать пока закончится переход на новый режим
  R_SYSTEM->SOPCCR_b.SOPCM = 1;                         // Включаем Subosc-speed mode
  while (R_SYSTEM->SOPCCR_b.SOPCMTSF) __no_operation(); // Ожидать пока закончится переход на новый режим

  R_ROMC->ROMCIV_b.ROMCIV = 1;   // Инвалидируем кэш FLASH
  while (R_ROMC->ROMCIV_b.ROMCIV) __no_operation();  // Ждем завершени инвалидации
  R_ROMC->ROMCE_b.ROMCEN =  1;   // Включаем кэширование FLASH
}


/*-----------------------------------------------------------------------------------------------------
  Переход в режим Deep Software Standby
  Ток потребления уменьшается до 58 мкА
  С включенным акселерометром до 66 мкА

  \param void
-----------------------------------------------------------------------------------------------------*/
void Goto_Deep_Software_Standby(void)
{
  volatile uint8_t   dummy;
  UINT               prev_threshold;

  tx_thread_preemption_change(tx_thread_identify(), 0,&prev_threshold);

  Delete_Charger_task();
  //Delete_Net_stack();
  Delete_File_Logger_task();
  Delete_SD_card_file_system();

  __set_PRIMASK(1);   // Запрещаем все прерывания с управлемым приоритетом
  __set_FAULTMASK(1); // Запрещаем все прерывания за исключением NMI

  Set_fast_boot_mode();

  BLUE_LED  = 1;
  RED_LED   = 1;
  GREEN_LED = 1;

  R_USBHS->SYSCFG_b.USBE  = 0; // Выключаем USB HS
  R_USBHS->PHYSET_b.DIRPD = 1; // Перевод USB HS Phy в  low power mode

  Disable_AGT0_AGT1(); // Выключаем таймер поскольку он может дать ложный сигнал в Deep Software Standby mode

  CODEC_PWR_ON     = 0;
  S7G2_SSI_pins_shutdown();

  S7G2_LoRa_pins_shutdown();

  // Полностью выключаем WiFi модуль если он был все еще включен
  BT_REG_ON        = 0;
  WIFI_REG_ON      = 0;
  S7G2_WIFI_pins_shutdown();
  R_SDHI1->SD_CLK_CTRL_b.SCLKEN = 0; // Выключаем тактирование SDIO интерфейса WiFi модуля

  R_SDHI0->SD_CLK_CTRL_b.SCLKEN = 0; // Выключаем тактирование интерфейса SD карты
  S7G2_SDCard_pins_shutdown();
  SD_CARD_PWR_ON   = 0;


  SDRAM_PWR_ON     = 0;
  S7G2_SDRAM_pins_shutdown();

  EXTADC_PWR_ON    = 0;

  // Снижаем частоту до 32768 Гц
  Switch_to_MOCO_clock(0);
  Switch_to_SUBOSC_clock();

  // Установка флагов. Чтобы не препятствовать переходу в Deep Software Standby mode
  // OFS0.IWDTSTPCTL = 1  Option Function Select Register 0            27.2.6. IWDT Start Mode Select. 1: Disable IWDT. Флаг должен быть установлен в области опций Flash в процессе программирования  Flash
  R_SYSTEM->OSTDCR_b.OSTDE      = 0; // Oscillation Stop Detection Control Register 9.2.16. Oscillation Stop Detection Function Enable. 0: Disable oscillation stop detection function
  R_FACI->FENTRYR_b.FENTRYC     = 0; // В документации описание регистра не найдено. Но нужно установить в такое состояние чтобы не мешало переходу в Deep Software Standby mode
  R_FACI->FENTRYR_b.FENTRYD     = 0; // В документации описание регистра не найдено. Но нужно установить в такое состояние чтобы не мешало переходу в Deep Software Standby mode
  R_SYSTEM->LVDnCR0_b[0].RI     = 0; // Voltage Monitor 1 Circuit Control Register 0  8.2.7.  Voltage Monitor 1 Circuit Mode Select. 0: Generate voltage monitor 1 interrupt on Vdet1 passage
  R_SYSTEM->LVDnCR0_b[1].RI     = 0; // Voltage Monitor 2 Circuit Control Register 0  8.2.8.  Voltage Monitor 2 Circuit Mode Select. 0: Generate voltage monitor 2 interrupt on Vdet2 passage

  R_SYSTEM->DSPBYCR_b.DEEPCUT   = 0; // Supply power to the Standby SRAM, low-speed on-chip oscillator, AGTn, and USBFS/USBHS resume detecting unit in Deep Software Standby mode
  R_SYSTEM->DSPBYCR_b.IOKEEP    = 1; // When Deep Software Standby mode is canceled, keep the I/O ports in the same state as in Deep Software Standby mode.

  R_SYSTEM->DPSIER1_b.DIRQ13E   = 1; // Выход из Deep Software Standby mode по прерыванию IMU
  R_SYSTEM->DPSIEGR1_b.DIRQ13EG = 1; // Реагирование на фронт сигнала

  R_SYSTEM->DPSIER1_b.DIRQ15E   = 1; // Выход из Deep Software Standby mode по алярму от внешнего чипа RTC
  R_SYSTEM->DPSIEGR1_b.DIRQ15EG = 0; // Реагирование на спад сигнала

  //  R_SYSTEM->DPSIER1_b.DIRQ11E   = 1; // Выход из Deep Software Standby mode по сигналу детектора уровня аудиокодека
  //  R_SYSTEM->DPSIEGR1_b.DIRQ11EG = 0; // Реагирование на спад сигнала

  dummy = R_SYSTEM->DPSIER0;  // Чтение чтобы прошло 6-ть циклов BCLK
  dummy = R_SYSTEM->DPSIER1;  //
  dummy = R_SYSTEM->DPSIER2;  //

  R_SYSTEM->DPSIER2 = BIT(4); // Enables canceling of Deep Software Standby mode by the NMI pin

  // Очищаем флаги
  dummy = R_SYSTEM->DPSIFR0;
  R_SYSTEM->DPSIFR0 = 0;
  dummy = R_SYSTEM->DPSIFR1;
  R_SYSTEM->DPSIFR1 = 0;
  dummy = R_SYSTEM->DPSIFR2;
  R_SYSTEM->DPSIFR2 = 0;
  dummy = R_SYSTEM->DPSIFR3;
  R_SYSTEM->DPSIFR3 = 0;

  R_SYSTEM->DSPBYCR_b.DEEPCUT = 0; //  Supply power to the Standby SRAM, low-speed on-chip oscillator, AGTn, and USBFS/USBHS resume detecting unit in Deep Software Standby mode
  R_SYSTEM->DSPBYCR_b.IOKEEP  = 0; //  После сброса состояния портов возвращаются в дефолтное состояние


  //    VOUT   VSEL3  VSEL2 VSEL1
  //    2.6    0      0      0
  //    2.7    0      0      1
  //    2.8    0      1      0
  //    2.9    0      1      1
  //    3.0    1      0      0
  //    3.1    1      0      1
  //    3.2    1      1      0
  //    3.3    1      1      1

  //  Снижаем напряжение до 2.8 В. На акселерометре установится 2.5 V
  VSEL3 = 0;
  VSEL2 = 1;
  VSEL1 = 0;


  // Переход в режим Deep Software Standby mode
  R_SYSTEM->SBYCR_b.SSBY    = 1;
  R_SYSTEM->DSPBYCR_b.DPSBY = 1;
  __DSB();
  __WFI();

}

/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
void Goto_power_off(void)
{
  UINT               prev_threshold;  
  tx_thread_preemption_change(tx_thread_identify(), 0,&prev_threshold);

  Delete_Charger_task();
  Delete_File_Logger_task();
  Delete_SD_card_file_system();


  BQ25619_sw_off_accum();
}

