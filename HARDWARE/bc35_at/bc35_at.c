#include "stdio.h"
#include "stdlib.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "string.h"
#include "bc35_at.h"


#define BUFFER_SIZE 	256

static char bc35_uart_buffer[BUFFER_SIZE] = {0};
static uint16_t buffer_index = 0;
	

static SemaphoreHandle_t bc35_sem;

static uint8_t bc35_protocol_receive(uint8_t _input)
{
	BaseType_t xHigherPriorityTaskWoken;

	if(_input == '\n')
	{
		if(buffer_index > 1)
		{
			xSemaphoreGiveFromISR(bc35_sem, &xHigherPriorityTaskWoken);
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}
		else
		{
			memset(bc35_uart_buffer, 0, BUFFER_SIZE);
			buffer_index = 0;	
		}
	}
	else
	{
		if(buffer_index < (BUFFER_SIZE - 1))
		{
			bc35_uart_buffer[buffer_index++] = _input;
		}
		else
		{
			memset(bc35_uart_buffer, 0, BUFFER_SIZE);
			buffer_index = 0;	
		}
	}

	return 0;
}


static void bc35_send_byte(char byte)
{
	uart2_send(byte);
}

static void bc35_send_str(const char *str)
{
	while((*str) != '\0')
		uart2_send(*str++);
}

static uint16_t bc35_recv_bytes(uint8_t *buf, uint16_t offset, uint16_t len, uint32_t timeout)
{
	BaseType_t err = pdFALSE;
	err = xSemaphoreTake(bc35_sem, timeout);
	if(err == pdFALSE)
		return 0;

	if(len > buffer_index)
		len = buffer_index;
	memcpy(buf+offset, bc35_uart_buffer, len);
	memset(bc35_uart_buffer, 0, BUFFER_SIZE);
	buffer_index = 0;
	return len;
}


static char* bc35_recv_wait_str(uint32_t timeout, const char *str_wait)
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
        recv_len = bc35_recv_bytes((uint8_t *)buf_chk, recv_offset, recv_left, 100);
        recv_offset += recv_len;     
        if (recv_len > 0) 
        {
            buf_chk[recv_offset-1] = '\0';
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


void bc35_buffer_flush()
{
	xSemaphoreTake(bc35_sem, 0);
	memset(bc35_uart_buffer, 0, BUFFER_SIZE);
}

void bc35_init()
{
	uart6_init(BC35_BUADRATE, bc35_protocol_receive);
	bc35_sem = xSemaphoreCreateBinary();
}


bool bc35_setup(uint32_t timeout)
{
	bc35_send_str("ATE0\r\n");

	if(!bc35_set_Function(1, timeout))
		return false;

	if(!bc35_set_autoconnect(1, timeout))
		return false;

	return true;
}


bool bc35_testAT(uint32_t timeout)
{
	bc35_send_str("AT\r\n");
	return (bc35_recv_wait_str(timeout, "OK") != NULL);
}


bool bc35_set_Function(uint8_t mode, uint32_t timeout)
{
	bc35_send_str("AT+CFUN=");
	bc35_send_byte(mode+'0');
	bc35_send_str("AT\r\n");
	return (bc35_recv_wait_str(timeout, "OK") != NULL);
}


bool bc35_set_autoconnect(uint8_t on, uint32_t timeout)
{
	if(on)
		bc35_send_str("AT+NCONFIG=AUTOCONNECT,TRUE\r\n");
	else
		bc35_send_str("AT+NCONFIG=AUTOCONNECT,FALSE\r\n");
	
	return (bc35_recv_wait_str(timeout, "OK") != NULL);
}


bool bc35_get_nbconnect(uint32_t timeout)
{
	char *str_out = NULL;
	char *p = NULL;

	bc35_send_str("AT+CGATT?\r\n");
	str_out =  bc35_recv_wait_str(timeout, "OK");

	if(str_out == NULL || strstr(str_out, "+CGATT") == NULL)
		return false;

	p = strchr(str_out, ':');

	return (*(++p) == '1');
}


bool bc35_get_rssi(int *rssi, uint32_t timeout)
{
	char *str_out = NULL;
	char *p = NULL;

	bc35_send_str("AT+CSQ\r\n");
	str_out =  bc35_recv_wait_str(timeout, "OK");

	if(str_out == NULL || strstr(str_out, "+CSQ") == NULL)
		return false;

	p = strchr(str_out, ':');
	*rssi = atoi(++p);

	return true;
}


