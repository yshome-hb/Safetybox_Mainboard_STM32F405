#include <stdlib.h>
#include "stdio.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "queue.h"
#include "input_output.h"
#include "bc35_at.h"
#include "message.h"
#include "activity_task.h"
#include "nbiot_task.h"


#define NBIOT_QUEUE_NUM   	4

#define NBIOT_TASK_PRIO		7
#define NBIOT_STK_SIZE 		256  

//#define WIFI_SMARTCONFIG_BIT   (1<<0)


TaskHandle_t Nbiot_Task_Handler;
static QueueHandle_t nbiot_queue;
static EventGroupHandle_t nbiot_event_group = NULL;//24bit


static int32_t nbiot_rssi = -999;


int32_t nbiot_get_rssi()
{
	return nbiot_rssi;
}
	

int32_t nbiot_send_msg(const void *msg, uint32_t timeout)
{
	return xQueueSend(nbiot_queue, msg, timeout);
}


void nbiot_task(void *pvParameters)
{	
	bool ret = 0;
	Msg_Value_t nbrecv_msg = {0};	
	Msg_Value_t nbsend_msg = {0};
	uint32_t nbiot_event_bit = 0;

	io_output_set(OUTPUT_POWER_NB, 1);
	vTaskDelay(2000);
	bc35_init();
	bc35_setup(1000);

	nbiot_queue = xQueueCreate(NBIOT_QUEUE_NUM, sizeof(Msg_Value_t));
	nbiot_event_group = xEventGroupCreate();
		
	while(1)
  	{
		// face_event_bit = xEventGroupGetBits(face_event_group);
		// if(face_event_bit & FACE_IDENTIFY_BIT)
		// {
		// 	vTaskDelay(100);
		// 	asend_msg.cmd = FACE_CMD_QUERY;
		// 	asend_msg.value.s_val = 0;
		// 	face_send_msg(&asend_msg, 1000);		
		// }

		if(xQueueReceive(nbiot_queue, &nbrecv_msg, 3000) == pdTRUE)
		{	
			nbsend_msg.cmd = MSG_CMD_NBIOT;
			ret = false;
			
			switch(nbrecv_msg.cmd)
			{	
				case NBIOT_CMD_INFO:
					// if(wifi_rssi > -99)
					// {
					// 	ret = esp8266_get_ssid(ssid, 1000);
					// 	if(ret)
					// 		ret = esp8266_get_ip(ipaddr, 1000);
					// }

					break;
				
			// 	case FACE_CMD_CLEAR:
			// 		vTaskDelay(1000);
			// 		ret = face_db_clear(3000);
			// 		break;
				
			// 	case FACE_CMD_ADD:
			// 		vTaskDelay(1000);
			// 		ret = face_db_add(arecv_msg.value.s_val, 5500);
			// 		break;

			// 	case FACE_CMD_ENABLE:
			// 		ret = face_db_up(arecv_msg.value.s_val, 1000);
			// 		break;					
				
				default:
					
					break;
			}			

			if(ret)
				nbsend_msg.value.b_val[1] = NBIOT_RETURN_SUCCESS;
			else
				nbsend_msg.value.b_val[1] = NBIOT_RETURN_FAIL;

			acitivy_send_msg(&nbsend_msg, 1000);
			
			vTaskDelay(100);
		}
		else
		{
			if(!bc35_get_rssi(&nbiot_rssi, 1000))
			{
				nbiot_rssi = -999;
			}
		}
	} 
	vEventGroupDelete(nbiot_event_group);
}  


void nbiot_task_create(void *pvParameters)
{
    xTaskCreate((TaskFunction_t )nbiot_task,     
                (const char*    )"nbiot_task",   
                (uint16_t       )NBIOT_STK_SIZE, 
                (void*          )pvParameters,
                (UBaseType_t    )NBIOT_TASK_PRIO,
                (TaskHandle_t*  )&Nbiot_Task_Handler);  

}

