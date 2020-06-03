#include "string.h"
#include "lcd192x64.h"
#include "input_output.h"
#include "device_info.h"
#include "delay.h"
#include "message.h"
#include "identify_task.h"
#include "activity_task.h"
#include "io_task.h"
#include "fpc_task.h"
#include "vfinger_task.h"
#include "face_task.h"
#include "rtc_task.h"
#include "wifi_task.h"
#include "menu.h"


static char m_password[9] = {0};


static void show_wifi_rssi(int32_t r, uint8_t force)
{
	static int32_t wrssi = 0;

	if(r == wrssi && force == 0)
		return;

	wrssi = r;

	if(wrssi < -99)
	{
		lcd_fb_fbmp(153, 4, BMP_MAIN_WIFI0_13X13, 0, 1);
	}
	else if(wrssi < -80)
	{
		lcd_fb_fbmp(153, 4, BMP_MAIN_WIFI1_13X13, 0, 1);
	}
	else if(wrssi < -50)
	{
		lcd_fb_fbmp(153, 4, BMP_MAIN_WIFI2_13X13, 0, 1);
	}
	else
	{
		lcd_fb_fbmp(153, 4, BMP_MAIN_WIFI3_13X13, 0, 1);
	}
}


uint16_t main_int_process(void *param, uint16_t input)
{
	uint8_t int_cmd = input>>8;
	uint8_t int_val = input&0xFF;
	Msg_Value_t m_int_msg = {0};
	
	switch(int_cmd)
	{
		case INPUT_VEIN:
			m_int_msg.cmd = VEIN_CMD_QUERY;
			vein_send_msg(&m_int_msg, 1000);
			lcd_fb_fbmp(73, 24, BMP_UNLOCK_WARN_16X14, 0, 1);
			break;

		case INPUT_SHAKE:
			rtc_cnt_enable(1, RTC_FLAG_BEEP);
			break;

		default:
			
			break;
	}
	
	return 0;
}


uint16_t main_key_process(void *param, uint16_t input)
{
	static uint8_t admin_cnt = 0;
	uint8_t key_cmd = input>>8;
	uint8_t key_val = input&0xFF;	
	char m_password_show[9] = {0};
	uint8_t len = strlen(m_password);
	Msg_Value_t mksend_msg = {0};		
	Msg_Identify_t send_msg_id = {0};
	
	if(key_cmd != 0xA5)
		return 0;
	
	if(key_val == '\n')
	{
		if(len > 0)
		{
			if(admin_cnt == 2)
				send_msg_id.type = IDENTIFY_TYPE_ADMIN;
			else
				send_msg_id.type = IDENTIFY_TYPE_PASSOWRD;
			strcpy(send_msg_id.info.password, m_password);
			identify_send_msg(&send_msg_id, 1000);
		}
		admin_cnt = 0;
		memset(m_password, 0, 9);
	}
	else if(key_val == '\b')
	{
		if(len > 0)
			m_password[len-1] = '\0';
		else
			admin_cnt = 0;
	}
	else if(key_val == '*')
	{
		rtc_cnt_enable(1, RTC_FLAG_FACE);
		face_identify_enable(1);		
		delay_ms(100);
		mksend_msg.cmd = FACE_CMD_ENABLE;
		mksend_msg.value.s_val = 1;
		face_send_msg(&mksend_msg, 1000);
	}
	else if(key_val >= '0' && key_val <= '9')
	{
		if(len < 8)
			m_password[len] = key_val;
	}
	else if(key_val == '#')
	{
		if(len == 0)
			admin_cnt++; 
	}	

	memset(m_password_show, ' ', 8);
	memset(m_password_show, '*', strlen(m_password));
	lcd_fb_puts(106, 47, ASCII_10X12, m_password_show, 0, 1);

	return 0;
}


