
#ifndef __BC35_AT_H__
#define __BC35_AT_H__

#include "stdbool.h"
#include "sys.h"

#define BC35_BUADRATE		9600


#define BC35_ACK_SUCCESS  		0x00	//操作成功
#define BC35_ACK_FAIL	  		0x01	//操作失败

#define BC35_NAK 				0xFF 	//


void bc35_buffer_flush(void);
void bc35_init(void);
void bc35_send_null(void);
bool bc35_setup(uint32_t timeout);
bool bc35_testAT(uint32_t timeout);
bool bc35_set_Function(uint8_t mode, uint32_t timeout);
bool bc35_set_autoconnect(uint8_t on, uint32_t timeout);
bool bc35_set_regmode(uint8_t mode, uint32_t timeout);
bool bc35_get_nbconnect(uint32_t timeout);
bool bc35_get_rssi(int *rssi, uint32_t timeout);
bool bc35_get_ip(char *ip, uint32_t timeout);
int bc35_create_tcp_socket(char *ip, int port, uint32_t timeout);
bool bc35_send_tcp_msg(int sock, char *data, int len, uint32_t timeout);
int bc35_recv_tcp_msg(int sock, char *data, int len, uint32_t timeout);
bool bc35_close_tcp_socket(int sock, uint32_t timeout);


#endif

