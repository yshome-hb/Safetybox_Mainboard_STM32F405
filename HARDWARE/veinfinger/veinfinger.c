
#include "usart.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "veinfinger.h"

struct XG_Protocol_Handle_t xg_handle = {
	.flag = 0,
	.step = XG_STEP_WAIT_BB,
	.len = XG_DATA_LEN,
};

static SemaphoreHandle_t xg_sem;

static uint8_t xg_protocol_parse(uint8_t _input)
{
	BaseType_t xHigherPriorityTaskWoken;
	switch(xg_handle.step)
	{
			case XG_STEP_WAIT_BB:
			if(_input == XG_PREFIX_BB)
				xg_handle.step = XG_STEP_WAIT_AA;
			break;		
		
		case XG_STEP_WAIT_AA:
			if(_input == XG_PREFIX_AA)
				xg_handle.step = XG_STEP_WAIT_ADDR;
			break;
			
		case XG_STEP_WAIT_ADDR:
			xg_handle.addr = _input;
			xg_handle.chk =  0xBB + 0xAA + _input;
			xg_handle.step = XG_STEP_WAIT_CMD;			
			break;

		case XG_STEP_WAIT_CMD:
			xg_handle.cmd = _input;
			xg_handle.chk += _input;
			xg_handle.step = XG_STEP_WAIT_ENCODE;			
			break;

		case XG_STEP_WAIT_ENCODE:
			xg_handle.encode = _input;
			xg_handle.chk += _input;
			xg_handle.step = XG_STEP_WAIT_LEN;			
			break;		

		case XG_STEP_WAIT_LEN:
			//xg_handle.len = _input;
			xg_handle.chk += _input;
			xg_handle.index = 0;
			xg_handle.step = XG_STEP_WAIT_DATA;			
			break;	
		
		case XG_STEP_WAIT_DATA:
			xg_handle.data[xg_handle.index++] = _input;
			xg_handle.chk += _input;
			if(xg_handle.index >= xg_handle.len)
				xg_handle.step = XG_STEP_WAIT_CHK1;
			break;			

		case XG_STEP_WAIT_CHK1:
			xg_handle.chk ^= _input;
			xg_handle.step = XG_STEP_WAIT_CHK2;		
			break;

		case XG_STEP_WAIT_CHK2:
			xg_handle.chk ^= ((uint16_t)_input << 8);
			xg_handle.step = XG_STEP_WAIT_BB;
			if(xg_handle.chk == 0)
			{
				xSemaphoreGiveFromISR(xg_sem,&xHigherPriorityTaskWoken);
				portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
			}
			break;
		
		default:
			xg_handle.step = XG_STEP_WAIT_BB;				
			break;	
	}
	return 0;
}


static void xg_drv_send_data(uint8_t cmd, uint8_t *pdata, uint8_t len)
{
	uint8_t i=0;
	uint16_t sum = 0;
	
	if(len > XG_DATA_LEN)
		len = XG_DATA_LEN;
	
	uart4_send(XG_PREFIX_BB);	
	uart4_send(XG_PREFIX_AA);	
	uart4_send(0x00);
	uart4_send(cmd);	
	uart4_send(0x00);
	uart4_send(len);	
	
	sum = XG_PREFIX_BB + XG_PREFIX_AA + cmd + len;
	if(pdata != NULL && len > 0)
	{
		for(i = 0; i < len; i++)
		{
			uart4_send(pdata[i]);
			sum += pdata[i];
		}
	}
  for(; i < XG_DATA_LEN; i++)
	{
    uart4_send(0x00);
	}	
	
	uart4_send(sum&0xFF);	
	uart4_send(sum>>8);	
} 


void xg_drv_init()
{
	uart4_init(XG_BUADRATE, xg_protocol_parse);
	xg_sem = xSemaphoreCreateBinary();
}

/*
功能:复位重启设备
返回值:
XG_ERR_SUCCESS:成功
XG_ERR_FAIL:失败
*/

