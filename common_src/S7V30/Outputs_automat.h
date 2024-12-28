#ifndef OUTPUTS_AUTOMAT_H
  #define OUTPUTS_AUTOMAT_H


#define S7V30_RED_LED     0
#define S7V30_GREEN_LED   1
#define S7V30_BLUE_LED    2

#define S7V30_OTPUTS_NUM  3

void S7V30_set_output_pattern(const int32_t *pttn, uint32_t n);
void S7V30_Outputs_state_automat(void);

void Output_OFF(uint32_t val);
void Output_ON(uint32_t val);
void Output_SLOW_BLINK(uint32_t val);
void Output_FAST_BLINK(uint32_t val);

#endif // OUTPUTS_AUTOMAT_H



