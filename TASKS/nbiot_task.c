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

static uint8_t nbiot_access = 0;
static int32_t nbiot_rssi = 99;
static char ipaddr[32] = {0};
static char send_ip[32] = {0};
static char *send_data = NULL;
static int send_len = 0;
static char recv_data[128] = {0};
static int recv_len = 0;
static int nbiot_sock = 0;


int32_t nbiot_get_rssi()
{
	if(nbiot_access)
		return nbiot_rssi;
	else
		return 99;	
}


char* nbiot_get_ip()
{
	return ipaddr;
}


void nbiot_connect_tcp(char *ip, uint16_t port)
{
	Msg_Value_t nbtcpsend_msg = {0};

	memset(send_ip, 0, 32);
	strncpy(send_ip, ip, 31);

	nbtcpsend_msg.cmd = NBIOT_CMD_CONNECT;
	nbtcpsend_msg.value.s_val = port;
	nbiot_send_msg(&nbtcpsend_msg, 1000);	
}


void nbiot_close_tcp(uint16_t port)
{
	Msg_Value_t nbtcpsend_msg = {0};

	nbtcpsend_msg.cmd = NBIOT_CMD_CLOSE;
	nbtcpsend_msg.value.s_val = 0;
	nbiot_send_msg(&nbtcpsend_msg, 1000);	
}


void nbiot_send_tcpdata(const char *data, int len, uint16_t recv_time)
{
	Msg_Value_t nbtcpsend_msg = {0};

	if(send_data != NULL)
		vPortFree(send_data);

	send_data = (char *)pvPortMalloc(len+1);
	memcpy(send_data, data, len);
	send_data[len] = '\0';

	send_len = len;

	nbtcpsend_msg.cmd = NBIOT_CMD_SEND;
	nbtcpsend_msg.value.s_val = recv_time;
	nbiot_send_msg(&nbtcpsend_msg, 1000);	
}


char* nbiot_recv_tcpdata(int *len)
{
	if(recv_len <= 0)
		return NULL;

	*len = recv_len;
	return recv_data;
}


static int nbiot_recv_wait(int sock, char* data, int len, uint32_t timeout)
{
	uint32_t nowTime = xTaskGetTickCount();
	uint32_t endTime = nowTime + timeout;
	int recv_len = 0;

	while(nowTime < endTime)
	{
		vTaskDelay(500);
		recv_len = bc35_recv_tcp_msg(sock, data, len, 500);
		if(recv_len < 0 || recv_len > 0)
			break;

		nowTime = xTaskGetTickCount();	
	}

	return recv_len;	
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
	if(bc35_setup(1000))
		nbiot_access = bc35_get_rssi(&nbiot_rssi, 1000);

	nbiot_queue = xQueueCreate(NBIOT_QUEUE_NUM, sizeof(Msg_Value_t));
	nbiot_event_group = xEventGroupCreate();
		
	while(1)
  	{
		if(xQueueReceive(nbiot_queue, &nbrecv_msg, 3000) == pdTRUE)
		{	
			nbsend_msg.cmd = MSG_CMD_NBIOT;
			ret = false;
			bc35_buffer_flush();

			switch(nbrecv_msg.cmd)
			{	
				case NBIOT_CMD_INFO:
					if(nbiot_access)
					{
						ret = bc35_get_ip(ipaddr, 1000);
					}

					break;

				case NBIOT_CMD_CONNECT:
					if(nbiot_access)
					{
						if(nbiot_sock > 0)
							bc35_close_tcp_socket(nbiot_sock, 3000);
						else
							bc35_close_tcp_socket(1, 3000);
						
						nbiot_sock = bc35_create_tcp_socket(send_ip, nbrecv_msg.value.s_val, 3000);
						if(nbiot_sock > 0)
							ret = true;
					}

					break;

				case NBIOT_CMD_CLOSE:
					if(nbiot_sock > 0)
						ret = bc35_close_tcp_socket(nbiot_sock, 3000);
					else
						ret = bc35_close_tcp_socket(1, 3000);

					break;


				case NBIOT_CMD_SEND:
					if(nbiot_access)
					{
						//bc35_close_tcp_socket(1, 3000);

						if(nbiot_sock < 1)
							break;						

						ret = bc35_send_tcp_msg(nbiot_sock, send_data, send_len, 5000);
						if(ret == false)
							break;

						memset(recv_data, 0, sizeof(recv_data));
						recv_len = nbiot_recv_wait(nbiot_sock, recv_data, sizeof(recv_data), nbrecv_msg.value.s_val);

						//bc35_close_tcp_socket(nbiot_sock, 3000);
						//nbiot_sock = 0;
					}
					break;				
				
				default:
					
					break;
			}			

			if(send_data != NULL)
			{
				vPortFree(send_data);
				send_data = NULL;
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
			nbiot_access = bc35_get_rssi(&nbiot_rssi, 1000);
			bc35_send_null();
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

