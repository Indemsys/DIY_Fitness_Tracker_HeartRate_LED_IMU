#ifndef CHARGER_TASK_H
  #define CHARGER_TASK_H


#define     CHARGER_RESET_POWER      BIT(0)
#define     CHARGER_SERVICE_OFF      BIT(1)
#define     CHARGER_SERVICE_ON       BIT(2)
#define     CHARGER_SW_OFF_ACCUM     BIT(3)
#define     CHARGER_STOP_TASK        BIT(4)
#define     CHARGER_TASK_STOPPED     BIT(5)

typedef struct
{
  uint8_t           no_accum;
  const char*       charger_state_str;
  float             accum_voltage;
  float             accum_current;
  float             accum_capacity;
  float             pcb_temp;

} T_emb_charger_cbl;

extern T_emb_charger_cbl  emb_charger;

uint32_t    Charger_thread_create(void);
uint32_t    Delete_Charger_task(void);
char const* Get_BQ25619_power_status_str(uint8_t src);
void        Set_charger_event(uint32_t events_mask);
void        Charger_service_off_request(void);
void        Charger_service_on_request(void);

#endif