uint16_t main_password_process(void *param, uint16_t input)
{
	uint8_t ret = input>>8;
	uint8_t u_id = input&0xFF;
	Msg_Value_t mpsend_msg = {0};	
	Msg_Identify_t mpsend_msg_id = {0};	
	
	switch(ret)
	{
		case PWD_RETURN_NONE:
			lcd_fb_rectangle(170, 24, 185, 37, 0, 1);	
			break;		

		case PWD_RETURN_SUCCESS:
			lcd_fb_fbmp(170, 24, BMP_UNLOCK_SUCC_16X14, 0, 1);
			if(u_id == 1)	
			{
				main_item.index = 1;
				mpsend_msg.cmd = MSG_CMD_SUB;				
				acitivy_send_msg(&mpsend_msg, 1000);
				rtc_cnt_enable(1, RTC_FLAG_UNLOCK);
				mpsend_msg_id.type = IDENTIFY_TYPE_CLEAR;
				identify_send_msg(&mpsend_msg_id, 1000);
			}
			break;
		
		case PWD_RETURN_FAIL:
			lcd_fb_fbmp(170, 24, BMP_UNLOCK_ERROR_16X14, 0, 1);
			break;

		default:

			break;
	}

	return 0;
}



uint16_t main_fpc_process(void *param, uint16_t input)
{
	uint8_t ret = input>>8;
	uint8_t u_id = input&0xFF;
	Msg_Identify_t send_msg_id = {0};
	
	switch(ret)
	{
		case FPC_RETURN_NONE:
			lcd_fb_rectangle(73, 3, 88, 16, 0, 1);	
			break;		

		case FPC_RETURN_SUCCESS:
			lcd_fb_fbmp(73, 3, BMP_UNLOCK_SUCC_16X14, 0, 1);
			send_msg_id.type = IDENTIFY_TYPE_FPC;
			send_msg_id.info.uid[0] = u_id;
			identify_send_msg(&send_msg_id, 1000);
			break;
		
		case FPC_RETURN_FAIL:
			lcd_fb_fbmp(73, 3, BMP_UNLOCK_ERROR_16X14, 0, 1);
			send_msg_id.type = IDENTIFY_TYPE_FPC;
			send_msg_id.info.uid[0] = 0;
			identify_send_msg(&send_msg_id, 1000);
			break;

		default:

			break;
	}

	return 0;
}


uint16_t main_vein_process(void *param, uint16_t input)
{
	uint8_t ret = input>>8;
	uint8_t u_id = input&0xFF;
	Msg_Identify_t send_msg_id = {0};	

	switch(ret)
	{
		case VEIN_RETURN_NONE:
			lcd_fb_rectangle(73, 24, 88, 37, 0, 1);	
			break;		

		case VEIN_RETURN_SUCCESS:
			lcd_fb_fbmp(73, 24, BMP_UNLOCK_SUCC_16X14, 0, 1);
			send_msg_id.type = IDENTIFY_TYPE_VEIN;
			send_msg_id.info.uid[0] = u_id;
			identify_send_msg(&send_msg_id, 1000);
			break;
		
		case VEIN_RETURN_FAIL:
			lcd_fb_fbmp(73, 24, BMP_UNLOCK_ERROR_16X14, 0, 1);
			send_msg_id.type = IDENTIFY_TYPE_VEIN;
			send_msg_id.info.uid[0] = 0;
			identify_send_msg(&send_msg_id, 1000);
			break;
		
		case VEIN_RETURN_INPUT_FINGER:
			
			break;
		
		case VEIN_RETURN_RELEASE_FINGER:
			
			break;

		default:

			break;
	}
	
	return 0;
}


