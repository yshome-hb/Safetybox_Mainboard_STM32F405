#ifndef __HW_IIC_H__
#define __HW_IIC_H__

#include "sys.h"
   	

void hw_iic1_init(void);
uint8_t hw_iic1_writebyte(uint8_t saddr, uint8_t reg, uint8_t data);
uint8_t hw_iic1_writebytes(uint8_t saddr, uint8_t reg, uint8_t *data, uint8_t num);
uint8_t hw_iic1_readbyte(uint8_t saddr, uint8_t reg);
uint8_t hw_iic1_readbytes(uint8_t saddr, uint8_t reg, uint8_t* buf, uint8_t num);


#endif

//------------------End of File----------------------------







