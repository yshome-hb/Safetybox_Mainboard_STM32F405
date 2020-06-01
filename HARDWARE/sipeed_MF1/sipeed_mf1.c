#include "stdio.h"
#include "stdlib.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "string.h"
#include "sipeed_mf1.h"


#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

#define TIMEOUT_MIN		1
#define TIMEOUT_MAX		100
#define BUFFER_SIZE 	256

static char mf1_uart_buffer[BUFFER_SIZE] = {0};
static char mf1_json_buffer[BUFFER_SIZE] = {0};

#ifdef USE_JSON
static cJSON *mf1_json = NULL;
#endif

static SemaphoreHandle_t mf1_sem;

static uint8_t mf1_protocol_receive(uint8_t _input)
{
	static uint16_t buffer_index = 0;
	BaseType_t xHigherPriorityTaskWoken;


	if(_input == '\n')
	{
		if(buffer_index > 10)
		{
			memset(mf1_json_buffer, 0, BUFFER_SIZE);
			strncpy(mf1_json_buffer, mf1_uart_buffer, buffer_index);
			memset(mf1_uart_buffer, 0, BUFFER_SIZE);
			buffer_index = 0;
			xSemaphoreGiveFromISR(mf1_sem,&xHigherPriorityTaskWoken);
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}
		else
		{
			memset(mf1_uart_buffer, 0, BUFFER_SIZE);
			buffer_index = 0;	
		}
	}
	else
	{
		if(buffer_index < (BUFFER_SIZE - 1))
		{
			mf1_uart_buffer[buffer_index++] = _input;
		}
		else
		{
			memset(mf1_uart_buffer, 0, BUFFER_SIZE);
			buffer_index = 0;	
		}
	}

	return 0;
}


static void mf1_send_str(const char *str)
{
	while((*str) != '\0')
		uart3_send(*str++);
}


void mf1_init()
{
	uart3_init(MF1_BUADRATE, mf1_protocol_receive);
	mf1_sem = xSemaphoreCreateBinary();
}


#ifdef USE_JSON
struct MF1_Protocol_Handle_t *mf1_buffer_parse(uint32_t timeout)
{
	struct MF1_Protocol_Handle_t *mf1_protocol = NULL;
	cJSON *cjversion = NULL;
	cJSON *cjtype = NULL;
	cJSON *cjcode = NULL;
	cJSON *cjmsg = NULL;

	if(xSemaphoreTake(mf1_sem, timeout) == pdFALSE)
	{			
		return NULL;
	}

	if(mf1_json != NULL)
		cJSON_Delete(mf1_json);

	mf1_json = cJSON_Parse(mf1_json_buffer);
	if(mf1_json == NULL)
	{
		return NULL;
	}

	mf1_protocol = (struct MF1_Protocol_Handle_t *)pvPortMalloc(sizeof(struct MF1_Protocol_Handle_t));
	if(mf1_protocol == NULL)
	{
		cJSON_Delete(mf1_json);
		mf1_json = NULL;
		return NULL;
	}

	memset(mf1_protocol, 0, sizeof(struct MF1_Protocol_Handle_t));

	cjversion = cJSON_GetObjectItem(mf1_json, "version");
	cjtype = cJSON_GetObjectItem(mf1_json, "type");
	cjcode = cJSON_GetObjectItem(mf1_json, "code");
	cjmsg = cJSON_GetObjectItem(mf1_json, "msg");	

	if(cjversion != NULL)
		mf1_protocol->version = cjversion->valueint;

	if(cjtype != NULL)
		strncpy(mf1_protocol->type, cjtype->valuestring, 31);

	if(cjcode != NULL)
		mf1_protocol->code = cjcode->valueint;

	if(cjmsg != NULL)
		strncpy(mf1_protocol->msg, cjmsg->valuestring, 127);

	mf1_protocol->info = cJSON_GetObjectItem(mf1_json, "info");
	mf1_protocol->user = cJSON_GetObjectItem(mf1_json, "user");

	return 	mf1_protocol;
}


void mf1_buffer_delete(struct MF1_Protocol_Handle_t *mf1_protocol)
{
	if(mf1_json != NULL)
	{
		cJSON_Delete(mf1_json);
		mf1_json = NULL;
	}

	if(mf1_protocol == NULL)
		return;

	if(mf1_protocol->info != NULL)
	{
		cJSON_Delete(mf1_protocol->info);
		mf1_protocol->info = NULL;
	}
	
	if(mf1_protocol->user != NULL)
	{
		cJSON_Delete(mf1_protocol->user);
		mf1_protocol->user = NULL;
	}	

	vPortFree(mf1_protocol);
	mf1_protocol = NULL;
}
#else

static char *mf1_buffer_parse(uint32_t timeout)
{
	if(xSemaphoreTake(mf1_sem, timeout) == pdFALSE)
	{			
		return NULL;
	}

	return mf1_json_buffer;
}


