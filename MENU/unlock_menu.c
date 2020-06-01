#include "string.h"
#include "lcd192x64.h"
#include "input_output.h"
#include "device_info.h"
#include "message.h"
#include "identify_task.h"
#include "activity_task.h"
#include "fpc_task.h"
#include "face_task.h"
#include "io_task.h"
#include "rtc_task.h"
#include "menu.h"


static char u_password[9] = {0};
static uint8_t admin_flag = 0;


uint16_t unlock_int_process(void *param, uint16_t input)
{
	uint8_t int_cmd = input>>8;
	uint8_t int_val = input&0xFF;
	
	switch(int_cmd)
	{
		case INPUT_CONFIG:
	
			// lcd_fb_fbmp(73, 24, BMP_UNLOCK_WARN_16X14, 0, 1);
		
			lcd_fb_rectangle(28, 14, 163, 50, 0, 1);        
	    	lcd_fb_puthzs(30, 16, "请输入管理员密码", 0, 1);
		
			for(int j = 0; j < 8; j++)
			{
				lcd_fb_line(45+j*10, 48, 53+j*10, 48, 1);		
			}
			 
			lcd_fb_rectangle(130, 33, 147, 48, 1, 0);	

			admin_flag = 1;
            break;
	
		default:
			
			break;
	}
	
	return 0;
}


uint16_t unlock_key_process(void *param, uint16_t input)
{
	uint8_t key_cmd = input>>8;
	uint8_t key_val = input&0xFF;	
    char u_password_show[9] = {0};
	uint8_t len = strlen(u_password);
	Msg_Value_t ulsend_msg = {0};	
	Msg_Identify_t ulsend_id = {0};
	
	if(key_cmd != 0xA5)
		return 0;
	
	if(key_val == '\n')
	{
		if(len > 0)
		{
			ulsend_id.type = IDENTIFY_TYPE_ADMIN;
			strcpy(ulsend_id.info.password, u_password);
			identify_send_msg(&ulsend_id, 1000);
		}
		memset(u_password, 0, 9);
	}
	else if(key_val == '\b')
	{
		if(len > 0)
        {
			u_password[len-1] = '\0';	
        }
        else
        {
			ulsend_msg.cmd = MSG_CMD_PUB;				
			acitivy_send_msg(&ulsend_msg, 1000);
        }
        
	}
	else if(key_val >= '0' && key_val <= '9')
	{
		if(admin_flag > 0 && len < 8)
			u_password[len] = key_val;
	}	

	if(admin_flag > 0)
	{
		memset(u_password_show, ' ', 8);
		memset(u_password_show, '*', strlen(u_password));
		lcd_fb_puts(44, 36, ASCII_10X12, u_password_show, 0, 1);
	}

	return 0;
}


uint16_t unlock_password_process(void *param, uint16_t input)
{
	uint8_t ret = input>>8;
	uint8_t u_id = input&0xFF;
	Msg_Value_t ulsend_msg = {0};	

	switch(ret)
	{
		case PWD_RETURN_NONE:
			lcd_fb_rectangle(131, 34, 146, 47, 0, 1);	
			break;		

		case PWD_RETURN_SUCCESS:
			lcd_fb_fbmp(131, 34, BMP_UNLOCK_SUCC_16X14, 0, 1);
			ulsend_msg.cmd = MSG_CMD_SUB;				
			acitivy_send_msg(&ulsend_msg, 1000);
			break;
		
		case PWD_RETURN_FAIL:
			lcd_fb_fbmp(131, 34, BMP_UNLOCK_ERROR_16X14, 0, 1);
			break;

		default:

			break;
	}

	return 0;
}


uint16_t unlock_rtc_process(void *param, uint16_t input)
{	
	// uint8_t rtc_cmd = input>>8;
	// uint8_t rtc_val = input&0xFF;
	// Msg_Identify_t send_msg_id = {0};

	// switch(rtc_cmd)
	// {
	// 	case RTC_CMD_CLEAR:
	// 		lcd_fb_rectangle(98, 5, 210, 17, 0, 1);	

	// 		lcd_fb_rectangle(73, 3, 88, 16, 0, 1);	
	// 		lcd_fb_rectangle(73, 24, 88, 37, 0, 1);	
	// 		lcd_fb_rectangle(73, 45, 88, 58, 0, 1);
	// 		lcd_fb_rectangle(170, 24, 185, 37, 0, 1);

	// 		send_msg_id.type = IDENTIFY_TYPE_CLEAR;
	// 		identify_send_msg(&send_msg_id, 1000);
	// 		break;		

	// 	case RTC_CMD_SET:
	// 		lcd_fb_rectangle(98, 5, 210, 17, 0, 1);	
	// 		lcd_fb_putn(98, 5, ASCII_10X12, rtc_val, 0, 1);
	// 		break;
		
	// 	case RTC_CMD_DECREASE:
	// 		lcd_fb_rectangle(98, 5, 210, 17, 0, 1);	
	// 		lcd_fb_putn(98, 5, ASCII_10X12, rtc_val, 0, 1);
	// 		break;
		
	// 	default:

	// 		break;
	// }

	return 0;	
}


uint16_t unlock_setup(uint16_t input)
{
	uint8_t lock_cmd = input>>8;
	uint8_t lock_val = input&0xFF;
	User_Info_t* u_temp = NULL;

	lcd_fb_clear(0);

	lcd_fb_rectangle(25, 11, 166, 53, 1, 0);
	lcd_fb_rectangle(27, 13, 164, 51, 1, 0);

    if(lock_cmd == IDENTIFY_RESULT_SUCCESS)
    {
		rtc_cnt_enable(1, RTC_FLAG_UNLOCK);

       u_temp = device_get_user(lock_val);
        
	    lcd_fb_puthzs(30, 16, "认证通过", 0, 1);
	    lcd_fb_puthzs(30, 32, "欢迎用户:", 0, 1);
        lcd_fb_puthzs(102, 33, u_temp->username, 0, 1);
    }
    else
    {
	    lcd_fb_puthzs(30, 16, "认证失败", 0, 1);
	    lcd_fb_puthzs(30, 32, "信息不属于同一用户", 0, 1);
    }

	admin_flag = 0;
	task_identify_enable(0);
	fpc_identify_enable(0);
	face_identify_enable(0);
	rtc_cnt_enable(0, RTC_FLAG_FACE);
		
	return 0;
}


struct Menu_Item_t unlock_item = {
	.index = 0,    
	.setup = unlock_setup,
	.int_process = unlock_int_process,
	.key_process = unlock_key_process,
	.password_process = unlock_password_process,
	.fpc_process = NULL,
	.vein_process = NULL,
	.face_process = NULL,
	.rtc_process = unlock_rtc_process,
    .pub_menu = &main_item,
	.sub_menu[0] = &manage1_item,

};



