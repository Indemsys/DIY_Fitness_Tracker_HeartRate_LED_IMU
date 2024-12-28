// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 2023-04-09
// 17:56:39
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include   "Test.h"


#include   "gx_display.h"
#include   "MKR_RGB_Shield_specifications.h"
#include   "MKR_RGB_Shield_resources.h"

extern GX_STUDIO_DISPLAY_INFO MKR_RGB_Shield_display_table[];

uint16_t               *video_buffer;
uint32_t               video_buffer_size;

GX_WINDOW_ROOT         *root;

char                    gui_strings[GX_STRINGS_NUMBER][GX_STRING_MAX_LEN];
GX_STRING               gx_strings[GX_STRINGS_NUMBER];

static uint8_t          init_done;

int16_t                 screen_str_width;
int16_t                 screen_offset;
uint8_t                 screen_mov_dir;
int32_t                 screen_scroll_num;

GX_DISPLAY              *g_display;
/*-----------------------------------------------------------------------------------------------------


  \param v
-----------------------------------------------------------------------------------------------------*/
void Restart_screen_moving(void)
{
  screen_str_width   = Get_screen_text_with();
  screen_offset      = 0;
  screen_mov_dir     = 0;
  screen_scroll_num  = 0;
}

/*-----------------------------------------------------------------------------------------------------


  \param void

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
int32_t Get_screen_scroll_num(void)
{
  return screen_scroll_num;
}

/*-----------------------------------------------------------------------------------------------------


  \param str

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t Convert_string_to_RGB_code(const char *str)
{
  uint32_t result = 0;
  uint8_t bytes[4] = {0};
  char *end_ptr;

  for (int i = 0; i < 3; i++)
  {
    long value = strtol(str,&end_ptr, 10);
    // Если не удалось извлечь число, подставляем 1
    if (str == end_ptr)
    {
      value = 1;
    }
    // Проверка на диапазон значений
    if (value < 0 || value > 255)
    {
      return 0;       // Ошибка, недопустимое значение
    }
    bytes[i] = (uint8_t)value;

    // Перейти к следующему числу, пропустив пробелы
    str = end_ptr;
    while (*str == ' ')
    {
      str++;
    }
  }
  // Собираем итоговое значение из байтов
  result = (bytes[0] << 16) | (bytes[1] << 8) | (bytes[2] << 0);
  return result;
}

/*-----------------------------------------------------------------------------------------------------
   Функция для установки цвета RGB в таблице по индексу

  \param display
  \param color_id
  \param rgb_color

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t Set_custom_color(GX_DISPLAY *display, GX_UBYTE color_id, GX_COLOR rgb_color)
{
  if (display->gx_display_color_table == NULL || color_id >= display->gx_display_color_table_size)
  {
    return GX_FAILURE;     // Ошибка: недопустимый индекс или таблица недоступна
  }

  // Записываем новый цвет в таблицу
  display->gx_display_color_table[color_id] = rgb_color;
  return GX_SUCCESS;
}


/*-----------------------------------------------------------------------------------------------------
  Функция для преобразования числа в RGB цвет в формате 8-8-8-8

  \param number
  \param range
  \param alpha

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
static uint32_t Convert_number_to_rgb_color_circle(int number, int range, uint8_t alpha)
{
    if (number < 0 || number >= range || range <= 0)
    {
        // Если число вне диапазона или диапазон некорректен, возвращаем чёрный цвет с полной прозрачностью
        return 0x00000000;
    }

    // Нормализуем число в диапазон [0, 1]
    float normalized = (float)number / (range - 1);

    // Hue от 0 до 1
    float hue = normalized;
    float saturation = 1.0f; // Насыщенность
    float value = 1.0f;      // Яркость

    // Преобразование из HSV в RGB
    float r, g, b;
    int i = (int)(hue * 6);
    float f = (hue * 6) - i;
    float p = value * (1.0f - saturation);
    float q = value * (1.0f - f * saturation);
    float t = value * (1.0f - (1.0f - f) * saturation);

    switch (i % 6)
    {
        case 0: r = value; g = t; b = p; break;
        case 1: r = q; g = value; b = p; break;
        case 2: r = p; g = value; b = t; break;
        case 3: r = p; g = q; b = value; break;
        case 4: r = t; g = p; b = value; break;
        case 5: r = value; g = p; b = q; break;
        default: r = g = b = 0; break; // Этот случай не должен наступить
    }

    // Преобразуем RGB значения в диапазон 0-255
    uint8_t R = (uint8_t)(r * 255);
    uint8_t G = (uint8_t)(g * 255);
    uint8_t B = (uint8_t)(b * 255);

    // Собираем результат в формате 0xAARRGGBB
    return (alpha << 24) | (R << 16) | (G << 8) | B;
}

/*-----------------------------------------------------------------------------------------------------


  \param color

  \return uint16_t
-----------------------------------------------------------------------------------------------------*/
uint16_t rgb8888_to_rgb565(uint32_t color)
{
    // Извлечение компонентов RGB из 32-битного цвета
    uint8_t red = (color >> 16) & 0xFF;
    uint8_t green = (color >> 8) & 0xFF;
    uint8_t blue = (color >> 0) & 0xFF;

    // Преобразование в формат RGB565
    uint16_t r5 = (red >> 3) & 0x1F;      // 5 бит для красного
    uint16_t g6 = (green >> 2) & 0x3F;    // 6 бит для зелёного
    uint16_t b5 = (blue >> 3) & 0x1F;     // 5 бит для синего

    return (r5 << 11) | (g6 << 5) | b5;
}

