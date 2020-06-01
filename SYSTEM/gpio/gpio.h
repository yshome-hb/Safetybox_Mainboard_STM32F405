#ifndef __GPIO_H__
#define __GPIO_H__

#include "sys.h"  	


typedef struct
{
	GPIO_TypeDef* gpiox; 
	uint16_t pin;
	GPIOPuPd_TypeDef type;
	uint8_t filter;
	uint8_t value;
	uint8_t prevalue;
	
}IO_Filter_t;


typedef struct
{
	GPIO_TypeDef* gpiox; 
	uint16_t pin;
	GPIOPuPd_TypeDef type;
	uint8_t dvalue;
	
}IO_Output_t;


static inline void gpio_filter(IO_Filter_t *io_filter)
{
	io_filter->filter = ((io_filter->filter << 1) | GPIO_ReadInputDataBit(io_filter->gpiox, io_filter->pin));
}

void gpio_output_init(IO_Output_t *io_output, uint16_t num);
void gpio_filter_init(IO_Filter_t *io_filter, uint16_t num);
uint8_t gpio_filter_value(IO_Filter_t *io_filter);



#endif






















