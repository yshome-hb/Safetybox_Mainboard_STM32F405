#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "input_output.h"
#include "sht2x.h"
#include "adc.h"
#include "message.h"
#include "activity_task.h"
#include "rtc_task.h"


#define BATTERY_CHANNEL ADC_Channel_14


TimerHandle_t 	RTC_Cnt_Loop_Handle;
static uint8_t cnt_flag = 0;
static uint8_t unlock_cnt = 0;
static uint8_t face_cnt = 0;
static uint8_t beep_cnt = 0;
static float temperature = 0;
static uint16_t battery_val = 0;


static void rtc_set_flag(uint8_t on, uint8_t flag)
{
	if(flag == RTC_FLAG_UNLOCK)
	{
		io_output_set(OUTPUT_UNLOCK, on);
		io_output_set(OUTPUT_LIGHT, on); 
		unlock_cnt = on ? 5 : 0;	
	}
	else if(flag == RTC_FLAG_FACE)
	{
		io_output_set(OUTPUT_POWER_FACE, on);
		face_cnt = on ? 10 : 0;
	}
	else if(flag == RTC_FLAG_BEEP)
	{
		io_output_set(OUTPUT_BEEP, on);
		beep_cnt = on ? 10 : 0;
	}
}



uint16_t rtc_get_battery()
{
	return battery_val;
}


uint16_t rtc_get_temperature()
{
	if(temperature > 0)
		return (uint16_t)(temperature*10);
	return 0;
} 


int32_t rtc_cnt_enable(uint8_t on, uint8_t flag)
{
	Msg_Value_t rtsend_msg = {0};
	
	rtc_set_flag(on, flag);
	rtsend_msg.cmd = MSG_CMD_RTC;
	if(on)
	{
		cnt_flag |= (((uint8_t)0x01) << flag);
		rtsend_msg.value.b_val[1] = RTC_CMD_SET;

	}
	else
	{
		cnt_flag &= (~(((uint8_t)0x01) << flag));
		rtsend_msg.value.b_val[1] = RTC_CMD_CLEAR;
	}

	rtsend_msg.value.b_val[0] = flag;
	
	return acitivy_send_msg(&rtsend_msg, 1000);
} 


static void rtc_cnt_loop(TimerHandle_t xTimer)
{	
  	uint16_t bat_raw = 0;		
		temperature = sht2x_temperature() - (float)5.0;

  	if(adc1_get_value(BATTERY_CHANNEL, &bat_raw))
  	{
		adc1_start_conv(BATTERY_CHANNEL);
		battery_val = bat_raw * 66 / 4096;
  	}


	if(cnt_flag)
	{
		Msg_Value_t rsend_msg = {0};
	
		rsend_msg.cmd = MSG_CMD_RTC;
		rsend_msg.value.b_val[1] = RTC_CMD_DECREASE;

		acitivy_send_msg(&rsend_msg, 1000);

		if(unlock_cnt > 0)
		{
			unlock_cnt--;
			if(unlock_cnt == 0)
			{
				io_output_set(OUTPUT_UNLOCK, 0);
				io_output_set(OUTPUT_LIGHT, 0); 			
				cnt_flag &= (~(((uint8_t)0x01) << RTC_FLAG_UNLOCK));
			}
		}

		if(face_cnt > 0)
		{
			face_cnt--;
			if(face_cnt == 0)
			{
				io_output_set(OUTPUT_POWER_FACE, 0);
				face_identify_enable(0);
				cnt_flag &= (~(((uint8_t)0x01) << RTC_FLAG_FACE));
			}
		}		

		if(beep_cnt > 0)
		{
			beep_cnt--;
			if(beep_cnt == 0)
			{
				io_output_set(OUTPUT_BEEP, 0);
				cnt_flag &= (~(((uint8_t)0x01) << RTC_FLAG_BEEP));
			}
		}

	}
}   


void rtc_task_create(void *pvParameters)
{

  	RTC_Cnt_Loop_Handle = xTimerCreate((const char*)"rtc_cnt_loop",
									(TickType_t	 )1000,
									(UBaseType_t )pdTRUE,
									(void*			 )2,
									(TimerCallbackFunction_t)rtc_cnt_loop); 																	

	xTimerStart(RTC_Cnt_Loop_Handle, 0);

	adc1_start_conv(BATTERY_CHANNEL);

	//sht2x_init(RH_12_T_14);
}