/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
static void Convert_to_rgb_shield_buf(void)
{
  uint16_t rgb565;
  uint32_t rgba8888;
  uint32_t led_pixel_indx = 0;

  uint8_t red;
  uint8_t green;
  uint8_t blue;

  uint8_t red8;
  uint8_t green8;
  uint8_t blue8;

  // Конвертируем видеобуфер video_buffer в буфер LED матрицы mkr_rgb_shield_buf и выводим на LED матрицу
  for (uint32_t raw = 0; raw < DISPLAY_120_28_Y_RESOLUTION; raw++)
  {
    if (raw >= MKR_RGB_SHIELD_ROWS) break;
    for (uint32_t col = screen_offset; col < (MKR_RGB_SHIELD_COLS + screen_offset); col++)
    {
      if (col >= DISPLAY_120_28_X_RESOLUTION) break;
      rgb565 = video_buffer[col + raw * DISPLAY_120_28_X_RESOLUTION];

      if ((rgb565!=0) && (g_display_mode == 1))
      {
        rgba8888 = Convert_number_to_rgb_color_circle(raw, 7, 0xE0 + wvar.led_matrix_intensity);
      }
      else
      {
        // Извлечение значений из RGB-565
        red   = (rgb565 >> 11) & 0x1F; // 5 бит красного
        green = (rgb565 >> 5) & 0x3F; // 6 бит зелёного
        blue  = rgb565 & 0x1F;         // 5 бит синего

        // Преобразование значений в диапазон 0–255
        red8   = (red << 3) | (red >> 2);    // 5 бит -> 8 бит
        green8 = (green << 2) | (green >> 4); // 6 бит -> 8 бит
        blue8  = (blue << 3) | (blue >> 2);  // 5 бит -> 8 бит

        rgba8888 = 0xE0000000 | (wvar.led_matrix_intensity << 24) | (red8 << 16) | (green8 << 8) | blue8;
      }

      mkr_rgb_shield_buf.data[led_pixel_indx++] = rgba8888;
    }
  }
}

/*-----------------------------------------------------------------------------------------------------
  Переносим весь буфер на дисплей

  \param canvas
  \param dirty
-----------------------------------------------------------------------------------------------------*/
static void _565rgb_buffer_toggle(GX_CANVAS *canvas, GX_RECTANGLE *dirty)
{

  if (init_done)
  {
    Convert_to_rgb_shield_buf();
    SPI1_MKR_RGB_Shield_wr_buff_to_display((uint32_t *)&mkr_rgb_shield_buf, sizeof(mkr_rgb_shield_buf));


    // Прокрутка текста на дисплее в случае если его пиксельная длина превышает длину количество точек у дисплея по горизонтали
    if (screen_str_width > MKR_RGB_SHIELD_COLS)
    {
      if (screen_mov_dir == 0)
      {
        screen_offset++;
        if (screen_offset > (screen_str_width - MKR_RGB_SHIELD_COLS))
        {
          screen_mov_dir ^= 1;
          screen_scroll_num++;
        }
      }
      else
      {
        screen_offset--;
        if (screen_offset == 0)
        {
          screen_mov_dir ^= 1;
          screen_scroll_num++;
        }
      }
    }
    else
    {
      screen_scroll_num = -1;
    }

  }
}

/*-----------------------------------------------------------------------------------------------------
  Вызывается в течении вызова функции gx_studio_display_configure

  \param display

  \return UINT
-----------------------------------------------------------------------------------------------------*/
static UINT _565rgb_driver_setup(GX_DISPLAY *display)
{
  g_display = display;
  _gx_display_driver_565rgb_setup(display, (VOID *)SCREEN_HANDLE, _565rgb_buffer_toggle);

  MKR_RGB_Shield_init();

  init_done = 1;
  return (GX_SUCCESS);
}

/*-----------------------------------------------------------------------------------------------------


  \param void

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t Get_switch_press_signal(void)
{
  return 0;
}

/*-----------------------------------------------------------------------------------------------------


  \param void

  \return uint8_t*
-----------------------------------------------------------------------------------------------------*/
uint16_t* Get_HMI_videobuffer_ptr(void)
{
  return video_buffer;
}

/*-----------------------------------------------------------------------------------------------------


  \param width
  \param height
-----------------------------------------------------------------------------------------------------*/
void Get_HMI_videobuffer_W_H(uint32_t *width, uint32_t *height)
{
  *width  = MKR_RGB_Shield_display_table[0].x_resolution;
  *height = MKR_RGB_Shield_display_table[0].y_resolution;
}

/*-----------------------------------------------------------------------------------------------------


  \param arg
-----------------------------------------------------------------------------------------------------*/
void GuiX_start(void)
{
  uint32_t       actual_flags;

  if (ivar.en_wifi_module)
  {
    // Ожидаем окончания инициализации WiFi модуля, поскольку его инициализация мешает выводу на дисплей
    Wait_wifi_event(NET_EVT_WIFI_AP_READY,&actual_flags, 5000);
  }

  video_buffer_size = DISPLAY_120_28_X_RESOLUTION * DISPLAY_120_28_Y_RESOLUTION * 2;
  video_buffer      = App_malloc(video_buffer_size);

  gx_system_initialize();

  for (uint32_t i = 0; i < GX_STRINGS_NUMBER; i++)
  {
    gx_strings[i].gx_string_ptr    = gui_strings[i];
    gx_strings[i].gx_string_length = 0;
  }

  MKR_RGB_Shield_display_table[0].canvas_memory =  (ULONG *)video_buffer;
  gx_studio_display_configure(DISPLAY_120_28, _565rgb_driver_setup, LANGUAGE_ENGLISH, DISPLAY_120_28_THEME_1,&root);

  Init_main_screen();

  gx_widget_show(root);
  gx_system_start();

}


