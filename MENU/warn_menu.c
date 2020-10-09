#include "string.h"
#include "lcd192x64.h"
#include "device_info.h"
#include "message.h"
#include "identify_task.h"
#include "activity_task.h"
#include "menu.h"


static void warn_show_mode(uint16_t mode)
{
	lcd_fb_rectangle(73, 29, 77, 33, (mode & WARN_BEEP), 1);
	lcd_fb_rectangle(73, 50, 77, 54, (mode & WARN_PUSH), 1);
	//lcd_fb_rectangle(169, 29, 173, 33, (mode & WARN_VOICE), 1);	
}


uint16_t warn_setup(uint16_t input)
{
	common_setup("±¨¾¯²ßÂÔ", "À®°È", "ÍÆËÍ", NULL, NULL);
	lcd_fb_puthzs(20, 2, "1/1", 0, 1);
	
	lcd_fb_rectangle(72, 28, 78, 34, 1, 0);
	lcd_fb_rectangle(72, 49, 78, 55, 1, 0);
	//lcd_fb_rectangle(168, 28, 174, 34, 1, 0);

	warn_show_mode(device_get_warnmode());

	return 0;
}



uint16_t warn_key_process(void *param, uint16_t input)
{
	static uint8_t warn_index = 0;
	uint8_t key_cmd = input>>8;
	uint8_t key_val = input&0xFF;
	Msg_Value_t wasend_msg = {0};
	Msg_Identify_t wasend_id = {0};
	uint16_t warn_mode = 0;

	if(key_cmd != 0xA5)
		return 0;

	if(key_val == '\n')
	{
		warn_mode = device_get_warnmode();
		if(warn_mode & (0x01 << warn_index))
		{
			warn_mode &= ((~(0x01 << warn_index)) & 0x07);
		}
		else
		{
			warn_mode |= (0x01 << warn_index);
		}

		device_set_warnmode(warn_mode);
		warn_show_mode(warn_mode);

	}
	else if(key_val == '\b')
	{
		wasend_id.type = IDENTIFY_TYPE_SET_FLASH;
		identify_send_msg(&wasend_id, 1000);

		wasend_msg.cmd = MSG_CMD_PUB;				
		acitivy_send_msg(&wasend_msg, 1000);

		warn_index = 0;
	}	
	else if(key_val >= '1' && key_val <= '2')
	{
		warn_index = key_val - '1';
		common_show_rectangle(warn_index);
	}

	return 0;
}


struct Menu_Item_t warn_item = {
	.index = 0,
	.setup = warn_setup,
	.int_process = NULL,
	.key_process = warn_key_process,
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

