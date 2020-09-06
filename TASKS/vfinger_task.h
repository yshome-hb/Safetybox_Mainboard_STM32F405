#ifndef __VFINGER_TASK_H__
#define __VFINGER_TASK_H__

#include "sys.h"


enum vein_cmd_e
{
	VEIN_CMD_CLEAR = 1,
	VEIN_CMD_DELETE,
	VEIN_CMD_ADD,
	VEIN_CMD_QUERY,
};


enum vein_return_e
{
	VEIN_RETURN_NONE = 0,	
	VEIN_RETURN_SUCCESS,
	VEIN_RETURN_INPUT_FINGER,
	VEIN_RETURN_RELEASE_FINGER,
	VEIN_RETURN_FAIL = 0xFF,
};


extern int32_t vein_send_msg(const void *msg, uint32_t timeout);
void vein_task_create(void *pvParameters);
void vein_task_delete(void);

#endif
