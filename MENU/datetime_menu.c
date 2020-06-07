#include "string.h"
#include "lcd192x64.h"
#include "rtc.h"
#include "message.h"
#include "activity_task.h"
#include "menu.h"


const uint8_t DATETIME_MIN[6] = {0,  1,	 1,  0,  0,  0,};
const uint8_t DATETIME_MAX[6] = {99, 12, 31, 23, 59, 59,};
const uint8_t MONTH_MAX[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static uint8_t datetime[6] = {0};
static uint8_t datatime_index = 0;
static uint8_t time_set_flag = 0;


static void datetime_show_num(uint8_t index, uint8_t num, uint8_t flag)
{
	int32_t x = 0;
	int32_t y = 0;

	if(index == 0)
	{
		x = 62; y = 23;
	}
	else if(index == 1)
	{
		x = 94; y = 23;
	}
	else if(index == 2)
	{
		x = 126; y = 23;
	}
	else if(index == 3)
	{
		x = 46; y = 43;
	}
	else if(index == 4)
	{
		x = 78; y = 43;
	}
	else
	{
		x = 110; y = 43;
	}
		
	//lcd_fb_rectangle(x, y, x+16, y+16, 0, 1);		

	if(num < 10)
	{
		lcd_fb_puts(x, y, ASCII_8X16, "0", 0, 1);
		lcd_fb_putn(x+8, y, ASCII_8X16, num, 0, 1);
	}
	else
		lcd_fb_putn(x, y, ASCII_8X16, num, 0, 1);

	lcd_fb_rectangle(x, y, x+16, y+16, flag, 0);	
}


uint16_t datetime_key_process(void *param, uint16_t input)
{
	uint8_t key_cmd = input>>8;
	uint8_t key_val = input&0xFF;
	Msg_Value_t dtkmsend_msg = {0};

	if(key_cmd != 0xA5)
		return 0;

	if(key_val == 'u')
	{
		if(datetime[datatime_index] > DATETIME_MIN[datatime_index])
			datetime[datatime_index]--;
		else if(datatime_index == 2)
			datetime[datatime_index] = MONTH_MAX[datetime[1]-1];
		else 
			datetime[datatime_index] = DATETIME_MAX[datatime_index];

		time_set_flag = 1;
	}
	else if(key_val == 'd')
	{
		if(datatime_index == 2)
		{
			if(datetime[datatime_index] < MONTH_MAX[datetime[1]-1])
				datetime[datatime_index]++;
			else
				datetime[datatime_index] = DATETIME_MIN[datatime_index];
		}
		else
		{
			if(datetime[datatime_index] < DATETIME_MAX[datatime_index])
				datetime[datatime_index]++;
			else
				datetime[datatime_index] = DATETIME_MIN[datatime_index];
		}

		time_set_flag = 1;		
	}
	else if(key_val == '\n')
	{
		datetime_show_num(datatime_index, datetime[datatime_index], 0);
		datatime_index++;
		if(datatime_index >= 6)
			datatime_index = 0;
	}
	else if(key_val == '\b')
	{
		if(time_set_flag)
		{
			rtc_set_time(datetime[3], datetime[4], datetime[5]);
			rtc_set_date(datetime[0], datetime[1], datetime[2]);
		}

		dtkmsend_msg.cmd = MSG_CMD_PUB;				
		acitivy_send_msg(&dtkmsend_msg, 1000);
		datatime_index = 0;
	}

	datetime_show_num(datatime_index, datetime[datatime_index], 1);

	return 0;
}


uint16_t datetime_setup(uint16_t input)
{
	lcd_fb_rectangle(1, 21, 191, 60, 0, 1);

	lcd_fb_puthzs(80, 2, "时间设置", 0, 1);
	lcd_fb_puthzs(20, 2, "1/1", 0, 1);	

	rtc_get_time(datetime+3, datetime+4, datetime+5);	
	rtc_get_date(datetime, datetime+1, datetime+2);

	datetime[0] -= 2;

	lcd_fb_puts(46, 23, ASCII_8X16, "20", 0, 1);
	lcd_fb_puthzs(78, 23, "年", 0, 1);
	lcd_fb_puthzs(110, 23, "月", 0, 1);
	lcd_fb_puthzs(142, 23, "日", 0, 1);
	lcd_fb_puthzs(62, 43, "时", 0, 1);
	lcd_fb_puthzs(94, 43, "分", 0, 1);
	lcd_fb_puthzs(126, 43, "秒", 0, 1);
	
	datetime_show_num(0, datetime[0], 1);
	datetime_show_num(1, datetime[1], 0);
	datetime_show_num(2, datetime[2], 0);
	datetime_show_num(3, datetime[3], 0);
	datetime_show_num(4, datetime[4], 0);
	datetime_show_num(5, datetime[5], 0);

	time_set_flag = 0;

	return 0;
}


struct Menu_Item_t datetime_item = {
	.index = 0,
	.setup = datetime_setup,
	.int_process = NULL,
	.key_process = datetime_key_process,
	.password_process = NULL,
	.fpc_process = NULL,
	.vein_process = NULL,
	.face_process = NULL,
	.rtc_process = NULL,
    .pub_menu = &manage2_item,	
	.sub_menu[0] = NULL,
	.sub_menu[1] = NULL,
	.sub_menu[2] = NULL,
	.sub_menu[3] = NULL,
	.next_menu = NULL,
	.pre_menu = NULL,
};

