#include "string.h"
#include "stmflash.h"
#include "device_config.h"
#include "device_info.h"


static Dev_Info_t dev_info;

bool device_info_init()
{
	bool ret = false;
	
	ret = stmflash_read(FLASH_ADDR_DEVICE_INFO, (uint8_t *)&dev_info, sizeof(Dev_Info_t));
	
	//dev_info.a_info.reset = 0xFFFFFFFF;
	
	if(ret == true && dev_info.a_info.reset == 0xFFFFFFFF)
	{
		device_info_reset();
		ret = device_info_flash();
	}

	return ret;
}


bool device_save_reset_flag()
{
	dev_info.a_info.reset = 0xFFFFFFFF;
	return device_info_flash();
}


void device_info_reset()
{
	memset(&dev_info, 0, sizeof(dev_info));
	
	dev_info.a_info.verion_major = VERSION_MAJOR;
	dev_info.a_info.version_junior = VERSION_JUNIOR;
	dev_info.a_info.mask1 = (ID_FPC_BIT | ID_VEIN_BIT | ID_FACE_BIT);	
	dev_info.a_info.mask2 = (ID_PWD_BIT);
	dev_info.a_info.num = USER_MAX;
	dev_info.a_info.warn_mode = WARN_BEEP;
	dev_info.a_info.network_type = OFF_LINE;
	dev_info.a_info.network_tick = 0;
	dev_info.a_info.unlock_time = 10;
	strncpy(dev_info.a_info.adminname, DEFAULT_ADMIN_NAME, 12);
	strncpy(dev_info.a_info.password, DEFAULT_ADMIN_PASSWORD, 12);
}


bool device_info_flash()
{
	return stmflash_write(FLASH_ADDR_DEVICE_INFO, (uint8_t *)&dev_info, sizeof(Dev_Info_t));
}


char* device_get_admin()
{
	return dev_info.a_info.password;
}


User_Info_t* device_get_user(uint32_t id)
{
	if(id >= dev_info.a_info.num)
		return NULL;
	
	return (dev_info.u_info+id);
}


bool device_delete_user(uint32_t id)
{
	if(id >= dev_info.a_info.num)
		return false;

	memset(&dev_info.u_info[id], 0, sizeof(User_Info_t));
	return true;
}


bool device_set_admin(const char *adminname, const char *password)
{
	bool ret = false;
	
	if(adminname != NULL)
	{
		memset(dev_info.a_info.adminname, 0, sizeof(dev_info.a_info.adminname));
		strcpy(dev_info.a_info.adminname, adminname);
		ret = true;
	}

	if(password != NULL)
	{
		memset(dev_info.a_info.password, 0, sizeof(dev_info.a_info.password));
		strcpy(dev_info.a_info.password, password);
		ret = true;
	}

	return ret;
}


bool device_set_user(uint32_t id, const char *username, const char *password, uint32_t uid)
{
	User_Info_t *u_temp = NULL;
	if(id >= dev_info.a_info.num || password == NULL)
		return false;
	
	if(dev_info.u_info[id].enable > 0)
		return false;

	u_temp = &dev_info.u_info[id];	
	u_temp->enable = 1;
	strcpy(u_temp->username, username);
	strcpy(u_temp->password, password);	
	u_temp->fid = uid;
	u_temp->vid = uid;
	u_temp->aid = uid;	

	return true;
}


void device_set_or_mask(uint32_t mask)
{
	dev_info.a_info.mask1 = mask;
}


void device_set_and_mask(uint32_t mask)
{
	dev_info.a_info.mask2 = mask;
}


uint32_t device_get_or_mask(void)
{
	return dev_info.a_info.mask1;
}


uint32_t device_get_and_mask(void)
{
	return dev_info.a_info.mask2;
}


void device_set_networktype(uint16_t type)
{
	dev_info.a_info.network_type = type;
}


uint16_t device_get_networktype(void)
{
	return dev_info.a_info.network_type;
}


void device_set_warnmode(uint16_t mode)
{
	dev_info.a_info.warn_mode = mode;
}


uint16_t device_get_warnmode(void)
{
	return dev_info.a_info.warn_mode;
}


uint16_t device_get_unlocktime(void)
{
	return dev_info.a_info.unlock_time;
}

