#ifndef __SPI_SLAVE_H__
#define __SPI_SLAVE_H__

#include "stdio.h"
#include "sys.h"

typedef uint16_t (*Receive_Parse16_f)(uint16_t _input);

void spi2_init(Receive_Parse16_f receive_parse);

#endif

