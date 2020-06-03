#include "string.h"
#include "lcd192x64.h"
#include "message.h"
#include "identify_task.h"
#include "fpc_task.h"
#include "face_task.h"
#include "menu.h"


uint16_t manage1_setup(uint16_t input)
{
	manage1_item.index = 0;
	common_setup("����", "����Ա����", "�û�����", "����ģʽ", "��������");
	lcd_fb_puthzs(20, 2, "1/2", 0, 1);

	task_identify_enable(0);
	fpc_identify_enable(0);	
	face_identify_enable(0);

	return 0;
}


uint16_t manage2_setup(uint16_t input)
{
	manage2_item.index = 0;
	common_setup("����", "��������", "ʱ������", "ϵͳ״̬", "ϵͳ����");
	lcd_fb_puthzs(20, 2, "2/2", 0, 1);
	
	return 0;
}


uint16_t user_setup(uint16_t input)
{
	user_item.index = 0;	
	common_setup("�û�����", "����û�", "ɾ���û�", NULL, NULL);
	lcd_fb_puthzs(20, 2, "1/1", 0, 1);
	
	return 0;
}


uint16_t networkconfig_setup(uint16_t input)
{
	networkconfig_item.index = 0;	
	common_setup("�������� ", "wifi״̬", "wifi����", "nb״̬", "nb����");
	lcd_fb_puthzs(20, 2, "1/1", 0, 1);
	
	return 0;
}


uint16_t systeminfo_setup(uint16_t input)
{
	systeminfo_item.index = 0;	
	common_setup("ϵͳ״̬", "�豸��Ϣ", NULL, NULL, NULL);
	lcd_fb_puthzs(20, 2, "1/1", 0, 1);
	
	return 0;
}


struct Menu_Item_t manage1_item = {
	.index = 0,
	.setup = manage1_setup,
	.int_process = common_int_process,
	.key_process = common_key_process,
	.password_process = NULL,
	.fpc_process = NULL,
	.vein_process = NULL,
	.face_process = NULL,
	.rtc_process = NULL,
    .pub_menu = &main_item,	
	.sub_menu[0] = &admin_item,
	.sub_menu[1] = &user_item,
	.sub_menu[2] = &lockmode_item,
	.sub_menu[3] = &warn_item,
	.next_menu = &manage2_item,
	.pre_menu = NULL,
};


struct Menu_Item_t manage2_item = {
	.index = 0,
	.setup = manage2_setup,
	.int_process = common_int_process,
	.key_process = common_key_process,
	.password_process = NULL,
	.fpc_process = NULL,
	.vein_process = NULL,
	.face_process = NULL,
	.rtc_process = NULL,
    .pub_menu = &main_item,	
	.sub_menu[0] = &networkconfig_item,
	.sub_menu[1] = NULL,
	.sub_menu[2] = &systeminfo_item,
	.sub_menu[3] = &systemconfig_item,
	.next_menu = NULL,
	.pre_menu = &manage1_item,
};


struct Menu_Item_t user_item = {
	.index = 0,
	.setup = user_setup,
	.int_process = common_int_process,
	.key_process = common_key_process,
	.password_process = NULL,
	.fpc_process = NULL,
	.vein_process = NULL,
	.face_process = NULL,
	.rtc_process = NULL,
    .pub_menu = &manage1_item,	
	.sub_menu[0] = &useradd_item,
	.sub_menu[1] = &userdel_item,
	.sub_menu[2] = NULL,
	.sub_menu[3] = NULL,
	.next_menu = NULL,
	.pre_menu = NULL,
};


struct Menu_Item_t networkconfig_item = {
	.index = 0,
	.setup = networkconfig_setup,
	.int_process = common_int_process,
	.key_process = common_key_process,
	.password_process = NULL,
	.fpc_process = NULL,
	.vein_process = NULL,
	.face_process = NULL,
	.rtc_process = NULL,
    .pub_menu = &manage2_item,	
	.sub_menu[0] = &wifiinfo_item,
	.sub_menu[1] = &wificonfig_item,
	.sub_menu[2] = NULL,
	.sub_menu[3] = NULL,
	.next_menu = NULL,
	.pre_menu = NULL,
};


struct Menu_Item_t systeminfo_item = {
	.index = 0,
	.setup = systeminfo_setup,
	.int_process = common_int_process,
	.key_process = common_key_process,
	.password_process = NULL,
	.fpc_process = NULL,
	.vein_process = NULL,
	.face_process = NULL,
	.rtc_process = NULL,
    .pub_menu = &manage2_item,	
	.sub_menu[0] = NULL,
	.sub_menu[1] = &wifiinfo_item,
	.sub_menu[2] = NULL,
	.sub_menu[3] = NULL,
	.next_menu = NULL,
	.pre_menu = NULL,
};
