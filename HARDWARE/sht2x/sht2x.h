#ifndef __SHT2X_H__
#define __SHT2X_H__

#include "stdbool.h"
#include "sys.h"


#define SHT2X_TEMP_HOLD     0xE3
#define SHT2X_HUMI_HOLD     0xE5
#define SHT2X_TEMP_UNHOLD   0xF3
#define SHT2X_HUMI_UNHOLD   0xF5
#define SHT2X_WRITE_REG     0xE6
#define SHT2X_READ_REG      0xE7
#define SHT2X_SOFT_RESET    0xFE

#define SHT2X_ADDR_DEFAULT  0x40


enum resolution_e
{
  RH_12_T_14 = 0x00, 
  RH_8_T_12  = 0x01, 
  RH_10_T_13 = 0x80, 
  RH_11_T_11 = 0x81, 
};


void sht2x_init(uint8_t resolution);
void sht2x_reset(void);
float sht2x_temperature(void);
float sht2x_humidity(void);
void sht2x_heater(uint8_t on);
void sht2x_otpreload(uint8_t on);
void sht2x_resolution(uint8_t resolution);
bool sht2x_isEOD(void);


#endif
