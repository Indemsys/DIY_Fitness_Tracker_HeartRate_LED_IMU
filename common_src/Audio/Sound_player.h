#ifndef SOUND_PLAYER_H
  #define SOUND_PLAYER_H

  #define DAC_MID_LEVEL      (4096/2)

  #define DEALAYED_STOP_TIMEOUT_MS    1000


  #define EVT_PLAY_SOUND             BIT( 1 )
  #define EVT_READ_NEXT              BIT( 2 )
  #define EVT_STOP_PLAY              BIT( 3 )
  #define EVT_PLAY_FILE              BIT( 4 )
  #define EVT_DELAYED_STOP           BIT( 5 )
  #define EVT_MSG_IN_QUEUE           BIT( 6 )

  #define PLAYER_IDLE           0
  #define PLAYING_SOUND         1
  #define PLAYING_FILE          2

  #define PLAY_QUEUE_SZ         32
  #define MAX_FILE_PATH_LEN     128

  #define MAX_SOUND_VOLUME 10

  #define ANNCR_ENG  0
  #define ANNCR_RUS  1
  #define ANNCR_NOR  2
  #define ANNCR_DER  3
  #define ANNCR_SWE  4

  #define DIR_ENG  "SOUNDS\\ENG\\"
  #define DIR_RUS  "SOUNDS\\RUS\\"
  #define DIR_NOR  "SOUNDS\\NOR\\"
  #define DIR_DER  "SOUNDS\\DER\\"
  #define DIR_SWE  "SOUNDS\\SWE\\"


typedef struct
{
    uint8_t       mode;       // Режим работы плеера. 0- неактивен, 1 - воспроизведение тона, 2- воспроизведение файла
    uint8_t       bank;       // Индекс текущего сэмпла
    uint32_t      tone_freq;  // Частота тона в Гц
    uint32_t      sample_rate;
    int16_t      *file_ptr;   // Указатель на данные файла
    char          file_name[MAX_FILE_PATH_LEN+1];  // Имя файла
    FX_FILE       sound_file;
    int16_t       tmp_audio_buf[AUDIO_BUF_SAMPLES_NUM];
    uint32_t      block_sz;   // Размер прочитанного их файл блока данных
    uint8_t       file_end;   // Флаг завершения воспроизведения файла

    uint8_t       attenuation;
    uint32_t      voice_language;

    int32_t       play_queue[PLAY_QUEUE_SZ]; //  Очередь на воспроизведение файлов
    int32_t       queue_head;
    int32_t       queue_tail;
    int32_t       queue_sz;

} T_sound_player;

typedef struct
{
    uint8_t data[8];
} T_player_msg;


typedef struct
{
    uint32_t           sid;
    const char        *file_name;

} T_sound_file;

typedef void (*T_play_func)(int num);

typedef struct
{
    uint32_t msg_id;
    T_play_func play_func;
    const char *description;

} T_vannouncer_item;


extern T_sound_player    ply;


uint32_t     Create_Sound_Player_task(void);
uint32_t     Player_Send_msg_to_queue(T_player_msg *player_msg);
uint32_t     Player_Get_msg_from_queue(T_player_msg *player_msg);
void         Player_Set_MUTE(uint8_t state);

void         Player_Set_Volume(uint32_t v);
uint32_t     Player_Get_Volume(void);

void         Player_Set_Language(uint32_t v);
uint32_t     Player_Get_Language(void);

void         Player_Set_Tone_freq(uint32_t v);
uint32_t     Player_Get_Tone_freq(void);


void         Player_Play_tone(uint32_t tone_freq);
void         Player_Stop(void);
void         Player_Delayed_Stop(void);
void         Player_Enqueue_file(int file);

unsigned int Player_Get_wave_file_sample_rate(void);
void         Player_Set_evt_to_read_next_block(void);
void         Player_CANrx_play(uint32_t can_id, uint8_t msg_len, uint8_t *data);
uint32_t     Player_Get_state(void);

const char*  VAnnouncer_get_files_dir(void);
uint32_t     Announcer_Get_language_num(void);
void         Announcer_Set_language_num(uint32_t val);
char        *Announcer_Get_language(void);

#endif
