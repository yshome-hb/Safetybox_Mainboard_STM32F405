#ifndef __IWDG_H__
#define __IWDG_H__

#include "sys.h"

void iwatchdog_init(uint16_t reload);
void iwatchdog_feed(void);

#endif
