#include "hw_iic.h"


#define I2C_SPEED             100000
#define I2C_SLAVE_ADDRESS7    0

void hw_iic1_init(void)
{			
	GPIO_InitTypeDef  GPIO_InitStructure;
	I2C_InitTypeDef  I2C_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);    

  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = I2C_SLAVE_ADDRESS7;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;

  I2C_Init(I2C1, &I2C_InitStructure);	
  I2C_Cmd(I2C1, ENABLE);

  I2C_AcknowledgeConfig(I2C1, ENABLE);
}


uint8_t hw_iic1_writebytes(uint8_t saddr, uint8_t reg, uint8_t *data, uint8_t num)
{
  uint8_t i = 0;
	I2C_AcknowledgeConfig(I2C1, ENABLE);
  I2C_GenerateSTART(I2C1, ENABLE);           //*****start  *****//
  /* Test on EV5 and clear it */
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)));
  I2C_Send7bitAddress(I2C1, saddr<<1, I2C_Direction_Transmitter);  /***device addr*/
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)));

  I2C_SendData(I2C1, reg);   /*send reg to write *************/
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING));

  while(i < num)
  {
		/* Send the byte to be written */
    I2C_SendData(I2C1, data[i++]);   /******send data*********/
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  }
  /* Send STOP condition */
  I2C_GenerateSTOP(I2C1, ENABLE);  /******stop****/
  return i;
}


uint8_t hw_iic1_writebyte(uint8_t saddr, uint8_t reg, uint8_t data)
{
	return hw_iic1_writebytes(saddr<<1, reg, &data, 1);
}


uint8_t hw_iic1_readbyte(uint8_t saddr, uint8_t reg)
{
	I2C_AcknowledgeConfig(I2C1, ENABLE);
	I2C_GenerateSTART(I2C1, ENABLE);      /**********start**********/

  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
  I2C_Send7bitAddress(I2C1, saddr<<1, I2C_Direction_Transmitter); /*******device addr********/
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(I2C1, reg);    /*****reg addr**********/
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING));
   
	I2C_GenerateSTART(I2C1, ENABLE); /***restart *************/
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))  /* Test on EV5 and clear it */
  
	I2C_Send7bitAddress(I2C1, saddr<<1, I2C_Direction_Receiver);    /*******device addr******/
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)); /* Test on EV6 and clear it */
  I2C_AcknowledgeConfig(I2C1, DISABLE);
  I2C_GenerateSTOP(I2C1, ENABLE);   /********stop******/

  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)); /* EV7 */
  return I2C_ReceiveData(I2C1);   /***get data****/
}


uint8_t hw_iic1_readbytes(uint8_t saddr, uint8_t reg, uint8_t* buf, uint8_t num)
{
	int i = 0;
  I2C_AcknowledgeConfig(I2C1, ENABLE);
	I2C_GenerateSTART(I2C1, ENABLE);/***start *******/

	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))/* Test on EV5 and clear it */
  {
    if(i++ > 10)
      return 1;
    vTaskDelay(10);
  }
  
	I2C_Send7bitAddress(I2C1, saddr<<1, I2C_Direction_Transmitter); /***device addr**/

  i = 0;
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) /* Test on EV6 and clear it */
  {
    if(i++ > 10)
      return 1;
    vTaskDelay(10);
  }

  I2C_SendData(I2C1, reg);   /* send reg addr */
  
  i = 0;
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) /* Test on EV8 and clear it */
  {
    if(i++ > 10)
      return 1;
    vTaskDelay(10);
  }


  I2C_GenerateSTART(I2C1, ENABLE); /*restart  */
 
  i = 0;
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))  /* Test on EV5 and clear it */
  {
    if(i++ > 10)
      return 1;
    vTaskDelay(10);
  }

  I2C_Send7bitAddress(I2C1, saddr<<1, I2C_Direction_Receiver);  /* re send device addr */

  i = 0;
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))  /* Test on EV6 and clear it */
  {
    if(i++ > 10)
      return 1;
    vTaskDelay(10);
  }

  /* While there is data to be read */
  while(num)
  {
		if(num == 1)
    {
			/*the last one*/
			I2C_AcknowledgeConfig(I2C1, DISABLE);/* Disable Acknowledgement */
			I2C_GenerateSTOP(I2C1, ENABLE);/* Send STOP Condition */
    }

    /* Test on EV7 and clear it */
    if(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))
    {
			/* Read a byte from the EEPROM */
      *buf = I2C_ReceiveData(I2C1);
      buf++; /* Point to the next location where the byte read will be saved */
      num--;        /* Decrement the read bytes counter */
		}
	}
  /* Enable Acknowledgement to be ready for another reception */
  I2C_AcknowledgeConfig(I2C1, ENABLE);
  return 0;
}

//------------------End of File----------------------------
