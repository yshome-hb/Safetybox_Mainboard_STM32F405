#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "queue.h"
#include "device_info.h"
#include "message.h"
#include "activity_task.h"
#include "fpc_task.h"
#include "vfinger_task.h"
#include "face_task.h"
#include "rtc_task.h"
#include "identify_task.h"


#define IDENTIFY_QUEUE_NUM    4

#define IDENTIFY_TASK_PRIO	  7
#define IDENTIFY_STK_SIZE 	  512  


#define IDENTIFY_ENABLE_BIT   (1<<0)


TaskHandle_t Identify_Task_Handler;
static QueueHandle_t identify_queue;
static EventGroupHandle_t identify_event_group = NULL;//24bit


void task_identify_enable(uint8_t on)
{
	if(on)
		xEventGroupSetBits(identify_event_group, IDENTIFY_ENABLE_BIT);
	else
		xEventGroupClearBits(identify_event_group, IDENTIFY_ENABLE_BIT);
}


static uint16_t identify_set_mask(uint32_t *mask)
{
	if(mask[0] == 0 && mask[1] == 0)
		mask[0] = ID_PWD_BIT;

	device_set_or_mask(mask[0]);
	device_set_and_mask(mask[1]);
	return 1;
}


static uint16_t identify_del_user(uint8_t index)
{
	return device_delete_user(index);
}


static uint16_t identify_reset_user(uint8_t index)
{
	device_info_reset();
	return 1;
}


static void identify_clear_bit(uint32_t *mask, uint16_t size, uint32_t bit)
{
	for(uint16_t i = 0; i < size; i++)
	{
		mask[i] &= (~bit);
	}
}


static uint16_t identify_check_admin(const char *str)
{
	uint16_t ret = 0xFFFF;
	
	if(strncmp(str, device_get_admin(), 12) == 0)
		ret = 0;

	return ret;
}


static uint16_t identify_set_admin(const char *str)
{
	return device_set_admin("admin", str);
}


uint16_t identify_check_username(const char *str)
{
	uint16_t ret = 0xFFFF;
	uint16_t i = 0;
	User_Info_t *u_info = NULL;
	
	for(i = 0; i < 10; i++)
	{
		u_info = device_get_user(i);
		if(u_info->enable && (strncmp(str, u_info->username, 12) == 0))
		{
			ret = i;
			break;
		}
	}

	return ret;
}


uint16_t identify_check_password(const char *str)
{
	uint16_t ret = 0xFFFF;
	uint16_t i = 0;
	User_Info_t *u_info = NULL;
	
	for(i = 0; i < 10; i++)
	{
		u_info = device_get_user(i);
		if(u_info->enable && (strncmp(str, u_info->password, 12) == 0))
		{
			ret = i;
			break;
		}
	}

	return ret;
}


static uint16_t identify_check_fpc(uint16_t id)
{
	uint16_t ret = 0xFFFF;	
	uint16_t i = 0;
	User_Info_t *u_info = NULL;

	if(id > 0)
	{
		for(i = 0; i < USER_MAX; i++)
		{
			u_info = device_get_user(i);
			if(u_info->enable && id == u_info->fid)
			{
				ret = i;
				break;
			}
		}
	}

	return ret;
}


static uint16_t identify_check_vein(uint16_t id)
{
	uint16_t ret = 0xFFFF;	
	uint16_t i = 0;
	User_Info_t *u_info = NULL;

	if(id > 0)
	{
		for(i = 0; i < USER_MAX; i++)
		{
			u_info = device_get_user(i);
			if(u_info->enable && id == u_info->vid)
			{
				ret = i;
				break;
			}
		}
	}

	return ret;
}


static uint16_t identify_check_face(uint16_t id)
{
	uint16_t ret = 0xFFFF;	
	uint16_t i = 0;
	User_Info_t *u_info = NULL;

	if(id > 0)
	{
		for(i = 0; i < USER_MAX; i++)
		{
			u_info = device_get_user(i);
			if(u_info->enable && id == u_info->aid)
			{
				ret = i;
				break;
			}
		}
	}

	return ret;
}



int32_t identify_send_msg(const void *msg, uint32_t timeout)
{
	return xQueueSend(identify_queue, msg, timeout);
}


