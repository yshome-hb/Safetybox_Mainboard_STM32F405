#include "string.h"
#include "lcd192x64.h"
#include "device_info.h"
#include "message.h"
#include "identify_task.h"
#include "activity_task.h"
#include "menu.h"


static uint8_t lock_mode_set[ID_POS_MAX] = {0};
static uint32_t lm_or_mask = 0;
static uint32_t lm_and_mask = 0;


static void lockmode_show_mode(uint8_t i, uint8_t mode)
{
	if(i == 0)
	{
		if(mode == 2)
		{
			lcd_fb_rectangle(73, 29, 77, 33, 1, 1);
		}
		else if(mode == 1)
		{
			lcd_fb_fbmp(72, 28, BMP_MODE_OPTIONAL_7X7, 0, 1);
		}
		else
		{
			lcd_fb_rectangle(73, 29, 77, 33, 0, 1);
		}
	}
	else if(i == 1)
	{
		if(mode == 2)
		{
			lcd_fb_rectangle(73, 50, 77, 54, 1, 1);
		}
		else if(mode == 1)
		{
			lcd_fb_fbmp(72, 49, BMP_MODE_OPTIONAL_7X7, 0, 1);
		}
		else
		{
			lcd_fb_rectangle(73, 50, 77, 54, 0, 1);
		}
	}
	else if(i == 2)
	{
		if(mode == 2)
		{
			lcd_fb_rectangle(169, 29, 173, 33, 1, 1);
		}
		else if(mode == 1)
		{
			lcd_fb_fbmp(168, 28, BMP_MODE_OPTIONAL_7X7, 0, 1);
		}
		else
		{
			lcd_fb_rectangle(169, 29, 173, 33, 0, 1);
		}
	}
	else if(i == 3)
	{
		if(mode == 2)
		{
			lcd_fb_rectangle(169, 50, 173, 54, 1, 1);
		}
		else if(mode == 1)
		{
			lcd_fb_fbmp(168, 49, BMP_MODE_OPTIONAL_7X7, 0, 1);
		}
		else
		{
			lcd_fb_rectangle(169, 50, 173, 54, 0, 1);
		}
	}	
}


uint16_t lockmode_key_process(void *param, uint16_t input)
{
	static uint8_t lockmode_index = 0;
	uint8_t key_cmd = input>>8;
	uint8_t key_val = input&0xFF;
	Msg_Value_t lmmsend_msg = {0};
	Msg_Identify_t lmsend_id = {0};

	if(key_cmd != 0xA5)
		return 0;

	if(key_val == '\n')
	{
		if(lock_mode_set[lockmode_index] == 0)
		{
			lock_mode_set[lockmode_index]++;
			lm_or_mask |= (((uint32_t)0x01)<<lockmode_index);
		}
		else if(lock_mode_set[lockmode_index] == 1)
		{
			lock_mode_set[lockmode_index]++;
			lm_and_mask |= (((uint32_t)0x01)<<lockmode_index);
			lm_or_mask &= ~(((uint32_t)0x01)<<lockmode_index);
		}
		else
		{
			lock_mode_set[lockmode_index] = 0;
			lm_and_mask &= ~(((uint32_t)0x01)<<lockmode_index);
		}

		lockmode_show_mode(lockmode_index, lock_mode_set[lockmode_index]);
	}
	else if(key_val == '\b')
	{
		lmsend_id.type = IDENTIFY_TYPE_SET_MASK;
		lmsend_id.info.uid[0] = lm_or_mask;
		lmsend_id.info.uid[1] = lm_and_mask;
		identify_send_msg(&lmsend_id, 1000);
		lmmsend_msg.cmd = MSG_CMD_PUB;				
		acitivy_send_msg(&lmmsend_msg, 1000);
	}	
	else if(key_val >= '1' && key_val <= '4')
	{
		lockmode_index = key_val - '1';
		common_show_rectangle(lockmode_index);
	}

	return 0;
}


uint16_t lockmode_setup(uint16_t input)
{
	lm_or_mask = device_get_or_mask();
	lm_and_mask = device_get_and_mask();

	common_setup("开锁模式", "密码", "指纹", "指静脉", "人脸");
	lcd_fb_puthzs(20, 2, "1/1", 0, 1);

	if(lm_and_mask & ID_PWD_BIT)
	{
		lcd_fb_rectangle(72, 28, 78, 34, 1, 1);
		lock_mode_set[ID_POS_PWD] = 2;
	}
	else if(lm_or_mask & ID_PWD_BIT)
	{
		lcd_fb_fbmp(72, 28, BMP_MODE_OPTIONAL_7X7, 0, 1);
		lock_mode_set[ID_POS_PWD] = 1;
	}
	else
	{
		lcd_fb_rectangle(72, 28, 78, 34, 1, 0);
		lock_mode_set[ID_POS_PWD] = 0;
	}

	if(lm_and_mask & ID_FPC_BIT)
	{
		lcd_fb_rectangle(72, 49, 78, 55, 1, 1);
		lock_mode_set[ID_POS_FPC] = 2;
	}
	else if(lm_or_mask & ID_FPC_BIT)
	{
		lcd_fb_fbmp(72, 49, BMP_MODE_OPTIONAL_7X7, 0, 1);
		lock_mode_set[ID_POS_FPC] = 1;
	}
	else
	{
		lcd_fb_rectangle(72, 49, 78, 55, 1, 0);
		lock_mode_set[ID_POS_FPC] = 0;
	}

	if(lm_and_mask & ID_VEIN_BIT)
	{
		lcd_fb_rectangle(168, 28, 174, 34, 1, 1);
		lock_mode_set[ID_POS_VEIN] = 2;
	}
	else if(lm_or_mask & ID_VEIN_BIT)
	{
		lcd_fb_fbmp(168, 28, BMP_MODE_OPTIONAL_7X7, 0, 1);
		lock_mode_set[ID_POS_VEIN] = 1;
	}
	else
	{
		lcd_fb_rectangle(168, 28, 174, 34, 1, 0);
		lock_mode_set[ID_POS_VEIN] = 0;
	}

	if(lm_and_mask & ID_FACE_BIT)
	{
		lcd_fb_rectangle(168, 49, 174, 55, 1, 1);
		lock_mode_set[ID_POS_FACE] = 2;
	}
	else if(lm_or_mask & ID_FACE_BIT)
	{
		lcd_fb_fbmp(168, 49, BMP_MODE_OPTIONAL_7X7, 0, 1);
		lock_mode_set[ID_POS_FACE] = 1;
	}
	else
	{		
		lcd_fb_rectangle(168, 49, 174, 55, 1, 0);
		lock_mode_set[ID_POS_FACE] = 0;
	}

	return 0;
}


struct Menu_Item_t lockmode_item = {
	.index = 0,
	.setup = lockmode_setup,
	.int_process = NULL,
	.key_process = lockmode_key_process,
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

