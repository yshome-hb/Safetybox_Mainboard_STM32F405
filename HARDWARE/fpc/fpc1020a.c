#include "usart.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "fpc1020a.h"

#define BUF_N 	5

static struct FPC_Protocol_Handle_t fpc_handle = {
	.flag = 0,
	.step = FPC1020A_STEP_WAIT_STX,
	.index = 0,
	.len = FPC1020A_DATA_LEN,
	.cmd = 0,
	.chk = 0,
};

static SemaphoreHandle_t fpc_sem;

static uint8_t fpc1020a_protocol_parse(uint8_t _input)
{
	BaseType_t xHigherPriorityTaskWoken;
	switch(fpc_handle.step)
	{
		case FPC1020A_STEP_WAIT_STX:
			if(_input == FPC1020A_STX)
				fpc_handle.step = FPC1020A_STEP_WAIT_CMD;
			break;
			
		case FPC1020A_STEP_WAIT_CMD:
			fpc_handle.cmd = _input;
			fpc_handle.index = 0;
			fpc_handle.chk = _input;
			fpc_handle.step = FPC1020A_STEP_WAIT_DATA;			
			break;
			
		case FPC1020A_STEP_WAIT_DATA:
			fpc_handle.data[fpc_handle.index++] = _input;
			fpc_handle.chk ^= _input;
			if(fpc_handle.index >= fpc_handle.len)
				fpc_handle.step = FPC1020A_STEP_WAIT_CHK;
			break;			
			
		case FPC1020A_STEP_WAIT_CHK:
			fpc_handle.chk ^= _input;
			fpc_handle.step = FPC1020A_STEP_WAIT_ETX;		
			break;
			
		case FPC1020A_STEP_WAIT_ETX:
			fpc_handle.flag = (_input == FPC1020A_ETX && fpc_handle.chk == 0x00);	
			fpc_handle.step = FPC1020A_STEP_WAIT_STX;
			if(fpc_handle.flag)
			{
				xSemaphoreGiveFromISR(fpc_sem,&xHigherPriorityTaskWoken);
				portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
			}
			return fpc_handle.flag;
		
		default:
			fpc_handle.step = FPC1020A_STEP_WAIT_STX;				
			break;	
	}
	return 0;
}

static BaseType_t fpc1020a_drv_send_data(uint8_t *pdata, uint8_t len, uint32_t timeout)
{
	uint8_t i=0;
	uint8_t bcc = 0;
	
	uart5_send(FPC1020A_STX);	
  for(i = 0; i < len; i++)
	{
    uart5_send(pdata[i]);
		bcc ^= pdata[i];
	}
	uart5_send(bcc);	
	uart5_send(FPC1020A_ETX);	
	
	return xSemaphoreTake(fpc_sem, timeout);
} 


void fpc1020a_drv_init()
{
	uart5_init(FPC1020A_BUADRATE, fpc1020a_protocol_parse);
	fpc_sem = xSemaphoreCreateBinary();
}


/*******************************************************************************
**读取用户总数
**
**参数?N
*******************************************************************************/
uint8_t fpc1020a_number(uint16_t *num, uint32_t timeout)
{
  uint8_t buf[BUF_N] = {0};
  BaseType_t err = pdFALSE;	
	
	buf[0] = FPC1020A_CMD_USERNUMB;
	err = fpc1020a_drv_send_data(buf, BUF_N, timeout);
	if(err == pdFALSE)
	{
		return FPC1020A_NAK;
	}
	
	if(fpc_handle.data[2] == FPC1020A_ACK_SUCCESS)
	{
		*num = ((uint16_t)fpc_handle.data[0]<<8)|fpc_handle.data[1];
	}
	
	return fpc_handle.data[2];
}

/*******************************************************************************
**读取用户总数
**
**参数?N
*******************************************************************************/
uint8_t fpc1020a_authority(uint16_t u_id, uint8_t *auth, uint32_t timeout)
{
  uint8_t buf[BUF_N] = {0};
  BaseType_t err = pdFALSE;	
	
	buf[0] = FPC1020A_CMD_AUTHORITY;
  buf[1] = u_id>>8;
  buf[2] = u_id&0xff;
  buf[3] = 0x00;
  buf[4] = 0x00;	
	err = fpc1020a_drv_send_data(buf, BUF_N, timeout);
	if(err == pdFALSE)
	{
		return FPC1020A_NAK;
	}
	
	if(fpc_handle.data[2] < FPC1020A_ACK_NOUSER)
	{
		*auth = fpc_handle.data[2];
		return FPC1020A_ACK_SUCCESS;
	}

	return fpc_handle.data[2];
}

