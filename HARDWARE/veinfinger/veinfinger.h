
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


//指令列表
#define XG_CMD_CONNECTION             0x01 //连接设备
#define XG_CMD_CLOSE_CONNECTION       0x02 //关闭连接
#define XG_CMD_GET_SYSTEM_INFO        0x03 //获取版本号和设置信息
#define XG_CMD_FACTORY_SETTING        0x04 //恢复出厂设置
#define XG_CMD_SET_DEVICE_ID          0x05 //设置设备编号0-255
#define XG_CMD_SET_BAUDRATE           0x06 //设置波特率0-4
#define XG_CMD_SET_SECURITYLEVEL      0x07 //设置安全等级0-4
#define XG_CMD_SET_TIMEOUT            0x08 //设置等待手指放入超时1-255秒
#define XG_CMD_SET_DUP_CHECK          0x09 //设置重复登录检查0-1
#define XG_CMD_SET_PASSWORD           0x0A //设置通信密码
#define XG_CMD_CHECK_PASSWORD         0x0B //检查密码是否正确
#define XG_CMD_REBOOT                 0x0C //复位重启设备
#define XG_CMD_SET_SAME_FV            0x0D //登记的时候检查是否为同一根手指
#define XG_CMD_SET_USB_MODE           0x0E //设置USB驱动模式 
#define XG_CMD_GET_DUID               0x0F //获取设备序列号 
#define XG_CMD_FINGER_STATUS          0x10 //检测手指放置状态
#define XG_CMD_CLEAR_ENROLL           0x11 //清除指定ID登录数据
#define XG_CMD_CLEAR_ALL_ENROLL       0x12 //清除所有ID登录数据
#define XG_CMD_GET_EMPTY_ID           0x13 //获取空（无登录数据）ID
#define XG_CMD_GET_ENROLL_INFO        0x14 //获取总登录用户数和模板数
#define XG_CMD_GET_ID_INFO            0x15 //获取指定ID登录信息
#define XG_CMD_ENROLL                 0x16 //指定ID登录
#define XG_CMD_VERIFY                 0x17 //1:1认证或1:N识别
#define XG_CMD_READ_DATA              0x20 //从设备读取数据
#define XG_CMD_WRITE_DATA             0x21 //写入数据到设备
#define XG_CMD_READ_ENROLL            0x22 //读取指定ID登录数据
#define XG_CMD_WRITE_ENROLL           0x23 //写入（覆盖）指定ID登录数据
#define XG_CMD_GET_CHARA              0x28 //读取当前采集的特征 
#define XG_CMD_READ_USER_DATA         0x29 //从用户扩展存储区读取数据，最大4K
#define XG_CMD_WRITE_USER_DATA        0x2A //写入数据到用户扩展存储区，最大4K
#define XG_CMD_GET_SYS_SET            0x2E //获取D700设备系统设置数据结构 
#define XG_CMD_SET_SYS_SET            0x2F //下载D700系统设置数据结构 
#define XG_CMD_OPEN_DOOR              0x32 //开门
#define XG_CMD_READ_LOG               0x33 //读取门禁出入日志
#define XG_CMD_SET_DEVNAME            0x34 //设置设备名称
#define XG_CMD_GET_DATETIME           0x35 //获取门禁实时时钟
#define XG_CMD_SET_DATETIME           0x36 //设置门禁实时时钟
#define XG_CMD_ENROLL_EXT             0x38 //扩展语音登记
#define XG_CMD_VERIFY_EXT             0x39 //扩展语音验证
#define XG_CMD_DEL_LOG                0x3a //删除门禁日志
#define XG_CMD_PLAY_VOICE             0x3b //播放语音
#define XG_CMD_BLUE_LED								0x4b
#define XG_CMD_GET_USER_BATCH         0x51 //批量读取用户信息 
#define XG_CMD_SET_USER_BATCH         0x52 //批量写入用户信息 
#define XG_CMD_READ_CARDNO            0x53 //D700请求用户刷卡并读取卡号

