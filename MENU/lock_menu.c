#include "string.h"
#include "lcd192x64.h"
#include "rtc.h"
#include "device_config.h"
#include "message.h"
#include "identify_task.h"
#include "activity_task.h"
#include "fpc_task.h"
#include "face_task.h"
#include "io_task.h"
#include "rtc_task.h"
#include "menu.h"


uint16_t lock_rtc_process(void *param, uint16_t input)
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


uint16_t lock_setup(uint16_t input)
{
	lcd_fb_clear(0);

	lcd_fb_rectangle(25, 11, 166, 53, 1, 0);
	lcd_fb_rectangle(27, 13, 164, 51, 1, 0);

	lcd_fb_puthzs(30, 16, "√‹¬Î ‰¥ÌÃ´∂‡", 0, 1);
	lcd_fb_puthzs(30, 32, "«Îµ»¥˝1∑÷÷”", 0, 1);

	task_identify_enable(0);
	fpc_identify_enable(0);
	face_identify_enable(0);
	rtc_cnt_enable(0, RTC_FLAG_FACE);
	rtc_timeout_stop();

	rtc_set_wakeup(LOCK_TIMEOUT);
		
	return 0;
}


struct Menu_Item_t lock_item = {
	.index = 0,    
	.setup = lock_setup,
	.int_process = NULL,
	.key_process = NULL,
	.password_process = NULL,
	.fpc_process = NULL,
	.vein_process = NULL,
	.face_process = NULL,
	.rtc_process = lock_rtc_process,
    .pub_menu = &main_item,
	.sub_menu[0] = NULL,

};



