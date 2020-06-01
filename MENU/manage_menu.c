#include "string.h"
#include "lcd192x64.h"
#include "message.h"
#include "identify_task.h"
#include "fpc_task.h"
#include "face_task.h"
#include "menu.h"


uint16_t manage1_setup(uint16_t input)
{
	manage1_item.index = 0;
	common_setup("设置", "管理员设置", "用户设置", "开锁模式", "报警策略");
	lcd_fb_puthzs(20, 4, "1/2", 0, 1);

	task_identify_enable(0);
	fpc_identify_enable(0);	
	face_identify_enable(0);

	return 0;
}


uint16_t manage2_setup(uint16_t input)
{
	manage2_item.index = 0;
	common_setup("设置", "网络设置", "时间设置", "系统状态", "系统设置");
	lcd_fb_puthzs(20, 4, "2/2", 0, 1);
	
	return 0;
}


uint16_t user_setup(uint16_t input)
{
	user_item.index = 0;	
	common_setup("用户设置", "添加用户", "删除用户", NULL, NULL);
	lcd_fb_puthzs(20, 4, "1/1", 0, 1);
	
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
	.sub_menu[0] = NULL,
	.sub_menu[1] = NULL,
	.sub_menu[2] = NULL,
	.sub_menu[3] = &systemconfig_item,
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

