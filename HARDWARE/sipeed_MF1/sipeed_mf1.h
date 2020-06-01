
#ifndef __SIPEED_MF1_H__
#define __SIPEED_MF1_H__

#include "sys.h"

#ifdef USE_JSON
#include "cJSON.h"
#endif


#define MF1_BUADRATE		115200


#define MF1_CODE  	"code"

#define MF1_CMD_FACE_RECON  	"face_recon"
#define MF1_CMD_FACE_INFO  		"face_info"
#define MF1_CMD_ADD_UID  		"add_user_spec_uid"
#define MF1_CMD_DELETE_UID  	"del_user_by_uid"

#define MF1_ACK_SUCCESS  		0x00	//操作成功
#define MF1_ACK_FAIL	  		0x01	//操作失败
#define MF1_ACK_NOUSER   		0x02	//无此用户
#define MF1_ACK_USER_EXIST 		0x03 	//用户已存在
#define FM1_ACK_TIMEOUT  		0x04	//采集超时
#define MF1_NAK 				0xFF 	//


#ifdef USE_JSON
struct MF1_Protocol_Handle_t{
	int32_t version;
	char type[32];
	int32_t code;
	char msg[128];
	cJSON *info;
	cJSON *user;
};
#endif


void mf1_init(void);

#ifdef USE_JSON
struct MF1_Protocol_Handle_t *mf1_buffer_parse(uint32_t timeout);
void mf1_buffer_delete(struct MF1_Protocol_Handle_t *mf1_protocol);
#else

void mf1_buffer_flush(void);
uint8_t mf1_face_recon(uint8_t enable, uint32_t timeout);
uint8_t mf1_wait_face(char **uid, uint32_t timeout);
uint8_t mf1_add_user(const char *uid, uint32_t timeout);
uint8_t mf1_delete_user(const char *uid, uint32_t timeout);

#endif

#endif

