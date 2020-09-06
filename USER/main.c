#include "FreeRTOS.h"
#include "task.h"
#include "delay.h"
#include "hw_iic.h"
#include "rtc.h"
#include "device_config.h"
#include "device_info.h"
#include "input_output.h"
#include "adc.h"
#include "exti.h"
#include "wakeup.h"
#include "wwdg.h"
#include "message.h"
#include "activity_task.h"
#include "identify_task.h"
#include "io_task.h"
#include "rtc_task.h"
#include "wifi_task.h"
#include "nbiot_task.h"


#define START_TASK_PRIO		3
#define START_STK_SIZE 		256  


TaskHandle_t StartTask_Handler;


uint16_t sleep_setup(uint16_t input)
{
  	io_output_set(OUTPUT_LIGHT, 0);	//关闭补光灯
  	io_output_set(OUTPUT_BEEP, 0);	//关闭蜂鸣器

	io_task_stop();
	rtc_timeout_stop();
	wifi_task_delete();
	nbiot_task_delete();	
	identify_task_delete();
	activity_task_delete();

  	io_output_set(OUTPUT_POWER_5V, 0); //关闭系统5V电源
  	io_output_set(OUTPUT_POWER_3V, 0);	//关闭系统3V电源

	wakeup_stop_prepare();	//关闭系统外设
	//exti1_init(soft_reset);
	exti15_10_init(soft_reset);	//设置按键信号为唤醒源
	wakeup_stop();	//系统休眠
	return 0;
}


static void rtc_timeout()
{
	Msg_Value_t ltsend_msg = {0};

	ltsend_msg.cmd = MSG_CMD_PUB;	
	acitivy_send_msg_isr(&ltsend_msg);
	rtc_enable_wakeup(0);
}


void activity_hook()
{
	rtc_timeout_reset();
}


struct Menu_Item_t start_item = {
	.index = 0,
	.setup = sleep_setup,
	.sub_menu[0] = &main_item,
};


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
	
	rtc_init_wakeup(LOCK_TIMEOUT, rtc_timeout);	
}



void start_task(void *pvParameters)
{
	Msg_Value_t start_send_msg = {0};

    taskENTER_CRITICAL();
		
	rtc_task_create(NULL);	

	identify_task_create(NULL);
		
	activity_task_create(&start_item, activity_hook);

	wifi_task_create(NULL);

	nbiot_task_create(NULL);

	taskEXIT_CRITICAL();

	start_send_msg.cmd = MSG_CMD_SUB;
	acitivy_send_msg(&start_send_msg, 1000);

	vTaskDelay(2000);
	io_task_create(NULL);

 	vTaskDelete(StartTask_Handler);
}


int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
	delay_init(168);
	
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

