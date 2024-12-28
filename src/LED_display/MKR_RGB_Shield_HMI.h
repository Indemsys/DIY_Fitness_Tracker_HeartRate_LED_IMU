#ifndef MKR_RGB_SHIELD_HMI_H
  #define MKR_RGB_SHIELD_HMI_H



    #include "gx_api.h"

    #define  SCREEN_HANDLE                0x12345679

    #define  GX_STRINGS_NUMBER            2
    #define  GX_STRING_MAX_LEN            128

    #define USER_INPUT_PROC_TIMER_ID      20
    #define USER_INPUT_PROC_INTIT_TICKS   7   // Величина выражается в тиках GX_SYSTEM_TIMER_MS
    #define USER_INPUT_PROC_PERIOD_TICKS  7   // Величина выражается в тиках GX_SYSTEM_TIMER_MS


    #define MENU_ITEM_WINDOW   1
    #define MENU_ITEM_COMMAND  2
    #define MENU_ITEM_RETURN   3


extern char                    gui_strings[GX_STRINGS_NUMBER][GX_STRING_MAX_LEN];
extern GX_STRING               gx_strings[GX_STRINGS_NUMBER];
extern GX_WINDOW_ROOT         *root;
extern uint16_t               *video_buffer;
extern GX_DISPLAY             *g_display;

void       GuiX_start(void);
uint32_t   Get_switch_press_signal(void);
uint16_t*  Get_HMI_videobuffer_ptr(void);
void       Get_HMI_videobuffer_W_H(uint32_t *width, uint32_t *height);
void       Restart_screen_moving(void);
int32_t    Get_screen_scroll_num(void);
uint32_t   Convert_string_to_RGB_code(const char *str);
uint32_t   Set_custom_color(GX_DISPLAY *display, GX_UBYTE color_id, GX_COLOR rgb_color);
uint16_t   rgb8888_to_rgb565(uint32_t color);
    #include "MKR_RGB_Shield_Screen.h"


#endif



