#include "stdlib.h"
#include "delay.h"
#include "rtc.h"


static Rtc_Int_f wakeup_int = NULL;
static Rtc_Int_f alarmA_int = NULL;


uint8_t rtc_init(void)
{
	RTC_InitTypeDef RTC_InitStructure;
	u16 retry=0X1FFF; 
	
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	PWR_BackupAccessCmd(ENABLE);
	
	if(RTC_ReadBackupRegister(RTC_BKP_DR0)!=0x5050)
	{
		RCC_LSEConfig(RCC_LSE_ON);
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
		{
			retry++;
			delay_ms(10);
		}
		if(retry==0)
			return 1;
			
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		RCC_RTCCLKCmd(ENABLE);

    	RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
    	RTC_InitStructure.RTC_SynchPrediv  = 0xFF;
    	RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;
    	RTC_Init(&RTC_InitStructure);
 
		//RTC_Set_Time(23,59,56,RTC_H12_AM);	//设置时间
		//RTC_Set_Date(14,5,5,6);		//设置日期
	 
		RTC_WriteBackupRegister(RTC_BKP_DR0, 0x5050);
	} 
 
	return 0;
}


void rtc_enable_wakeup(uint8_t on)
{ 	
	RTC_WakeUpCmd(on);
}


void rtc_init_wakeup(uint16_t cnt, Rtc_Int_f rtc_int)
{ 
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RTC_WakeUpCmd(DISABLE);
	
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
	
	RTC_SetWakeUpCounter(cnt);
	
	// RTC_ClearITPendingBit(RTC_IT_WUT);
  	// EXTI_ClearITPendingBit(EXTI_Line22);
	 
	 RTC_ITConfig(RTC_IT_WUT, ENABLE);
	RTC_WakeUpCmd( ENABLE);

	RTC_ClearITPendingBit(RTC_IT_WUT);
  	EXTI_ClearITPendingBit(EXTI_Line22);
	 
	//RTC_ITConfig(RTC_IT_WUT, ENABLE);

	wakeup_int = rtc_int;
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
 	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);
 
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn; 
 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 10;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
}


void rtc_set_wakeup(uint16_t cnt)
{ 
	RTC_WakeUpCmd(DISABLE);
	
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
	
	RTC_SetWakeUpCounter(cnt);

	RTC_ITConfig(RTC_IT_WUT, ENABLE);
	RTC_WakeUpCmd( ENABLE);
}


uint8_t rtc_set_time(uint8_t hour, uint8_t min, uint8_t sec, uint8_t ampm)
{
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;
	
	RTC_TimeTypeInitStructure.RTC_Hours = hour;
	RTC_TimeTypeInitStructure.RTC_Minutes = min;
	RTC_TimeTypeInitStructure.RTC_Seconds = sec;
	RTC_TimeTypeInitStructure.RTC_H12 = ampm;
	
	return RTC_SetTime(RTC_Format_BIN, &RTC_TimeTypeInitStructure);
}


uint8_t rtc_set_date(uint8_t year, uint8_t month, uint8_t date, uint8_t week)
{
	RTC_DateTypeDef RTC_DateTypeInitStructure;

	RTC_DateTypeInitStructure.RTC_Date = date;
	RTC_DateTypeInitStructure.RTC_Month = month;
	RTC_DateTypeInitStructure.RTC_WeekDay = week;
	RTC_DateTypeInitStructure.RTC_Year = year;
	return RTC_SetDate(RTC_Format_BIN, &RTC_DateTypeInitStructure);
}


void rtc_set_alarmA(uint8_t week, uint8_t hour, uint8_t min, uint8_t sec, Rtc_Int_f rtc_int)
{ 
	EXTI_InitTypeDef EXTI_InitStructure;
	RTC_AlarmTypeDef RTC_AlarmTypeInitStructure;
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE);
	
  	RTC_TimeTypeInitStructure.RTC_Hours = hour;
	RTC_TimeTypeInitStructure.RTC_Minutes = min;
	RTC_TimeTypeInitStructure.RTC_Seconds = sec;
	RTC_TimeTypeInitStructure.RTC_H12 = RTC_H12_AM;
  
	RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDay=week;
	RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDaySel=RTC_AlarmDateWeekDaySel_WeekDay;
	RTC_AlarmTypeInitStructure.RTC_AlarmMask=RTC_AlarmMask_None;
	RTC_AlarmTypeInitStructure.RTC_AlarmTime=RTC_TimeTypeInitStructure;
  	RTC_SetAlarm(RTC_Format_BIN,RTC_Alarm_A,&RTC_AlarmTypeInitStructure);
 
	RTC_ClearITPendingBit(RTC_IT_ALRA);
  	EXTI_ClearITPendingBit(EXTI_Line17);
	
	RTC_ITConfig(RTC_IT_ALRA,ENABLE);
	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);
	
	alarmA_int = rtc_int;

	EXTI_InitStructure.EXTI_Line = EXTI_Line17;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn; 
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 10;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
}



void RTC_WKUP_IRQHandler(void)
{    
	if(RTC_GetFlagStatus(RTC_FLAG_WUTF)==SET)
	{ 
		RTC_ClearFlag(RTC_FLAG_WUTF);
		if(wakeup_int != NULL)
			wakeup_int();
	}   
	EXTI_ClearITPendingBit(EXTI_Line22);								
}



void RTC_Alarm_IRQHandler(void)
{    
	if(RTC_GetFlagStatus(RTC_FLAG_ALRAF)==SET)
	{
		RTC_ClearFlag(RTC_FLAG_ALRAF);
		if(alarmA_int != NULL)
			alarmA_int();
	}   
	EXTI_ClearITPendingBit(EXTI_Line17);											 
}
