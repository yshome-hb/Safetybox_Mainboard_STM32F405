#ifndef __EXTI_H__
#define __EXIT_H__

#include "sys.h"  	


typedef void (*Exti_Int_f)(void);

void exti15_10_init(Exti_Int_f exti_int);	 					    

#endif

























