#ifndef __DEVICE_INFO_H__
#define __DEVICE_INFO_H__

#include "stdbool.h"
#include "sys.h"

#define USER_MAX 	10

#define FLASH_ADDR_DEVICE_INFO   (ADDR_FLASH_SECTOR_11)

#define DEFAULT_ADMIN_NAME			"admin"
#define DEFAULT_ADMIN_PASSWORD		"43218765"

#define VERSION_MAJOR		1
#define VERSION_JUNIOR	1


enum id_bit_e
{
	ID_POS_PWD = 0,	
	ID_POS_FPC,
  ID_POS_VEIN,
  ID_POS_FACE,
  ID_POS_MAX,
};


#define ID_PWD_BIT			(((uint32_t)0x01)<<ID_POS_PWD)
#define ID_FPC_BIT			(((uint32_t)0x01)<<ID_POS_FPC)
#define ID_VEIN_BIT	    (((uint32_t)0x01)<<ID_POS_VEIN)
#define ID_FACE_BIT			(((uint32_t)0x01)<<ID_POS_FACE)


typedef struct 
{
  uint32_t reset;
  uint16_t verion_major;
  uint16_t version_junior;
  uint32_t manufactory;
  uint32_t mask1;
	uint32_t mask2;
	char adminname[12];
	char password[12];
  uint32_t num;
}Admin_Info_t;


typedef struct
{
	uint32_t enable;
	char username[12];
	char password[12];
  uint32_t fid;
  uint32_t vid;
  uint32_t aid;
}User_Info_t;


typedef struct
{
	Admin_Info_t a_info;
	User_Info_t u_info[USER_MAX];
	
} Dev_Info_t;


bool device_info_init(void);
void device_info_reset(void);
bool device_info_flash(void);
char* device_get_admin(void);
User_Info_t* device_get_user(uint32_t id);
bool device_delete_user(uint32_t id);
bool device_set_admin(const char *adminname, const char *password);
bool device_set_user(uint32_t id, const char *username, const char *password, uint32_t uid);
void device_set_or_mask(uint32_t mask);
void device_set_and_mask(uint32_t mask);
uint32_t device_get_or_mask(void);
uint32_t device_get_and_mask(void);


#endif
