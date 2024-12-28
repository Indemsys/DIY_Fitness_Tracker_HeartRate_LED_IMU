// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 2024-11-27
// 18:33:37
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include   "Test.h"

#include   "MKR_RGB_Shield_specifications.h"
#include   "MKR_RGB_Shield_resources.h"

static GX_WINDOW              *screen_main;
static TX_MUTEX                screen_mutex;



#define ID_SCREEN_TXT  0

/*-----------------------------------------------------------------------------------------------------



  \return uint16_t
-----------------------------------------------------------------------------------------------------*/
uint16_t Get_screen_text_with(void)
{
  int16_t          str_width;
  GX_FONT         *font;
  gx_widget_font_get((GX_WIDGET *)&window_main.window_main_txt, GX_FONT_ID_MYFONT3X5_2,&font);
  gx_system_string_width_get_ext(font,&gx_strings[ID_SCREEN_TXT],&str_width);
  return str_width;
}



/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
void Init_main_screen(void)
{
  char *screen_str = (char *)gx_strings[ID_SCREEN_TXT].gx_string_ptr;

  if (screen_main == 0)
  {
    gx_studio_named_widget_create("window_main" , 0, (GX_WIDGET **)&screen_main);
    screen_str[0] = 0;
    gx_strings[ID_SCREEN_TXT].gx_string_length = 0;
    tx_mutex_create(&screen_mutex, "Screen Mutex", TX_INHERIT);
  }

  gx_widget_attach((GX_WIDGET *)root, (GX_WIDGET *)screen_main);
}


/*-----------------------------------------------------------------------------------------------------


  \param str_id
  \param fmt

  \return GX_STRING*
-----------------------------------------------------------------------------------------------------*/
GX_STRING* MKR_RGB_Shield_print_str(const char *fmt, ...)
{
  va_list          ap;
  GX_STRING       *p = 0;
  uint32_t         rgb_color;

  va_start(ap, fmt);
  tx_mutex_get(&screen_mutex, TX_WAIT_FOREVER);
  vsnprintf(gui_strings[ID_SCREEN_TXT], GX_STRING_MAX_LEN, (const char *)fmt, ap);
  gx_strings[ID_SCREEN_TXT].gx_string_length = strlen(gui_strings[ID_SCREEN_TXT]);
  p = &gx_strings[ID_SCREEN_TXT];
  rgb_color = Convert_string_to_RGB_code((char const*)wvar.led_matrix_text_color);

  Set_custom_color(g_display, GX_COLOR_ID_CUSTOM_COLOR, rgb8888_to_rgb565(rgb_color));
  gx_prompt_text_color_set(&window_main.window_main_txt, GX_COLOR_ID_CUSTOM_COLOR, GX_COLOR_ID_CUSTOM_COLOR, GX_COLOR_ID_CUSTOM_COLOR);
  Restart_screen_moving();

  tx_mutex_put(&screen_mutex);
  va_end(ap);
  return p;
}

/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
static void Show_main_screen(void)
{
  tx_mutex_get(&screen_mutex, TX_WAIT_FOREVER);
  gx_prompt_text_set_ext(&window_main.window_main_txt  ,&gx_strings[ID_SCREEN_TXT]);
  tx_mutex_put(&screen_mutex);
}

/*-----------------------------------------------------------------------------------------------------
  Функция обработки пользовательского ввода
  Вызывается каждые 100 мс

-----------------------------------------------------------------------------------------------------*/
static uint32_t _User_input_processing(void)
{

  return 1;
}


/*-----------------------------------------------------------------------------------------------------


  \param window
-----------------------------------------------------------------------------------------------------*/
VOID Main_window_draw_callback(GX_WINDOW *window)
{
  gx_window_draw(window);
  gx_widget_children_draw(window);
}


/*-----------------------------------------------------------------------------------------------------


  \param window
  \param event_ptr

  \return UINT
-----------------------------------------------------------------------------------------------------*/
UINT Main_window_event_callback(GX_WINDOW *window, GX_EVENT *event_ptr)
{
  UINT status;

  switch (event_ptr->gx_event_type)
  {
  case GX_EVENT_SHOW:
    gx_system_timer_start((GX_WIDGET *)window, USER_INPUT_PROC_TIMER_ID, USER_INPUT_PROC_INTIT_TICKS, USER_INPUT_PROC_PERIOD_TICKS);
    status = gx_window_event_process(window, event_ptr);
    break;

  case GX_EVENT_TIMER:
    if (event_ptr->gx_event_payload.gx_event_timer_id == USER_INPUT_PROC_TIMER_ID)
    {
      if (_User_input_processing()) Show_main_screen();
    }
    break;

  default:
    status = gx_window_event_process(window, event_ptr);
    return status;
  }

  return 0;
}

/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
void Stop_main_screen(void)
{
  gx_system_timer_stop((GX_WIDGET *)screen_main, USER_INPUT_PROC_TIMER_ID);
  MKR_RGB_clear_screen();
  LED_MATRIX_EN = 0;
}

