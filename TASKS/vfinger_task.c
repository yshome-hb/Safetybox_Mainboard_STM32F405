#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "veinfinger.h"
#include "message.h"
#include "activity_task.h"
#include "vfinger_task.h"

#define VEIN_QUEUE_NUM    	4

#define VEIN_TASK_PRIO		10
#define VEIN_STK_SIZE 		350  


TaskHandle_t Vein_Task_Handler = NULL;
static QueueHandle_t vein_queue = NULL;


static uint8_t vein_db_init(void)
{
	xg_drv_init();
	xg_reboot(1000);
	vTaskDelay(1000);
	xg_connect(portMAX_DELAY);
	
	return 0;
}


static uint8_t vein_db_led(uint8_t on)
{
	return xg_blue_led(on, 1000);
}


static uint8_t vein_db_clear(uint32_t timeout)
{
	return xg_clear_all(timeout);
}


static uint8_t vein_db_delete(uint16_t u_id, uint32_t timeout)
{
	uint8_t u_info = 0;
	uint8_t ret = xg_get_info(u_id, &u_info, timeout);
	
	if(ret == XG_ERR_SUCCESS)
	{
		ret = xg_clear_id(u_id, timeout);
	}

	if(ret == XG_ERR_EMPTY_ID)
	{
		ret = XG_ERR_SUCCESS;
	}	
	
	return ret;
}


int32_t vein_send_msg(const void *msg, uint32_t timeout)
{
	return xQueueSend(vein_queue, msg, timeout);
}


void vein_task(void *pvParameters)
{	
	uint8_t ret = 0;	
	uint16_t user_id = 0;
	Msg_Value_t vrecv_msg = {0};
	Msg_Value_t vsend_msg = {0};
	uint32_t led_time = 0;
	uint16_t wait_flag = 0;
	uint16_t err_flag = 0;

	vein_queue = xQueueCreate(VEIN_QUEUE_NUM, sizeof(Msg_Value_t));	

	vein_db_init();
	vein_db_led(0);

	while(1)
  {
		if(xQueueReceive(vein_queue, &vrecv_msg, 1000) == pdTRUE)
		{	
			wait_flag = 0;
			switch(vrecv_msg.cmd)
			{
				case VEIN_CMD_QUERY:
					vein_db_led(1);
					xg_verify_send();
					wait_flag = 1;
					break;
				
				case VEIN_CMD_DELETE:
					ret = vein_db_delete(vrecv_msg.value.s_val, 3000);
					break;
				
				case VEIN_CMD_CLEAR:
					ret = vein_db_clear(3000);
					break;
				
				case VEIN_CMD_ADD:		
					ret = vein_db_delete(vrecv_msg.value.s_val, 3000);
					if(ret == XG_ERR_SUCCESS)
					{
						vein_db_led(1);
						xg_add_send(vrecv_msg.value.s_val, 2, 5);
						wait_flag = 1;
					}
					break;
			
				default:
					
					break;
			}	
			
			user_id = 0;
			while(wait_flag)
			{
				ret = xg_verify_wait(&user_id, portMAX_DELAY);
				if(ret == XG_INPUT_FINGER)
				{	

				}
				else if(ret == XG_RELEASE_FINGER)
				{
					vsend_msg.cmd = MSG_CMD_VEIN;
					vsend_msg.value.b_val[1] = VEIN_RETURN_RELEASE_FINGER;
					vsend_msg.value.b_val[0] = 0;
					acitivy_send_msg(&vsend_msg, 1000);	
				}
				else
				{
				  	wait_flag = 0;
				}			
			}
			
			if(ret == XG_ERR_SUCCESS)
			{
				vsend_msg.value.b_val[1] = VEIN_RETURN_SUCCESS;
				vsend_msg.value.b_val[0] = user_id;
				led_time = xTaskGetTickCount();
				err_flag = 0;
			}
			// else if(ret == XG_INPUT_FINGER)
			// {
			// 	vsend_msg.value.b_val[1] = VEIN_RETURN_INPUT_FINGER;
			// }
			// else if(ret == VEIN_RETURN_INPUT_FINGER)
			// {
			// 	vsend_msg.value.b_val[1] = VEIN_RETURN_RELEASE_FINGER;
			// }
			else
			{
				vsend_msg.value.b_val[1] = VEIN_RETURN_FAIL;
				vsend_msg.value.b_val[0] = ret;
				led_time = xTaskGetTickCount();
				err_flag = 1;
			}
			
			vsend_msg.cmd = MSG_CMD_VEIN;
			acitivy_send_msg(&vsend_msg, 1000);	
			
			vTaskDelay(100);
		}
		else
		{
			if(led_time > 0 && (xTaskGetTickCount() - led_time) > 2000)
			{
				led_time = 0;
				vein_db_led(0);
				if(err_flag > 0)
				{
					err_flag = 0;
					vsend_msg.cmd = MSG_CMD_VEIN;
					vsend_msg.value.b_val[1] = VEIN_RETURN_NONE;
					vsend_msg.value.b_val[0] = 0;
					acitivy_send_msg(&vsend_msg, 1000);	
				}
			}
		}
	}
}   


void vein_task_create(void *pvParameters)
{
    xTaskCreate((TaskFunction_t )vein_task,     
                (const char*    )"vein_task",   
                (uint16_t       )VEIN_STK_SIZE, 
                (void*          )pvParameters,
                (UBaseType_t    )VEIN_TASK_PRIO,
                (TaskHandle_t*  )&Vein_Task_Handler);  

}


void vein_task_delete(void)
{
	if(Vein_Task_Handler == NULL)
		return;

	vTaskDelete(Vein_Task_Handler);
	Vein_Task_Handler = NULL;

	if(vein_queue != NULL)
	{
		vQueueDelete(vein_queue);
		vein_queue = NULL;
	}

	xg_drv_deinit();
}
