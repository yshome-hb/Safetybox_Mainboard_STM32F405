#include "string.h"
#include "lcd192x64.h"
#include "input_output.h"
#include "message.h"
#include "identify_task.h"
#include "activity_task.h"
#include "vfinger_task.h"
#include "menu.h"


static uint8_t login_index = 0;
static uint8_t vein_login_flag = 0;


uint16_t useradd_vein_setup(uint16_t input)
{
	lcd_fb_clear(0);	
	
	lcd_fb_line(1, 20, 191, 20, 1);	
	
	lcd_fb_fbmp(4, 4, BMP_MENU_BACK_12X12, 0, 1);

	lcd_fb_rectangle(95, 22, 96, 62, 1, 1);				

	lcd_fb_puthzs(30, 2, "添加用户/4指静脉", 0, 1);
	lcd_fb_puthzs(10, 24, "指静脉录入", 0, 1);
	lcd_fb_puthzs(10, 43, "请伸入手指", 0, 1);
	lcd_fb_puthzs(108, 24, "伸入手指", 0, 1);	
	lcd_fb_puthzs(108, 43, "开始录入", 0, 1);

	lcd_fb_rectangle(98, 22, 189, 62, 1, 0);

	login_index = input;
	vein_login_flag = 0;
	
	return 0;
}


uint16_t useradd_vein_key_process(void *param, uint16_t input)
{	
	uint8_t key_cmd = input>>8;
	uint8_t key_val = input&0xFF;	
	Msg_Value_t uvsend_msg = {0};	
	
	if(key_cmd != 0xA5)
		return 0;
	
	if(key_val == '\n')
	{
		if(vein_login_flag == 2)
		{
			vein_login_flag = 0;
			uvsend_msg.cmd = MSG_CMD_NEXT;
			uvsend_msg.value.s_val = login_index;		
			acitivy_send_msg(&uvsend_msg, 1000);
		}
	}
	else if(key_val == '\b')
	{	
		if(vein_login_flag == 0 || vein_login_flag == 2)
		{
			uvsend_msg.cmd = MSG_CMD_PUB;				
			acitivy_send_msg(&uvsend_msg, 1000);
		}
	}

	return 0;
}


uint16_t useradd_vein_int_process(void *param, uint16_t input)
{
	uint8_t int_cmd = input>>8;
	uint8_t int_val = input&0xFF;
	Msg_Value_t uv_int_msg = {0};
	
	switch(int_cmd)
	{
		case INPUT_VEIN:
			if(vein_login_flag == 0)
			{
				vein_login_flag = 1;
				uv_int_msg.cmd = VEIN_CMD_ADD;
				uv_int_msg.value.s_val = login_index;
				vein_send_msg(&uv_int_msg, 1000);
				lcd_fb_rectangle(99, 23, 188, 61, 0, 1);		
				lcd_fb_puthzs(108, 24, "录入中...", 0, 1);
			}
			break;
	
		default:
			
			break;
	}
	
	return 0;
}


uint16_t useradd_vein_vein_process(void *param, uint16_t input)
{
	uint8_t ret = input>>8;
	uint8_t u_id = input&0xFF;	

	switch(ret)
	{
		case VEIN_RETURN_NONE:
			lcd_fb_rectangle(99, 23, 188, 61, 0, 1);
			lcd_fb_puthzs(108, 24, "伸入手指", 0, 1);	
			lcd_fb_puthzs(108, 43, "开始录入", 0, 1);
			vein_login_flag = 0;	
			break;		

		case VEIN_RETURN_SUCCESS:
			lcd_fb_rectangle(99, 23, 188, 61, 0, 1);		
			lcd_fb_puthzs(108, 24, "录入成功", 0, 1);
			lcd_fb_puthzs(108, 43, "OK键继续", 0, 1);
			vein_login_flag = 2;
			break;
		
		case VEIN_RETURN_FAIL:
			lcd_fb_rectangle(99, 23, 188, 61, 0, 1);
			if(u_id == 0x0A)
			{
				lcd_fb_puthzs(108, 24, "该手指", 0, 1);
				lcd_fb_puthzs(108, 43, "已录入!", 0, 1);			
			}
			else		
				lcd_fb_puthzs(108, 24, "录入失败", 0, 1);
			break;
		
		case VEIN_RETURN_INPUT_FINGER:
			
			break;
		
		case VEIN_RETURN_RELEASE_FINGER:
			lcd_fb_rectangle(99, 23, 188, 61, 0, 1);		
			lcd_fb_puthzs(108, 24, "重新伸入", 0, 1);
			lcd_fb_puthzs(108, 43, "手指!", 0, 1);			
			break;

		default:

			break;
	}
	
	return 0;
}


struct Menu_Item_t useradd_vein_item = {
	.index = 0,
	.setup = useradd_vein_setup,
	.int_process = useradd_vein_int_process,
	.key_process = useradd_vein_key_process,
	.password_process = NULL,
	.fpc_process = NULL,
	.vein_process = useradd_vein_vein_process,
	.face_process = NULL,
	.rtc_process = NULL,
    .pub_menu = &user_item,	
	.sub_menu[0] = NULL,
	.sub_menu[1] = NULL,
	.sub_menu[2] = NULL,
	.sub_menu[3] = NULL,
	.next_menu = &useradd_face_item,
	.pre_menu = NULL,
};

