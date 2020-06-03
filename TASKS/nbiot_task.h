#ifndef __NBIOT_TASK_H__
#define __NBIOT_TASK_H__

#include "sys.h"


enum nbiot_cmd_e
{
    NBIOT_CMD_NONE = 1,
    NBIOT_CMD_INFO,
    
};


enum nbiot_return_e
{
	NBIOT_RETURN_NONE = 0,	
	NBIOT_RETURN_SUCCESS,
	NBIOT_RETURN_TIMEOUT,
	NBIOT_RETURN_FAIL = 0xFF,
};


int32_t nbiot_get_rssi(void);
int32_t nbiot_send_msg(const void *msg, uint32_t timeout);
void nbiot_task_create(void *pvParameters);


#endif
