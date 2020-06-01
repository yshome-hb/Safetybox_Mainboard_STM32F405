#include "string.h"
#include "lcd192x64.h"
#include "message.h"
#include "menu.h"
#include "activity_task.h"


uint16_t common_show_popup(const char *pop)
{
	lcd_fb_rectangle(34, 27, 157, 54, 0, 1);
	lcd_fb_rectangle(34, 27, 157, 54, 1, 0);
	lcd_fb_rectangle(37, 30, 154, 51, 1, 0);

	lcd_fb_puthzs(40, 32, pop, 0, 1);
	
	return 0;
}


void common_show_rectangle(uint8_t i)
{
	lcd_fb_rectangle(1, 22, 93, 39, 0, 0);	
	lcd_fb_rectangle(1, 43, 93, 60, 0, 0);	
	lcd_fb_rectangle(97, 22, 190, 39, 0, 0);	
	lcd_fb_rectangle(97, 43, 190, 60, 0, 0);	
	
	if(i == 0)
		lcd_fb_rectangle(1, 22, 93, 39, 1, 0);	
	else if(i == 1)
		lcd_fb_rectangle(1, 43, 93, 60, 1, 0);	
	else if(i == 2)
		lcd_fb_rectangle(97, 22, 190, 39, 1, 0);	
	else if(i == 3)
		lcd_fb_rectangle(97, 43, 190, 60, 1, 0);	

}


uint16_t common_int_process(void *param, uint16_t input)
{
	uint8_t int_cmd = input>>8;
	uint8_t int_val = input&0xFF;
	Msg_Value_t n_int_msg = {0};
	
	switch(int_cmd)
	{
		// case INPUT_VEIN:
		// 	m_int_msg.cmd = VEIN_CMD_QUERY;
		// 	vein_send_msg(&m_int_msg, 1000);
		// 	lcd_fb_fbmp(73, 24, BMP_UNLOCK_WARN_16X14, 0, 1);
		// 	break;
	
		default:
			
			break;
	}
	
	return 0;
}


uint16_t common_key_process(void *param, uint16_t input)
{
	uint8_t key_cmd = input>>8;
	uint8_t key_val = input&0xFF;
	struct Menu_Item_t *c_item = (struct Menu_Item_t *)param;
	Msg_Value_t comsend_msg = {0};

	if(key_cmd != 0xA5)
		return 0;

	if(key_val == '\n')
	{
		comsend_msg.cmd = MSG_CMD_SUB;				
		acitivy_send_msg(&comsend_msg, 1000);
	}
	else if(key_val == '\b')
	{
		comsend_msg.cmd = MSG_CMD_PUB;
		acitivy_send_msg(&comsend_msg, 1000);
	}
	else if(key_val == 'u')
	{
		comsend_msg.cmd = MSG_CMD_PRE;				
		acitivy_send_msg(&comsend_msg, 1000);
	}
	else if(key_val == 'd')
	{
		comsend_msg.cmd = MSG_CMD_NEXT;				
		acitivy_send_msg(&comsend_msg, 1000);		
	}	
	else if(key_val >= '1' && key_val <= '4')
	{
		c_item->index = key_val - '1';
		common_show_rectangle(c_item->index);
	}

	return 0;
}


uint16_t common_setup(const char *title,
					  const char *option1,
					  const char *option2,
					  const char *option3,
					  const char *option4)
{
	lcd_fb_clear(0);	
	
	lcd_fb_line(1, 20, 191, 20, 1);
	lcd_fb_line(1, 41, 93, 41, 1);
	lcd_fb_line(98, 41, 191, 41, 1);	
	lcd_fb_line(1, 62, 93, 62, 1);	
	lcd_fb_line(98, 62, 191, 62, 1);	
	
	lcd_fb_fbmp(4, 4, BMP_MENU_BACK_12X12, 0, 1);

	lcd_fb_puthzs(80, 2, title, 0, 1);
	
	if(option1 != NULL) 
	{
		lcd_fb_puthzs(3, 26, "1", 0, 1);	
		lcd_fb_puthzs(15, 23, option1, 0, 1);
	}

	if(option2 != NULL) 
	{
		lcd_fb_puthzs(3, 47, "2", 0, 1);
		lcd_fb_puthzs(15, 44, option2, 0, 1);
	}

	if(option3 != NULL) 
	{
		lcd_fb_puthzs(99, 26, "3", 0, 1);	
		lcd_fb_puthzs(111, 23, option3, 0, 1);
	}

	if(option4 != NULL) 
	{
		lcd_fb_puthzs(99, 47, "4", 0, 1);
		lcd_fb_puthzs(111, 44, option4, 0, 1);
	}

	common_show_rectangle(0);
	
	return 0;
}

