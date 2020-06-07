#ifndef __IDENTIFY_TASK_H__
#define __IDENTIFY_TASK_H__

#include "sys.h"


enum identify_type_e
{
	IDENTIFY_TYPE_CLEAR = 1,
    IDENTIFY_TYPE_ADMIN,	
    IDENTIFY_TYPE_PASSOWRD,
    IDENTIFY_TYPE_FPC,
    IDENTIFY_TYPE_VEIN,
    IDENTIFY_TYPE_FACE,
	IDENTIFY_TYPE_SET_ADMIN,
	IDENTIFY_TYPE_SET_PASSWORD,
	IDENTIFY_TYPE_SET_MASK,	
	IDENTIFY_TYPE_SET_FLASH,
	IDENTIFY_TYPE_DEL_USER,
	IDENTIFY_TYPE_RESET_USER,
};


enum password_return_e
{
	PWD_RETURN_NONE = 0,	
	PWD_RETURN_SUCCESS,

	PWD_RETURN_FAIL = 0xFF,
};


enum identify_result_e
{
	IDENTIFY_RESULT_SUCCESS = 0,	

	IDENTIFY_RESULT_FAIL = 0xFF,
};


typedef struct 
{
	uint16_t type;
	union
	{
        char password[9];
		uint32_t uid[2];
	}info;
	
}Msg_Identify_t;


void task_identify_enable(uint8_t on);
uint16_t identify_check_username(const char *str);
uint16_t identify_check_password(const char *str);
int32_t identify_send_msg(const void *msg, uint32_t timeout);
void identify_task_create(void *pvParameters);


#endif
