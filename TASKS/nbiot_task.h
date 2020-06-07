#ifndef __NBIOT_TASK_H__
#define __NBIOT_TASK_H__

#include "sys.h"


enum nbiot_cmd_e
{
    NBIOT_CMD_NONE = 1,
    NBIOT_CMD_INFO,
    NBIOT_CMD_CONNECT,
    NBIOT_CMD_SEND,
    NBIOT_CMD_RECV,
    NBIOT_CMD_CLOSE,	    
};


enum nbiot_return_e
{
	NBIOT_RETURN_NONE = 0,	
	NBIOT_RETURN_SUCCESS,
	NBIOT_RETURN_TIMEOUT,
	NBIOT_RETURN_FAIL = 0xFF,
};


int32_t nbiot_get_rssi(void);
char* nbiot_get_ip(void);
void nbiot_connect_tcp(char *ip, uint16_t port);
void nbiot_close_tcp(uint16_t port);
void nbiot_send_tcpdata(const char *data, int len, uint16_t recv_time);
char* nbiot_recv_tcpdata(int *len);
int32_t nbiot_send_msg(const void *msg, uint32_t timeout);
void nbiot_task_create(void *pvParameters);


#endif
