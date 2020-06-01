#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"
#include "fpc_task.h"
#include "fpc1020a.h"
#include "menu.h"
#include "message.h"
#include "activity_task.h"

#define FPC_QUEUE_NUM   4

#define FPC_TASK_PRIO	5
#define FPC_STK_SIZE 	350  

#define FPC_IDENTIFY_BIT   (1<<0)


TaskHandle_t FPC_Task_Handler;
static QueueHandle_t fpc_queue;
static EventGroupHandle_t fpc_event_group = NULL;//24bit


void fpc_identify_enable(uint8_t on)
{
	if(on)
		xEventGroupSetBits(fpc_event_group, FPC_IDENTIFY_BIT);
	else
		xEventGroupClearBits(fpc_event_group, FPC_IDENTIFY_BIT);
}


static uint8_t fpc_db_init(void)
{
	fpc1020a_drv_init();	
	
	return 0;
}


static inline uint8_t fpc_db_clear(uint32_t timeout)
{
	return fpc1020a_clear(timeout);
}

static uint8_t fpc_db_query(uint16_t *u_id, uint32_t timeout)
{
	uint16_t num = 0;
	uint8_t ret = fpc1020a_query(u_id, timeout);
	if(ret == FPC1020A_NAK)
	{
		fpc1020a_number(&num, timeout);
	}
	return ret;
}

static uint8_t fpc_db_delete(uint16_t u_id, uint32_t timeout)
{
	uint8_t auth = 0;
	uint8_t ret = fpc1020a_authority(u_id, &auth, timeout);

	if(ret == FPC1020A_ACK_SUCCESS)
	{
		ret = fpc1020a_delete(u_id, timeout);
	}
	
	if(ret == FPC1020A_ACK_NOUSER)
	{
		ret = FPC1020A_ACK_SUCCESS;
	}

	return ret; 
}

static uint8_t fpc_db_add(uint16_t u_id, uint32_t timeout)
{
	uint16_t num = 0;
	uint8_t ret = fpc_db_delete(u_id, timeout);

	if(ret != FPC1020A_ACK_SUCCESS)
	{
		return ret;		
	}
	
	ret = fpc1020a_enroll(FPC1020A_CMD_ENROLL1, u_id, timeout);
	if(ret != FPC1020A_ACK_SUCCESS)
	{
		if(ret == FPC1020A_NAK)
		{
			fpc1020a_number(&num, timeout);
		}
		return ret;		
	}
	
	ret = fpc1020a_enroll(FPC1020A_CMD_ENROLL2, u_id, timeout);	
	if(ret != FPC1020A_ACK_SUCCESS)
	{
		if(ret == FPC1020A_NAK)
		{
			fpc1020a_number(&num, timeout);
		}		
		return ret;		
	}	
	
	ret = fpc1020a_enroll(FPC1020A_CMD_ENROLL3, u_id, timeout);
	if(ret == FPC1020A_NAK)
	{
		fpc1020a_number(&num, timeout);
	}
	return ret;
}


int32_t fpc_send_msg(const void *msg, uint32_t timeout)
{
	return xQueueSend(fpc_queue, msg, timeout);
}


void fpc_task(void *pvParameters)
{	
	uint8_t ret = 0;	
	uint16_t user_id = 0;
	Msg_Value_t frecv_msg;	
	Msg_Value_t fsend_msg;
	uint32_t fpc_event_bit = 0;	
	uint32_t fpc_time = 0;

	fpc_db_init();

	fpc_queue = xQueueCreate(FPC_QUEUE_NUM, sizeof(Msg_Value_t));
	fpc_event_group = xEventGroupCreate();

	while(1)
	{
		fpc_event_bit = xEventGroupGetBits(fpc_event_group);
		if(fpc_event_bit & FPC_IDENTIFY_BIT)
		{		
			vTaskDelay(500);
			fsend_msg.cmd = FPC_CMD_QUERY;
			fsend_msg.value.s_val = 0;
			fpc_send_msg(&fsend_msg, 1000);
		}

		if(xQueueReceive(fpc_queue, &frecv_msg, 1000) == pdTRUE)
		{		
			user_id = 0;
			
			switch(frecv_msg.cmd)
			{
				case FPC_CMD_QUERY:
					ret = fpc_db_query(&user_id, 3000);
					break;
				
				case FPC_CMD_DELETE:
					ret = fpc_db_delete(frecv_msg.value.s_val, 3000);
					break;
				
				case FPC_CMD_CLEAR:
					ret = fpc_db_clear(3000);
					break;
				
				case FPC_CMD_ADD:
					ret = fpc_db_add(frecv_msg.value.s_val, 5000);
					break;
				
				default:
					
					break;
			}

			if(ret == FPC1020A_ACK_SUCCESS)
			{			
				fsend_msg.value.b_val[1] = FPC_RETURN_SUCCESS;
				fsend_msg.value.b_val[0] = user_id;
				fsend_msg.cmd = MSG_CMD_FPC;
				acitivy_send_msg(&fsend_msg, 1000);				
				fpc_time = 0;
			}
			else if(ret == FPC1020A_NAK)
			{
				fsend_msg.value.b_val[1] = FPC_RETURN_TIMEOUT;
				fsend_msg.value.b_val[0] = 0;
				if((fpc_event_bit & FPC_IDENTIFY_BIT) == 0)
				{				
					fsend_msg.cmd = MSG_CMD_FPC;
					acitivy_send_msg(&fsend_msg, 1000);					
					fpc_time = xTaskGetTickCount();
				}
			}
			else
			{
				fsend_msg.value.b_val[1] = FPC_RETURN_FAIL;
				fsend_msg.value.b_val[0] = ret;
				fpc_time = xTaskGetTickCount();
				fsend_msg.cmd = MSG_CMD_FPC;
				acitivy_send_msg(&fsend_msg, 1000);				
			}
			
			vTaskDelay(100);
		}	
//		else
//		{
			if(fpc_time > 0 && (xTaskGetTickCount() - fpc_time) > 2000)
			{
				fpc_time = 0;
				fsend_msg.cmd = MSG_CMD_FPC;
				fsend_msg.value.b_val[1] = FPC_RETURN_NONE;
				fsend_msg.value.b_val[0] = 0;
				acitivy_send_msg(&fsend_msg, 1000);	
//			}			
		}
	}
}  


void fpc_task_create(void *pvParameters)
{
    xTaskCreate((TaskFunction_t )fpc_task,     
                (const char*    )"fpc_task",   
                (uint16_t       )FPC_STK_SIZE, 
                (void*          )pvParameters,
                (UBaseType_t    )FPC_TASK_PRIO,
                (TaskHandle_t*  )&FPC_Task_Handler);  

}

