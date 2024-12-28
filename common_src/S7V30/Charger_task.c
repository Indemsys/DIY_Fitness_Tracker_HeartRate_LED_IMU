// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 2024-04-24
// 14:31:55
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include   "App.h"


TX_THREAD charger_thread;
#define CHARGER_THREAD_STACK_SIZE (1024*4)
static uint8_t charger_thread_stack[CHARGER_THREAD_STACK_SIZE] BSP_PLACE_IN_SECTION_V2(".stack.charger_thread")BSP_ALIGN_VARIABLE_V2(BSP_STACK_ALIGNMENT);

uint8_t  bq25619_control0;
uint8_t  bq25619_status0;
uint8_t  bq25619_status1;
uint8_t  bq25619_status2;
uint8_t  bq25619_status0_prev;
uint8_t  charge_state;
uint8_t  charge_state_prev;
uint32_t charge_state_change_cnt;

static uint8_t  charger_task_active;
static uint8_t  charger_task_created;

uint8_t  fuel_guage_error;

static TX_EVENT_FLAGS_GROUP   charger_flags;

T_emb_charger_cbl  emb_charger;

/*-----------------------------------------------------------------------------------------------------


  \param eventmask
-----------------------------------------------------------------------------------------------------*/
void Set_charger_event(uint32_t events_mask)
{
  tx_event_flags_set(&charger_flags, events_mask, TX_OR);
}



/*-----------------------------------------------------------------------------------------------------


  \param src

  \return char const*
-----------------------------------------------------------------------------------------------------*/
char const* Get_BQ25619_power_status_str(uint8_t b)
{
  uint8_t v = (b >> 2) & 0x01;
  switch (v)
  {
  case 0:
    return "Power NOT good";
  case 1:
    return "Power good";
  }
  return "UNDEF   ";
}

/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
static void Get_fuel_guage_state(void)
{
  ssp_err_t  res = SSP_SUCCESS;
  uint16_t reg;
  if (Is_MAX17262_opened() && (fuel_guage_error == 0))
  {
    res |= MAX17262_read_reg(MAX17262_AVGVCELL,&reg);
    emb_charger.accum_voltage = reg * VOLTAGE_MEASUREMENT_STEP / 1000.0f;

    res |= MAX17262_read_reg(MAX17262_AVGCURRENT,&reg);
    emb_charger.accum_current = ((int16_t)reg) * CURRENT_MEASUREMENT_STEP;

    res |= MAX17262_read_reg(MAX17262_REPSOC,&reg);
    emb_charger.accum_capacity = reg * PERCENTAGE_MEASUREMENT_STEP;

    res |= MAX17262_read_reg(MAX17262_AVGTA,&reg);
    emb_charger.pcb_temp = reg * TEMPER_MEASUREMENT_STEP;
    if (res != SSP_SUCCESS)
    {
      EAPPLOG("Fuel Gauge IC MAX17262 communication error %d", (uint32_t)res);
      fuel_guage_error = 1;
      emb_charger.accum_capacity = 0;
    }
  }
}

/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
static void Set_accum_capacity(void)
{
  MAX17262_write_reg(MAX17262_DESIGNCAP,  (uint16_t)(ivar.accum_nominal_capacity / CAPACITY_MEASUREMENT_STEP));
  MAX17262_write_reg(MAX17262_FULLCAPREP,  (uint16_t)(ivar.accum_nominal_capacity / CAPACITY_MEASUREMENT_STEP));
}