uint8_t xg_reboot(uint32_t timeout)
{
	BaseType_t err = pdFALSE;	
	
	xg_drv_send_data(XG_CMD_REBOOT, NULL, 0);
	err = xSemaphoreTake(xg_sem, timeout);
	if(err == pdFALSE)
	{
		return XG_ERR_NAK;
	}
	
	return xg_handle.data[0];
}

/*
功能:通过串口发送连接指令并接收回包
Timeout:接收超时，单位毫秒
返回值:
XG_ERR_SUCCESS:连接成功
XG_ERR_FAIL:连接失败
XG_ERR_INVALID_PWD:密码错误
*/

uint8_t xg_connect(uint32_t timeout)
{
	BaseType_t err = pdFALSE;	
	
	xg_drv_send_data(XG_CMD_CONNECTION, "00000000", 8);
	err = xSemaphoreTake(xg_sem, timeout);	
	if(err == pdFALSE)
	{
		return XG_ERR_NAK;
	}
	
	if(xg_handle.data[0] == XG_ERR_SUCCESS) 
		return XG_ERR_SUCCESS;
	else 
		return xg_handle.data[1];	
}


/*
功能:发送关闭连接指令
Timeout:接收超时，单位毫秒
返回值:
XG_ERR_SUCCESS:成功
XG_ERR_FAIL:失败
*/

uint8_t xg_close_connect(uint32_t timeout)
{
	BaseType_t err = pdFALSE;	
	
	xg_drv_send_data(XG_CMD_CLOSE_CONNECTION, NULL, 0);
	err = xSemaphoreTake(xg_sem, timeout);
	if(err == pdFALSE)
	{
		return XG_ERR_NAK;
	}
	
	if(xg_handle.data[0] == XG_ERR_SUCCESS) 
		return XG_ERR_SUCCESS;
	else 
		return xg_handle.data[1];	
}

/*
功能:控制V7的蓝灯
led:灯状态0=不亮 1=亮
返回值:
XG_ERR_SUCCESS:成功
XG_ERR_FAIL:失败
*/

uint8_t xg_blue_led(uint8_t on, uint32_t timeout)
{
	uint8_t bData[2] = { 0 };
	BaseType_t err = pdFALSE;		
	
	bData[0] = 0x07;
	bData[1] = on;
	xg_drv_send_data(XG_CMD_BLUE_LED, bData, 2);
	err = xSemaphoreTake(xg_sem, timeout);
	if(err == pdFALSE)
	{
		return XG_ERR_NAK;
	}
	
	if(xg_handle.data[0] == XG_ERR_SUCCESS) 
		return XG_ERR_SUCCESS;
	else 
		return xg_handle.data[1];	
}


/*
功能:获取登记信息
pUserNum:已经登记的用户数
pUserMax:最大用户数
返回值:
XG_ERR_SUCCESS:获取成功
XG_ERR_FAIL:获取失败
*/

uint8_t xg_get_num(uint16_t* p_num, uint16_t* p_max, uint32_t timeout)
{
	BaseType_t err = pdFALSE;	
	
	xg_drv_send_data(XG_CMD_GET_ENROLL_INFO, NULL, 0);
	err = xSemaphoreTake(xg_sem, timeout);
	if(err == pdFALSE)
	{
		return XG_ERR_NAK;
	}
	
	if(xg_handle.data[0] == XG_ERR_SUCCESS)
	{
		*p_num = xg_handle.data[1] + (xg_handle.data[2]<<8);
		*p_max = xg_handle.data[9] + (xg_handle.data[10]<<8);	
	}
	
	
	return xg_handle.data[0];	
}


