#include "string.h"
#include "lcd192x64.h"
#include "device_info.h"
#include "message.h"
#include "identify_task.h"
#include "activity_task.h"
#include "menu.h"


static uint8_t choose_index = 0;
static char u_name[9] = {0};
static char u_password[2][9] = {0};


uint16_t useradd_setup(uint16_t input)
{
	lcd_fb_clear(0);	

	lcd_fb_line(1, 20, 191, 20, 1);
	lcd_fb_fbmp(4, 4, BMP_MENU_BACK_12X12, 0, 1);
	lcd_fb_puthzs(80, 2, "添加用户", 0, 1);
	lcd_fb_puthzs(20, 4, "1/1", 0, 1);	

	user_show_background(0);
	lcd_fb_puts(15, 47, ASCII_10X12, "1", 0, 1);		

	choose_index = 0;
	user_show_username(choose_index, 1);
	
	return 0;
}


uint16_t useradd_name_setup(uint16_t input)
{
	lcd_fb_clear(0);	
	
	lcd_fb_line(1, 20, 191, 20, 1);	
	
	lcd_fb_fbmp(4, 4, BMP_MENU_BACK_12X12, 0, 1);

	lcd_fb_rectangle(95, 22, 96, 62, 1, 1);				

	lcd_fb_puthzs(30, 2, "添加用户/1户名设置", 0, 1);
	lcd_fb_puthzs(10, 24, "请输入新的", 0, 1);
	lcd_fb_puthzs(10, 43, "用户名", 0, 1);

	for(int j = 0; j < 8; j++)
	{
		lcd_fb_line(107+j*10, 38, 115+j*10, 38, 1);		
	}

	lcd_fb_rectangle(102, 22, 189, 41, 1, 0);

	memset(u_name, 0, 9);
	
	return 0;
}


uint16_t useradd_password_setup(uint16_t input)
{
	lcd_fb_clear(0);	
	
	lcd_fb_line(1, 20, 191, 20, 1);	
	
	lcd_fb_fbmp(4, 4, BMP_MENU_BACK_12X12, 0, 1);

	lcd_fb_rectangle(95, 22, 96, 62, 1, 1);				

	lcd_fb_puthzs(30, 2, "添加用户/2密码设置", 0, 1);
	lcd_fb_puthzs(10, 24, "请输入新的", 0, 1);
	lcd_fb_puthzs(10, 43, "用户密码", 0, 1);

	for(int j = 0; j < 8; j++)
	{
		lcd_fb_line(107+j*10, 38, 115+j*10, 38, 1);		
	}

	for(int j = 0; j < 8; j++)
	{
		lcd_fb_line(107+j*10, 59, 115+j*10, 59, 1);		
	}

	lcd_fb_rectangle(102, 22, 189, 41, 1, 0);

	memset(u_password, 0, 18);	
	
	return 0;
}


