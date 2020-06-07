#include "string.h"
#include "lcd192x64.h"
#include "device_info.h"
#include "rtc.h"
#include "device_config.h"
#include "message.h"
#include "rtc_task.h"
#include "activity_task.h"
#include "menu.h"


static uint8_t info_index = 0;

static void systeminfo_sensor(void)
{
	uint16_t temp = rtc_get_temperature();
	uint16_t bat = rtc_get_battery();

	lcd_fb_rectangle(1, 21, 191, 60, 0, 1);

	lcd_fb_puthzs(80, 2, "系统状态1", 0, 1);
	lcd_fb_puthzs(20, 2, "1/3", 0, 1);	
	
	lcd_fb_puthzs(4, 23, "电量:", 0, 1);
	lcd_fb_puthzs(4, 46, "温度:", 0, 1);
	lcd_fb_puts(84, 23, ASCII_8X16, "%", 0, 1);
	lcd_fb_puts(68, 46, ASCII_8X16, ".", 0, 1);
	lcd_fb_puts(84, 46, ASCII_8X16, "C", 0, 1);

	if(bat < 10)
		lcd_fb_putn(76, 23, ASCII_8X16, bat, 0, 1);
	else if(bat < 100)
		lcd_fb_putn(68, 23, ASCII_8X16, bat, 0, 1);
	else
		lcd_fb_putn(60, 23, ASCII_8X16, bat, 0, 1);	
	
	lcd_fb_putn((temp >= 100) ? 52 : 60, 46, ASCII_8X16, temp/10, 0, 1);	
	lcd_fb_putn(76, 46, ASCII_8X16, temp%10, 0, 1);	

}


static void systeminfo_time(void)
{
	uint8_t hour = 0;
	uint8_t min = 0;
	uint8_t sec = 0;
	uint8_t year = 0;
	uint8_t month = 0;
	uint8_t date = 0;

	rtc_get_time(&hour, &min, &sec);
	rtc_get_date(&year, &month, &date);

	lcd_fb_rectangle(1, 21, 191, 60, 0, 1);

	lcd_fb_puthzs(80, 2, "系统状态2", 0, 1);
	lcd_fb_puthzs(20, 2, "2/3", 0, 1);	
	
	lcd_fb_puthzs(4, 23, "日期:", 0, 1);
	lcd_fb_puts(46, 23, ASCII_8X16, "20", 0, 1);
	lcd_fb_puthzs(78, 23, "年", 0, 1);
	lcd_fb_puthzs(110, 23, "月", 0, 1);
	lcd_fb_puthzs(142, 23, "日", 0, 1);
	lcd_fb_puthzs(4, 46, "时间:", 0, 1);
	lcd_fb_puthzs(62, 46, "时", 0, 1);
	lcd_fb_puthzs(94, 46, "分", 0, 1);
	
	year -= 2;
	if(year < 10)
	{
		lcd_fb_puts(62, 23, ASCII_8X16, "0", 0, 1);
		lcd_fb_putn(70, 23, ASCII_8X16, year, 0, 1);
	}
	else
		lcd_fb_putn(62, 23, ASCII_8X16, year, 0, 1);

	if(month < 10)
	{
		lcd_fb_puts(94, 23, ASCII_8X16, "0", 0, 1);
		lcd_fb_putn(102, 23, ASCII_8X16, month, 0, 1);
	}
	else
		lcd_fb_putn(94, 23, ASCII_8X16, month, 0, 1);

	if(date < 10)
	{
		lcd_fb_puts(126, 23, ASCII_8X16, "0", 0, 1);
		lcd_fb_putn(134, 23, ASCII_8X16, date, 0, 1);
	}
	else
		lcd_fb_putn(126, 23, ASCII_8X16, date, 0, 1);

	if(hour < 10)
	{
		lcd_fb_puts(46, 46, ASCII_8X16, "0", 0, 1);
		lcd_fb_putn(54, 46, ASCII_8X16, hour, 0, 1);
	}
	else
		lcd_fb_putn(46, 46, ASCII_8X16, hour, 0, 1);

	if(min < 10)
	{
		lcd_fb_puts(78, 46, ASCII_8X16, "0", 0, 1);
		lcd_fb_putn(86, 46, ASCII_8X16, min, 0, 1);
	}
	else
		lcd_fb_putn(78, 46, ASCII_8X16, min, 0, 1);		
	
}


static void systeminfo_version(void)
{
	lcd_fb_rectangle(1, 21, 191, 60, 0, 1);

	lcd_fb_puthzs(80, 2, "系统状态3", 0, 1);
	lcd_fb_puthzs(20, 2, "3/3", 0, 1);	
	
	lcd_fb_puthzs(4, 23, "固件版本:", 0, 1);

	lcd_fb_puts(78, 23, ASCII_8X16, "V", 0, 1);
	lcd_fb_puts(94, 23, ASCII_8X16, ".", 0, 1);
	lcd_fb_putn(86, 23, ASCII_8X16, VERSION_MAJOR, 0, 1);
	lcd_fb_putn(102, 23, ASCII_8X16, VERSION_JUNIOR, 0, 1);
	//lcd_fb_puthzs(4, 46, "时间:", 0, 1);

}


uint16_t systeminfo_setup(uint16_t input)
{
	systeminfo_item.index = 0;	
	lcd_fb_clear(0);	

	lcd_fb_line(1, 20, 191, 20, 1);
	lcd_fb_fbmp(4, 4, BMP_MENU_BACK_12X12, 0, 1);

	info_index = 0;
	systeminfo_sensor();

	return 0;
}


uint16_t systeminfo_key_process(void *param, uint16_t input)
{
	uint8_t key_cmd = input>>8;
	uint8_t key_val = input&0xFF;
	Msg_Value_t sysi1send_msg = {0};

	if(key_cmd != 0xA5)
		return 0;

	if(key_val == '\b')
	{
		sysi1send_msg.cmd = MSG_CMD_PUB;				
		acitivy_send_msg(&sysi1send_msg, 1000);
	}	
	else if(key_val == 'u')
	{
		if(info_index > 0)
			info_index--;
	}
	else if(key_val == 'd')
	{
		if(info_index < 2)
			info_index++;
	}
	
	if(info_index == 0)
		systeminfo_sensor();
	else if(info_index == 1)
		systeminfo_time();
	else if(info_index == 2)
		systeminfo_version();

	return 0;
}


struct Menu_Item_t systeminfo_item = {
	.index = 0,
	.setup = systeminfo_setup,
	.int_process = NULL,
	.key_process = systeminfo_key_process,
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
