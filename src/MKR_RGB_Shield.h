#ifndef MKR_RGB_SHIELD_H
  #define MKR_RGB_SHIELD_H


#define  MKR_RGB_SHIELD_ROWS  7
#define  MKR_RGB_SHIELD_COLS  12
#define  MKR_RGB_SHIELD_LEDS_NUM (MKR_RGB_SHIELD_ROWS*MKR_RGB_SHIELD_COLS)


typedef struct
{
    uint32_t  header;
    uint32_t  data[MKR_RGB_SHIELD_LEDS_NUM];
    uint32_t  footer;

} T_mkr_rgb_shield_buf;

extern T_mkr_rgb_shield_buf mkr_rgb_shield_buf;

void     MKR_RGB_Shield_init(void);
void     MKR_RGB_Fill_const(uint32_t val);
uint32_t SPI1_MKR_RGB_Shield_wr_buff_to_display(uint32_t *buf, uint32_t sz);
void     MKR_RGB_clear_screen(void);


#endif // MKR_RGB_SHIELD_H