uint16_t useradd_key_process(void *param, uint16_t input)
{
	uint8_t key_cmd = input>>8;
	uint8_t key_val = input&0xFF;
	static uint8_t dir_flag = 0; 
	static uint8_t useradd_flag = 0;	
	User_Info_t *ua_info = NULL;
	Msg_Value_t uamsend_msg = {0};


	if(key_cmd != 0xA5)
		return 0;

	if(key_val == '\n')
	{
		if(useradd_flag == 0)
		{
			useradd_flag = 1;
			ua_info = device_get_user(choose_index);

			if(ua_info->enable == 0)
			{
				useradd_flag = 0;
				dir_flag = 0;
				uamsend_msg.cmd = MSG_CMD_NEXT;				
				acitivy_send_msg(&uamsend_msg, 1000);			
				common_show_popup("添加用户");
				lcd_fb_putn(106, 34, ASCII_10X12, choose_index+1, 0, 1);
			}
			else
			{
				common_show_popup("用户已存在!");
			}
		}
		else
		{
			useradd_flag = 0;
			lcd_fb_rectangle(1, 21, 191, 63, 0, 1);
			user_show_background(dir_flag);
			user_show_username(choose_index-dir_flag, 1);
		}
	}
	else if(key_val == '\b')
	{
		useradd_flag = 0;
		dir_flag = 0;
		uamsend_msg.cmd = MSG_CMD_PUB;				
		acitivy_send_msg(&uamsend_msg, 1000);
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


uint16_t useradd_result_setup(uint16_t input)
{
	Msg_Identify_t uasend_id = {0};	

	lcd_fb_clear(0);	
	
	lcd_fb_line(1, 20, 191, 20, 1);	
	
	lcd_fb_fbmp(4, 4, BMP_MENU_BACK_12X12, 0, 1);			

	lcd_fb_puthzs(30, 2, "添加用户/6设置结果", 0, 1);

	common_show_popup("添加用户成功!");

	device_set_user(choose_index, u_name, u_password[0], input);			

	uasend_id.type = IDENTIFY_TYPE_SET_USER;
	uasend_id.info.uid[0] = choose_index;
	identify_send_msg(&uasend_id, 1000);
	
	return 0;
}


uint16_t useradd_name_key_process(void *param, uint16_t input)
{
	static uint8_t set_flag = 0;	
	uint8_t key_cmd = input>>8;
	uint8_t key_val = input&0xFF;	
	char u_name_show[9] = {0};
	uint8_t len = strlen(u_name);
	Msg_Value_t unsend_msg = {0};	
	
	if(key_cmd != 0xA5)
		return 0;
	
	if(key_val == '\n')
	{
		if(set_flag == 1)
		{
			set_flag = 0;
			memset(u_name, 0, 9);
			useradd_name_setup(0);
		}		
		else if(set_flag == 2)
		{
			set_flag = 0;
			unsend_msg.cmd = MSG_CMD_NEXT;				
			acitivy_send_msg(&unsend_msg, 1000);
		}		
		else if(len > 0)
		{
			if(identify_check_username(u_name) == 0xFFFF)
			{
				set_flag = 2;
				common_show_popup("户名设置完成!");
			}
			else
			{
				set_flag = 1;
				common_show_popup("户名已存在!");
			}
		}	
	}
	else if(key_val == '\b')
	{	
		if(set_flag == 0 && len > 0)
			u_name[len-1] = '\0';
		else
		{
			set_flag = 0;
			memset(u_name, 0, 9);
			unsend_msg.cmd = MSG_CMD_PUB;				
			acitivy_send_msg(&unsend_msg, 1000);
		}
	}
	else if(key_val >= '0' && key_val <= '9')
	{
		if(len < 8)
			u_name[len] = key_val;
	}	

	if(set_flag == 0)
	{
		memset(u_name_show, ' ', 8);
		strncpy(u_name_show, u_name, strlen(u_name));
		lcd_fb_puts(106, 26, ASCII_10X12, u_name_show, 0, 1);	
	}

	return 0;
}



uint16_t useradd_password_key_process(void *param, uint16_t input)
{
	static uint8_t cnt_flag = 0;
	static uint8_t set_flag = 0;
	uint8_t key_cmd = input>>8;
	uint8_t key_val = input&0xFF;	
	char u_password_show[9] = {0};
	uint8_t len[2] = {0};
	Msg_Value_t ussend_msg = {0};	
	
	if(key_cmd != 0xA5)
		return 0;
	
	len[0] = strlen(u_password[0]);
	len[1] = strlen(u_password[1]);
	if(key_val == '\n')
	{
		if(set_flag > 0 && set_flag < 3)
		{
			set_flag = 0;
			memset(u_password, 0, 18);
			useradd_password_setup(0);
		}
		else if(set_flag == 3)
		{
			set_flag = 0;
			ussend_msg.cmd = MSG_CMD_NEXT;		
			ussend_msg.value.s_val = choose_index+1;		
			acitivy_send_msg(&ussend_msg, 1000);
		}
		else if(cnt_flag == 0 && len[0] > 0)
		{
			cnt_flag = 1;
			lcd_fb_rectangle(102, 22, 189, 41, 0, 0);
			lcd_fb_rectangle(102, 43, 189, 62, 1, 0);			
		}
		else if(cnt_flag == 1 && len[1] > 0)
		{
			cnt_flag = 0;
			if(strncmp(u_password[0], u_password[1], 9))
			{
				set_flag = 1;
				common_show_popup("密码不一致!");
			}
			else if(identify_check_password(u_password[0]) < 0xFFFF)
			{
				set_flag = 2;
				common_show_popup("该密码已存在!");
			}
			else
			{
				set_flag = 3;
				common_show_popup("密码设置完成!");
			}
		}	
	}
	else if(key_val == '\b')
	{
		if(set_flag > 0)
		{
			set_flag = 0;
			memset(u_password, 0, 18);
			useradd_password_setup(0);
		}		
		else if(len[cnt_flag] > 0)
			u_password[cnt_flag][len[cnt_flag]-1] = '\0';
		else if(cnt_flag == 0)
		{
			ussend_msg.cmd = MSG_CMD_PUB;				
			acitivy_send_msg(&ussend_msg, 1000);
		}
	}
	else if(key_val >= '0' && key_val <= '9')
	{
		if(len[cnt_flag] < 8)
			u_password[cnt_flag][len[cnt_flag]] = key_val;
	}	

	if(set_flag == 0)
	{
		memset(u_password_show, ' ', 8);
		memset(u_password_show, '*', strlen(u_password[cnt_flag]));
		lcd_fb_puts(106, 26+21*cnt_flag, ASCII_10X12, u_password_show, 0, 1);
	}	

	return 0;
}


uint16_t useradd_result_key_process(void *param, uint16_t input)
{	
	uint8_t key_cmd = input>>8;
	uint8_t key_val = input&0xFF;	
	Msg_Value_t ursend_msg = {0};	
	
	if(key_cmd != 0xA5)
		return 0;
	
	if(key_val == '\n')
	{
		ursend_msg.cmd = MSG_CMD_PUB;				
		acitivy_send_msg(&ursend_msg, 1000);
	}
	else if(key_val == '\b')
	{	
		ursend_msg.cmd = MSG_CMD_PUB;				
		acitivy_send_msg(&ursend_msg, 1000);
	}

	return 0;
}


struct Menu_Item_t useradd_item = {
	.index = 0,
	.setup = useradd_setup,
	.int_process = NULL,
	.key_process = useradd_key_process,
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
	.next_menu = &useradd_name_item,
	.pre_menu = NULL,
};


struct Menu_Item_t useradd_name_item = {
	.index = 0,
	.setup = useradd_name_setup,
	.int_process = NULL,
	.key_process = useradd_name_key_process,
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
	.next_menu = &useradd_password_item,
	.pre_menu = NULL,
};



struct Menu_Item_t useradd_password_item = {
	.index = 0,
	.setup = useradd_password_setup,
	.int_process = NULL,
	.key_process = useradd_password_key_process,
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
	.next_menu = &useradd_fpc_item,
	.pre_menu = NULL,
};


struct Menu_Item_t useradd_result_item = {
	.index = 0,
	.setup = useradd_result_setup,
	.int_process = NULL,
	.key_process = useradd_result_key_process,
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

