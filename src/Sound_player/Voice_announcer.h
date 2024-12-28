#ifndef VOICE_ANNOUNCER_H
  #define VOICE_ANNOUNCER_H


  #define SND_0                         1
  #define SND_1                         2
  #define SND_2                         3
  #define SND_3                         4
  #define SND_4                         5
  #define SND_5                         6
  #define SND_6                         7
  #define SND_7                         8
  #define SND_8                         9
  #define SND_9                         10
  #define SND_GONG                      11
  #define SND_ALARM                     12
  #define SND_SENSOR                    13
  #define SND_SILENCE                   14
  #define MISC_SOUND                    15


// data[0] - Тип команды
  #define VOAN_CMD_SETT                    0 // Настройка параметров проигрывателя
// data[1] - Номер параметра. data[2..5] - значение параметра 4 байта
  #define VOAN_EN_VOICE_ANNOUNCER          2 // Установка значения переменной en_voice_announcer
  #define VOAN_SOUND_VOLUME                3 // Установка значения переменной sound_volume
  #define VOAN_VOICE_LANGUAGE              4 // Установка значения переменной voice_language




// data[0] - Тип команды
  #define VOAN_CMD_PLAY      1 // Команда на проигрывание определенного сообщения
// data[1] - номер сообщения, data[2..5] - аргумент сообщения 4 байта
  #define VOAN_MSG_ERROR_                  0 //
  #define VOAN_MSG_SENSOR_NUMBER           2 //
  #define VOAN_MSG_ALARM                   16

  #define VOAN_LAST_MSG_ID                 17 //


// data[0] - Тип команды
  #define VOAN_CMD_SOUND_FREQ     2 // Установка частоты тона
// data[1...4] - частота тона

// data[0] - Тип команды
  #define VOAN_CMD_SOUND_GEN      3 // Команда на генерацию тона

// data[0] - Тип команды
  #define VOAN_CMD_STOP           4 // Команда на отсновку генерации звука

void        Play_msg_by_num(unsigned int  msg_num, int msg_arg);
void        Play_error_msg(int num);
void        Play_msg_sensor_number(int num);
void        Play_msg_alarm(int num);

void        VAnnouncer_messages_processing(const uint8_t *buf);

uint32_t    Get_en_voice_announcer(void);
void        Set_en_voice_announcer(uint8_t val);

const char* Get_sound_description(uint32_t sid);


#endif