void identify_task(void *pvParameters)
{		
	uint16_t admin_id = 0;		
	uint16_t password_id = 0;	
	uint16_t fpc_id = 0;
	uint16_t vein_id = 0;
	uint16_t face_id = 0;	
	uint16_t set_id = 0;		
	uint32_t dev_or_mask = 0;
	uint32_t dev_and_mask = 0;
	uint8_t dev_or_flag = 0;
	uint8_t dev_and_flag = 0;	
	uint32_t user_mask[USER_MAX] = {0};
	uint32_t admin_mask = 0;
	uint32_t all_mask = 0;
	uint32_t pwd_time = 0;
	uint16_t cnt_flag = 0;
	uint32_t identify_event_bit = 0;	

	Msg_Identify_t recv_msg_id = {0};
	Msg_Value_t idsend_msg = {0};

	identify_event_group = xEventGroupCreate();
	identify_queue = xQueueCreate(IDENTIFY_QUEUE_NUM, sizeof(Msg_Identify_t));	

	fpc_task_create(NULL);      
	vein_task_create(NULL);
	face_task_create(NULL);

	while(1)
	{
		if(xQueueReceive(identify_queue, &recv_msg_id, 1000) == pdTRUE)
		{
			set_id = 0;
			switch(recv_msg_id.type)
			{
				case IDENTIFY_TYPE_CLEAR:
					identify_clear_bit(user_mask, USER_MAX, ID_PWD_BIT | ID_FPC_BIT | ID_VEIN_BIT | ID_FACE_BIT);
					admin_mask = 0;
					break;

				case IDENTIFY_TYPE_ADMIN:
					admin_id = identify_check_admin(recv_msg_id.info.password);
					if(admin_id < 0xFFFF)
					{
						//admin_mask |= ID_PWD_BIT;
						idsend_msg.value.b_val[1] = PWD_RETURN_SUCCESS;	
						pwd_time = 0;
					}
					else
					{
						//admin_mask = 0;
						identify_clear_bit(user_mask, USER_MAX, ID_PWD_BIT);
						idsend_msg.value.b_val[1] = PWD_RETURN_FAIL;	
						pwd_time = xTaskGetTickCount();
					}

					idsend_msg.cmd = MSG_CMD_PASSWORD;
					idsend_msg.value.b_val[0] = 1;
					acitivy_send_msg(&idsend_msg, 1000);					
					break;

				case IDENTIFY_TYPE_PASSOWRD:
					password_id = identify_check_password(recv_msg_id.info.password);
					if(password_id < 0xFFFF)
					{
						user_mask[password_id] |= ID_PWD_BIT;
						idsend_msg.value.b_val[1] = PWD_RETURN_SUCCESS;	
						pwd_time = 0;
					}
					else
					{
						identify_clear_bit(user_mask, USER_MAX, ID_PWD_BIT);
						idsend_msg.value.b_val[1] = PWD_RETURN_FAIL;	
						pwd_time = xTaskGetTickCount();
					}

					idsend_msg.cmd = MSG_CMD_PASSWORD;
					idsend_msg.value.b_val[0] = 0;
					acitivy_send_msg(&idsend_msg, 1000);
					break;

				case IDENTIFY_TYPE_FPC:
					fpc_id = identify_check_fpc(recv_msg_id.info.uid[0]);
					if(fpc_id < 0xFFFF)
						user_mask[fpc_id] |= ID_FPC_BIT;
					else
					{
						if(recv_msg_id.info.uid[0] > 0)
						{
							admin_mask |= ID_FPC_BIT;
						}
						else
						{
							identify_clear_bit(user_mask, USER_MAX, ID_FPC_BIT);
							admin_mask &= (~ID_FPC_BIT);
						}
					}
					break;

				case IDENTIFY_TYPE_VEIN:
					vein_id = identify_check_vein(recv_msg_id.info.uid[0]);
					if(vein_id < 0xFFFF)
						user_mask[vein_id] |= ID_VEIN_BIT;
					else
					{
						if(recv_msg_id.info.uid[0] > 0)
						{
							admin_mask |= ID_VEIN_BIT;
						}
						else
						{
							identify_clear_bit(user_mask, USER_MAX, ID_VEIN_BIT);
							admin_mask &= (~ID_VEIN_BIT);
						}
					}
					break;

				case IDENTIFY_TYPE_FACE:
					face_id = identify_check_face(recv_msg_id.info.uid[0]);
					if(face_id < 0xFFFF)
						user_mask[face_id] |= ID_FACE_BIT;
					else
					{
						if(recv_msg_id.info.uid[0] > 0)
						{
							admin_mask |= ID_FACE_BIT;
						}
						else
						{						
							identify_clear_bit(user_mask, USER_MAX, ID_FACE_BIT);
							admin_mask &= (~ID_FACE_BIT);
						}
					}
					break;

				case IDENTIFY_TYPE_SET_ADMIN:
					set_id = identify_set_admin(recv_msg_id.info.password);
					break;

				case IDENTIFY_TYPE_SET_PASSWORD:
					
					break;					

				case IDENTIFY_TYPE_SET_MASK:
					set_id = identify_set_mask(recv_msg_id.info.uid);
					break;

				case IDENTIFY_TYPE_SET_USER:
					set_id = 1;
					break;					

				case IDENTIFY_TYPE_DEL_USER:
					set_id = identify_del_user(recv_msg_id.info.uid[0]);
					break;

				case IDENTIFY_TYPE_RESET_USER:
					set_id = identify_reset_user(recv_msg_id.info.uid[0]);
					break;


				default:

					break;
			}	

			identify_event_bit = xEventGroupGetBits(identify_event_group);

			if(identify_event_bit & IDENTIFY_ENABLE_BIT)
			{
				dev_or_mask = device_get_or_mask();
				dev_and_mask = device_get_and_mask();
				all_mask = admin_mask;
				dev_or_flag = 0xFF;
				dev_and_flag = 0xFF;

				for(uint32_t i = 0; i < USER_MAX; i++)
				{
					all_mask |= user_mask[i];
					if((user_mask[i] & dev_and_mask) == dev_and_mask)
					{	
						dev_and_flag = i;
						if(dev_or_mask == 0)
							dev_or_flag = i;
						else if((user_mask[i] & dev_or_mask) > 0)
							dev_or_flag = i;
					}
				}

				if(dev_and_flag < 0xFF && dev_and_flag == dev_or_flag)
				{
					idsend_msg.cmd = MSG_CMD_SUB;
					idsend_msg.value.b_val[1] = IDENTIFY_RESULT_SUCCESS;
					idsend_msg.value.b_val[0] = dev_and_flag;
					acitivy_send_msg(&idsend_msg, 1000);	
					identify_clear_bit(user_mask, USER_MAX, ID_PWD_BIT | ID_FPC_BIT | ID_VEIN_BIT | ID_FACE_BIT);
					admin_mask = 0;
					all_mask = 0;
				}
				else if((all_mask & dev_and_mask) == dev_and_mask && 
						(dev_or_mask == 0 || (all_mask & dev_or_mask) > 0))
				{
					idsend_msg.cmd = MSG_CMD_SUB;
					idsend_msg.value.b_val[1] = IDENTIFY_RESULT_FAIL;
					idsend_msg.value.b_val[0] = 0;
					acitivy_send_msg(&idsend_msg, 1000);
					identify_clear_bit(user_mask, USER_MAX, ID_PWD_BIT | ID_FPC_BIT | ID_VEIN_BIT | ID_FACE_BIT);
					admin_mask = 0;
					all_mask = 0;
				}

				if(cnt_flag == 0 && all_mask > 0)
				{
					cnt_flag = 1;
					rtc_cnt_enable(1, RTC_FLAG_IDENTIY);
				}
				else if(cnt_flag > 0 && all_mask == 0)
				{
					cnt_flag = 0;
					rtc_cnt_enable(0, RTC_FLAG_IDENTIY);
				}

			}

			if(set_id)
				device_info_flash();
					
			vTaskDelay(50);
		}
		else
		{
			if(pwd_time > 0 && (xTaskGetTickCount() - pwd_time) > 2000)
			{
				pwd_time = 0;
				idsend_msg.cmd = MSG_CMD_PASSWORD;
				idsend_msg.value.b_val[1] = PWD_RETURN_NONE;
				idsend_msg.value.b_val[0] = 0;
				acitivy_send_msg(&idsend_msg, 1000);	
			}
		}
	}
}   



void identify_task_create(void *pvParameters)
{
    xTaskCreate((TaskFunction_t )identify_task,     
                (const char*    )"identify_task",   
                (uint16_t       )IDENTIFY_STK_SIZE, 
                (void*          )pvParameters,
                (UBaseType_t    )IDENTIFY_TASK_PRIO,
                (TaskHandle_t*  )&Identify_Task_Handler);  
}


