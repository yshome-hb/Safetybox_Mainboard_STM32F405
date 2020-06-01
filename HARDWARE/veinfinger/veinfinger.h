
#ifndef  __VEIN_FINGER_H__
#define  __VEIN_FINGER_H__

#include "sys.h"

#define XG_BUADRATE		57600

#define XG_DATA_LEN		16

#define XG_PREFIX_BB  0xBB
#define XG_PREFIX_AA  0xAA


enum xg_step_wait_e
{
		XG_STEP_WAIT_BB = 0,
    XG_STEP_WAIT_AA,
    XG_STEP_WAIT_ADDR,
		XG_STEP_WAIT_CMD,
    XG_STEP_WAIT_ENCODE,
		XG_STEP_WAIT_LEN,
    XG_STEP_WAIT_DATA,
		XG_STEP_WAIT_CHK1,
    XG_STEP_WAIT_CHK2,
};


struct XG_Protocol_Handle_t{
	uint8_t flag;
	uint8_t step;
	uint8_t index;
	uint8_t addr;
	uint8_t cmd;
	uint8_t encode;	
	uint8_t len;
	uint8_t data[XG_DATA_LEN];
	uint16_t chk;
};


//ָ���б�
#define XG_CMD_CONNECTION             0x01 //�����豸
#define XG_CMD_CLOSE_CONNECTION       0x02 //�ر�����
#define XG_CMD_GET_SYSTEM_INFO        0x03 //��ȡ�汾�ź�������Ϣ
#define XG_CMD_FACTORY_SETTING        0x04 //�ָ���������
#define XG_CMD_SET_DEVICE_ID          0x05 //�����豸���0-255
#define XG_CMD_SET_BAUDRATE           0x06 //���ò�����0-4
#define XG_CMD_SET_SECURITYLEVEL      0x07 //���ð�ȫ�ȼ�0-4
#define XG_CMD_SET_TIMEOUT            0x08 //���õȴ���ָ���볬ʱ1-255��
#define XG_CMD_SET_DUP_CHECK          0x09 //�����ظ���¼���0-1
#define XG_CMD_SET_PASSWORD           0x0A //����ͨ������
#define XG_CMD_CHECK_PASSWORD         0x0B //��������Ƿ���ȷ
#define XG_CMD_REBOOT                 0x0C //��λ�����豸
#define XG_CMD_SET_SAME_FV            0x0D //�Ǽǵ�ʱ�����Ƿ�Ϊͬһ����ָ
#define XG_CMD_SET_USB_MODE           0x0E //����USB����ģʽ 
#define XG_CMD_GET_DUID               0x0F //��ȡ�豸���к� 
#define XG_CMD_FINGER_STATUS          0x10 //�����ָ����״̬
#define XG_CMD_CLEAR_ENROLL           0x11 //���ָ��ID��¼����
#define XG_CMD_CLEAR_ALL_ENROLL       0x12 //�������ID��¼����
#define XG_CMD_GET_EMPTY_ID           0x13 //��ȡ�գ��޵�¼���ݣ�ID
#define XG_CMD_GET_ENROLL_INFO        0x14 //��ȡ�ܵ�¼�û�����ģ����
#define XG_CMD_GET_ID_INFO            0x15 //��ȡָ��ID��¼��Ϣ
#define XG_CMD_ENROLL                 0x16 //ָ��ID��¼
#define XG_CMD_VERIFY                 0x17 //1:1��֤��1:Nʶ��
#define XG_CMD_READ_DATA              0x20 //���豸��ȡ����
#define XG_CMD_WRITE_DATA             0x21 //д�����ݵ��豸
#define XG_CMD_READ_ENROLL            0x22 //��ȡָ��ID��¼����
#define XG_CMD_WRITE_ENROLL           0x23 //д�루���ǣ�ָ��ID��¼����
#define XG_CMD_GET_CHARA              0x28 //��ȡ��ǰ�ɼ������� 
#define XG_CMD_READ_USER_DATA         0x29 //���û���չ�洢����ȡ���ݣ����4K
#define XG_CMD_WRITE_USER_DATA        0x2A //д�����ݵ��û���չ�洢�������4K
#define XG_CMD_GET_SYS_SET            0x2E //��ȡD700�豸ϵͳ�������ݽṹ 
#define XG_CMD_SET_SYS_SET            0x2F //����D700ϵͳ�������ݽṹ 
#define XG_CMD_OPEN_DOOR              0x32 //����
#define XG_CMD_READ_LOG               0x33 //��ȡ�Ž�������־
#define XG_CMD_SET_DEVNAME            0x34 //�����豸����
#define XG_CMD_GET_DATETIME           0x35 //��ȡ�Ž�ʵʱʱ��
#define XG_CMD_SET_DATETIME           0x36 //�����Ž�ʵʱʱ��
#define XG_CMD_ENROLL_EXT             0x38 //��չ�����Ǽ�
#define XG_CMD_VERIFY_EXT             0x39 //��չ������֤
#define XG_CMD_DEL_LOG                0x3a //ɾ���Ž���־
#define XG_CMD_PLAY_VOICE             0x3b //��������
#define XG_CMD_BLUE_LED								0x4b
#define XG_CMD_GET_USER_BATCH         0x51 //������ȡ�û���Ϣ 
#define XG_CMD_SET_USER_BATCH         0x52 //����д���û���Ϣ 
#define XG_CMD_READ_CARDNO            0x53 //D700�����û�ˢ������ȡ����

