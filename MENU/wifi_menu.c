#include "string.h"
#include "lcd192x64.h"
#include "message.h"
#include "activity_task.h"
#include "wifi_task.h"
#include "menu.h"


static uint8_t wificonfig_flag = 0; 


uint16_t wifiinfo_setup(uint16_t input)
{
	Msg_Value_t wisend_msg = {0};

	lcd_fb_clear(0);	

	lcd_fb_line(1, 20, 191, 20, 1);
	lcd_fb_fbmp(4, 4, BMP_MENU_BACK_12X12, 0, 1);
	lcd_fb_puthzs(80, 2, "wifi状态", 0, 1);
	lcd_fb_puthzs(20, 2, "1/1", 0, 1);	
	
	wisend_msg.cmd = WIFI_CMD_INFO;
	wisend_msg.value.s_val = 0;
	wifi_send_msg(&wisend_msg, 1000);	

	return 0;
}


uint16_t wificonfig_setup(uint16_t input)
{
	int32_t rssi = wifi_get_rssi();

	lcd_fb_clear(0);	

	lcd_fb_line(1, 20, 191, 20, 1);
	lcd_fb_fbmp(4, 4, BMP_MENU_BACK_12X12, 0, 1);
	lcd_fb_puthzs(80, 2, "wifi设置", 0, 1);
	lcd_fb_puthzs(20, 2, "1/1", 0, 1);	
	
	if(rssi < -200)
		lcd_fb_puthzs(5, 23, "未连接网络", 0, 1);
	else
		lcd_fb_puthzs(5, 23, "已连接网络", 0, 1);
	
	lcd_fb_puthzs(5, 42, "按OK键开始配置网络", 0, 1);

	return 0;
}


uint16_t wifiinfo_key_process(void *param, uint16_t input)
{
	uint8_t key_cmd = input>>8;
	uint8_t key_val = input&0xFF;
	Msg_Value_t wisend_msg = {0};

	if(key_cmd != 0xA5)
		return 0;

	if(key_val == '\b')
	{
		wisend_msg.cmd = MSG_CMD_PUB;				
		acitivy_send_msg(&wisend_msg, 1000);
	}	

	return 0;
}


uint16_t wificonfig_key_process(void *param, uint16_t input)
{
	uint8_t key_cmd = input>>8;
	uint8_t key_val = input&0xFF;
	Msg_Value_t wcsend_msg = {0};


	if(key_cmd != 0xA5)
		return 0;

	if(key_val == '\n')
	{
		if(wificonfig_flag > 1)
		{
			wificonfig_flag = 0;
			lcd_fb_rectangle(5, 42, 188, 61, 0, 1);		
			lcd_fb_puthzs(5, 42, "按OK键开始配置网络", 0, 1);
		}
		else if(wificonfig_flag == 1)
		{
			wificonfig_flag = 0;
			wifi_smartconfig_enable(0);
		}
		else
		{
			wificonfig_flag = 1;

			wcsend_msg.cmd = WIFI_CMD_SMART;
			wcsend_msg.value.s_val = 0;
			wifi_send_msg(&wcsend_msg, 1000);					

			lcd_fb_rectangle(5, 23, 188, 61, 0, 1);		
			lcd_fb_puthzs(5, 23, "未连接网络", 0, 1);
			lcd_fb_puthzs(5, 42, "网络配置中...", 0, 1);
		}
	}
	else if(key_val == '\b')
	{
		if(wificonfig_flag == 1)
		{
			wificonfig_flag = 0;
			wifi_smartconfig_enable(0);
		}		
		else
		{
			wificonfig_flag = 0;
			wcsend_msg.cmd = MSG_CMD_PUB;				
			acitivy_send_msg(&wcsend_msg, 1000);
		}
	}	

	return 0;
}


uint16_t wifiinfo_wifi_process(void *param, uint16_t input)
{	
	uint8_t cmd = input>>8;
	uint8_t val = input&0xFF;

	if(cmd == WIFI_RETURN_SUCCESS)
	{
		char *ssid = wifi_get_ssid();
		char *ip = wifi_get_ip();

		lcd_fb_puts(5, 23, ASCII_8X16, "ssid:", 0, 1);
		lcd_fb_puts(48, 23, ASCII_8X16, ssid, 0, 1);
		lcd_fb_puts(5, 42, ASCII_8X16, "ip:", 0, 1);
		lcd_fb_puts(30, 42, ASCII_8X16, ip, 0, 1);
	}
	else
	{
		lcd_fb_puthzs(30, 30, "未连接网络", 0, 1);		
	}
	
	return 0;
}


uint16_t wificonfig_wifi_process(void *param, uint16_t input)
{	
	uint8_t cmd = input>>8;
	uint8_t val = input&0xFF;

	wificonfig_flag = 2;
	lcd_fb_rectangle(5, 23, 188, 61, 0, 1);	
	if(cmd == WIFI_RETURN_SUCCESS)
	{
		lcd_fb_puthzs(5, 23, "已连接网络", 0, 1);
		lcd_fb_puthzs(5, 42, "网络配置成功", 0, 1);
	}
	else
	{
		lcd_fb_puthzs(5, 23, "未连接网络", 0, 1);
		lcd_fb_puthzs(5, 42, "网络配置中断", 0, 1);		
	}
	
	return 0;
}


struct Menu_Item_t wifiinfo_item = {
	.index = 0,
	.setup = wifiinfo_setup,
	.int_process = NULL,
	.key_process = wifiinfo_key_process,
	.password_process = NULL,
	.fpc_process = NULL,
	.vein_process = NULL,
	.face_process = NULL,
	.rtc_process = NULL,
	.wifi_process = wifiinfo_wifi_process,
    .pub_menu = &networkconfig_item,	
	.sub_menu[0] = NULL,
	.sub_menu[1] = NULL,
	.sub_menu[2] = NULL,
	.sub_menu[3] = NULL,
	.next_menu = NULL,
	.pre_menu = NULL,
};


struct Menu_Item_t wificonfig_item = {
	.index = 0,
	.setup = wificonfig_setup,
	.int_process = NULL,
	.key_process = wificonfig_key_process,
	.password_process = NULL,
	.fpc_process = NULL,
	.vein_process = NULL,
	.face_process = NULL,
	.rtc_process = NULL,
	.wifi_process = wificonfig_wifi_process,
    .pub_menu = &networkconfig_item,	
	.sub_menu[0] = NULL,
	.sub_menu[1] = NULL,
	.sub_menu[2] = NULL,
	.sub_menu[3] = NULL,
	.next_menu = NULL,
	.pre_menu = NULL,
};

