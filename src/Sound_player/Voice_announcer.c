// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 2020.07.13
// 16:29:15
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include   "Test.h"
#include   "Voice_announcer.h"

static uint32_t en_voice_announcer;
static uint32_t sound_freq  = 440;


uint32_t Get_en_voice_announcer(void)                   { return  en_voice_announcer;}
void     Set_en_voice_announcer(uint8_t val)            {  en_voice_announcer = val; }


const T_sound_file    sound_files_map[] =
{
  {SND_0                         , "snd_0.wav"                         },
  {SND_1                         , "snd_1.wav"                         },
  {SND_2                         , "snd_2.wav"                         },
  {SND_3                         , "snd_3.wav"                         },
  {SND_4                         , "snd_4.wav"                         },
  {SND_5                         , "snd_5.wav"                         },
  {SND_6                         , "snd_6.wav"                         },
  {SND_7                         , "snd_7.wav"                         },
  {SND_8                         , "snd_8.wav"                         },
  {SND_9                         , "snd_9.wav"                         },
  {SND_GONG                      , "snd_gong.wav"                      },
  {SND_ALARM                     , "snd_Alarm.wav"                     },
  {SND_SENSOR                    , "snd_Sensor.wav"                    },
  {SND_SILENCE                   , "snd_Silence.wav"                   },
  {MISC_SOUND                    , "misc.wav"                          },
};

const T_vannouncer_item vannouncer_map[] =
{
  { VOAN_MSG_ERROR_,                Play_error_msg                , "ERROR MESSAGE       " },  // 0
  { VOAN_MSG_SENSOR_NUMBER,         Play_msg_sensor_number        , "SENSOR NUMBER       " },  // 2
  { VOAN_MSG_ALARM                , Play_msg_alarm                , "ALARM MESSAGE       " },  // 10
};

#define  SIZEOF_SOUNDS_MAP   (sizeof(vannouncer_map)/ sizeof(vannouncer_map[0]) )

/*------------------------------------------------------------------------------
  Обработка команд приходящих по CAN
 ------------------------------------------------------------------------------*/
void VAnnouncer_messages_processing(const uint8_t *buf)
{
  if (buf[0] == VOAN_CMD_SETT)
  {
    switch (buf[1])
    {
    case VOAN_EN_VOICE_ANNOUNCER :
      memcpy(&en_voice_announcer,&buf[2], 4);
      LOGs(__FUNCTION__, __LINE__, 0, "Voice announcer mode = %d", en_voice_announcer);
      break;
    case VOAN_SOUND_VOLUME  :
      {
        uint32_t sound_volume;
        memcpy(&sound_volume,&buf[2], 4);
        Player_Set_Volume(sound_volume);
        LOGs(__FUNCTION__, __LINE__, 0, "Sound attenuation = %d", sound_volume );
      }
      break;
    case VOAN_VOICE_LANGUAGE:
      memcpy(&ply.voice_language,&buf[2], 4);
      LOGs(__FUNCTION__, __LINE__, 0, "Voice laguage = %d", ply.voice_language);
      break;

    }
  }
  else if (buf[0] == VOAN_CMD_PLAY)
  {
    uint32_t msg_num;
    uint32_t msg_arg;

    msg_num = buf[1];
    memcpy(&msg_arg,&buf[2], 4);
    Play_msg_by_num(msg_num, msg_arg);
    LOGs(__FUNCTION__, __LINE__, 0, "Play command = %d, arg = %d", msg_num, msg_arg);
  }
  else if (buf[0] == VOAN_CMD_SOUND_FREQ)
  {
    memcpy(&sound_freq,&buf[1], 4);
    Player_Set_Tone_freq(sound_freq);
    LOGs(__FUNCTION__, __LINE__, 0, "Sound frequency = %d", sound_freq);
  }
  else if (buf[0] == VOAN_CMD_SOUND_GEN)
  {
    Player_Play_tone(sound_freq);
    LOGs(__FUNCTION__, __LINE__, 0, "Play sound %d", Player_Get_Tone_freq());
  }
  else if (buf[0] == VOAN_CMD_STOP)
  {
    Player_Stop();
    LOGs(__FUNCTION__, __LINE__, 0, "Stop play");
  }
}


/*------------------------------------------------------------------------------

 ------------------------------------------------------------------------------*/
void Play_msg_by_num(unsigned int  msg_num, int msg_arg)
{
  unsigned int i;
  unsigned int n = sizeof(vannouncer_map) / sizeof(vannouncer_map[0]);

  for (i = 0; i < n; i++)
  {
    if (vannouncer_map[i].msg_id == msg_num)
    {
      vannouncer_map[i].play_func(msg_arg);
      return;
    }
  }
}



/*------------------------------------------------------------------------------
    Английская версия
 ------------------------------------------------------------------------------*/
void Play_error_msg(int num)
{
  if (en_voice_announcer)
  {
    Player_Enqueue_file(SND_GONG);
    Player_Enqueue_file(SND_SILENCE);
    Player_Enqueue_file(SND_SILENCE);
    Player_Enqueue_file((num / 100)% 10);
    Player_Enqueue_file(SND_SILENCE);
    Player_Enqueue_file((num / 10)% 10);
    Player_Enqueue_file(SND_SILENCE);
    Player_Enqueue_file(num % 10);
    Player_Enqueue_file(SND_SILENCE);
  }
}


/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
void Play_msg_alarm(int num)
{
  if (en_voice_announcer)
  {
    Player_Enqueue_file(SND_ALARM);
  }
  else
  {
    Player_Play_tone(440);
  }
  Player_Delayed_Stop();
}


/*------------------------------------------------------------------------------


 ------------------------------------------------------------------------------*/
void Play_msg_sensor_number(int num)
{
  if (en_voice_announcer)
  {
    Player_Enqueue_file(SND_SILENCE);
    Player_Enqueue_file(SND_SENSOR);
    Player_Enqueue_file(SND_SILENCE);
    Player_Enqueue_file(SND_SILENCE);
    Player_Enqueue_file((num / 10)% 10);
    Player_Enqueue_file(SND_SILENCE);
    Player_Enqueue_file(num % 10);
    Player_Enqueue_file(SND_SILENCE);
  }
}


/*-----------------------------------------------------------------------------------------------------


  \param sid

  \return const char*
-----------------------------------------------------------------------------------------------------*/
const char *Get_sound_description(uint32_t sid)
{
  if (sid>=SIZEOF_SOUNDS_MAP )
  {
    return "Undefined";
  }

  return vannouncer_map[sid].description;

}