uint16_t main_face_process(void *param, uint16_t input)
{	
	uint8_t ret = input>>8;
	uint8_t u_id = input&0xFF;
	Msg_Identify_t send_msg_id = {0};	

	switch(ret)
	{
		case FACE_RETURN_NONE:
			lcd_fb_rectangle(73, 45, 88, 58, 0, 1);	
			break;		

		case FACE_RETURN_SUCCESS:
			lcd_fb_fbmp(73, 45, BMP_UNLOCK_SUCC_16X14, 0, 1);
			send_msg_id.type = IDENTIFY_TYPE_FACE;
			send_msg_id.info.uid[0] = u_id;
			identify_send_msg(&send_msg_id, 1000);
			break;
		
		case FACE_RETURN_FAIL:
			lcd_fb_fbmp(73, 45, BMP_UNLOCK_ERROR_16X14, 0, 1);
			send_msg_id.type = IDENTIFY_TYPE_FACE;
			send_msg_id.info.uid[0] = 0;
			identify_send_msg(&send_msg_id, 1000);
			break;
		
		default:

			break;
	}

	return 0;	
}


uint16_t main_rtc_process(void *param, uint16_t input)
{	
	static uint8_t id_cnt = 0;
	uint8_t rtc_cmd = input>>8;
	uint8_t rtc_flag = input&0xFF;
	Msg_Identify_t mrsend_msg_id = {0};

	switch(rtc_cmd)
	{
		case RTC_CMD_CLEAR:
			if(rtc_flag == RTC_FLAG_IDENTIY)
			{
				id_cnt = 0;
				lcd_fb_rectangle(98, 5, 114, 17, 0, 1);	

				lcd_fb_rectangle(73, 3, 88, 16, 0, 1);	
				lcd_fb_rectangle(73, 24, 88, 37, 0, 1);	
				lcd_fb_rectangle(73, 45, 88, 58, 0, 1);
				lcd_fb_rectangle(170, 24, 185, 37, 0, 1);
			}
			break;		

		case RTC_CMD_SET:
			if(rtc_flag == RTC_FLAG_IDENTIY)
			{
				lcd_fb_rectangle(98, 4, 114, 17, 0, 1);	
				id_cnt = 60;
				lcd_fb_putn(98, 2, ASCII_8X16, id_cnt, 0, 1);
			}
			break;
		
		case RTC_CMD_DECREASE:
			if(id_cnt > 0)
			{
				id_cnt--;
				lcd_fb_rectangle(98, 4, 114, 17, 0, 1);	
				lcd_fb_putn(98, 2, ASCII_8X16, id_cnt, 0, 1);

				if(id_cnt == 0)
				{
					mrsend_msg_id.type = IDENTIFY_TYPE_CLEAR;
					identify_send_msg(&mrsend_msg_id, 1000);
				}	
			}
	
			break;
		
		default:

			break;
	}

	return 0;	
}


uint16_t main_wifi_process(void *param, uint16_t input)
{	
	uint8_t cmd = input>>8;
	uint8_t val = input&0xFF;

	if(cmd == WIFI_RETURN_SUCCESS)
	{
		int len = 0;
		char *data = wifi_recv_tcpdata(&len);
		
		if(len > 0)
			printf(data);
	}
	
	return 0;
}


