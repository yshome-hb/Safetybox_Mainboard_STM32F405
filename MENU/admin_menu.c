#include "string.h"
#include "lcd192x64.h"
#include "message.h"
#include "identify_task.h"
#include "activity_task.h"
#include "menu.h"


static char a_password[2][9] = {0};


uint16_t admin_setup(uint16_t input)
{
	lcd_fb_clear(0);	
	
	lcd_fb_line(1, 20, 191, 20, 1);	
	
	lcd_fb_fbmp(4, 4, BMP_MENU_BACK_12X12, 0, 1);

	lcd_fb_rectangle(95, 22, 96, 62, 1, 1);				

	lcd_fb_puthzs(60, 2, "管理员密码", 0, 1);
	lcd_fb_puthzs(10, 24, "请输入新的", 0, 1);
	lcd_fb_puthzs(10, 43, "管理员密码", 0, 1);

	for(int j = 0; j < 8; j++)
	{
		lcd_fb_line(107+j*10, 38, 115+j*10, 38, 1);		
	}

	for(int j = 0; j < 8; j++)
	{
		lcd_fb_line(107+j*10, 59, 115+j*10, 59, 1);		
	}

	lcd_fb_rectangle(102, 22, 189, 41, 1, 0);
	
	return 0;
}


uint16_t admin_key_process(void *param, uint16_t input)
{
	static uint8_t cnt_flag = 0;
	static uint8_t set_flag = 0;
	uint8_t key_cmd = input>>8;
	uint8_t key_val = input&0xFF;	
	char a_password_show[9] = {0};
	uint8_t len[2] = {0};
	Msg_Value_t adsend_msg = {0};	
	Msg_Identify_t adsend_id = {0};
	
	if(key_cmd != 0xA5)
		return 0;
	
	len[0] = strlen(a_password[0]);
	len[1] = strlen(a_password[1]);
	if(key_val == '\n')
	{
		if(set_flag > 0)
		{
			set_flag = 0;
			memset(a_password, 0, 18);
			admin_setup(0);
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
			if(strncmp(a_password[0], a_password[1], 9))
			{
				set_flag = 2;
				common_show_popup("密码不一致!");
			}
			else
			{
				set_flag = 1;
				common_show_popup("密码设置成功!");
				adsend_id.type = IDENTIFY_TYPE_SET_ADMIN;
				strcpy(adsend_id.info.password, a_password[0]);
				identify_send_msg(&adsend_id, 1000);
			}
		}	
	}
	else if(key_val == '\b')
	{
		if(set_flag > 0)
		{
			set_flag = 0;
			memset(a_password, 0, 18);
			admin_setup(0);
		}		
		else if(len[cnt_flag] > 0)
			a_password[cnt_flag][len[cnt_flag]-1] = '\0';
		else if(cnt_flag == 0)
		{
			adsend_msg.cmd = MSG_CMD_PUB;				
			acitivy_send_msg(&adsend_msg, 1000);
		}
	}
	else if(key_val >= '0' && key_val <= '9')
	{
		if(len[cnt_flag] < 8)
			a_password[cnt_flag][len[cnt_flag]] = key_val;
	}	


	if(set_flag == 0)
	{
		memset(a_password_show, ' ', 8);
		memset(a_password_show, '*', strlen(a_password[cnt_flag]));
		lcd_fb_puts(106, 26+21*cnt_flag, ASCII_10X12, a_password_show, 0, 1);
	}	

	return 0;
}


struct Menu_Item_t admin_item = {
	.index = 0,
	.setup = admin_setup,
	.int_process = NULL,
	.key_process = admin_key_process,
	.password_process = NULL,
	.fpc_process = NULL,
	.vein_process = NULL,
	.face_process = NULL,
	.rtc_process = NULL,
    .pub_menu = &manage1_item,	
	.sub_menu[0] = NULL,
	.sub_menu[1] = NULL,
	.sub_menu[2] = NULL,
	.sub_menu[3] = NULL,
	.next_menu = NULL,
	.pre_menu = NULL,
};
