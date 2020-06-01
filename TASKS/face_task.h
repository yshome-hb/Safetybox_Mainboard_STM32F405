#ifndef __FACE_TASK_H__
#define __FACE_TASK_H__

#include "sys.h"


enum face_cmd_e
{
    FACE_CMD_CLEAR = 1,
    FACE_CMD_DELETE,
    FACE_CMD_ADD,
    FACE_CMD_QUERY,
    FACE_CMD_ENABLE,    
};


enum face_return_e
{
	FACE_RETURN_NONE = 0,	
	FACE_RETURN_SUCCESS,
	FACE_RETURN_TIMEOUT,
	FACE_RETURN_FAIL = 0xFF,
};


void face_identify_enable(uint8_t on);
void face_config_enable(uint8_t on);
int32_t face_send_msg(const void *msg, uint32_t timeout);
void face_task_create(void *pvParameters);


#endif