/*-----------------------------------------------------------------------------------------------------


  \param void
-----------------------------------------------------------------------------------------------------*/
static void Print_fuel_guage_state(void)
{
  if (Is_MAX17262_opened() && (fuel_guage_error == 0))
  {
    EAPPLOG("Fuel guage MAX17262. Accum.voltage=%0.3f V, Accum current=%0.1f mA, Accum. capacity=%0.1f%%, PCB temper.=%0.1f C",(double)emb_charger.accum_voltage,(double)emb_charger.accum_current,(double)emb_charger.accum_capacity,(double)emb_charger.pcb_temp);
  }
  else
  {
    EAPPLOG("Fuel guage MAX17262 communication error");
  }
}
/*-----------------------------------------------------------------------------------------------------


  \param arg
-----------------------------------------------------------------------------------------------------*/
static void Charger_thread(ULONG arg)
{
  ssp_err_t       ssp_res;
  uint8_t         b;
  T_sys_timestump tst;
  ULONG           actual_flags;

  ssp_res = BQ25619_open();
  if (ssp_res != SSP_SUCCESS)
  {
    EAPPLOG("Charger IC BQ25619 communication error %d", (uint32_t)ssp_res);
    return;
  }
  charger_task_created = 1;

  ssp_res = BQ25619_read_reg(BQ25619_PART_INFORMATION,&b);
  if (ssp_res != SSP_SUCCESS)
  {
    EAPPLOG("Charger IC BQ25619 communication error %d", (uint32_t)ssp_res);
    return;
  }
  if ((b & 0x7F) != 0x2C)
  {
    EAPPLOG("Charger chip BQ25619 is not identified %02X", b);
    return;
  }


  ssp_res = BQ25619_read_reg(BQ25619_SYSTEM_STATUS0,&bq25619_status0);
  if (ssp_res != SSP_SUCCESS)
  {
    EAPPLOG("Charger chip BQ25619_SYSTEM_STATUS0 error");
    return;
  }
  ssp_res = BQ25619_read_reg(BQ25619_SYSTEM_STATUS1,&bq25619_status1);
  if (ssp_res != SSP_SUCCESS)
  {
    EAPPLOG("Charger chip BQ25619_SYSTEM_STATUS1 error");
    return;
  }
  ssp_res = BQ25619_read_reg(BQ25619_SYSTEM_STATUS2,&bq25619_status2);
  if (ssp_res != SSP_SUCCESS)
  {
    EAPPLOG("Charger chip BQ25619_SYSTEM_STATUS2 error");
    return;
  }
  EAPPLOG("Charger chip BQ25619 Status 0 = 0x%02X: %s, %s, %s ", bq25619_status0, Get_BQ25619_VBUS_status_str(bq25619_status0), Get_BQ25619_power_status_str(bq25619_status0), Get_BQ25619_charging_status_str(bq25619_status0));
  EAPPLOG("Charger chip BQ25619 Status 1 = 0x%02X, Status 2 = 0x%02X", bq25619_status1, bq25619_status2);
  bq25619_status0_prev = bq25619_status0;
  charge_state = BQ25619_get_charge_state(bq25619_status0);
  charge_state_prev = charge_state;

  ssp_res = MAX17262_open();
  if (ssp_res != SSP_SUCCESS)
  {
    EAPPLOG("Fuel Gauge IC MAX17262 communication error %d", (uint32_t)ssp_res);
  }
  else
  {
    Set_accum_capacity();
  }
  Get_fuel_guage_state();
  Print_fuel_guage_state();

  Get_hw_timestump(&tst);

  tx_event_flags_create(&charger_flags, "Charger");

  BQ25619_set_max_sys_voltage();
  BQ25619_set_charger_current_limit(ivar.emb_charger_max_current);

  PWR_SEL = 0;  // Разрешаем ток до 2.4 А

  charger_task_active = 1;

  do
  {
    // Ожидаем события 10 мс
    if (tx_event_flags_get(&charger_flags,0xFFFFFFFF, TX_OR_CLEAR,&actual_flags, MS_TO_TICKS(10)) == TX_SUCCESS)
    {
      if (actual_flags & CHARGER_RESET_POWER)
      {
        BQ25619_switch_off_vbus();
      }
      if (actual_flags & CHARGER_SW_OFF_ACCUM)
      {
        BQ25619_switch_off_accum();
      }
      if (actual_flags & CHARGER_SERVICE_OFF)
      {
        charger_task_active = 0;
      }
      if (actual_flags & CHARGER_SERVICE_ON)
      {
        charger_task_active = 1;
      }
      if (actual_flags & CHARGER_STOP_TASK)
      {
        tx_event_flags_set(&charger_flags, CHARGER_TASK_STOPPED, TX_OR);
      }
    }


    // Цикл опроса BQ25619 и MAX17262 длиться 900 мкс
    Get_fuel_guage_state();

    if (charger_task_active == 1)
    {

      ssp_res = BQ25619_read_reg(BQ25619_CHARGER_CONTROL_0,&bq25619_control0);
      if (ssp_res != SSP_SUCCESS)
      {
        EAPPLOG("Read chip BQ25619_SYSTEM_STATUS0 error");
        Wait_ms(1000);
      }
      else
      {
        bq25619_control0 |= BIT(6); // Сброс вотчдога
        BQ25619_write_reg(BQ25619_CHARGER_CONTROL_0,bq25619_control0);
      }

      ssp_res = BQ25619_read_reg(BQ25619_SYSTEM_STATUS0,&bq25619_status0);
      if (ssp_res != SSP_SUCCESS)
      {
        EAPPLOG("Read chip BQ25619_SYSTEM_STATUS0 error");
        Wait_ms(1000);
      }
      else
      {
        emb_charger.charger_state_str = Get_BQ25619_charging_status_str(bq25619_status0);

        if (bq25619_status0_prev != bq25619_status0)
        {
          EAPPLOG("Charger chip BQ25619 Status = 0x%02X: %s, %s, %s ", bq25619_status0, Get_BQ25619_VBUS_status_str(bq25619_status0), Get_BQ25619_power_status_str(bq25619_status0), emb_charger.charger_state_str);
          bq25619_status0_prev = bq25619_status0;
          charge_state = BQ25619_get_charge_state(bq25619_status0);
          if (charge_state_prev != charge_state)
          {
            // Если состояние зарядки поменялось более 5 раз за 1 сек, то это признак отсутствия аккумулятора и отключаем аккумулятор в таком случае
            charge_state_change_cnt++;
            if  (charge_state_change_cnt > 5)
            {
              if (Time_elapsed_msec(&tst) < 1000)
              {
                emb_charger.no_accum  = 1;
                // Определяем состояние с отключенным аккумулятором и запрещаем работу зарядника
                BQ25619_disable_charger();
                EAPPLOG("Charger disabled");
              }
              Get_hw_timestump(&tst);
              charge_state_change_cnt = 0;
            }
            charge_state_prev = charge_state;
          }
        }
      }
    }

  } while (1);

}

