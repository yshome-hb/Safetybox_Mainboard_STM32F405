
#ifndef __FPC1020A_H__
#define __FPC1020A_H__

#include "sys.h"

#define FPC1020A_BUADRATE		19200

#define FPC1020A_STX			0xF5
#define FPC1020A_ETX			0xF5

#define FPC1020A_DATA_LEN		0x04

#define FPC1020A_CMD_ENROLL1  		0x01//���ָ�Ʋ���һ
#define FPC1020A_CMD_ENROLL2  		0x02//���ָ�Ʋ����
#define FPC1020A_CMD_ENROLL3  		0x03//���ָ�Ʋ�����
#define FPC1020A_CMD_DELETE  			0x04//ɾ��ָ�����ָ��
#define FPC1020A_CMD_CLEAR  			0x05//�������ָ��
#define FPC1020A_CMD_USERNUMB  		0x09//ȡ�û�����
#define FPC1020A_CMD_AUTHORITY  	0x0A//ȡ�û�
#define FPC1020A_CMD_IDENTIFY  		0x0B//1:1�ȶ�
#define FPC1020A_CMD_QUERY  			0x0C//1:N�ȶ�

#define FPC1020A_ACK_SUCCESS  		0x00	//�����ɹ�
#define FPC1020A_ACK_FAIL	  			0x01	//����ʧ��
#define FPC1020A_ACK_FULL	  			0x04	//ָ�����ݿ�����
#define FPC1020A_ACK_NOUSER   		0x05	//�޴��û�
#define FPC1020A_ACK_OCCUPIED   	0x06	//
#define FPC1020A_ACK_USER_EXIST 	0x07 	//�û��Ѵ���
#define FPC1020A_ACK_TIMEOUT  		0x08	//�ɼ���ʱ
#define FPC1020A_NAK 				0xFF 	//

enum fpc_step_wait_e
{
    FPC1020A_STEP_WAIT_STX = 0,
    FPC1020A_STEP_WAIT_CMD,
    FPC1020A_STEP_WAIT_DATA,
    FPC1020A_STEP_WAIT_CHK,
    FPC1020A_STEP_WAIT_ETX,
};


struct FPC_Protocol_Handle_t{
	uint8_t flag;
	uint8_t step;
	uint8_t index;
	uint8_t len;
	uint8_t cmd;
	uint8_t data[FPC1020A_DATA_LEN];
	uint8_t chk;
};


void fpc1020a_drv_init(void);
void fpc1020a_drv_deinit(void);
uint8_t fpc1020a_number(uint16_t *num, uint32_t timeout);
uint8_t fpc1020a_authority(uint16_t u_id, uint8_t *auth, uint32_t timeout);
uint8_t fpc1020a_clear(uint32_t timeout);
uint8_t fpc1020a_delete(uint16_t u_id, uint32_t timeout);
uint8_t fpc1020a_identify(uint16_t u_id, uint32_t timeout);
uint8_t fpc1020a_query(uint16_t *u_id, uint32_t timeout);
uint8_t fpc1020a_enroll(uint8_t step, uint16_t u_id, uint32_t timeout);


#endif

