#ifndef __IIC_H__
#define __IIC_H__

#include "sys.h"
   	   		   
#define SDA_IN()  {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=(0<<(9*2));}
#define SDA_OUT() {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=(1<<(9*2));}

#define IIC_SCL    PBout(8) //SCL
#define IIC_SDA    PBout(9) //SDA	 
#define READ_SDA   PBin(9)  //输入SDA 

extern void iic1_init(void); 
extern void iic1_writebytes(uint8_t slaveAddr, uint8_t writeAddr, uint8_t *data, uint8_t length);
extern void iic1_writebyte(uint8_t slaveAddr, uint8_t writeAddr, uint8_t data);
extern void iic1_writebits(uint8_t slaveAddr, uint8_t writeAddr, uint8_t bitStart, uint8_t length, uint8_t data);
extern void iic1_writebit(uint8_t slaveAddr, uint8_t writeAddr, uint8_t bitNum, uint8_t data);
extern void iic1_readbytes(uint8_t slaveAddr, uint8_t readAddr, uint8_t *data, uint8_t length);
extern uint8_t iic1_readbyte(uint8_t slaveAddr, uint8_t readAddr);


#endif

//------------------End of File----------------------------