/*******************************************************************************
**功能：清空 flash 指纹库
**参数：
**返回：无
*******************************************************************************/
uint8_t fpc1020a_clear(uint32_t timeout)
{
  uint8_t buf[BUF_N] = {0};
  BaseType_t err = pdFALSE;		
	
	buf[0] = FPC1020A_CMD_CLEAR;
  err = fpc1020a_drv_send_data(buf, BUF_N, timeout);
	if(err == pdFALSE)
	{
		return FPC1020A_NAK;
	}
	
	return fpc_handle.data[2];	
}

/*******************************************************************************
**功能：删除指定编号指纹
**参数：u_id
**返回：void
*******************************************************************************/
uint8_t fpc1020a_delete(uint16_t u_id, uint32_t timeout)
{
  uint8_t buf[BUF_N] = {0};
  BaseType_t err = pdFALSE;	
	
	buf[0] = FPC1020A_CMD_DELETE;	
  buf[1] = u_id>>8;
  buf[2] = u_id&0xff;
  buf[3] = 0x00;
  buf[4] = 0x00;
	
  err = fpc1020a_drv_send_data(buf, BUF_N, timeout);
	if(err == pdFALSE)
	{
		return FPC1020A_NAK;
	}
	
	return fpc_handle.data[2];		
}

/*******************************************************************************
**功能：1:1比对
**参数：u_id
**返回：void
*******************************************************************************/
uint8_t fpc1020a_identify(uint16_t u_id, uint32_t timeout)
{
  uint8_t buf[BUF_N] = {0};
  BaseType_t err = pdFALSE;		
	
	buf[0] = FPC1020A_CMD_IDENTIFY;		
  buf[1] = u_id>>8;
  buf[2] = u_id&0xff;
  buf[3] = 0x00;
  buf[4] = 0x00;
	
  err = fpc1020a_drv_send_data(buf, BUF_N, timeout);
	if(err == pdFALSE)
	{
		return FPC1020A_NAK;
	}
	
	return fpc_handle.data[2];		
}

/*******************************************************************************
**功能：以CharBuffer1 或CharBuffer2 中的特征文件搜索整个或部分指纹库
**参数：
**返回：无
*******************************************************************************/
uint8_t fpc1020a_query(uint16_t *u_id, uint32_t timeout)
{
  uint8_t buf[BUF_N] = {0};
	BaseType_t err = pdFALSE;		
	
	buf[0] = FPC1020A_CMD_QUERY;		
  err = fpc1020a_drv_send_data(buf, BUF_N, timeout);
	if(err == pdFALSE)
	{
		return FPC1020A_NAK;
	}
	
	if(fpc_handle.data[2] < FPC1020A_ACK_NOUSER)
	{
		*u_id = ((uint16_t)fpc_handle.data[0]<<8) | fpc_handle.data[1];
		return FPC1020A_ACK_SUCCESS;
	}
	
	return fpc_handle.data[2];	
}

/*******************************************************************************
**注册指纹
**输入两次指纹注册一个指纹模板
**参数：UserID 指纹号
*******************************************************************************/
uint8_t fpc1020a_enroll(uint8_t step, uint16_t u_id, uint32_t timeout)
{
  uint8_t buf[BUF_N] = {0};
	BaseType_t err = pdFALSE;	
	
  if(step < FPC1020A_CMD_ENROLL1 || step >> FPC1020A_CMD_ENROLL3)
		return FPC1020A_NAK;
	
	buf[0] = step;			
	buf[1] = u_id>>8;
  buf[2] = u_id&0xff;
  buf[3] = 0x01;
  buf[4] = 0x00;
  err = fpc1020a_drv_send_data(buf, BUF_N, timeout);
	if(err == pdFALSE)
	{
		return FPC1020A_NAK;
	}
	
	return fpc_handle.data[2];
}

