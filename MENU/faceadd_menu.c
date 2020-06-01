#include "string.h"
#include "lcd192x64.h"
#include "message.h"
#include "identify_task.h"
#include "activity_task.h"
#include "face_task.h"
#include "rtc_task.h"
#include "menu.h"


static uint8_t login_index = 0;
static uint8_t face_login_flag = 0;


uint16_t useradd_face_setup(uint16_t input)
{
	lcd_fb_clear(0);	
	
	lcd_fb_line(1, 20, 191, 20, 1);	
	
	lcd_fb_fbmp(4, 4, BMP_MENU_BACK_12X12, 0, 1);

	lcd_fb_rectangle(95, 22, 96, 62, 1, 1);				

	lcd_fb_puthzs(30, 2, "添加用户/5人脸设置", 0, 1);
	lcd_fb_puthzs(10, 24, "人脸录入", 0, 1);
	lcd_fb_puthzs(10, 43, "请看摄像头", 0, 1);
	lcd_fb_puthzs(108, 24, "按OK键", 0, 1);	
	lcd_fb_puthzs(108, 43, "开始录入", 0, 1);

	lcd_fb_rectangle(98, 22, 189, 62, 1, 0);

	login_index = input;
	face_login_flag = 0;
	
	return 0;
}


uint16_t useradd_face_key_process(void *param, uint16_t input)
{	
	uint8_t key_cmd = input>>8;
	uint8_t key_val = input&0xFF;	
	Msg_Value_t uasend_msg = {0};	
	
	if(key_cmd != 0xA5)
		return 0;
	
	if(key_val == '\n')
	{
		if(face_login_flag == 0)
		{
			face_login_flag = 1;
			lcd_fb_rectangle(99, 23, 188, 61, 0, 1);		
			lcd_fb_puthzs(108, 24, "录入中...", 0, 1);
			rtc_cnt_enable(1, RTC_FLAG_FACE);
			uasend_msg.cmd = FACE_CMD_ADD;
			uasend_msg.value.s_val = login_index;
			face_send_msg(&uasend_msg, 1000);
		}
		else if(face_login_flag == 2)
		{
			face_login_flag = 0;
			uasend_msg.cmd = MSG_CMD_NEXT;	
			uasend_msg.value.s_val = login_index;	
			acitivy_send_msg(&uasend_msg, 1000);
		}
	}
	else if(key_val == '\b')
	{	
		if(face_login_flag == 0 || face_login_flag == 2)
		{
			uasend_msg.cmd = MSG_CMD_PUB;				
			acitivy_send_msg(&uasend_msg, 1000);
		}
	}
	else if(key_val == 'd')
	{	
		if(face_login_flag == 0 || face_login_flag == 2)
		{
			face_login_flag = 0;
			uasend_msg.cmd = MSG_CMD_NEXT;	
			uasend_msg.value.s_val = login_index;	
			acitivy_send_msg(&uasend_msg, 1000);			
		}
	}

	return 0;
}


uint16_t useradd_face_face_process(void *param, uint16_t input)
{
	uint8_t ret = input>>8;
	uint8_t u_id = input&0xFF;
	
	switch(ret)
	{
		case FACE_RETURN_NONE:
			lcd_fb_rectangle(99, 23, 188, 61, 0, 1);
			lcd_fb_puthzs(108, 24, "按OK键", 0, 1);	
			lcd_fb_puthzs(108, 43, "开始录入", 0, 1);
			face_login_flag = 0;		
			break;		

		case FACE_RETURN_SUCCESS:
			lcd_fb_rectangle(99, 23, 188, 61, 0, 1);		
			lcd_fb_puthzs(108, 24, "录入成功", 0, 1);
			lcd_fb_puthzs(108, 43, "OK键继续", 0, 1);
			face_login_flag = 2;
			break;
		
		case FACE_RETURN_FAIL:
			lcd_fb_rectangle(99, 23, 188, 61, 0, 1);		
			lcd_fb_puthzs(108, 24, "录入失败", 0, 1);	
			break;

		case FACE_RETURN_TIMEOUT:
			lcd_fb_rectangle(99, 23, 188, 61, 0, 1);		
			lcd_fb_puthzs(108, 24, "等待超时", 0, 1);	
			break;			

		default:

			break;
	}

	return 0;
}


struct Menu_Item_t useradd_face_item = {
	.index = 0,
	.setup = useradd_face_setup,
	.int_process = NULL,
	.key_process = useradd_face_key_process,
	.password_process = NULL,
	.fpc_process = NULL,
	.vein_process = NULL,
	.face_process = useradd_face_face_process,
	.rtc_process = NULL,
    .pub_menu = &user_item,	
	.sub_menu[0] = NULL,
	.sub_menu[1] = NULL,
	.sub_menu[2] = NULL,
	.sub_menu[3] = NULL,
	.next_menu = &useradd_result_item,
	.pre_menu = NULL,
};

