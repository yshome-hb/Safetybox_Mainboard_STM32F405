
#include "stdlib.h"
#include "hw_iic.h"
#include "sht2x.h"


static inline float convertStToTemperature(uint16_t st) 
{
    return (st * 175.72 / 65536.0 - 46.85);
}


static inline float convertSrhToHumidity(uint16_t srh) 
{
    return (srh * 125.0 / 65536.0 - 6.0);
}


static uint16_t sht2x_readraw(uint8_t _reg) 
{
    uint8_t buf[3];
    hw_iic1_readbytes(SHT2X_ADDR_DEFAULT, _reg, buf, 3);  
    return ((((uint16_t)buf[0])<<8) | buf[1]);
}


void sht2x_init(uint8_t resolution)
{
    sht2x_resolution(resolution);
}


void sht2x_reset(void)
{
    hw_iic1_writebytes(SHT2X_ADDR_DEFAULT, SHT2X_SOFT_RESET, NULL, 0);
}


float sht2x_temperature() 
{
  uint16_t st = sht2x_readraw(SHT2X_TEMP_HOLD);
  st &= ~0x0003;
  return convertStToTemperature(st);
}


float sht2x_humidity() 
{
  uint16_t srh = sht2x_readraw(SHT2X_HUMI_HOLD);
  srh &= ~0x0003;
  return convertSrhToHumidity(srh);
}


void sht2x_heater(uint8_t on) 
{
  uint8_t value = hw_iic1_readbyte(SHT2X_ADDR_DEFAULT, SHT2X_READ_REG);;
  if(on)
    value |= 0x04;
  else
    value &= 0xFB; 
  hw_iic1_writebyte(SHT2X_ADDR_DEFAULT, SHT2X_WRITE_REG, value);
}


void sht2x_otpreload(uint8_t on) 
{
  uint8_t value = hw_iic1_readbyte(SHT2X_ADDR_DEFAULT, SHT2X_READ_REG);
  if(on)
    value &= 0xFD;
  else
    value |= 0x02;
  hw_iic1_writebyte(SHT2X_ADDR_DEFAULT, SHT2X_WRITE_REG, value);
}


void sht2x_resolution(uint8_t resolution) 
{
  uint8_t value = hw_iic1_readbyte(SHT2X_ADDR_DEFAULT, SHT2X_READ_REG);
  value &= ~0x81;
  value |= resolution;
  hw_iic1_writebyte(SHT2X_ADDR_DEFAULT, SHT2X_WRITE_REG, value);
}


bool sht2x_isEOD(void) 
{
  uint8_t value = hw_iic1_readbyte(SHT2X_ADDR_DEFAULT, SHT2X_READ_REG);
  return ((value&0x40) > 0);
}
