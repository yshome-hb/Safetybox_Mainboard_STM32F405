#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "sys.h"


enum Msg_Cmd_e
{
  MSG_CMD_NONE 	= 0x00,
  MSG_CMD_INT 	= 0xA1,
  MSG_CMD_KEY,
  MSG_CMD_PASSWORD,  
  MSG_CMD_FPC,
  MSG_CMD_VEIN,
  MSG_CMD_FACE,
  MSG_CMD_RTC,
  MSG_CMD_WIFI,
  MSG_CMD_NBIOT,
  MSG_CMD_SUB,
  MSG_CMD_PUB,
  MSG_CMD_NEXT,
  MSG_CMD_PRE,
};


typedef struct 
{
	uint16_t cmd;
	union
	{
		uint16_t s_val;
		uint8_t b_val[2];	
	}value;
	
}Msg_Value_t;


#endif
