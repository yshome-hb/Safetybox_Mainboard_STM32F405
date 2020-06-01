#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "lcd192x64.h"
#include "spi_slave.h"
#include "menu.h"
#include "message.h"
#include "activity_task.h"


#define ACTIVITY_QUEUE_NUM    	10

#define ACTIVITY_TASK_PRIO		10
#define ACTIVITY_STK_SIZE 		2048 


typedef void (*Activity_Update_f)(void);


const char key_char[] = {
	'*', '7', '4', '1',
	'0', '8', '5', '2',
	'#', '9', '6', '3',
	'\b','\n','d', 'u',
};


TaskHandle_t Acitivity_Task_Handler;
static QueueHandle_t activity_queue;

static struct Menu_Item_t *acvitiy_item = NULL;


uint8_t activity_debug_parse(uint8_t _input)
{
	uint8_t temp = 0;
	for(temp = 0; temp < 16; temp++)
	{
		if(key_char[temp] == _input)
		{
			break;
		}
	}	
	
	if(temp < 16)
	{
		Msg_Value_t debug_msg = {
			.cmd = MSG_CMD_KEY,
			.value = 0xA500|temp,
		};
		BaseType_t xHigherPriorityTaskWoken;
		xQueueSendFromISR(activity_queue, &debug_msg, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
	return 0;
}


static inline uint16_t activity_protocol_parse(uint16_t _input)
{
	BaseType_t xHigherPriorityTaskWoken;
	Msg_Value_t key_msg = {
		.cmd = MSG_CMD_KEY,
		.value = _input,
	};
	xQueueSendFromISR(activity_queue, &key_msg, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	return 0;
}


int32_t acitivy_send_msg(const void *msg, uint32_t timeout)
{
	return xQueueSend(activity_queue, msg, timeout);
}


void acitivy_task(void *pvParameters)
{		
	uint16_t ret = 0;
	Msg_Value_t activity_msg = {0};	
	Activity_Update_f activity_update = (Activity_Update_f)pvParameters;

	spi2_init(activity_protocol_parse);
	activity_queue = xQueueCreate(ACTIVITY_QUEUE_NUM, sizeof(Msg_Value_t));

	lcd_drv_init();		

	while(1)
	{
		if(xQueueReceive(activity_queue, &activity_msg, portMAX_DELAY) == pdTRUE)
		{
			ret = 0;
			if(acvitiy_item != NULL)
			{
				switch(activity_msg.cmd)
				{
					case MSG_CMD_INT:
						if(acvitiy_item->int_process != NULL)
							ret = acvitiy_item->int_process(acvitiy_item, activity_msg.value.s_val);							 
						break;
					 
					case MSG_CMD_KEY:
						printf("key: %x\r\n", activity_msg.value.s_val);
					
						if(acvitiy_item->key_process != NULL)
						{
							if(activity_msg.value.b_val[1] == 0xA5)
								activity_msg.value.b_val[0] = key_char[activity_msg.value.b_val[0]];
							ret = acvitiy_item->key_process(acvitiy_item, activity_msg.value.s_val);	
						}
						break;

					case MSG_CMD_PASSWORD:
						if(acvitiy_item->password_process != NULL)
						{
							ret = acvitiy_item->password_process(acvitiy_item, activity_msg.value.s_val);	
						}						 
						break;

					case MSG_CMD_FPC:
						if(acvitiy_item->fpc_process != NULL)
						{
							ret = acvitiy_item->fpc_process(acvitiy_item, activity_msg.value.s_val);	
						}						 
						break;
					
					case MSG_CMD_VEIN:
						if(acvitiy_item->vein_process != NULL)
						{
							ret = acvitiy_item->vein_process(acvitiy_item, activity_msg.value.s_val);	
						}						 
						break;

					case MSG_CMD_FACE:
						if(acvitiy_item->face_process != NULL)
						{
							ret = acvitiy_item->face_process(acvitiy_item, activity_msg.value.s_val);	
						}						 
						break;	

					case MSG_CMD_RTC:
						if(acvitiy_item->rtc_process != NULL)
						{
							ret = acvitiy_item->rtc_process(acvitiy_item, activity_msg.value.s_val);	
						}						 
						break;					
						
					case MSG_CMD_SUB:
						if(acvitiy_item->sub_menu[acvitiy_item->index] != NULL)
						{
							acvitiy_item = acvitiy_item->sub_menu[acvitiy_item->index];
							ret = acvitiy_item->setup(activity_msg.value.s_val);
						} 
						break;
					case MSG_CMD_PUB:
						if(acvitiy_item->pub_menu != NULL)
						{
							acvitiy_item = acvitiy_item->pub_menu;
							ret = acvitiy_item->setup(activity_msg.value.s_val);
						}						 
						break;
					
					case MSG_CMD_NEXT:
						if(acvitiy_item->next_menu != NULL)
						{
							acvitiy_item = acvitiy_item->next_menu;
							ret = acvitiy_item->setup(activity_msg.value.s_val);
						}							 
						break;
					
					case MSG_CMD_PRE:
						if(acvitiy_item->pre_menu != NULL)
						{
							acvitiy_item = acvitiy_item->pre_menu;
							ret = acvitiy_item->setup(activity_msg.value.s_val);
						}							 
						break;
				 
					default:
						
						break;
					
				 }
				
			}
			if(activity_update != NULL)
			{
				activity_update();
			}

			lcd_drv_update();
			vTaskDelay(50);
		}
	}
}   


void activity_task_create(struct Menu_Item_t *menu, void *activity_hook)
{
	acvitiy_item = menu;

    xTaskCreate((TaskFunction_t )acitivy_task,     
                (const char*    )"activity_task",   
                (uint16_t       )ACTIVITY_STK_SIZE, 
                (void*          )activity_hook,
                (UBaseType_t    )ACTIVITY_TASK_PRIO,
                (TaskHandle_t*  )&Acitivity_Task_Handler);  

}


