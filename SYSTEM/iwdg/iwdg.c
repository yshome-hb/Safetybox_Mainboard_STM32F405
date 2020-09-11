#include "iwdg.h"

void iwatchdog_init(uint16_t reload)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); //ʹ�ܶ�IWDG->PR IWDG->RLR��д
	
	IWDG_SetPrescaler(IWDG_Prescaler_128); //����IWDG��Ƶϵ��

	IWDG_SetReload(reload);   //����IWDGװ��ֵ

	IWDG_ReloadCounter(); //reload
	
	IWDG_Enable();       //ʹ�ܿ��Ź�
}


void iwatchdog_feed(void)
{
	IWDG_ReloadCounter();//reload
}
