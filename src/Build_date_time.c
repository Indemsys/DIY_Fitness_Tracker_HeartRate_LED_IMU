#include "stdint.h"
/*-----------------------------------------------------------------------------------------------------


  \param void

  \return const char*
-----------------------------------------------------------------------------------------------------*/
const char *Get_build_date(void)
{
   return __DATE__;
}

/*-----------------------------------------------------------------------------------------------------


  \param void

  \return const char*
-----------------------------------------------------------------------------------------------------*/
const char *Get_build_time(void)
{
   return __TIME__;
}

/*-----------------------------------------------------------------------------------------------------



  \return const char*
-----------------------------------------------------------------------------------------------------*/
const char *Get_bootloader_date_time(void)
{
  uint8_t *ptr;
  ptr =  (uint8_t*)0x00000700;

  // Возвращаем ссылку на область во Flash если там достаточно короткая строка
  // Поскольку в старых бутлодерах там строки с датой нет
  for (uint32_t i=0;i<64;i++)
  {
    if (ptr[i]==0)
    {
      return (const char*)ptr;
    }
  }
  return "---";
}