uint8_t xg_clear_all(uint32_t timeout)
{
	uint16_t user_num = 0;
	uint16_t user_max = 0;	
	uint32_t clear_time = 0;
	BaseType_t err = pdFALSE;	

	//先获取登记有多少用户,估算一下大概需要多长的清除时间
	err = xg_get_num(&user_num, &user_max, timeout);
	if(err == XG_ERR_NAK)
	{
		return XG_ERR_NAK;
	}	
	
	clear_time = 1000 + user_num*300;
	if(timeout < clear_time)
		timeout = clear_time;
	
	xg_drv_send_data(XG_CMD_CLEAR_ALL_ENROLL, NULL, 0);	
	err = xSemaphoreTake(xg_sem, timeout);
	if(err == pdFALSE)
	{
		return XG_ERR_NAK;
	}
	
	if(xg_handle.data[0] == XG_ERR_SUCCESS) 
		return XG_ERR_SUCCESS;
	else 
		return xg_handle.data[1];	
}

/*
功能:清除指定用户数据
UserID:清除指定用户ID
返回值:
XG_ERR_SUCCESS:清除成功
XG_ERR_FAIL:清除失败
*/

uint8_t xg_clear_id(uint16_t u_id, uint32_t timeout)
{
	uint8_t bData[2] = { 0 };
	BaseType_t err = pdFALSE;		

	bData[0] = u_id&0xFF;
	bData[1] = (u_id>>8)&0xFF;	
	
	xg_drv_send_data(XG_CMD_CLEAR_ENROLL, bData, 2);		
	err = xSemaphoreTake(xg_sem, timeout);
	if(err == pdFALSE)
	{
		return XG_ERR_NAK;
	}
	
	if(xg_handle.data[0] == XG_ERR_SUCCESS) 
		return XG_ERR_SUCCESS;
	else 
		return xg_handle.data[1];	
}

/*
功能:获取指定用户DI的登记信息
UserID:指定ID
返回值:
0:此用户无登记
> 0:此用户已登记
*/
uint8_t xg_get_info(uint16_t u_id, uint8_t* info, uint32_t timeout)
{
	uint8_t bData[2] = { 0 };
	BaseType_t err = pdFALSE;		

	bData[0] = u_id&0xFF;
	bData[1] = (u_id>>8)&0xFF;	
	
	xg_drv_send_data(XG_CMD_GET_ID_INFO, bData, 2);	
	err = xSemaphoreTake(xg_sem, timeout);
	if(err == pdFALSE)
	{
		return XG_ERR_NAK;
	}
	
	if(xg_handle.data[0] == XG_ERR_SUCCESS)
	{
		*info = xg_handle.data[1];
		return XG_ERR_SUCCESS;
	}
	else 
		return xg_handle.data[1];		
}

/*
功能:发送验证指令并获取返回的验证结果
pUserID:返回验证成功用户ID的指针
返回值:
XG_ERR_SUCCESS:验证成功，验证成功的用户ID通过pUserID返回
XG_ERR_NO_VEIN:没有成功采集指静脉，可能手指没放好
*/

void xg_verify_send()
{
	xg_drv_send_data(XG_CMD_VERIFY, NULL, 4);	
}


uint8_t xg_verify_wait(uint16_t* p_id, uint32_t timeout)
{
	BaseType_t err = pdFALSE;	
	err = xSemaphoreTake(xg_sem, timeout);
	if(err == pdFALSE)
	{
		return XG_ERR_NAK;
	}
	
	if(xg_handle.data[0] == XG_ERR_SUCCESS) 
	{
		*p_id = xg_handle.data[1] + (xg_handle.data[2]<<8);
		return XG_ERR_SUCCESS;
	}
	else if(xg_handle.data[0] == XG_ERR_FAIL)
	{
		return xg_handle.data[1];
	}
	else
	{
		return xg_handle.data[0];	
	}
}


