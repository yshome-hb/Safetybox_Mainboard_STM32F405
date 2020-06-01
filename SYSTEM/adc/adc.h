#ifndef __ADC_H__
#define __ADC_H__

#include "stdbool.h"
#include "sys.h" 

 							   
void adc1_init(void);
void adc1_start_conv(uint8_t ch);
bool adc1_get_value(uint8_t ch, uint16_t *value);


#endif 