void mf1_buffer_flush()
{
	xSemaphoreTake(mf1_sem, 0);
	memset(mf1_json_buffer, 0, BUFFER_SIZE);
	memset(mf1_uart_buffer, 0, BUFFER_SIZE);
}


uint8_t mf1_face_recon(uint8_t enable, uint32_t timeout)
{
	char *recv_str = NULL;
	char *type_str = NULL;
	char *code_str = NULL;	
	uint8_t ret = MF1_NAK;	

	mf1_buffer_flush();

	mf1_send_str("{\"version\":2,\"type\":\""MF1_CMD_FACE_RECON"\",\"query_stat\":0,\"set_stat\":");
	mf1_send_str(enable ? "1" : "0");
	mf1_send_str("}\r\n");


	recv_str = mf1_buffer_parse(timeout);	

	type_str = strstr(recv_str, MF1_CMD_FACE_RECON);
	code_str = strstr(recv_str, MF1_CODE);

	if(type_str != NULL && code_str != NULL && strlen(code_str) > 9)
	{
		ret = atoi(code_str+6);	
	}
	else
	{
		ret = MF1_NAK;
	}

	return ret;
}


uint8_t mf1_wait_face(char **uid, uint32_t timeout)
{
	char *recv_str = NULL;
	char *type_str = NULL;
	char *msg_str = NULL;
	char *uid_str = NULL;

	recv_str = mf1_buffer_parse(timeout);	

	type_str = strstr(recv_str, MF1_CMD_FACE_INFO);
	msg_str = strstr(recv_str, "have face");
	uid_str = strstr(recv_str, "uid");

	if(type_str != NULL && msg_str != NULL && uid_str != NULL && strlen(uid_str) > 40)
	{
		*uid = uid_str+6;
		return MF1_ACK_SUCCESS;
	}

	return MF1_NAK;
}



uint8_t mf1_add_user(const char *uid, uint32_t timeout)
{
	char *recv_str = NULL;
	char *init_str = NULL;		
	char *face_str = NULL;
	char *pos_str = NULL;	
	char *type_str = NULL;
	char *code_str = NULL;	
	uint8_t ret = MF1_NAK;	
	char buf[5] = {0};
	uint32_t timeout_sec = constrain(timeout/1000, TIMEOUT_MIN, TIMEOUT_MAX);
	
	snprintf(buf, 4, "%d", timeout_sec);

	mf1_buffer_flush();

	mf1_send_str("{\"version\":2,\"type\":\""MF1_CMD_ADD_UID"\",\"user\":{\"uid\":\"");
	mf1_send_str(uid);	
	mf1_send_str("\",\"time_s\":");
	mf1_send_str((const char *)buf);
	mf1_send_str("}}\r\n");

	while(ret > MF1_ACK_FAIL)
	{
		recv_str = mf1_buffer_parse(timeout);	

		face_str = strstr(recv_str, MF1_CMD_FACE_INFO);
		pos_str = strstr(recv_str, "face_pos");
		init_str = strstr(recv_str, "init done");		
		type_str = strstr(recv_str, MF1_CMD_ADD_UID);
		code_str = strstr(recv_str, MF1_CODE);

		if(type_str != NULL && code_str != NULL && strlen(code_str) > 9)
		{
			ret = atoi(code_str+6);	
		}
		else if(face_str == NULL && pos_str == NULL && init_str == NULL)
		{
			ret = MF1_NAK;
			break;
		}
	}
	
	return ret;
}


uint8_t mf1_delete_user(const char *uid, uint32_t timeout)
{
	char *recv_str = NULL;
	char *init_str = NULL;		
	char *face_str = NULL;
	char *pos_str = NULL;	
	char *type_str = NULL;
	char *code_str = NULL;
	uint8_t ret = MF1_NAK;

	mf1_buffer_flush();

	mf1_send_str("{\"version\":2,\"type\":\""MF1_CMD_DELETE_UID"\",\"uid\":\"");
	mf1_send_str(uid);	
	mf1_send_str("\"}\r\n");

	while(ret > MF1_ACK_FAIL)
	{
		recv_str = mf1_buffer_parse(timeout);	

		face_str = strstr(recv_str, MF1_CMD_FACE_INFO);
		pos_str = strstr(recv_str, "face_pos");
		init_str = strstr(recv_str, "init done");		
		type_str = strstr(recv_str, MF1_CMD_DELETE_UID);
		code_str = strstr(recv_str, MF1_CODE);

		if(type_str != NULL && code_str != NULL && strlen(code_str) > 9)
		{
			ret = atoi(code_str+6);	
		
			if(ret == 0)
				return MF1_ACK_SUCCESS;
			else if(ret == 3)
				return MF1_ACK_NOUSER;
			else
				return MF1_ACK_FAIL;
		}
		else if(face_str == NULL && pos_str == NULL && init_str == NULL)
		{
			ret = MF1_NAK;
			break;
		}
	}

	return ret;
}


#endif
