#include "FreeRTOS.h"
#include "task.h"
#include "delay.h"
#include "hw_iic.h"
#include "rtc.h"
#include "device_info.h"
#include "input_output.h"
#include "adc.h"
#include "message.h"
#include "activity_task.h"
#include "identify_task.h"
#include "io_task.h"
#include "rtc_task.h"
#include "wifi_task.h"


#define START_TASK_PRIO		3
#define START_STK_SIZE 		256  

#define RTC_TIMEOUT			6


TaskHandle_t StartTask_Handler;

struct Menu_Item_t start_menu = {
	.index = 0,
	.sub_menu[0] = &main_item,
};


void rtc_debug()
{
	printf("rtc debug\r\n");
	rtc_enable_wakeup(0);
}


void activity_hook()
{
	rtc_set_wakeup(RTC_TIMEOUT);
}


void hw_system_init()
{
	hw_iic1_init();
  	io_output_init();
  	io_output_set(OUTPUT_POWER_3V, 1);
  	io_output_set(OUTPUT_POWER_5V, 1);
	device_info_init();
	adc1_init();
	rtc_init();

	uart1_init(115200, activity_debug_parse);

	rtc_init_wakeup(RTC_TIMEOUT, rtc_debug);
}



void start_task(void *pvParameters)
{
	Msg_Value_t start_send_msg = {0};

    taskENTER_CRITICAL();
		
	rtc_task_create(NULL);	

	identify_task_create(NULL);
		
	activity_task_create(&start_menu, activity_hook);

	io_task_create(NULL);

	wifi_task_create(NULL);

	wifi_task_create(NULL);

	taskEXIT_CRITICAL();

	start_send_msg.cmd = MSG_CMD_SUB;
	acitivy_send_msg(&start_send_msg, 1000);

    vTaskDelete(StartTask_Handler);
}


int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
	delay_init(168);		//初始化延时函数
	
	hw_system_init();
	
	vPortInitialiseBlocks(); 
	
	xTaskCreate((TaskFunction_t )start_task,            //任务函数
              	(const char*    )"start_task",          //任务名称
              	(uint16_t       )START_STK_SIZE,        //任务堆栈大小
              	(void*          )NULL,                  //传递给任务函数的参数
              	(UBaseType_t    )START_TASK_PRIO,       //任务优先级
              	(TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
  	vTaskStartScheduler();          //开启任务调度
}

