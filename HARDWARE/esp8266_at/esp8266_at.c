#include "stdio.h"
#include "stdlib.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "string.h"
#include "esp8266_at.h"


#define BUFFER_SIZE 	256

static char esp8266_uart_buffer[BUFFER_SIZE] = {0};
static uint16_t buffer_index = 0;
	

static SemaphoreHandle_t esp8266_sem;

static uint8_t esp8266_protocol_receive(uint8_t _input)
{
	BaseType_t xHigherPriorityTaskWoken;

	if(_input == '\n')
	{
		if(buffer_index > 1)
		{
			xSemaphoreGiveFromISR(esp8266_sem, &xHigherPriorityTaskWoken);
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}
		else
		{
			memset(esp8266_uart_buffer, 0, BUFFER_SIZE);
			buffer_index = 0;	
		}
	}
	else
	{
		if(buffer_index < (BUFFER_SIZE - 1))
		{
			esp8266_uart_buffer[buffer_index++] = _input;
		}
		else
		{
			memset(esp8266_uart_buffer, 0, BUFFER_SIZE);
			buffer_index = 0;	
		}
	}

	return 0;
}


static void esp8266_send_byte(char byte)
{
	uart6_send(byte);
}

static void esp8266_send_str(const char *str)
{
	while((*str) != '\0')
		uart6_send(*str++);
}

static uint16_t esp8266_recv_bytes(uint8_t *buf, uint16_t offset, uint16_t len, uint32_t timeout)
{
	BaseType_t err = pdFALSE;
	err = xSemaphoreTake(esp8266_sem, timeout);
	if(err == pdFALSE)
		return 0;

	if(len > buffer_index)
		len = buffer_index;
	memcpy(buf+offset, esp8266_uart_buffer, len);
	memset(esp8266_uart_buffer, 0, BUFFER_SIZE);
	buffer_index = 0;
	return len;
}


static char* esp8266_recv_wait_str(uint32_t timeout, char *str_wait)
{
 	static char buf_chk[512] = "";	 
	uint16_t recv_len = 0;
	uint16_t recv_offset = 0;	
	uint16_t recv_left = 0;
	uint32_t nowTime = xTaskGetTickCount();
	uint32_t endTime = nowTime + timeout;

	memset(buf_chk, 0, 512);	

	while(nowTime < endTime)
	{
		recv_left = ((recv_offset + BUFFER_SIZE) > 512) ? (512 - recv_offset) : BUFFER_SIZE;
        recv_len = esp8266_recv_bytes((uint8_t *)buf_chk, recv_offset, recv_left, 100);
        recv_offset += recv_len;     
        if (recv_len > 0) 
        {
            buf_chk[recv_offset-1] = ' ';
            if(strstr(buf_chk, str_wait))
			{
                return buf_chk;  
			}
		    else if(strstr(buf_chk, "ERROR"))
			{
                break;
			}   
			else if(recv_offset > 510)
			{
				break;
			}
            // else if(strstr(str_chk, ignore_msg))
            // {
            //     str_index = 0;
            // }
            		
		}        
        nowTime = xTaskGetTickCount();
	}	

	return NULL;
}


void esp8266_buffer_flush()
{
	xSemaphoreTake(esp8266_sem, 0);
	memset(esp8266_uart_buffer, 0, BUFFER_SIZE);
}

void esp8266_init()
{
	uart6_init(ESP8266_BUADRATE, esp8266_protocol_receive);
	esp8266_sem = xSemaphoreCreateBinary();
}


bool esp8266_setup(uint32_t timeout)
{
	esp8266_send_str("ATE0\r\n");

	esp8266_buffer_flush();

	if(!esp8266_set_mode(1, timeout))
		return false;
	
	return true;
}


bool esp8266_testAT(uint32_t timeout)
{
	esp8266_send_str("AT\r\n");
	return (esp8266_recv_wait_str(timeout, "OK") != NULL);
}


bool esp8266_set_mode(uint8_t mode, uint32_t timeout)
{
	esp8266_send_str("AT+CWMODE=");
	esp8266_send_byte(mode+'0');
	esp8266_send_str("\r\n");
	return (esp8266_recv_wait_str(timeout, "OK") != NULL);
}


bool esp8266_get_rssi(int *rssi, uint32_t timeout)
{
	int i = 0;
	char *str_out = NULL;
	char *p = NULL;

	esp8266_send_str("AT+CWJAP?\r\n");
	str_out =  esp8266_recv_wait_str(timeout, "OK");

	if(str_out == NULL || strstr(str_out, "CWJAP") == NULL)
		return false;

	p = str_out;
	for(i = 0; i < 3; i++)
	{
		p = strchr(p+1, ',');
		if(p == NULL)
			return false;
	}
	
	*rssi = atoi(++p);
	return true;
}