uint16_t main_setup(uint16_t input)
{
	uint32_t m_or_mask = device_get_or_mask();
	uint32_t m_and_mask = device_get_and_mask();

	lcd_fb_clear(0);

	if(m_and_mask & ID_PWD_BIT)
		lcd_fb_rectangle(103, 28, 110, 35, 1, 1);
	else if(m_or_mask & ID_PWD_BIT)
		lcd_fb_fbmp(103, 28, BMP_MODE_OPTIONAL_7X7, 0, 1);
	else
		lcd_fb_rectangle(103, 28, 110, 35, 1, 0);

	if(m_and_mask & ID_FPC_BIT)
		lcd_fb_rectangle(6, 7, 13, 14, 1, 1);
	else if(m_or_mask & ID_FPC_BIT)
		lcd_fb_fbmp(6, 7, BMP_MODE_OPTIONAL_7X7, 0, 1);
	else
		lcd_fb_rectangle(6, 7, 13, 14, 1, 0);

	if(m_and_mask & ID_VEIN_BIT)
		lcd_fb_rectangle(6, 28, 13, 35, 1, 1);
	else if(m_or_mask & ID_VEIN_BIT)
		lcd_fb_fbmp(6, 28, BMP_MODE_OPTIONAL_7X7, 0, 1);
	else
		lcd_fb_rectangle(6, 28, 13, 35, 1, 0);

	if(m_and_mask & ID_FACE_BIT)
		lcd_fb_rectangle(6, 49, 13, 56, 1, 1);
	else if(m_or_mask & ID_FACE_BIT)
		lcd_fb_fbmp(6, 49, BMP_MODE_OPTIONAL_7X7, 0, 1);
	else		
		lcd_fb_rectangle(6, 49, 13, 56, 1, 0);
	
	lcd_fb_rectangle(95, 1, 96, 62, 1, 1);	
	
	lcd_fb_line(2, 20, 93, 20, 1);
	lcd_fb_line(1, 41, 93, 41, 1);
	lcd_fb_line(1, 62, 93, 62, 1);	
	lcd_fb_line(98, 20, 191, 20, 1);	

	lcd_fb_rectangle(72, 2, 89, 17, 1, 0);		
	lcd_fb_rectangle(72, 23, 89, 38, 1, 0);	
	lcd_fb_rectangle(72, 44, 89, 59, 1, 0);	
	lcd_fb_rectangle(169, 23, 186, 38, 1, 0);		
	
	for(int j = 0; j < 12; j++)
	{
		lcd_fb_line(98+j*8, 41, 101+j*8, 41, 1);		
	}

	lcd_fb_puthzs(17, 3, "Ö¸ÎÆ", 0, 1);
	lcd_fb_puthzs(17, 24, "Ö¸¾²Âö", 0, 1);
	lcd_fb_puthzs(17, 45, "ÈËÁ³", 0, 1);	
	lcd_fb_puthzs(114, 24, "ÃÜÂë", 0, 1);	
	
	lcd_fb_line(184, 2, 187, 2, 1);
	lcd_fb_rectangle(182, 3, 189, 16, 1, 0);
	lcd_fb_rectangle(183, 7, 188, 15, 1, 1);

	lcd_fb_rectangle(177, 5, 178, 16, 1, 1);
	lcd_fb_rectangle(174, 8, 175, 16, 1, 1);
	lcd_fb_rectangle(171, 11, 172, 16, 1, 1);	
	lcd_fb_rectangle(168, 14, 169, 16, 1, 1);

	show_wifi_rssi(-999, 1);

	lcd_fb_puts(135, 6, ASCII_5X8, ".", 0, 1);
	lcd_fb_puts(145, 6, ASCII_5X8, "C", 0, 1);
	lcd_fb_putn(125, 6, ASCII_5X8, 27, 0, 1);	
	lcd_fb_putn(140, 6, ASCII_5X8, 5, 0, 1);		

	for(int j = 0; j < 8; j++)
	{
		lcd_fb_line(107+j*10, 59, 115+j*10, 59, 1);		
	}

	main_item.index = 0;
	task_identify_enable(1);
	fpc_identify_enable(1);
	rtc_cnt_enable(0, RTC_FLAG_UNLOCK);
	
	return 0;
}


void main_idle(void)
{
	uint16_t temp = 0;
	temp = rtc_get_temperature();
	lcd_fb_putn((temp >= 100) ? 125 : 130, 6, ASCII_5X8, temp/10, 0, 1);	
	lcd_fb_putn(140, 6, ASCII_5X8, temp%10, 0, 1);	
	show_wifi_rssi(wifi_get_rssi(), 0);

}

struct Menu_Item_t main_item = {
	.index = 0,
	.setup = main_setup,
	.idle = main_idle,
	.int_process = main_int_process,
	.key_process = main_key_process,
	.password_process = main_password_process,
	.fpc_process = main_fpc_process,
	.vein_process = main_vein_process,
	.face_process = main_face_process,
	.rtc_process = main_rtc_process,
	.wifi_process = main_wifi_process,
	.sub_menu[0] = &unlock_item,
	.sub_menu[1] = &manage1_item,
};



