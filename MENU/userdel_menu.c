#include "string.h"
#include "lcd192x64.h"
#include "device_info.h"
#include "message.h"
#include "menu.h"
#include "identify_task.h"
#include "activity_task.h"
#include "fpc_task.h"
#include "vfinger_task.h"
#include "face_task.h"
#include "rtc_task.h"


static uint8_t choose_index = 0; 


void user_show_username(uint8_t index, uint8_t force)
{
	static uint8_t show_index = USER_MAX;
	uint8_t show_pos1 = 1;
	uint8_t show_pos2 = 1;

	if(force == 0 && show_index == index)
		return;

	show_index = index;

	if(show_index > (USER_MAX - 2))
		show_index = (USER_MAX - 2);

	User_Info_t *u1_info = device_get_user(show_index);
	User_Info_t *u2_info = device_get_user(show_index+1);

	lcd_fb_rectangle(65, 23, 188, 40, 0, 1);
	lcd_fb_rectangle(65, 44, 188, 61, 0, 1);

	if(show_index > 8)
		show_pos1 = 0;

	lcd_fb_putn(67+10*show_pos1, 26, ASCII_10X12, show_index+1, 0, 1);
	lcd_fb_puts(87, 26, ASCII_10X12, ":", 0, 1);
	if(u1_info->enable)
		lcd_fb_puts(102, 26, ASCII_10X12, u1_info->username, 0, 1);
	else
		lcd_fb_puthzs(102, 24, "空", 0, 1);
	
	if(show_index > 7)
		show_pos2 = 0;

	lcd_fb_putn(67+10*show_pos2, 47, ASCII_10X12, show_index+2, 0, 1);
	lcd_fb_puts(87, 47, ASCII_10X12, ":", 0, 1);
	if(u2_info->enable)	
		lcd_fb_puts(102, 47, ASCII_10X12, u2_info->username, 0, 1);
	else
		lcd_fb_puthzs(102, 45, "空", 0, 1);
	
}


void user_show_background(uint8_t flag)
{
	lcd_fb_rectangle(58, 22, 60, 62, 1, 1);	

	lcd_fb_puthzs(15, 24, "用户", 0, 1);
	lcd_fb_puts(25, 47, ASCII_10X12, "/", 0, 1);		
	lcd_fb_putn(35, 47, ASCII_10X12, USER_MAX, 0, 1);	

	if(flag == 0)
		lcd_fb_rectangle(64, 22, 189, 41, 1, 0);	
	else
		lcd_fb_rectangle(64, 43, 189, 62, 1, 0);
}


uint16_t userdel_key_process(void *param, uint16_t input)
{
	uint8_t key_cmd = input>>8;
	uint8_t key_val = input&0xFF;
	static uint8_t dir_flag = 0; 
	static uint8_t userdel_flag = 0;	
	User_Info_t *ud_info = NULL;
	Msg_Value_t udmsend_msg = {0};
	Msg_Identify_t udsend_id = {0};	

	if(key_cmd != 0xA5)
		return 0;

	if(key_val == '\n')
	{
		if(userdel_flag == 0)
		{
			userdel_flag = 1;
			ud_info = device_get_user(choose_index);

			if(ud_info->enable == 0)
				common_show_popup("该用户为空!");
			else
			{
				rtc_cnt_enable(1, RTC_FLAG_FACE);

				udsend_id.type = IDENTIFY_TYPE_DEL_USER;
				udsend_id.info.uid[0] = choose_index;
				identify_send_msg(&udsend_id, 1000);	

				udmsend_msg.cmd = FPC_CMD_DELETE;	
				udmsend_msg.value.s_val = ud_info->fid;	
				fpc_send_msg(&udmsend_msg, 1000);

				udmsend_msg.cmd = VEIN_CMD_DELETE;	
				udmsend_msg.value.s_val = ud_info->vid;				
				vein_send_msg(&udmsend_msg, 1000);

				udmsend_msg.cmd = FACE_CMD_DELETE;
				udmsend_msg.value.s_val = ud_info->aid;					
				face_send_msg(&udmsend_msg, 1000);	

				common_show_popup("用户已删除!");
			}
		}
		else
		{
			userdel_flag = 0;
			lcd_fb_rectangle(1, 21, 191, 63, 0, 1);
			user_show_background(dir_flag);
			user_show_username(choose_index-dir_flag, 1);
		}
	}
	else if(key_val == '\b')
	{
		userdel_flag = 0;
		udmsend_msg.cmd = MSG_CMD_PUB;				
		acitivy_send_msg(&udmsend_msg, 1000);
	}	
	else if(key_val == 'u')
	{
		if(choose_index > 0)
			choose_index--;
		
		if(dir_flag == 1)
		{
			dir_flag = 0;		
			lcd_fb_rectangle(64, 22, 189, 41, 1, 0);
			lcd_fb_rectangle(64, 43, 189, 62, 0, 0);
		}
	}	
	else if(key_val == 'd')
	{
		if(choose_index < (USER_MAX - 1))
			choose_index++;

		if(dir_flag == 0)
		{
			dir_flag = 1;
			lcd_fb_rectangle(64, 22, 189, 41, 0, 0);
			lcd_fb_rectangle(64, 43, 189, 62, 1, 0);
		}
	}

	lcd_fb_rectangle(5, 47, 25, 59, 0, 1);

	if(choose_index > 8)
		lcd_fb_putn(5, 47, ASCII_10X12, choose_index+1, 0, 1);
	else
		lcd_fb_putn(15, 47, ASCII_10X12, choose_index+1, 0, 1);

	user_show_username(choose_index-dir_flag, 0);

	return 0;
}


uint16_t userdel_setup(uint16_t input)
{
	lcd_fb_clear(0);	

	lcd_fb_line(1, 20, 191, 20, 1);
	lcd_fb_fbmp(4, 4, BMP_MENU_BACK_12X12, 0, 1);
	lcd_fb_puthzs(80, 2, "删除用户", 0, 1);
	lcd_fb_puthzs(20, 2, "1/1", 0, 1);	

	user_show_background(0);
	lcd_fb_puts(15, 47, ASCII_10X12, "1", 0, 1);		

	choose_index = 0;
	user_show_username(choose_index, 1);
	
	return 0;
}


struct Menu_Item_t userdel_item = {
	.index = 0,
	.setup = userdel_setup,
	.int_process = NULL,
	.key_process = userdel_key_process,
	.password_process = NULL,
	.fpc_process = NULL,
	.vein_process = NULL,
	.face_process = NULL,
	.rtc_process = NULL,
    .pub_menu = &user_item,	
	.sub_menu[0] = NULL,
	.sub_menu[1] = NULL,
	.sub_menu[2] = NULL,
	.sub_menu[3] = NULL,
	.next_menu = NULL,
	.pre_menu = NULL,
};

