#include "iwdg.h"

void iwatchdog_init(uint16_t reload)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); //使能对IWDG->PR IWDG->RLR的写
	
	IWDG_SetPrescaler(IWDG_Prescaler_128); //设置IWDG分频系数

	IWDG_SetReload(reload);   //设置IWDG装载值

	IWDG_ReloadCounter(); //reload
	
	IWDG_Enable();       //使能看门狗
}


void iwatchdog_feed(void)
{
	IWDG_ReloadCounter();//reload
}
