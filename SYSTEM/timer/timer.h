#ifndef __TIMER_H__
#define __TIMER_H__

#include "sys.h"

typedef void (*Timer_Int_f)(void);

void timer3_init(uint16_t period, Timer_Int_f timer_int);

#endif