/*
功能:发送登记指令增加指静脉用户
UserID:指定登记用户的ID
返回值:
XG_ERR_SUCCESS:登记成功
*/
void xg_add_send(uint16_t u_id, uint8_t cnt, uint8_t retries)
{
	uint8_t bData[16] = { 0 };

	bData[0] = u_id&0xff;
	bData[1] = (u_id>>8)&0xff;
	bData[2] = 0;
	bData[3] = 0;
	bData[4] = 0;
	bData[5] = cnt; //采集成功的次数，只要采集了3次成功就登记成功
	bData[10] = retries; //如果采集不成功一共可重试多少次

	xg_drv_send_data(XG_CMD_ENROLL, bData, 12);
}

uint8_t xg_add_wait(uint32_t timeout)
{
	BaseType_t err = pdFALSE;	

	err = xSemaphoreTake(xg_sem, timeout);	
	if(err == pdFALSE)
	{
		return XG_ERR_NAK;
	}
	
	if(xg_handle.data[0] == XG_ERR_SUCCESS) 
	{
		return XG_ERR_SUCCESS;
	}
	else if(xg_handle.data[0] == XG_ERR_FAIL)
	{
		return xg_handle.data[1];
	}
	else
	{
		return xg_handle.data[0];	
	}
}

/*
功能:获取空ID，也就是没有登记的ID
pUserID:返回可登记ID的指针
返回值:
XG_ERR_SUCCESS:接收成功
XG_ERR_FAIL:通讯错误
XG_ERR_NO_NULL_ID:无空ID
*/

uint8_t xg_get_nullid(uint16_t* u_id, uint32_t timeout)
{
	BaseType_t err = pdFALSE;		
	
	xg_drv_send_data(XG_CMD_GET_EMPTY_ID, NULL, 0);	
	err = xSemaphoreTake(xg_sem, timeout);
	if(err == pdFALSE)
	{
		return XG_ERR_NAK;
	}
	
	if(xg_handle.data[0] == XG_ERR_SUCCESS)
	{
		*u_id = xg_handle.data[1] + (xg_handle.data[2]<<8);
		return XG_ERR_SUCCESS;
	}
	else 
		return xg_handle.data[1];			
}

#if 0
/*
功能:获取设置信息
Timeout:接收超时，单位毫秒
返回值:
XG_ERR_SUCCESS:成功
XG_ERR_FAIL:失败
*/

UINT8 GetDevSetting(int Timeout)
{
	UINT8 ret = 0;
	UINT8 bCmd = 0;
	UINT8 bData[16] = { 0 };

	SendPack(XG_CMD_GET_SYSTEM_INFO, NULL, 0);
	gUartByte = 0; //清除串口接收缓存
	ret = RecvPack(NULL, bData, Timeout);
	if(ret == XG_ERR_SUCCESS)
	{
		if(bData[0] == XG_ERR_SUCCESS) 
		{
			int iBaud[5] = {9600, 19200, 38400, 57600, 115200};
			UINT8 Mver = bData[1];
			UINT8 Sver = bData[2];
			UINT8 DevID = bData[3];
			int Baud = iBaud[bData[4]];
			UINT8 Securty = bData[5];
			UINT8 Timeout = bData[6];
			UINT8 DupCheck = bData[7];
			UINT8 SameFingerCheck = bData[8];
			UINT8 Volume = bData[11]; //0-16,>16语音关闭
			return XG_ERR_SUCCESS;
		}
		else return bData[1];
	}
	return XG_ERR_FAIL;
}

/*
功能:恢复出厂设置
Timeout:接收超时，单位毫秒
返回值:
XG_ERR_SUCCESS:成功
XG_ERR_FAIL:失败
*/

UINT8 SetDevFactory(int Timeout)
{
	UINT8 ret = 0;
	UINT8 bData[16] = { 0 };

	SendPack(XG_CMD_FACTORY_SETTING, NULL, 0);
	gUartByte = 0; //清除串口接收缓存
	ret = RecvPack(NULL, bData, Timeout);
	if(ret == XG_ERR_SUCCESS)
	{
		if(bData[0] == XG_ERR_SUCCESS) return XG_ERR_SUCCESS;
		else return bData[1];
	}
	return XG_ERR_FAIL;
}

