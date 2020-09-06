																   
#include "wakeup.h"	


void soft_reset(void)
{
    __set_FAULTMASK(1);
    NVIC_SystemReset();
}


void wakeup_stop_prepare(void)
{
	RCC_APB1PeriphResetCmd(0xFFFFF, ENABLE);

	RCC_APB2PeriphResetCmd(0xBFF, ENABLE);

	RCC_AHB1PeriphResetCmd(0X04FD, ENABLE);

	//RCC_APB1PeriphResetCmd(RCC_APB1Periph_PWR, DISABLE);
	//RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOB, DISABLE);
	//RCC_APB2PeriphResetCmd(RCC_APB2Periph_SYSCFG, DISABLE);
}


void wakeup_stop(void)
{		
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);		 
	
	PWR_BackupAccessCmd(ENABLE);

	PWR_ClearFlag(PWR_FLAG_WU);
	
	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);	 
}
