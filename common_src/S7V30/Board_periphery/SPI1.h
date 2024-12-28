#ifndef S7V30__SPI1_H
  #define S7V30__SPI1_H

#define CPOL  0   // RSPCK Polarity Setting
#define CPHA  0   // RSPCK Phase Setting

#define FLAG_SPI1_IDLE         BIT(0)
#define FLAG_SPI1_TRANSFER_END BIT(1)


extern TX_EVENT_FLAGS_GROUP           spi1_flags;


void     spi1_rxi_isr(void);
void     spi1_txi_isr(void);
void     spi1_tei_isr(void);
void     spi1_eri_isr(void);
void     spi1_idle_isr(void);
void     spi1_init_interrupts(void);

extern void    Set_LCD_CS(int32_t v);

uint32_t SPI1_init(void);
void     SPI1_set_spbr(uint8_t val);
uint32_t SPI1_set_for_byte_transfer_to_display(void);
uint32_t SPI1_set_for_buffer_transfer_to_display(void);
uint32_t SPI1_send_byte_to_display(uint8_t b);
uint32_t SPI1_send_buff_to_display(uint16_t *buf, uint32_t sz);

// Макросы объявлены здесь
// поскольку при записи их в С файле перестает нормально работать броузинг кода в редакторе
#ifdef S7V30_spi1


//........................................................................................
//  Организация канала передачи для spi1
//  Используется модуль DMAC канал 6
//........................................................................................
SSP_VECTOR_DEFINE_CHAN(dmac_int_isr  , DMAC , INT  , 6)

SSP_VECTOR_DEFINE_CHAN(spi1_rxi_isr  ,  SPI , RXI  , 1)
//SSP_VECTOR_DEFINE_CHAN(spi1_txi_isr  ,  SPI , TXI  , 0) // Если объявить этот вектор, то после инициализации ICU не будут передаваться запросы на DMA
SSP_VECTOR_DEFINE_CHAN(spi1_tei_isr  ,  SPI , TEI  , 1)
SSP_VECTOR_DEFINE_CHAN(spi1_eri_isr  ,  SPI , ERI  , 1)
SSP_VECTOR_DEFINE_CHAN(spi1_idle_isr ,  SPI , IDLE , 1)
#endif

#endif



