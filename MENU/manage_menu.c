#include "string.h"
#include "lcd192x64.h"
#include "device_info.h"
#include "message.h"
#include "activity_task.h"
#include "identify_task.h"
#include "fpc_task.h"
#include "face_task.h"
#include "rtc_task.h"
#include "menu.h"


uint16_t manage1_setup(uint16_t input)
{
	manage1_item.index = 0;
	common_setup("设置", "管理员设置", "用户设置", "开锁模式", "报警策略");
	lcd_fb_puthzs(20, 2, "1/2", 0, 1);

	task_identify_enable(0);
	fpc_identify_enable(0);	
	face_identify_enable(0);
	rtc_timeout_stop();

	return 0;
}


uint16_t manage2_setup(uint16_t input)
{
	manage2_item.index = 0;
	common_setup("设置", "网络设置", "时间设置", "系统设置", "系统状态");
	lcd_fb_puthzs(20, 2, "2/2", 0, 1);
	
	return 0;
}


uint16_t user_setup(uint16_t input)
{
	user_item.index = 0;	
	common_setup("用户设置", "添加用户", "删除用户", NULL, NULL);
	lcd_fb_puthzs(20, 2, "1/1", 0, 1);
	
	return 0;
}


uint16_t networkconfig_setup(uint16_t input)
{
	networkconfig_item.index = 0;	
	common_setup("网络设置 ", "联网方式", "wifi状态", "nb状态", NULL);
	lcd_fb_puthzs(20, 2, "1/1", 0, 1);
	
	return 0;
}


void networkaccess_show_type(uint16_t type)
{
	if(type == OFF_LINE)
	{
		lcd_fb_rectangle(73, 29, 77, 33, 1, 1);
		lcd_fb_rectangle(73, 50, 77, 54, 0, 1);
		lcd_fb_rectangle(169, 29, 173, 33, 0, 1);
	}
	else if(type == WIFI_ACCESS)
	{
		lcd_fb_rectangle(73, 29, 77, 33, 0, 1);
		lcd_fb_rectangle(73, 50, 77, 54, 1, 1);
		lcd_fb_rectangle(169, 29, 173, 33, 0, 1);
	}
	else if(type == NBIOT_ACCESS)
	{
		lcd_fb_rectangle(73, 29, 77, 33, 0, 1);
		lcd_fb_rectangle(73, 50, 77, 54, 0, 1);
		lcd_fb_rectangle(169, 29, 173, 33, 1, 1);
	}	
}



uint16_t networkaccess_setup(uint16_t input)
{
	networkconfig_item.index = 0;	
	common_setup("联网方式 ", "离线", "wifi", "nbiot", NULL);
	lcd_fb_puthzs(20, 2, "1/1", 0, 1);
	
	lcd_fb_rectangle(72, 28, 78, 34, 1, 0);
	lcd_fb_rectangle(72, 49, 78, 55, 1, 0);
	lcd_fb_rectangle(168, 28, 174, 34, 1, 0);

	networkaccess_show_type(device_get_networktype());

	return 0;
}


uint16_t networkaccess_key_process(void *param, uint16_t input)
{
	static uint8_t networkaccess_index = 0;
	uint8_t key_cmd = input>>8;
	uint8_t key_val = input&0xFF;
	Msg_Value_t nwasend_msg = {0};
	Msg_Identify_t nwasend_id = {0};

	if(key_cmd != 0xA5)
		return 0;

	if(key_val == '\n')
	{
		device_set_networktype(networkaccess_index);
		networkaccess_show_type(networkaccess_index);
	}
	else if(key_val == '\b')
	{
		nwasend_id.type = IDENTIFY_TYPE_SET_FLASH;
		identify_send_msg(&nwasend_id, 1000);

		nwasend_msg.cmd = MSG_CMD_PUB;				
		acitivy_send_msg(&nwasend_msg, 1000);

		networkaccess_index =0;
	}	
	else if(key_val >= '1' && key_val <= '3')
	{
		networkaccess_index = key_val - '1';
		common_show_rectangle(networkaccess_index);
	}

	return 0;
}


struct Menu_Item_t manage1_item = {
	.index = 0,
	.setup = manage1_setup,
	.int_process = common_int_process,
	.key_process = common_key_process,
	.password_process = NULL,
	.fpc_process = NULL,
	.vein_process = NULL,
	.face_process = NULL,
	.rtc_process = NULL,
    .pub_menu = &main_item,	
	.sub_menu[0] = &admin_item,
	.sub_menu[1] = &user_item,
	.sub_menu[2] = &lockmode_item,
	.sub_menu[3] = &warn_item,
	.next_menu = &manage2_item,
	.pre_menu = NULL,
};


struct Menu_Item_t manage2_item = {
	.index = 0,
	.setup = manage2_setup,
	.int_process = common_int_process,
	.key_process = common_key_process,
	.password_process = NULL,
	.fpc_process = NULL,
	.vein_process = NULL,
	.face_process = NULL,
	.rtc_process = NULL,
    .pub_menu = &main_item,	
	.sub_menu[0] = &networkconfig_item,
	.sub_menu[1] = &datetime_item,
	.sub_menu[2] = &systemconfig_item,
	.sub_menu[3] = &systeminfo_item,
	.next_menu = NULL,
	.pre_menu = &manage1_item,
};


struct Menu_Item_t user_item = {
	.index = 0,
	.setup = user_setup,
	.int_process = common_int_process,
	.key_process = common_key_process,
	.password_process = NULL,
	.fpc_process = NULL,
	.vein_process = NULL,
	.face_process = NULL,
	.rtc_process = NULL,
    .pub_menu = &manage1_item,	
	.sub_menu[0] = &useradd_item,
	.sub_menu[1] = &userdel_item,
	.sub_menu[2] = NULL,
	.sub_menu[3] = NULL,
	.next_menu = NULL,
	.pre_menu = NULL,
};


struct Menu_Item_t networkconfig_item = {
	.index = 0,
	.setup = networkconfig_setup,
	.int_process = common_int_process,
	.key_process = common_key_process,
	.password_process = NULL,
	.fpc_process = NULL,
	.vein_process = NULL,
	.face_process = NULL,
	.rtc_process = NULL,
    .pub_menu = &manage2_item,	
	.sub_menu[0] = &networkaccess_item,
	.sub_menu[1] = &wifiinfo_item,
	.sub_menu[2] = &nbiotinfo_item,
	.sub_menu[3] = NULL,
	.next_menu = NULL,
	.pre_menu = NULL,
};


struct Menu_Item_t networkaccess_item = {
	.index = 0,
	.setup = networkaccess_setup,
	.int_process = NULL,
	.key_process = networkaccess_key_process,
	.password_process = NULL,
	.fpc_process = NULL,
	.vein_process = NULL,
	.face_process = NULL,
	.rtc_process = NULL,
    .pub_menu = &networkconfig_item,	
	.sub_menu[0] = NULL,
	.sub_menu[1] = NULL,
	.sub_menu[2] = NULL,
	.sub_menu[3] = NULL,
	.next_menu = NULL,
	.pre_menu = NULL,
};

