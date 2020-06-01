#include "iic.h"
#include "delay.h"


void iic1_init(void)
{			
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	IIC_SCL=1;
	IIC_SDA=1;
}


void IIC_Start(void)
{
	SDA_OUT();
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL=0; 
}


void IIC_Stop(void)
{
	SDA_OUT();
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1; 
	IIC_SDA=1;
	delay_us(4);							   	
}

uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	SDA_IN();   
	IIC_SDA=1;
	delay_us(1);	   
	IIC_SCL=1;
	delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>50)
		{
			IIC_Stop();
			return 1;
		}
	  delay_us(1);
	}
	IIC_SCL=0;	   
	return 0;  
} 


void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}


void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}


void IIC_Send_Byte(uint8_t txd)
{                        
	uint8_t i;   
	SDA_OUT(); 	    
  	IIC_SCL=0;
  	for(i=0; i<8; i++)
  	{              
		IIC_SDA=(txd&0x80)>>7;
    	txd<<=1; 	  
		delay_us(2);   
		IIC_SCL=1;
		delay_us(2); 
		IIC_SCL=0;	
		delay_us(2);
  }	 
} 

uint8_t IIC_Read_Byte(uint8_t ack)
{
	uint8_t i,receive=0;
	SDA_IN();
  	for(i=0; i<8; i++ )
	{
		IIC_SCL=0; 
		delay_us(2);
		IIC_SCL=1;
		receive<<=1;
		if(READ_SDA)
			receive++;   
		delay_us(1); 
	}					 
	(ack)? IIC_Ack() : IIC_NAck();

	return receive;
}


void iic1_writebytes(uint8_t slaveAddr, uint8_t writeAddr, uint8_t *data, uint8_t length)
{
 	uint8_t count = 0;
	IIC_Start();
	IIC_Send_Byte(slaveAddr);
	IIC_Wait_Ack();
	IIC_Send_Byte(writeAddr);
	IIC_Wait_Ack();	  
	while(count < length)
	{
		IIC_Send_Byte(data[count++]); 
		IIC_Wait_Ack(); 
	}
	IIC_Stop();
}

void iic1_writebyte(uint8_t slaveAddr, uint8_t writeAddr, uint8_t data)
{
	iic1_writebytes(slaveAddr, writeAddr, &data, 1);
}


void iic1_writebits(uint8_t slaveAddr, uint8_t writeAddr, uint8_t bitStart, uint8_t length, uint8_t data)
{
	uint8_t bit, mask;
	bit = iic1_readbyte(slaveAddr, writeAddr);
	mask = (0xFF << (bitStart + 1)) | 0xFF >> ((8 - bitStart) + length - 1);
  	data <<= (8 - length);
	data >>= (7 - bitStart);
	bit &= mask;
	bit |= data;
  	iic1_writebyte(slaveAddr, writeAddr, bit); 
}


void iic1_writebit(uint8_t slaveAddr, uint8_t writeAddr, uint8_t bitNum, uint8_t data)
{
	uint8_t bit;
	bit = iic1_readbyte(slaveAddr, writeAddr);
	bit = (data != 0) ? (bit | (1 << bitNum)) : (bit & ~(1 << bitNum));
 	iic1_writebyte(slaveAddr, writeAddr, bit);
}



void iic1_readbytes(uint8_t slaveAddr, uint8_t readAddr, uint8_t *data, uint8_t length)
{
	uint8_t count = 0;
	IIC_Start();
	IIC_Send_Byte(slaveAddr);
	IIC_Wait_Ack();
	IIC_Send_Byte(readAddr);
	IIC_Wait_Ack();	  
	IIC_Start();
	IIC_Send_Byte(slaveAddr+1);
	IIC_Wait_Ack();
	
  	while(count < length)
	{	 
		if(count != (length-1))
			data[count++] = IIC_Read_Byte(1);  //ACK
		else  
			data[count++] = IIC_Read_Byte(0);	 //NACK
	}
	IIC_Stop();
}



uint8_t iic1_readbyte(uint8_t slaveAddr, uint8_t readAddr)
{
	uint8_t result;
	iic1_readbytes(slaveAddr, readAddr, &result, 1);
	return result;
}


//------------------End of File----------------------------