/******************************错误代码****************************************/
#define	XG_ERR_SUCCESS                0x00 //操作成功
#define	XG_ERR_FAIL                   0x01 //操作失败
#define XG_ERR_COM                    0x02 //通讯错误
#define XG_ERR_DATA                   0x03 //数据校验错误
#define XG_ERR_INVALID_PWD            0x04 //密码错误
#define XG_ERR_INVALID_PARAM          0x05 //参数错误
#define XG_ERR_INVALID_ID             0x06 //ID错误
#define XG_ERR_EMPTY_ID               0x07 //指定ID为空（无登录数据）
#define XG_ERR_NOT_ENOUGH             0x08 //无足够登录空间
#define XG_ERR_NO_SAME_FINGER         0x09 //不是同一根手指
#define XG_ERR_DUPLICATION_ID         0x0A //有相同登录ID
#define XG_ERR_TIME_OUT               0x0B //等待手指输入超时
#define XG_ERR_VERIFY                 0x0C //认证失败
#define XG_ERR_NO_NULL_ID             0x0D //已无空ID
#define XG_ERR_BREAK_OFF              0x0E //操作中断 
#define XG_ERR_NO_CONNECT             0x0F //未连接 
#define XG_ERR_NO_SUPPORT             0x10 //不支持此操作 
#define XG_ERR_NO_VEIN                0x11 //无静脉数据 
#define XG_ERR_MEMORY                 0x12 //内存不足
#define XG_ERR_NO_DEV                 0x13 //设备不存在
#define XG_ERR_ADDRESS                0x14 //设备地址错误
#define XG_ERR_NO_FILE                0x15 //文件不存在
#define XG_ERR_VER                    0x16 //版本错误
#define XG_ERR_BREAK_CARD             0x17 //刷卡中断
#define	XG_ERR_NAK	                  0xFF

/******************************状态代码****************************************/
#define XG_INPUT_FINGER               0x20 //请求放入手指
#define XG_RELEASE_FINGER             0x21 //请求拿开手指

//采集仪等支持语音的设备的语音列表
enum VOICE_e
{
    VOICE_REG_OK = 0,//登记成功
    VOICE_USER_FULL = 1,//记录已满
    VOICE_REG_FAIL = 2,//登记失败
    VOICE_OVER_REG = 3, //登记重复 
    VOICE_ADMIN_AUTH_OK=8, //管理员验证成功 
    VOICE_ADMIN_AUTH_FAIL=9, //管理员验证失败
    VOICE_ERROR_CARD=10, //卡号错误
    VOICE_PWD_ERROR=13, //密码错误
    VOICE_PWD_REG_OK = 14,//密码登记成功 
    VOICE_PWD_REG_FAIL = 15,//密码登记失败 
    VOICE_ENROLL_ADMIN = 18,//请登记管理员
    VOICE_PINPUT_PWD = 19,//请输入密码 
    VOICE_PINPUT_ID = 20,//请输入用户ID 
    VOICE_ADMIN_AUTH = 21,//请验证管理员
    VOICE_INPUT_FINGER_AGAIN = 23,//请再放一次 
    VOICE_RETRY = 24,//请再试一次
    VOICE_INPUT_AGAIN = 25,//请再输一次 
    VOICE_RIGHT_INPUT = 26, //请正确放置手指 
    VOICE_INPUT = 27,//请自然轻放手指
    VOICE_DEL_OK = 28,//删除成功
    VOICE_DEL_FAIL = 29,//删除失败
    VOICE_THANKS = 30, //谢谢 
    VOICE_DELING = 31,//正在删除
    VOICE_IDENT_FAIL = 32,//验证失败 
    VOICE_IDENT_OK = 33, //验证成功 
    VOICE_UNLOCK = 34, //已开锁
    VOICE_BEEP1 = 35,
    VOICE_KEY = 36,
    VOICE_BEEP3 = 37,
    VOICE_BEEP4 = 38,
    VOICE_ALARM = 39, //39 报警音 
    VOICE_OK_KEY_CONFIRM = 40, //按OK键确认
    VOICE_POWER_LOW = 41, //电量不足请更换电池 
    VOICE_ADMIN = 42,//管理员 
    VOICE_PWD = 43,//密码
    VOICE_CARDNO = 44,//刷卡
    VOICE_INPUT_CARD = 45, //请刷卡 
    VOICE_PLS_INPUT_ADMIN_PWD = 47, //请输入管理员密码
    VOICE_ADMIN_FULL = 49, //管理员已满
    VOICE_OK_KEY_DEL_CONFIRM = 50, //请按OK键确认删除 
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
