// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 2024-11-12
// 21:11:51
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "S7V30.h"


typedef struct
{
    uint8_t          active;
    uint32_t         init_state;
    T_sys_timestump  lastt;               // Время последнего вызова автомата состояний
    uint32_t         duration;            // Длительность состояний в мс
    int32_t          *pattern_start_ptr;  // Указатель на массив констант являющийся цепочкой состояний (шаблоном)
                                          // Если значение в массиве = 0xFFFFFFFF, то процесс обработки завершается
    // Если значение в массиве = 0x00000000, то вернуть указатель на начало цепочки
    int32_t          *pttn_ptr;           // Текущая позиция в цепочке состояний
    uint8_t          repeat;              // Флаг принудительного повторения сигнала

} T_outs_ptrn;


static T_outs_ptrn outs_cbl[S7V30_OTPUTS_NUM];


#define __ON   0
#define _OFF   1

//  Шаблон состоит из массива груп слов.
//  Первое слово в группе - значение напряжения
//  Второе слово в группе - длительность интервала времени в мс или специальный маркер остановки(0xFFFFFFFF) или цикла(0x00000000)


const int32_t   OUT_FAST_BLINK[] =
{
  __ON, 10,
  _OFF, 190,
  0, 0
};

const int32_t   OUT_SLOW_BLINK[] =
{
  __ON, 10,
  _OFF, 1000,
  0, 0
};

const int32_t   OUT_ON[] =
{
  __ON, 10,
  __ON, 0xFFFFFFFF
};

const int32_t   OUT_OFF[] =
{
  _OFF, 10,
  _OFF, 0xFFFFFFFF
};


/*-----------------------------------------------------------------------------------------------------


  \param val
-----------------------------------------------------------------------------------------------------*/
void Output_OFF(uint32_t val)
{
  S7V30_set_output_pattern(OUT_OFF, val);
}

/*-----------------------------------------------------------------------------------------------------


  \param val
-----------------------------------------------------------------------------------------------------*/
void Output_ON(uint32_t val)
{
  S7V30_set_output_pattern(OUT_ON, val);
}

/*-----------------------------------------------------------------------------------------------------


  \param val
-----------------------------------------------------------------------------------------------------*/
void Output_SLOW_BLINK(uint32_t val)
{
  S7V30_set_output_pattern(OUT_SLOW_BLINK, val);
}

/*-----------------------------------------------------------------------------------------------------


  \param val
-----------------------------------------------------------------------------------------------------*/
void Output_FAST_BLINK(uint32_t val)
{
  S7V30_set_output_pattern(OUT_FAST_BLINK, val);
}


/*------------------------------------------------------------------------------



 \param num - порядковый номер выхода (1..19)
 \param val - лог. уровень выхода
 ------------------------------------------------------------------------------*/
static void _Set_output_state(uint8_t num, uint8_t val)
{
  if (num >= S7V30_OTPUTS_NUM) return;
  switch (num)
  {
  case S7V30_RED_LED:
    RED_LED = val;
    break;
  case S7V30_GREEN_LED:
    GREEN_LED = val;
    break;
  case S7V30_BLUE_LED:
    BLUE_LED = val;
    break;
  }
}


/*-----------------------------------------------------------------------------------------------------
  Инициализация шаблона для машины состояний выходного сигнала

  Шаблон состоит из массива груп слов.
  Первое слово в группе - значение сигнала
  Второе слово в группе - длительность интервала времени в  мс
    интервал равный 0x00000000 - означает возврат в начало шаблона
    интервал равный 0xFFFFFFFF - означает застывание состояния


  \param pttn    - указатель на запись шаблоне
  \param n       - номер сигнала
  \param period  - периодичность вызова машины состояний
-----------------------------------------------------------------------------------------------------*/
void S7V30_set_output_pattern(const int32_t *pttn, uint32_t n)
{
  if (n >= S7V30_OTPUTS_NUM) return;
  if (pttn != 0)
  {
    if (outs_cbl[n].pattern_start_ptr != (int32_t *)pttn)
    {
      outs_cbl[n].pattern_start_ptr = (int32_t *)pttn;
      outs_cbl[n].pttn_ptr = (int32_t *)pttn;
      _Set_output_state(n,*outs_cbl[n].pttn_ptr);
      outs_cbl[n].pttn_ptr++;
      Get_hw_timestump(&outs_cbl[n].lastt);
      outs_cbl[n].pttn_ptr++;
      outs_cbl[n].active = 1;
    }
    else
    {
      outs_cbl[n].repeat = 1;
    }
  }
}

/*-----------------------------------------------------------------------------------------------------
   Автомат состояний выходных сигналов


  \param tnow  - текущее время
-----------------------------------------------------------------------------------------------------*/
void S7V30_Outputs_state_automat(void)
{
  uint32_t         duration;
  uint32_t         output_state;
  uint32_t         n;
  T_sys_timestump  tnow;

  Get_hw_timestump(&tnow);
  for (n = 0; n < S7V30_OTPUTS_NUM; n++)
  {
    // Управление состоянием выходного сигнала
    if (outs_cbl[n].active) // Отрабатываем шаблон только если активное состояние
    {
      uint32_t dt = Timestump_diff_to_msec(&outs_cbl[n].lastt  , &tnow);

      if (dt >= outs_cbl[n].duration)  // Меняем состояние сигнала при обнулении счетчика
      {
        memcpy(&outs_cbl[n].lastt, &tnow, sizeof(T_sys_timestump));

        if (outs_cbl[n].pattern_start_ptr != 0)  // Проверяем есть ли назначенный шаблон
        {
          output_state =*outs_cbl[n].pttn_ptr;   // Выборка значения состояния выхода
          outs_cbl[n].pttn_ptr++;
          duration =*outs_cbl[n].pttn_ptr;       // Выборка длительности состояния
          outs_cbl[n].pttn_ptr++;                // Переход на следующий элемент шаблона
          if (duration != 0xFFFFFFFF)
          {
            if (duration == 0)  // Длительность равная 0 означает возврат указателя элемента на начало шаблона и повторную выборку
            {
              outs_cbl[n].pttn_ptr = outs_cbl[n].pattern_start_ptr;
              output_state =*outs_cbl[n].pttn_ptr;
              outs_cbl[n].pttn_ptr++;
              outs_cbl[n].duration =*outs_cbl[n].pttn_ptr;
              outs_cbl[n].pttn_ptr++;
              _Set_output_state(n , output_state);
            }
            else
            {
              outs_cbl[n].duration = duration;
              _Set_output_state(n ,output_state);
            }
          }
          else
          {
            if (outs_cbl[n].repeat)
            {
              outs_cbl[n].repeat = 0;
              // Возврат указателя элемента на начало шаблона и повторная выборка
              outs_cbl[n].pttn_ptr = outs_cbl[n].pattern_start_ptr;
              output_state =*outs_cbl[n].pttn_ptr;
              outs_cbl[n].pttn_ptr++;
              outs_cbl[n].duration =*outs_cbl[n].pttn_ptr;
              outs_cbl[n].pttn_ptr++;
              _Set_output_state(n , output_state);
            }
            else
            {
              // Обнуляем счетчик и таким образом выключаем обработку паттерна
              _Set_output_state(n , output_state);
              outs_cbl[n].active = 0;
              outs_cbl[n].pattern_start_ptr = 0;
            }
          }
        }
        else
        {
          // Если нет шаблона обнуляем состояние выходного сигнала
          _Set_output_state(n, 0);
        }
      }
    }
  }

}


