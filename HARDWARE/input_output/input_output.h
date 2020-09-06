#ifndef __INPUT_OUTPUT_H__
#define __INPUT_OUTPUT_H__

#include "sys.h"


enum key_int_e
{
    INPUT_FPC = 0,
    INPUT_VEIN,
    INPUT_FACE,
    INPUT_LOCK,
    INPUT_CONFIG,
		INPUT_SHAKE,
		INPUT_KEYBOARD,
		INPUT_MAX,
};


enum io_output_e
{
    OUTPUT_POWER_3V = 0,
    OUTPUT_POWER_5V,       
    OUTPUT_POWER_NB,
    OUTPUT_POWER_WIFI,
		OUTPUT_POWER_FACE,  
    OUTPUT_UNLOCK,
		OUTPUT_LIGHT,
		OUTPUT_BEEP,  
		OUTPUT_BACKLIGHT,   
		OUTPUT_MAX,
};


void io_input_init(void);
void io_input_deinit(void);
uint8_t io_input_update(uint8_t idx);
uint8_t io_input_prevalue(uint8_t idx);
uint8_t io_input_getfilter(uint8_t idx);
void io_output_init(void);
void io_output_set(uint8_t idx, uint8_t value);


#endif
