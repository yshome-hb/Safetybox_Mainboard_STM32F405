#ifndef __RTC_TASK_H__
#define __RTC_TASK_H__

#include "sys.h"


enum rtc_cmd_e
{
	RTC_CMD_CLEAR = 1,
	RTC_CMD_SET,
	RTC_CMD_DECREASE,
};


enum rtc_flag_e
{
	RTC_FLAG_IDENTIY = 0,
	RTC_FLAG_UNLOCK,
	RTC_FLAG_FACE,
	RTC_FLAG_BEEP,
	RTC_FLAG_MAX = 8,	
};


typedef void (*timeout_cb_f)(void *xTimer);


uint16_t rtc_get_battery(void);
uint16_t rtc_get_temperature(void);
int32_t rtc_cnt_enable(uint8_t on, uint8_t flag);
void rtc_task_create(void *pvParameters);
void rtc_timeout_start(uint32_t tick, timeout_cb_f timeout_cb);
void rtc_timeout_stop(void);
void rtc_timeout_reset(void);


#endif
