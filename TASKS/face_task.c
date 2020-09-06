#include <stdlib.h>
#include "stdio.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "queue.h"
#include "sipeed_mf1.h"
#include "message.h"
#include "activity_task.h"
#include "face_task.h"


#define FACE_QUEUE_NUM   	4

#define FACE_TASK_PRIO		10
#define FACE_STK_SIZE 		1024  

#define FACE_IDENTIFY_BIT   (1<<0)
#define FACE_CONFIG_BIT   	(1<<1)


TaskHandle_t Face_Task_Handler = NULL;
static QueueHandle_t face_queue = NULL;
static EventGroupHandle_t face_event_group = NULL;//24bit


void face_identify_enable(uint8_t on)
{
	mf1_buffer_flush();
	if(on)
		xEventGroupSetBits(face_event_group, FACE_IDENTIFY_BIT);
	else
		xEventGroupClearBits(face_event_group, FACE_IDENTIFY_BIT);
}


void face_config_enable(uint8_t on)
{
	mf1_buffer_flush();
	if(on)
		xEventGroupSetBits(face_event_group, FACE_CONFIG_BIT);
	else
		xEventGroupClearBits(face_event_group, FACE_CONFIG_BIT);
}


static uint8_t face_db_check(uint16_t *u_id, uint32_t timeout)
{
	char *uid_str = NULL;
	uint8_t ret = mf1_wait_face(&uid_str, timeout);

	if(ret == MF1_ACK_SUCCESS)
	{
		*u_id = atoi(uid_str+30);	
	}
	
	return ret;
}


static uint8_t face_db_up(uint16_t on, uint32_t timeout)
{
	//uint8_t ret = mf1_face_recon(on, timeout);;
	uint8_t ret = 1;
	return ret;
}


static uint8_t face_db_clear(uint32_t timeout)
{
	uint8_t ret = mf1_delete_user("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF", timeout);

	if(ret == MF1_ACK_SUCCESS || ret == MF1_ACK_NOUSER)
		return MF1_ACK_SUCCESS;

	return ret;
}


static uint8_t face_db_delete(uint16_t u_id, uint32_t timeout)
{
	char tmp_str[34] = {0};
	uint8_t ret = 0xFF;
	snprintf(tmp_str, 33, "EDE6E800A2000000000000000000%04X", u_id);

	ret = mf1_delete_user(tmp_str, timeout);

	if(ret == MF1_ACK_SUCCESS || ret == MF1_ACK_NOUSER)
		return MF1_ACK_SUCCESS;

	return ret;
}



static uint8_t face_db_add(uint16_t u_id, uint32_t timeout)
{
	char tmp_str[34] = {0};
	uint8_t ret = 0xFF;
	snprintf(tmp_str, 33, "EDE6E800A2000000000000000000%04X", u_id);
	
	ret = mf1_delete_user(tmp_str, timeout);

	if(ret != MF1_ACK_SUCCESS && ret != MF1_ACK_NOUSER)
		return ret;
	
	ret = mf1_add_user(tmp_str, timeout);
	
	return ret;

}



int32_t face_send_msg(const void *msg, uint32_t timeout)
{
	return xQueueSend(face_queue, msg, timeout);
}


void face_task(void *pvParameters)
{	
	uint8_t ret = 0;
	uint16_t user_id = 0;
	Msg_Value_t arecv_msg = {0};	
	Msg_Value_t asend_msg = {0};
	uint32_t face_event_bit = 0;
	uint32_t face_time = 0;	

	mf1_init();

	face_queue = xQueueCreate(FACE_QUEUE_NUM, sizeof(Msg_Value_t));
	face_event_group = xEventGroupCreate();
		
	while(1)
  	{
		face_event_bit = xEventGroupGetBits(face_event_group);
		if(face_event_bit & FACE_IDENTIFY_BIT)
		{
			vTaskDelay(100);
			asend_msg.cmd = FACE_CMD_QUERY;
			asend_msg.value.s_val = 0;
			face_send_msg(&asend_msg, 1000);		
		}

		if(xQueueReceive(face_queue, &arecv_msg, 1000) == pdTRUE)
		{	
			user_id = 0;
			
			switch(arecv_msg.cmd)
			{
				case FACE_CMD_QUERY:
					ret = face_db_check(&user_id, 3000);
					break;
				
				case FACE_CMD_DELETE:
					vTaskDelay(1000);
					ret = face_db_delete(arecv_msg.value.s_val, 3000);
					break;
				
				case FACE_CMD_CLEAR:
					vTaskDelay(1000);
					ret = face_db_clear(3000);
					break;
				
				case FACE_CMD_ADD:
					vTaskDelay(1000);
					ret = face_db_add(arecv_msg.value.s_val, 5500);
					break;

				case FACE_CMD_ENABLE:
					ret = face_db_up(arecv_msg.value.s_val, 1000);
					break;					
				
				default:
					
					break;
			}			

			if(ret == MF1_ACK_SUCCESS)
			{
				asend_msg.cmd = MSG_CMD_FACE;				
				asend_msg.value.b_val[1] = FACE_RETURN_SUCCESS;
				asend_msg.value.b_val[0] = user_id;
				acitivy_send_msg(&asend_msg, 1000);
				face_time = 0;	
			}
			else if(ret == MF1_NAK)
			{
				asend_msg.cmd = MSG_CMD_FACE;
				asend_msg.value.b_val[1] = FACE_RETURN_TIMEOUT;
				asend_msg.value.b_val[0] = 0;
				acitivy_send_msg(&asend_msg, 1000);
				face_time = xTaskGetTickCount();
			}
			else
			{
				asend_msg.cmd = MSG_CMD_FACE;
				asend_msg.value.b_val[1] = FACE_RETURN_FAIL;
				asend_msg.value.b_val[0] = ret;
				acitivy_send_msg(&asend_msg, 1000);
				face_time = xTaskGetTickCount();
			}

			
			vTaskDelay(100);
		}
		// else
		// {
			if(face_time > 0 && (xTaskGetTickCount() - face_time) > 2000)
			{
				face_time = 0;
				asend_msg.cmd = MSG_CMD_FACE;
				asend_msg.value.b_val[1] = FACE_RETURN_NONE;
				asend_msg.value.b_val[0] = 0;
				acitivy_send_msg(&asend_msg, 1000);	
			}				
		// }
	} 
	vEventGroupDelete(face_event_group);
}  


void face_task_create(void *pvParameters)
{
    xTaskCreate((TaskFunction_t )face_task,     
                (const char*    )"face_task",   
                (uint16_t       )FACE_STK_SIZE, 
                (void*          )pvParameters,
                (UBaseType_t    )FACE_TASK_PRIO,
                (TaskHandle_t*  )&Face_Task_Handler);  

}


void face_task_delete(void)
{
	if(Face_Task_Handler == NULL)
		return;

	vTaskDelete(Face_Task_Handler);
	Face_Task_Handler = NULL;

	if(face_queue != NULL)
	{
		vQueueDelete(face_queue);
		face_queue = NULL;
	}

	mf1_deinit();
}
