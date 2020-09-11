#include "wwdg.h"

#define WWDG_CNT_MASK	0x7F


void wwatchdog_init(uint8_t window)
{
	NVIC_InitTypeDef NVIC_InitStructure;
 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
	
	WWDG_SetPrescaler(WWDG_Prescaler_8);
	WWDG_SetWindowValue(window);
	WWDG_Enable(WWDG_CNT_MASK);
	
	// NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;
	// NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
	// NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	// NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	// NVIC_Init(&NVIC_InitStructure);
	
	// WWDG_ClearFlag();
    // WWDG_EnableIT();
}


void WWDG_IRQHandler(void)
{
	WWDG_SetCounter(WWDG_CNT_MASK);
	WWDG_ClearFlag();
}
