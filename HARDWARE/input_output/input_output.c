#include "gpio.h"
#include "timer.h"
#include "input_output.h"


static IO_Filter_t input_filter[INPUT_MAX] = {0};
static IO_Output_t io_output[OUTPUT_MAX] = {0};


static void input_scan(void)
{
	for(int i = 0; i < INPUT_MAX; i++)
	{
		gpio_filter(input_filter+i);
	}
}


void io_input_init(void)
{
	input_filter[INPUT_FPC].gpiox = GPIOC;
	input_filter[INPUT_FPC].pin = GPIO_Pin_9;
	input_filter[INPUT_FPC].type = GPIO_PuPd_NOPULL;	
	input_filter[INPUT_FPC].filter = 0xFF;
	input_filter[INPUT_FPC].value = 1;	
	input_filter[INPUT_FPC].prevalue = 0x00;	

	input_filter[INPUT_VEIN].gpiox = GPIOA;
	input_filter[INPUT_VEIN].pin = GPIO_Pin_8;
	input_filter[INPUT_VEIN].type = GPIO_PuPd_NOPULL;
	input_filter[INPUT_VEIN].filter = 0x00;
	input_filter[INPUT_VEIN].value = 0;	
	input_filter[INPUT_VEIN].prevalue = 0x00;		
	
	input_filter[INPUT_FACE].gpiox = GPIOB;
	input_filter[INPUT_FACE].pin = GPIO_Pin_0;
	input_filter[INPUT_FACE].type = GPIO_PuPd_NOPULL;
	input_filter[INPUT_FACE].filter = 0xFF;
	input_filter[INPUT_FACE].value = 1;	
	input_filter[INPUT_FACE].prevalue = 0x00;	
	
	input_filter[INPUT_LOCK].gpiox = GPIOA;
	input_filter[INPUT_LOCK].pin = GPIO_Pin_0;
	input_filter[INPUT_LOCK].type = GPIO_PuPd_NOPULL;
	input_filter[INPUT_LOCK].filter = 0xFF;
	input_filter[INPUT_LOCK].value = 1;	
	input_filter[INPUT_LOCK].prevalue = 0x00;	

	input_filter[INPUT_CONFIG].gpiox = GPIOC;
	input_filter[INPUT_CONFIG].pin = GPIO_Pin_3;
	input_filter[INPUT_CONFIG].type = GPIO_PuPd_NOPULL;
	input_filter[INPUT_CONFIG].filter = 0xFF;
	input_filter[INPUT_CONFIG].value = 1;		
	input_filter[INPUT_CONFIG].prevalue = 0x00;	

	input_filter[INPUT_SHAKE].gpiox = GPIOA;
	input_filter[INPUT_SHAKE].pin = GPIO_Pin_1;
	input_filter[INPUT_SHAKE].type = GPIO_PuPd_NOPULL;
	input_filter[INPUT_SHAKE].filter = 0xFF;
	input_filter[INPUT_SHAKE].value = 1;	
	input_filter[INPUT_SHAKE].prevalue = 0x00;		
	
	input_filter[INPUT_KEYBOARD].gpiox = GPIOB;
	input_filter[INPUT_KEYBOARD].pin = GPIO_Pin_12;
	input_filter[INPUT_KEYBOARD].type = GPIO_PuPd_NOPULL;
	input_filter[INPUT_KEYBOARD].filter = 0xFF;
	input_filter[INPUT_KEYBOARD].value = 1;	
	input_filter[INPUT_KEYBOARD].prevalue = 0x00;		
	
	gpio_filter_init(input_filter, INPUT_MAX);	
	timer3_init(2000, input_scan);
}


void io_input_deinit(void)
{
	timer3_deinit();
}


uint8_t io_input_update(uint8_t idx)
{
	return gpio_filter_value(input_filter+idx);
}


uint8_t io_input_prevalue(uint8_t idx)
{
	return input_filter[idx].prevalue;
}


uint8_t io_input_getfilter(uint8_t idx)
{
	return input_filter[idx].filter;
}


void io_output_init(void)
{	
	io_output[OUTPUT_POWER_3V].gpiox = GPIOB;
	io_output[OUTPUT_POWER_3V].pin = GPIO_Pin_2;
	io_output[OUTPUT_POWER_3V].type = GPIO_PuPd_UP;	
	io_output[OUTPUT_POWER_3V].dvalue = 0;	

	io_output[OUTPUT_POWER_5V].gpiox = GPIOC;
	io_output[OUTPUT_POWER_5V].pin = GPIO_Pin_5;
	io_output[OUTPUT_POWER_5V].type = GPIO_PuPd_UP;	
	io_output[OUTPUT_POWER_5V].dvalue = 0;		

	io_output[OUTPUT_POWER_NB].gpiox = GPIOA;
	io_output[OUTPUT_POWER_NB].pin = GPIO_Pin_7;
	io_output[OUTPUT_POWER_NB].type = GPIO_PuPd_UP;	
	io_output[OUTPUT_POWER_NB].dvalue = 0;	

	io_output[OUTPUT_POWER_WIFI].gpiox = GPIOC;
	io_output[OUTPUT_POWER_WIFI].pin = GPIO_Pin_8;
	io_output[OUTPUT_POWER_WIFI].type = GPIO_PuPd_UP;	
	io_output[OUTPUT_POWER_WIFI].dvalue = 0;	

	io_output[OUTPUT_POWER_FACE].gpiox = GPIOB;
	io_output[OUTPUT_POWER_FACE].pin = GPIO_Pin_1;
	io_output[OUTPUT_POWER_FACE].type = GPIO_PuPd_UP;	
	io_output[OUTPUT_POWER_FACE].dvalue = 0;

	io_output[OUTPUT_UNLOCK].gpiox = GPIOC;
	io_output[OUTPUT_UNLOCK].pin = GPIO_Pin_2;
	io_output[OUTPUT_UNLOCK].type = GPIO_PuPd_UP;	
	io_output[OUTPUT_UNLOCK].dvalue = 0;

	io_output[OUTPUT_LIGHT].gpiox = GPIOC;
	io_output[OUTPUT_LIGHT].pin = GPIO_Pin_13;
	io_output[OUTPUT_LIGHT].type = GPIO_PuPd_UP;	
	io_output[OUTPUT_LIGHT].dvalue = 0;

	io_output[OUTPUT_BEEP].gpiox = GPIOA;
	io_output[OUTPUT_BEEP].pin = GPIO_Pin_15;
	io_output[OUTPUT_BEEP].type = GPIO_PuPd_UP;	
	io_output[OUTPUT_BEEP].dvalue = 0;

	io_output[OUTPUT_BACKLIGHT].gpiox = GPIOB;
	io_output[OUTPUT_BACKLIGHT].pin = GPIO_Pin_7;
	io_output[OUTPUT_BACKLIGHT].type = GPIO_PuPd_UP;	
	io_output[OUTPUT_BACKLIGHT].dvalue = 0;		
	
	gpio_output_init(io_output, OUTPUT_MAX);
}


void io_output_set(uint8_t idx, uint8_t value)
{
 	if(idx >= OUTPUT_MAX)
		return;

	if(value)
		GPIO_SetBits(io_output[idx].gpiox, io_output[idx].pin);
	else
		GPIO_ResetBits(io_output[idx].gpiox, io_output[idx].pin);
}


