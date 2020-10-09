#include "string.h"
#include "lcd192x64.h"
#include "message.h"
#include "identify_task.h"
#include "activity_task.h"
#include "fpc_task.h"
#include "vfinger_task.h"
#include "face_task.h"
#include "rtc_task.h"
#include "menu.h"


uint16_t systemconfig_setup(uint16_t input)
{
	common_setup("系统设置", "恢复出厂", "开锁延时", NULL, NULL);
	lcd_fb_puthzs(20, 2, "1/1", 0, 1);
	
	return 0;
}


uint16_t systemconfig_key_process(void *param, uint16_t input)
{
	static uint8_t sysconfig_flag = 0; 
	static uint8_t sysconfig_index = 0;
	uint8_t key_cmd = input>>8;
	uint8_t key_val = input&0xFF;
	Msg_Value_t syscmsend_msg = {0};
	Msg_Identify_t syscsend_id = {0};

	if(key_cmd != 0xA5)
		return 0;

	if(key_val == 'u')
	{	
		if(sysconfig_index == 2)
		{
			
		}
	}
	else if(key_val == 'd')
	{
		if(sysconfig_index == 2)
		{
			
		}	
	}
	else if(key_val == '\n')
	{
		if(sysconfig_flag)
		{
			sysconfig_flag = 0;
			sysconfig_index = 0;
			systemconfig_setup(0);
		}
		else
		{
			if(sysconfig_index == 0)
			{
				rtc_cnt_enable(1, RTC_FLAG_FACE);
				sysconfig_flag = 1;
				syscsend_id.type = IDENTIFY_TYPE_RESET_USER;
				syscsend_id.info.uid[0] = 0;
				identify_send_msg(&syscsend_id, 1000);

				syscmsend_msg.cmd = FPC_CMD_CLEAR;				
				fpc_send_msg(&syscmsend_msg, 1000);

				syscmsend_msg.cmd = VEIN_CMD_CLEAR;				
				vein_send_msg(&syscmsend_msg, 1000);

				syscmsend_msg.cmd = FACE_CMD_CLEAR;				
				face_send_msg(&syscmsend_msg, 1000);				

				common_show_popup("数据已清除!");

			}
			else if(sysconfig_index == 1)
			{
				sysconfig_flag = 2;
				common_show_popup("开锁延时:");
			}
		}
	}
	else if(key_val == '\b')
	{
		sysconfig_index = 0;
		syscmsend_msg.cmd = MSG_CMD_PUB;				
		acitivy_send_msg(&syscmsend_msg, 1000);
	}	
	else if(key_val >= '1' && key_val <= '2')
	{
		sysconfig_index = key_val - '1';
		common_show_rectangle(sysconfig_index);
	}

	return 0;
}


struct Menu_Item_t systemconfig_item = {
	.index = 0,
	.setup = systemconfig_setup,
	.int_process = NULL,
	.key_process = systemconfig_key_process,
	.password_process = NULL,
	.fpc_process = NULL,
	.vein_process = NULL,
	.face_process = NULL,
	.rtc_process = NULL,
    .pub_menu = &manage2_item,	
	.sub_menu[0] = NULL,
	.sub_menu[1] = NULL,
	.sub_menu[2] = NULL,
	.sub_menu[3] = NULL,
	.next_menu = NULL,
	.pre_menu = NULL,
};

