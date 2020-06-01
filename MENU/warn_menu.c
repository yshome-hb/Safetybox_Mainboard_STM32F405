#include "string.h"
#include "lcd192x64.h"
#include "device_info.h"
#include "message.h"
#include "identify_task.h"
#include "activity_task.h"
#include "menu.h"


uint16_t warn_key_process(void *param, uint16_t input)
{
	static uint8_t warn_index = 0;
	uint8_t key_cmd = input>>8;
	uint8_t key_val = input&0xFF;
	Msg_Value_t wasend_msg = {0};
	// Msg_Identify_t lmsend_id = {0};

	if(key_cmd != 0xA5)
		return 0;

	if(key_val == '\n')
	{
		// if(lock_mode_set[lockmode_index] == 0)
		// {
		// 	lock_mode_set[lockmode_index]++;
		// 	lm_or_mask |= (((uint32_t)0x01)<<lockmode_index);
		// }
		// else if(lock_mode_set[lockmode_index] == 1)
		// {
		// 	lock_mode_set[lockmode_index]++;
		// 	lm_and_mask |= (((uint32_t)0x01)<<lockmode_index);
		// 	lm_or_mask &= ~(((uint32_t)0x01)<<lockmode_index);
		// }
		// else
		// {
		// 	lock_mode_set[lockmode_index] = 0;
		// 	lm_and_mask &= ~(((uint32_t)0x01)<<lockmode_index);
		// }

		// lockmode_show_mode(lockmode_index, lock_mode_set[lockmode_index]);
	}
	else if(key_val == '\b')
	{
		// lmsend_id.type = IDENTIFY_TYPE_SET_MASK;
		// lmsend_id.info.uid[0] = lm_or_mask;
		// lmsend_id.info.uid[1] = lm_and_mask;
		// identify_send_msg(&lmsend_id, 1000);
		wasend_msg.cmd = MSG_CMD_PUB;				
		acitivy_send_msg(&wasend_msg, 1000);
	}	
	else if(key_val >= '1' && key_val <= '4')
	{
		warn_index = key_val - '1';
		common_show_rectangle(warn_index);
	}

	return 0;
}


uint16_t warn_setup(uint16_t input)
{
	common_setup("±¨¾¯²ßÂÔ", "ÓïÒô", "À®°È", "ÍÆËÍ", NULL);
	lcd_fb_puthzs(20, 4, "1/1", 0, 1);
	
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

