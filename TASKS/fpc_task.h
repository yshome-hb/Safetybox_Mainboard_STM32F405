#ifndef __FPC_TASK_H__
#define __FPC_TASK_H__

#include "sys.h"


enum fpc_cmd_e
{
    FPC_CMD_CLEAR = 1,
    FPC_CMD_DELETE,
    FPC_CMD_ADD,
    FPC_CMD_QUERY,
};


enum fpc_return_e
{
	FPC_RETURN_NONE = 0,	
	FPC_RETURN_SUCCESS,
	FPC_RETURN_TIMEOUT,
	FPC_RETURN_FAIL = 0xFF,
};

void fpc_identify_enable(uint8_t on);
int32_t fpc_send_msg(const void *msg, uint32_t timeout);
void fpc_task_create(void *pvParameters);


#endif
