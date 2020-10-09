																   
#include "wakeup.h"	


void soft_reset(void)
{
    __set_FAULTMASK(1);
    NVIC_SystemReset();
}


void wakeup_stop_prepare(void)
{
	ADC_Cmd(ADC1, DISABLE);

	WWDG_DeInit();

    SysTick->CTRL = 0x00;//关闭定时器
    SysTick->VAL = 0x00;//清空val,清空定时器

	RCC_APB1PeriphResetCmd(0xFFFFF, ENABLE);
	RCC_APB1PeriphResetCmd(0xFFFFF, DISABLE);

	RCC_APB2PeriphResetCmd(0xBFF, ENABLE);
	RCC_APB2PeriphResetCmd(0xBFF, DISABLE);

	RCC_AHB1PeriphResetCmd(0X04FD, ENABLE);
	RCC_AHB1PeriphResetCmd(0X04FD, DISABLE);


  	//RCC_AHB1PeriphClockCmd(uint32_t RCC_AHB1Periph, DISABLE);
	//RCC_AHB2PeriphClockCmd(uint32_t RCC_AHB2Periph, DISABLE);
	//RCC_AHB3PeriphClockCmd(uint32_t RCC_AHB3Periph, DISABLE);
	RCC_APB1PeriphClockCmd(0xFFFFFFFF, DISABLE);
	RCC_APB2PeriphClockCmd(0xFFFFFFFF, DISABLE);

	//RCC_APB1PeriphResetCmd(RCC_APB1Periph_PWR, DISABLE);
	//RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOB, DISABLE);
	//RCC_APB2PeriphResetCmd(RCC_APB2Periph_SYSCFG, DISABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD, ENABLE);

 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
 	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD; 
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
 	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Pin = 0xFFFF;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = 0xFFFF;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

 	GPIO_InitStructure.GPIO_Pin = 0x3FFF;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

 	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
 	GPIO_InitStructure.GPIO_Pin = 0x0004;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

 	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_0;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

}


void wakeup_stop(void)
{		
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);		 
	
	PWR_BackupAccessCmd(ENABLE);

	PWR_ClearFlag(PWR_FLAG_WU);
	
	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);	 
}