/*-----------------------------------------------------------------------------------------------------


  \param void

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
void Charger_service_off_request(void)
{
  Set_charger_event(CHARGER_SERVICE_OFF);
}

/*-----------------------------------------------------------------------------------------------------


  \param void

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
void Charger_service_on_request(void)
{
  Set_charger_event(CHARGER_SERVICE_ON);
}

/*-----------------------------------------------------------------------------------------------------


-----------------------------------------------------------------------------------------------------*/
uint32_t Charger_thread_create(void)
{
  uint32_t              res;

  if (ivar.en_embedded_charger == 0) return RES_ERROR;

  emb_charger.charger_state_str = "";
  res = tx_thread_create(
                         &charger_thread,
                         (CHAR *)"Charger Thread",
                         Charger_thread,
                         (ULONG) NULL,
                         &charger_thread_stack,
                         CHARGER_THREAD_STACK_SIZE,
                         CHARGER_TASK_PRIO,
                         CHARGER_TASK_PRIO,
                         1,
                         TX_AUTO_START
                        );
  if (res == TX_SUCCESS)
  {
    EAPPLOG("Embedded Li-Po charger task created.");
    return RES_OK;
  }
  else
  {
    EAPPLOG("Embedded Li-Po charger task creating error %04X.", res);
    return RES_ERROR;
  }
}

/*-----------------------------------------------------------------------------------------------------


  \param void

  \return uint32_t
-----------------------------------------------------------------------------------------------------*/
uint32_t Delete_Charger_task(void)
{
  ULONG actual_flags;
  if (charger_task_created == 0) return RES_OK;

  tx_event_flags_set(&charger_flags, CHARGER_STOP_TASK, TX_OR);
  tx_event_flags_get(&charger_flags, CHARGER_TASK_STOPPED, TX_OR_CLEAR,&actual_flags, MS_TO_TICKS(200));

  BQ25619_close();
  tx_thread_terminate(&charger_thread);
  tx_thread_delete(&charger_thread);


  return RES_OK;

}

