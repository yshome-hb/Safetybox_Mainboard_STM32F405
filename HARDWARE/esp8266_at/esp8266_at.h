
#ifndef __ESP8266_AT_H__
#define __ESP8266_AT_H__

#include "stdbool.h"
#include "sys.h"

#define ESP8266_BUADRATE		115200


#define ESP8266_ACK_SUCCESS  		0x00	//操作成功
#define ESP8266_ACK_FAIL	  		0x01	//操作失败

#define ESP8266_NAK 				0xFF 	//


void esp8266_buffer_flush(void);
void esp8266_init(void);
bool esp8266_setup(uint32_t timeout);
bool esp8266_testAT(uint32_t timeout);
bool esp8266_set_mode(uint8_t mode, uint32_t timeout);
bool esp8266_get_rssi(int *rssi, uint32_t timeout);
bool esp8266_get_ssid(char *ssid, uint32_t timeout);
bool esp8266_start_smartconfig(uint32_t timeout);
bool esp8266_stop_smartconfig(uint32_t timeout);
bool esp8266_wait_connection(uint32_t timeout);
bool esp8266_get_ip(char *ip, uint32_t timeout);
bool esp8266_create_tcp_socket(char *ip, int port, uint32_t timeout);
bool esp8266_send_tcp_msg(char *data, int len, uint32_t timeout);
int esp8266_recv_tcp_msg(char *data, int len, uint32_t timeout);
bool esp8266_close_tcp_socket(uint32_t timeout);

#endif

