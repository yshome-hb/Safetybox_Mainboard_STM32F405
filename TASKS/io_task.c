#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "input_output.h"
#include "message.h"
#include "activity_task.h"
#include "io_task.h"


#define IO_QUEUE_NUM    4

#define IO_TASK_PRIO	4
#define IO_STK_SIZE 	128  


TaskHandle_t IO_Task_Handler;
TimerHandle_t Key_Int_Loop_Handle;
static QueueHandle_t io_queue;

static Msg_Value_t io_msg = {0};


static void key_int_loop(TimerHandle_t xTimer)
{	
	static uint8_t shake_cnt = 0;
	
	io_input_update(INPUT_VEIN);
	io_input_update(INPUT_CONFIG);
	
	if(io_input_prevalue(INPUT_VEIN) == 0x01)
	{
		io_msg.cmd = MSG_CMD_INT;
		io_msg.value.b_val[1] = INPUT_VEIN;
		io_msg.value.b_val[0] = io_input_prevalue(INPUT_VEIN);
		acitivy_send_msg(&io_msg, 1000);
	}

	if(io_input_prevalue(INPUT_CONFIG) == 0x02)
	{
		io_msg.cmd = MSG_CMD_INT;
		io_msg.value.b_val[1] = INPUT_CONFIG;
		io_msg.value.b_val[0] = io_input_prevalue(INPUT_CONFIG);
		acitivy_send_msg(&io_msg, 1000);
	}

	if((io_input_getfilter(INPUT_SHAKE) & 0x7) != 0x07)
	{
		if(shake_cnt < 10)
		{
			shake_cnt++;
			if(shake_cnt == 10)
			{
				shake_cnt = 0;
				io_msg.cmd = MSG_CMD_INT;
				io_msg.value.b_val[1] = INPUT_SHAKE;
				io_msg.value.b_val[0] = io_input_prevalue(INPUT_SHAKE);
				acitivy_send_msg(&io_msg, 1000);
			}
		}
	}
	else
	{
		if(shake_cnt > 1)
		{
			shake_cnt -= 2;
		}		
	}	
}   



static inline int32_t io_send_msg(const void *msg, uint32_t timeout)
{
	return xQueueSend(io_queue, msg, timeout);
}


void io_task(void *pvParameters)
{		
	io_queue = xQueueCreate(IO_QUEUE_NUM, sizeof(Msg_Value_t));	
	
	while(1)
	{
		
		vTaskDelay(150);
	}
}   



void io_task_create(void *pvParameters)
{
	io_input_init();

  	Key_Int_Loop_Handle = xTimerCreate((const char*)"key_int_loop",
									  (TickType_t  )250,
									  (UBaseType_t )pdTRUE,
									  (void*	   )1,
									  (TimerCallbackFunction_t)key_int_loop); 																	
																		
	xTimerStart(Key_Int_Loop_Handle, 0);
																		
//    xTaskCreate((TaskFunction_t )io_task,     
//                (const char*    )"io_task",   
//                (uint16_t       )IO_STK_SIZE, 
//                (void*          )pvParameters,
//                (UBaseType_t    )IO_TASK_PRIO,
//                (TaskHandle_t*  )&IO_Task_Handler);  
}