/*
功能:设置设备编号
bDevID:设备编号
返回值:
XG_ERR_SUCCESS:成功
XG_ERR_FAIL:失败
*/

UINT8 SetDevID(UINT8 bDevID)
{
	UINT8 ret = 0;
	UINT8 bData[16] = { 0 };

	bData[0] = bDevID;
	SendPack(XG_CMD_SET_DEVICE_ID, bData, 1);
	gUartByte = 0;
	ret = RecvPack(NULL, bData, 1000);
	if(ret == XG_ERR_SUCCESS)
	{
		if(bData[0] == XG_ERR_SUCCESS)
		{
			return XG_ERR_SUCCESS;
		}
		else return bData[1];
	}
	return XG_ERR_FAIL;
}

/*
功能:设置波特率
bBaud:波特率,0=9600,1=19200,2=38400,3=57600,4-115200
返回值:
XG_ERR_SUCCESS:成功
XG_ERR_FAIL:失败
*/

UINT8 SetDevBaud(UINT8 bBaud)
{
	UINT8 ret = 0;
	UINT8 bData[16] = { 0 };

	bData[0] = bBaud;
	SendPack(XG_CMD_SET_BAUDRATE, bData, 1);
	gUartByte = 0;
	ret = RecvPack(NULL, bData, 1000);
	if(ret == XG_ERR_SUCCESS)
	{
		if(bData[0] == XG_ERR_SUCCESS)
		{
			return XG_ERR_SUCCESS;
		}
		else return bData[1];
	}
	return XG_ERR_FAIL;
}

/*
功能:设置安全等级
bSecurity:安全等级,0,1,2
返回值:
XG_ERR_SUCCESS:成功
XG_ERR_FAIL:失败
*/

UINT8 SetDevSecurity(UINT8 bSecurity)
{
	UINT8 ret = 0;
	UINT8 bData[16] = { 0 };

	bData[0] = bSecurity;
	SendPack(XG_CMD_SET_SECURITYLEVEL, bData, 1);
	gUartByte = 0;
	ret = RecvPack(NULL, bData, 1000);
	if(ret == XG_ERR_SUCCESS)
	{
		if(bData[0] == XG_ERR_SUCCESS)
		{
			return XG_ERR_SUCCESS;
		}
		else return bData[1];
	}
	return XG_ERR_FAIL;
}

/*
功能:设置手指检测超时
bTimeout:1-255秒
返回值:
XG_ERR_SUCCESS:成功
XG_ERR_FAIL:失败
*/

UINT8 SetDevCheckFingerTimeout(UINT8 bTimeout)
{
	UINT8 ret = 0;
	UINT8 bData[16] = { 0 };

	bData[0] = bTimeout;
	SendPack(XG_CMD_SET_TIMEOUT, bData, 1);
	gUartByte = 0;
	ret = RecvPack(NULL, bData, 1000);
	if(ret == XG_ERR_SUCCESS)
	{
		if(bData[0] == XG_ERR_SUCCESS)
		{
			return XG_ERR_SUCCESS;
		}
		else return bData[1];
	}
	return XG_ERR_FAIL;
}

/*
功能:设置重复登记检查
bCheck:0不检查 1检查
返回值:
XG_ERR_SUCCESS:成功
XG_ERR_FAIL:失败
*/

UINT8 SetDevDupCheck(UINT8 bCheck)
{
	UINT8 ret = 0;
	UINT8 bData[16] = { 0 };

	bData[0] = bCheck;
	SendPack(XG_CMD_SET_DUP_CHECK, bData, 1);
	gUartByte = 0;
	ret = RecvPack(NULL, bData, 1000);
	if(ret == XG_ERR_SUCCESS)
	{
		if(bData[0] == XG_ERR_SUCCESS)
		{
			return XG_ERR_SUCCESS;
		}
		else return bData[1];
	}
	return XG_ERR_FAIL;
}

