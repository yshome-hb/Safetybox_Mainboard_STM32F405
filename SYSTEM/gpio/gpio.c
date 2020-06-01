#include "gpio.h"

  
void gpio_output_init(IO_Output_t *io_output, uint16_t num)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	

	for(uint16_t i = 0; i < num; i++)
	{
		GPIO_InitStructure.GPIO_Pin = io_output[i].pin;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_PuPd = io_output[i].type;
		GPIO_Init(io_output[i].gpiox, &GPIO_InitStructure);
		if(io_output[i].dvalue)
			GPIO_SetBits(io_output[i].gpiox, io_output[i].pin);
		else
			GPIO_ResetBits(io_output[i].gpiox, io_output[i].pin);
	}
}


void gpio_filter_init(IO_Filter_t *io_filter, uint16_t num)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	uint16_t i = 0;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	for(i = 0; i < num; i++)
	{
		GPIO_InitStructure.GPIO_Pin = io_filter->pin;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_PuPd = io_filter->type;
		GPIO_Init(io_filter->gpiox, &GPIO_InitStructure);	
	}
}


uint8_t gpio_filter_value(IO_Filter_t *io_filter)
{
	io_filter->prevalue = (io_filter->prevalue << 1) & 0x03;
	
	if((io_filter->filter&0x0F) == 0x0F)
	{
		io_filter->value = 1;
	}	
	else if((io_filter->filter&0x0F) == 0x00)
	{
		io_filter->value = 0;
	}
	
	io_filter->prevalue |= io_filter->value;
	
	return io_filter->value;
}


