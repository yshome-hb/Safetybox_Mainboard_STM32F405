#include "string.h"
#include "lcd192x64.h"
#include "message.h"
#include "activity_task.h"
#include "nbiot_task.h"
#include "menu.h"



uint16_t nbiotinfo_setup(uint16_t input)
{
	Msg_Value_t nbisend_msg = {0};

	lcd_fb_clear(0);	

	lcd_fb_line(1, 20, 191, 20, 1);
	lcd_fb_fbmp(4, 4, BMP_MENU_BACK_12X12, 0, 1);
	lcd_fb_puthzs(80, 2, "nb状态", 0, 1);
	lcd_fb_puthzs(20, 2, "1/1", 0, 1);	
	
	nbisend_msg.cmd = NBIOT_CMD_INFO;
	nbisend_msg.value.s_val = 0;
	nbiot_send_msg(&nbisend_msg, 1000);	

	return 0;
}


uint16_t nbiotinfo_key_process(void *param, uint16_t input)
{
	uint8_t key_cmd = input>>8;
	uint8_t key_val = input&0xFF;
	Msg_Value_t nbisend_msg = {0};

	if(key_cmd != 0xA5)
		return 0;

	if(key_val == '\b')
	{
		nbisend_msg.cmd = MSG_CMD_PUB;				
		acitivy_send_msg(&nbisend_msg, 1000);
	}	

	return 0;
}


uint16_t nbiotinfo_nbiot_process(void *param, uint16_t input)
{	
	uint8_t cmd = input>>8;
	uint8_t val = input&0xFF;

	if(cmd == NBIOT_RETURN_SUCCESS)
	{
		char *ip = nbiot_get_ip();

		lcd_fb_puthzs(5, 23, "已连接网络", 0, 1);
		lcd_fb_puts(5, 42, ASCII_8X16, "ip:", 0, 1);
		lcd_fb_puts(30, 42, ASCII_8X16, ip, 0, 1);
	}
	else
	{
		lcd_fb_puthzs(5, 23, "未连接网络", 0, 1);		
	}
	
	return 0;
}


struct Menu_Item_t nbiotinfo_item = {
	.index = 0,
	.setup = nbiotinfo_setup,
	.int_process = NULL,
	.key_process = nbiotinfo_key_process,
	.password_process = NULL,
	.fpc_process = NULL,
	.vein_process = NULL,
	.face_process = NULL,
	.rtc_process = NULL,
	.nbiot_process = nbiotinfo_nbiot_process,
    .pub_menu = &networkconfig_item,	
	.sub_menu[0] = NULL,
	.sub_menu[1] = NULL,
	.sub_menu[2] = NULL,
	.sub_menu[3] = NULL,
	.next_menu = NULL,
	.pre_menu = NULL,
};