/*
功能:设置登记时是否同一根手指检查
bCheck:0不检查 1检查
返回值:
XG_ERR_SUCCESS:成功
XG_ERR_FAIL:失败
*/

UINT8 SetDevSameFingerCheck(UINT8 bCheck)
{
	UINT8 ret = 0;
	UINT8 bData[16] = { 0 };

	bData[0] = bCheck;
	SendPack(XG_CMD_SET_SAME_FV, bData, 1);
	gUartByte = 0;
	ret = RecvPack(NULL, bData, 1000);
	if(ret == XG_ERR_SUCCESS)
	{
		if(bData[0] == XG_ERR_SUCCESS)
		{
			return XG_ERR_SUCCESS;
		}
		else return bData[1];
	}
	return XG_ERR_FAIL;
}

/*
功能:检测手指状态
返回值:
0:无手指
1:有手指
*/

UINT8 CheckFinger()
{
	UINT8 ret = 0;
	UINT8 bData[16] = { 0 };

	SendPack(XG_CMD_FINGER_STATUS, NULL, 0);
	gUartByte = 0;
	ret = RecvPack(NULL, bData, 1000);
	if(ret == XG_ERR_SUCCESS)
	{
		if(bData[0] == XG_ERR_SUCCESS)
		{
			return bData[1];
		}
	}
	return 0;
}

/*
功能:获取设备序列号
pSN:返回设备序列号
返回值:
XG_ERR_SUCCESS:成功
XG_ERR_FAIL:错误
*/

UINT8 GetDevSN(UINT8* pSN)
{
	UINT8 ret = 0;
	UINT8 bData[16] = { 0 };
	SendPack(XG_CMD_GET_DUID, NULL, 0);
	gUartByte = 0;
	ret = RecvPack(NULL, bData, 1000);
	if(ret == XG_ERR_SUCCESS)
	{
		if(bData[0] == XG_ERR_SUCCESS)
		{
			if(pSN) memcpy(pSN, bData[1], 14);
			return XG_ERR_SUCCESS;
		}
		else return bData[1];
	}
	return XG_ERR_FAIL;
}


/*
功能:指令开门
UserId:以那个用户ID的身份去开门
返回值:
XG_ERR_SUCCESS:成功
XG_ERR_FAIL:失败
*/

UINT8 DevOpenDoor(UINT8 UserId)
{
	UINT8 ret = 0;
	UINT8 bData[16] = { 0 };

	bData[0] = UserId%256;
	bData[1] = UserId/256;
	SendPack(XG_CMD_OPEN_DOOR, bData, 2);
	gUartByte = 0;
	ret = RecvPack(NULL, bData, 1000); //异步的话此超时要延长，具体时间根据语音的长短
	if(ret == XG_ERR_SUCCESS)
	{
		if(bData[0] == XG_ERR_SUCCESS)
		{
			return XG_ERR_SUCCESS;
		}
		else return bData[1];
	}
	return XG_ERR_FAIL;
}

/*
功能:修改设备时间

返回值:
XG_ERR_SUCCESS:成功
XG_ERR_FAIL:失败
*/

UINT8 SetDevDateTime(UINT16 year, UINT8 mon, UINT8 day, UINT8 hour, UINT8 min, UINT8 sec)
{
	UINT8 ret = 0;
	UINT8 bData[16] = { 0 };

	bData[0] = year - 2000;
	bData[1] = mon;
	bData[2] = day;
	bData[3] = hour;
	bData[4] = min;
	bData[5] = sec;
	SendPack(XG_CMD_SET_DATETIME, bData, 6);
	gUartByte = 0;
	ret = RecvPack(NULL, bData, 1000);
	if(ret == XG_ERR_SUCCESS)
	{
		if(bData[0] == XG_ERR_SUCCESS)
		{
			return XG_ERR_SUCCESS;
		}
		else return bData[1];
	}
	return XG_ERR_FAIL;
}



#endif