bool esp8266_get_ssid(char *ssid, uint32_t timeout)
{
	char *str_out = NULL;
	char *p1 = NULL;
	char *p2 = NULL;

	esp8266_send_str("AT+CWJAP?\r\n");
	str_out =  esp8266_recv_wait_str(timeout, "OK");

	if(str_out == NULL || strstr(str_out, "CWJAP") == NULL)
		return false;

	p1 = strstr(str_out, ":\"");
	if(p1 == NULL)
		return false;

	p1 += 2;
	p2 = strstr(p1, "\"");
	if(p2 == NULL)
		return false;

	strncpy(ssid, p1, p2-p1);
	return true;
}


bool esp8266_start_smartconfig(uint32_t timeout)
{
	esp8266_send_str("AT+CWSTARTSMART\r\n");
	return (esp8266_recv_wait_str(timeout, "OK") != NULL);
}


bool esp8266_stop_smartconfig(uint32_t timeout)
{
	esp8266_send_str("AT+CWSTOPSMART\r\n");
	return (esp8266_recv_wait_str(timeout, "OK") != NULL);
}


bool esp8266_wait_connection(uint32_t timeout)
{
	return (esp8266_recv_wait_str(timeout, "smartconfig connected") != NULL);
}


bool esp8266_get_ip(char *ip, uint32_t timeout)
{
	char *str_out = NULL;
	char *p1 = NULL;
	char *p2 = NULL;

	esp8266_send_str("AT+CIPSTA?\r\n");
	str_out =  esp8266_recv_wait_str(timeout, "OK");

	if(str_out == NULL || strstr(str_out, "CIPSTA") == NULL)
		return false;

	p1 = strstr(str_out, "ip:\"");
	if(p1 == NULL)
		return false;

	p1 += 4;
	p2 = strstr(p1, "\"");
	if(p2 == NULL)
		return false;

	strncpy(ip, p1, p2-p1);
	return true;
}


bool esp8266_create_tcp_socket(char *ip, int port, uint32_t timeout)
{
	char tmp[32] = {0};
	char *str_out = NULL;

	esp8266_send_str("AT+CIPRECVMODE=1\r\n");
	str_out =  esp8266_recv_wait_str(timeout, "OK");

	if(str_out == NULL)
		return false;

  	sprintf(tmp, "%d", port);
	esp8266_send_str("AT+CIPSTART=\"TCP\",\"");
	esp8266_send_str(ip);
	esp8266_send_str("\",");
	esp8266_send_str(tmp);
	esp8266_send_str("\r\n");
	str_out =  esp8266_recv_wait_str(timeout, "OK");

	if(str_out == NULL)
		return false;

	return true;
}


bool esp8266_send_tcp_msg(char *data, int len, uint32_t timeout)
{
	char tmp[32] = {0};
	char *str_out = NULL;

  	sprintf(tmp, "%d", len);
	esp8266_send_str("AT+CIPSEND=");
	esp8266_send_str(tmp);
	esp8266_send_str("\r\n");
	str_out =  esp8266_recv_wait_str(timeout, "OK");

	if(str_out == NULL)
		return false;

	esp8266_send_str(data);	
	str_out =  esp8266_recv_wait_str(timeout, "OK");

	if(str_out == NULL)
		return false;	

	return true;
}


int esp8266_recv_tcp_msg(char *data, int len, uint32_t timeout)
{
	char tmp[32] = {0};
	char *str_out = NULL;
	char *p1 = NULL;
	char *p2 = NULL;
	int recv_len = 0;

  	sprintf(tmp, "%d", len);
	esp8266_send_str("AT+CIPRECVDATA=");
	esp8266_send_str(tmp);
	esp8266_send_str("\r\n");
	str_out =  esp8266_recv_wait_str(timeout, "OK");

	if(str_out == NULL)
		return -1;

	p1 = strstr(str_out, "+CIPRECVDATA");
	p2 = strchr(str_out, ':');

	if(p1 == NULL || p2 == NULL)
		return 0;

	recv_len = atoi(p1+13);
	if(len > recv_len)
		len = recv_len;

	memcpy(data, ++p2, len);

	return len;
}


bool esp8266_close_tcp_socket(uint32_t timeout)
{
	char *str_out = NULL;

	esp8266_send_str("AT+CIPCLOSE\r\n");
	str_out =  esp8266_recv_wait_str(timeout, "OK");

	if(str_out == NULL)
		return false;

	return true;
}