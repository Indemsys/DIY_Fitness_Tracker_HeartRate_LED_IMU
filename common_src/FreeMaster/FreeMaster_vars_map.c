#include "App.h"
#include "freemaster_cfg.h"
#include "freemaster.h"
#include "freemaster_tsa.h"

FMSTR_TSA_TABLE_BEGIN(module_tbl)
FMSTR_TSA_RW_VAR( emb_charger.no_accum         ,FMSTR_TSA_UINT16)
FMSTR_TSA_RW_VAR( emb_charger.accum_voltage    ,FMSTR_TSA_FLOAT)
FMSTR_TSA_RW_VAR( emb_charger.accum_current    ,FMSTR_TSA_FLOAT)
FMSTR_TSA_RW_VAR( emb_charger.accum_capacity   ,FMSTR_TSA_FLOAT)
FMSTR_TSA_RW_VAR( emb_charger.pcb_temp         ,FMSTR_TSA_FLOAT)
FMSTR_TSA_TABLE_END();


FMSTR_TSA_TABLE_LIST_BEGIN()

FMSTR_TSA_TABLE(wvar_tbl)
FMSTR_TSA_TABLE(ivar_tbl)
FMSTR_TSA_TABLE(module_tbl)

#ifdef APP_FREEMASTER_TBL
FMSTR_TSA_TABLE(app_freemaster_tbl)
#endif

FMSTR_TSA_TABLE_LIST_END()
