#include <stdlib.h>
#include "stdio.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "queue.h"
#include "input_output.h"
#include "esp8266_at.h"
#include "message.h"
#include "activity_task.h"
#include "wifi_task.h"


#define WIFI_QUEUE_NUM   	4

#define WIFI_TASK_PRIO		7
#define WIFI_STK_SIZE 		256  

#define WIFI_SMARTCONFIG_BIT   	(1<<0)
#define WIFI_RECEIVED_BIT    	(1<<1)


TaskHandle_t Wifi_Task_Handler;
static QueueHandle_t wifi_queue;
static EventGroupHandle_t wifi_event_group = NULL;//24bit


static uint8_t wifi_access = 0;
static int32_t wifi_rssi = -999;
static char ssid[32] = {0};
static char ipaddr[32] = {0};
static char send_ip[32] = {0};
static int send_port = 0;
static char *send_data = NULL;
static int send_len = 0;
static char recv_data[128] = {0};
static int recv_len = 0;


int32_t wifi_get_rssi()
{
	if(wifi_access)
		return wifi_rssi;
	else
		return -999;
}


char* wifi_get_ssid()
{
	return ssid;
}

char* wifi_get_ip()
{
	return ipaddr;
}


void wifi_smartconfig_enable(uint8_t on)
{
	if(on)
		xEventGroupSetBits(wifi_event_group, WIFI_SMARTCONFIG_BIT);
	else
		xEventGroupClearBits(wifi_event_group, WIFI_SMARTCONFIG_BIT);
}


void wifi_received_enable(uint8_t on)
{
	if(on)
		xEventGroupSetBits(wifi_event_group, WIFI_RECEIVED_BIT);
	else
		xEventGroupClearBits(wifi_event_group, WIFI_RECEIVED_BIT);
}


void wifi_send_tcpdata(char *ip, int port, const char *data, int len, uint16_t recv_time)
{
	Msg_Value_t stcpsend_msg = {0};

	memset(send_ip, 0, 32);
	strncpy(send_ip, ip, 31);
	send_port = port;

	if(send_data != NULL)
		vPortFree(send_data);

	send_data = (char *)pvPortMalloc(len+1);
	memcpy(send_data, data, len);
	send_data[len] = '\0';

	send_len = len;

	stcpsend_msg.cmd = WIFI_CMD_SEND;
	stcpsend_msg.value.s_val = recv_time;
	wifi_send_msg(&stcpsend_msg, 1000);	
}


char* wifi_recv_tcpdata(int *len)
{
	if(recv_len <= 0)
		return NULL;

	*len = recv_len;
	return recv_data;
}


int32_t wifi_send_msg(const void *msg, uint32_t timeout)
{
	return xQueueSend(wifi_queue, msg, timeout);
}


static int wifi_recv_wait(char* data, int len, uint32_t timeout)
{
	uint32_t nowTime = xTaskGetTickCount();
	uint32_t endTime = nowTime + timeout;
	int recv_len = 0;

	while(nowTime < endTime)
	{
		recv_len = esp8266_recv_tcp_msg(data, len, 500);
		if(recv_len < 0 || recv_len > 0)
			break;
	}

	return recv_len;	
}


void wifi_task(void *pvParameters)
{	
	bool ret = 0;
	Msg_Value_t wrecv_msg = {0};	
	Msg_Value_t wsend_msg = {0};
	uint32_t wifi_event_bit = 0;

	io_output_set(OUTPUT_POWER_WIFI, 1);
	vTaskDelay(2000);
	esp8266_init();
	if(esp8266_setup(1000))
		wifi_access = esp8266_get_rssi(&wifi_rssi, 1000);

	wifi_queue = xQueueCreate(WIFI_QUEUE_NUM, sizeof(Msg_Value_t));
	wifi_event_group = xEventGroupCreate();
		
	while(1)
  	{
		if(xQueueReceive(wifi_queue, &wrecv_msg, 3000) == pdTRUE)
		{	
			wsend_msg.cmd = MSG_CMD_WIFI;
			ret = false;
			
			switch(wrecv_msg.cmd)
			{
				case WIFI_CMD_SMART:
					ret = esp8266_stop_smartconfig(1000);
					if(ret == false)
						break;

					ret = esp8266_start_smartconfig(1000);
					if(ret == false)
						break;

					wifi_smartconfig_enable(1);
					do
					{
						ret = esp8266_wait_connection(1000);
						wifi_event_bit = xEventGroupGetBits(wifi_event_group);

					}while((wifi_event_bit & WIFI_SMARTCONFIG_BIT) && (ret == false));

					wifi_smartconfig_enable(0);
					wifi_access = 1;

					break;
				
				case WIFI_CMD_INFO:
					if(wifi_access)
					{
						ret = esp8266_get_ssid(ssid, 1000);
						if(ret)
							ret = esp8266_get_ip(ipaddr, 1000);
					}

					break;
				
				case WIFI_CMD_SEND:
					if(wifi_access)
					{
						esp8266_close_tcp_socket(1000);
						ret = esp8266_create_tcp_socket(send_ip, send_port, 1000);
						if(ret == false)
							break;

						ret = esp8266_send_tcp_msg(send_data, send_len, 1000);
						if(ret == false)
							break;


						memset(recv_data, 0, sizeof(recv_data));
						recv_len = wifi_recv_wait(recv_data, sizeof(recv_data), wrecv_msg.value.s_val);						
						
						esp8266_close_tcp_socket(1000);				
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
				wsend_msg.value.b_val[1] = WIFI_RETURN_SUCCESS;
			else
				wsend_msg.value.b_val[1] = WIFI_RETURN_FAIL;

			acitivy_send_msg(&wsend_msg, 1000);
			
			vTaskDelay(100);
		}
		else
		{
			wifi_event_bit = xEventGroupGetBits(wifi_event_group);
			wifi_access = esp8266_get_rssi(&wifi_rssi, 1000);
		}
	} 
	vEventGroupDelete(wifi_event_group);
}  


void wifi_task_create(void *pvParameters)
{
    xTaskCreate((TaskFunction_t )wifi_task,     
                (const char*    )"wifi_task",   
                (uint16_t       )WIFI_STK_SIZE, 
                (void*          )pvParameters,
                (UBaseType_t    )WIFI_TASK_PRIO,
                (TaskHandle_t*  )&Wifi_Task_Handler);  

}