/******************************�������****************************************/
#define	XG_ERR_SUCCESS                0x00 //�����ɹ�
#define	XG_ERR_FAIL                   0x01 //����ʧ��
#define XG_ERR_COM                    0x02 //ͨѶ����
#define XG_ERR_DATA                   0x03 //����У�����
#define XG_ERR_INVALID_PWD            0x04 //�������
#define XG_ERR_INVALID_PARAM          0x05 //��������
#define XG_ERR_INVALID_ID             0x06 //ID����
#define XG_ERR_EMPTY_ID               0x07 //ָ��IDΪ�գ��޵�¼���ݣ�
#define XG_ERR_NOT_ENOUGH             0x08 //���㹻��¼�ռ�
#define XG_ERR_NO_SAME_FINGER         0x09 //����ͬһ����ָ
#define XG_ERR_DUPLICATION_ID         0x0A //����ͬ��¼ID
#define XG_ERR_TIME_OUT               0x0B //�ȴ���ָ���볬ʱ
#define XG_ERR_VERIFY                 0x0C //��֤ʧ��
#define XG_ERR_NO_NULL_ID             0x0D //���޿�ID
#define XG_ERR_BREAK_OFF              0x0E //�����ж� 
#define XG_ERR_NO_CONNECT             0x0F //δ���� 
#define XG_ERR_NO_SUPPORT             0x10 //��֧�ִ˲��� 
#define XG_ERR_NO_VEIN                0x11 //�޾������� 
#define XG_ERR_MEMORY                 0x12 //�ڴ治��
#define XG_ERR_NO_DEV                 0x13 //�豸������
#define XG_ERR_ADDRESS                0x14 //�豸��ַ����
#define XG_ERR_NO_FILE                0x15 //�ļ�������
#define XG_ERR_VER                    0x16 //�汾����
#define XG_ERR_BREAK_CARD             0x17 //ˢ���ж�
#define	XG_ERR_NAK	                  0xFF

/******************************״̬����****************************************/
#define XG_INPUT_FINGER               0x20 //���������ָ
#define XG_RELEASE_FINGER             0x21 //�����ÿ���ָ

//�ɼ��ǵ�֧���������豸�������б�
enum VOICE_e
{
    VOICE_REG_OK = 0,//�Ǽǳɹ�
    VOICE_USER_FULL = 1,//��¼����
    VOICE_REG_FAIL = 2,//�Ǽ�ʧ��
    VOICE_OVER_REG = 3, //�Ǽ��ظ� 
    VOICE_ADMIN_AUTH_OK=8, //����Ա��֤�ɹ� 
    VOICE_ADMIN_AUTH_FAIL=9, //����Ա��֤ʧ��
    VOICE_ERROR_CARD=10, //���Ŵ���
    VOICE_PWD_ERROR=13, //�������
    VOICE_PWD_REG_OK = 14,//����Ǽǳɹ� 
    VOICE_PWD_REG_FAIL = 15,//����Ǽ�ʧ�� 
    VOICE_ENROLL_ADMIN = 18,//��Ǽǹ���Ա
    VOICE_PINPUT_PWD = 19,//���������� 
    VOICE_PINPUT_ID = 20,//�������û�ID 
    VOICE_ADMIN_AUTH = 21,//����֤����Ա
    VOICE_INPUT_FINGER_AGAIN = 23,//���ٷ�һ�� 
    VOICE_RETRY = 24,//������һ��
    VOICE_INPUT_AGAIN = 25,//������һ�� 
    VOICE_RIGHT_INPUT = 26, //����ȷ������ָ 
    VOICE_INPUT = 27,//����Ȼ�����ָ
    VOICE_DEL_OK = 28,//ɾ���ɹ�
    VOICE_DEL_FAIL = 29,//ɾ��ʧ��
    VOICE_THANKS = 30, //лл 
    VOICE_DELING = 31,//����ɾ��
    VOICE_IDENT_FAIL = 32,//��֤ʧ�� 
    VOICE_IDENT_OK = 33, //��֤�ɹ� 
    VOICE_UNLOCK = 34, //�ѿ���
    VOICE_BEEP1 = 35,
    VOICE_KEY = 36,
    VOICE_BEEP3 = 37,
    VOICE_BEEP4 = 38,
    VOICE_ALARM = 39, //39 ������ 
    VOICE_OK_KEY_CONFIRM = 40, //��OK��ȷ��
    VOICE_POWER_LOW = 41, //���������������� 
    VOICE_ADMIN = 42,//����Ա 
    VOICE_PWD = 43,//����
    VOICE_CARDNO = 44,//ˢ��
    VOICE_INPUT_CARD = 45, //��ˢ�� 
    VOICE_PLS_INPUT_ADMIN_PWD = 47, //���������Ա����
    VOICE_ADMIN_FULL = 49, //����Ա����
    VOICE_OK_KEY_DEL_CONFIRM = 50, //�밴OK��ȷ��ɾ�� 
    VOICE_HELP = 51, 
    VOICE_HELP1 = 52,
    VOICE_END
};

void xg_drv_init(void);
uint8_t xg_reboot(uint32_t timeout);
uint8_t xg_connect(uint32_t timeout);
uint8_t xg_close_connect(uint32_t timeout);
uint8_t xg_blue_led(uint8_t on, uint32_t timeout);
uint8_t xg_get_num(uint16_t* p_num, uint16_t* p_max, uint32_t timeout);
uint8_t xg_clear_all(uint32_t timeout);
uint8_t xg_clear_id(uint16_t u_id, uint32_t timeout);
uint8_t xg_get_info(uint16_t u_id, uint8_t* info, uint32_t timeout);
void xg_verify_send(void);
uint8_t xg_verify_wait(uint16_t* p_id, uint32_t timeout);
void xg_add_send(uint16_t u_id, uint8_t cnt, uint8_t retries);
uint8_t xg_add_wait(uint32_t timeout);
uint8_t xg_get_nullid(uint16_t* u_id, uint32_t timeout);

#endif //_XG_PROTOCOL_
