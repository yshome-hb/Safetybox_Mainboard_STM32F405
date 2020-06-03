#ifndef __USART_H__
#define __USART_H__

#include "stdio.h"
#include "sys.h" 

typedef uint8_t (*Receive_Parse8_f)(uint8_t _input);

void uart1_init(u32 buad, Receive_Parse8_f receive_parse);
void uart2_init(u32 buad, Receive_Parse8_f receive_parse);
void uart3_init(u32 buad, Receive_Parse8_f receive_parse);
void uart4_init(u32 buad, Receive_Parse8_f receive_parse);
void uart5_init(u32 buad, Receive_Parse8_f receive_parse);
void uart6_init(u32 buad, Receive_Parse8_f receive_parse);
void uart1_send(uint8_t c);
void uart2_send(uint8_t c);
void uart3_send(uint8_t c);
void uart4_send(uint8_t c);
void uart5_send(uint8_t c);
void uart6_send(uint8_t c);

#endif


