#ifndef MKR_RGB_SHIELD_SCREEN_H
  #define MKR_RGB_SHIELD_SCREEN_H


void       Init_main_screen(void);
GX_STRING* MKR_RGB_Shield_print_str(const char *fmt, ...);
void       Stop_main_screen(void);
uint16_t   Get_screen_text_with(void);

#endif // MKR_RGB_SHIELD_SCREEN_H



