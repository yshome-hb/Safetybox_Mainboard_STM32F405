#include "string.h"
#include "lcd192x64.h"
#include "message.h"
#include "identify_task.h"
#include "activity_task.h"
#include "fpc_task.h"
#include "menu.h"


static uint8_t login_index = 0;
static uint8_t fpc_login_flag = 0;


uint16_t useradd_fpc_setup(uint16_t input)
{
	lcd_fb_clear(0);	
	
	lcd_fb_line(1, 20, 191, 20, 1);	
	
	lcd_fb_fbmp(4, 4, BMP_MENU_BACK_12X12, 0, 1);

	lcd_fb_rectangle(95, 22, 96, 62, 1, 1);				

	lcd_fb_puthzs(30, 2, "添加用户/3指纹设置", 0, 1);
	lcd_fb_puthzs(10, 24, "指纹录入", 0, 1);
	lcd_fb_puthzs(10, 43, "请按手指", 0, 1);
	lcd_fb_puthzs(108, 24, "按OK键", 0, 1);	
	lcd_fb_puthzs(108, 43, "开始录入", 0, 1);	

	lcd_fb_rectangle(98, 22, 189, 62, 1, 0);

	login_index = input;
	fpc_login_flag = 0;
	
	return 0;
}


uint16_t useradd_fpc_key_process(void *param, uint16_t input)
{	
	uint8_t key_cmd = input>>8;
	uint8_t key_val = input&0xFF;	
	Msg_Value_t ufsend_msg = {0};	
	
	if(key_cmd != 0xA5)
		return 0;
	
	if(key_val == '\n')
	{
		if(fpc_login_flag == 0)
		{
			fpc_login_flag = 1;
			lcd_fb_rectangle(99, 23, 188, 61, 0, 1);		
			lcd_fb_puthzs(108, 24, "录入中...", 0, 1);
			ufsend_msg.cmd = FPC_CMD_ADD;
			ufsend_msg.value.s_val = login_index;
			fpc_send_msg(&ufsend_msg, 1000);
		}
		else if(fpc_login_flag == 2)
		{
			fpc_login_flag = 0;
			ufsend_msg.cmd = MSG_CMD_NEXT;
			ufsend_msg.value.s_val = login_index;		
			acitivy_send_msg(&ufsend_msg, 1000);
		}
	}
	else if(key_val == '\b')
	{	
		if(fpc_login_flag == 0 || fpc_login_flag == 2)
		{
			ufsend_msg.cmd = MSG_CMD_PUB;				
			acitivy_send_msg(&ufsend_msg, 1000);
		}
	}

	return 0;
}


uint16_t useradd_fpc_fpc_process(void *param, uint16_t input)
{
	uint8_t ret = input>>8;
	uint8_t u_id = input&0xFF;
	
	switch(ret)
	{
		case FPC_RETURN_NONE:
			lcd_fb_rectangle(99, 23, 188, 61, 0, 1);
			lcd_fb_puthzs(108, 24, "按OK键", 0, 1);	
			lcd_fb_puthzs(108, 43, "开始录入", 0, 1);
			fpc_login_flag = 0;		
			break;		

		case FPC_RETURN_SUCCESS:
			lcd_fb_rectangle(99, 23, 188, 61, 0, 1);		
			lcd_fb_puthzs(108, 24, "录入成功", 0, 1);
			lcd_fb_puthzs(108, 43, "OK键继续", 0, 1);
			fpc_login_flag = 2;
			break;
		
		case FPC_RETURN_FAIL:
			lcd_fb_rectangle(99, 23, 188, 61, 0, 1);
			if(u_id == 0x07)
			{
				lcd_fb_puthzs(108, 24, "该指纹", 0, 1);
				lcd_fb_puthzs(108, 43, "已录入!", 0, 1);			
			}
			else
				lcd_fb_puthzs(108, 24, "录入失败!", 0, 1);	
			break;

		case FPC_RETURN_TIMEOUT:
			lcd_fb_rectangle(99, 23, 188, 61, 0, 1);		
			lcd_fb_puthzs(108, 24, "等待超时!", 0, 1);	
			break;			

		default:

			break;
	}

	return 0;
}


struct Menu_Item_t useradd_fpc_item = {
	.index = 0,
	.setup = useradd_fpc_setup,
	.int_process = NULL,
	.key_process = useradd_fpc_key_process,
	.password_process = NULL,
	.fpc_process = useradd_fpc_fpc_process,
	.vein_process = NULL,
	.face_process = NULL,
	.rtc_process = NULL,
    .pub_menu = &user_item,	
	.sub_menu[0] = NULL,
	.sub_menu[1] = NULL,
	.sub_menu[2] = NULL,
	.sub_menu[3] = NULL,
	.next_menu = &useradd_vein_item,
	.pre_menu = NULL,
};

