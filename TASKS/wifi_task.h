#ifndef __WIFI_TASK_H__
#define __WIFI_TASK_H__

#include "sys.h"


enum wifi_cmd_e
{
    WIFI_CMD_SMART = 1,
    WIFI_CMD_INFO,
    WIFI_CMD_CONNECT,
    WIFI_CMD_SEND,
    WIFI_CMD_RECV,
    WIFI_CMD_CLOSE,	
    
};


enum wifi_return_e
{
	WIFI_RETURN_NONE = 0,	
	WIFI_RETURN_SUCCESS,
	WIFI_RETURN_TIMEOUT,
	WIFI_RETURN_FAIL = 0xFF,
};


int32_t wifi_get_rssi(void);
char* wifi_get_ssid(void);
char* wifi_get_ip(void);
void wifi_smartconfig_enable(uint8_t on);
void wifi_send_tcpdata(char *ip, uint16_t port, const char *data, int len, uint16_t recv_time);
char* wifi_recv_tcpdata(int *len);
int32_t wifi_send_msg(const void *msg, uint32_t timeout);
void wifi_task_create(void *pvParameters);
void wifi_task_delete(void);

#endif
