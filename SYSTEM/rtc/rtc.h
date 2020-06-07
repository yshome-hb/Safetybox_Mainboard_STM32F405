#ifndef __RTC_H__
#define __RTC_H__
	 
#include "sys.h" 

typedef void (*Rtc_Int_f)(void);

uint8_t rtc_init(void);
uint8_t rtc_set_time(uint8_t hour, uint8_t min, uint8_t sec);
uint8_t rtc_set_date(uint8_t year, uint8_t month, uint8_t date);
void rtc_get_time(uint8_t *hour, uint8_t *min, uint8_t *sec);
void rtc_get_date(uint8_t *year, uint8_t *month, uint8_t *date);
void rtc_enable_wakeup(uint8_t on);
void rtc_init_wakeup(uint16_t cnt, Rtc_Int_f rtc_int);
void rtc_set_wakeup(uint16_t cnt);
void rtc_set_alarmA(uint8_t week, uint8_t hour, uint8_t min, uint8_t sec, Rtc_Int_f rtc_int);


#endif
