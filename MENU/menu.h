#ifndef __MENU_H__
#define __MENU_H__

#include "sys.h"

#define SUB_MENU_MAX	4

typedef uint16_t (*Process_f)(void *param, uint16_t input);
typedef uint16_t (*Setup_f)(uint16_t input);


struct Menu_Item_t
{
	uint8_t index;
	Setup_f setup;
	Process_f int_process;
	Process_f key_process;
	Process_f password_process;    
	Process_f fpc_process;
	Process_f vein_process;
	Process_f face_process;
	Process_f rtc_process;
    struct Menu_Item_t *pub_menu;
    struct Menu_Item_t *sub_menu[SUB_MENU_MAX];
    struct Menu_Item_t *next_menu;
    struct Menu_Item_t *pre_menu;
};



extern struct Menu_Item_t main_item;
extern struct Menu_Item_t unlock_item;
extern struct Menu_Item_t manage1_item;
extern struct Menu_Item_t manage2_item;
extern struct Menu_Item_t admin_item;
extern struct Menu_Item_t user_item;
extern struct Menu_Item_t lockmode_item ;
extern struct Menu_Item_t warn_item;
extern struct Menu_Item_t useradd_item;
extern struct Menu_Item_t useradd_name_item;
extern struct Menu_Item_t useradd_password_item;
extern struct Menu_Item_t useradd_fpc_item;
extern struct Menu_Item_t useradd_vein_item;
extern struct Menu_Item_t useradd_face_item;
extern struct Menu_Item_t useradd_result_item;
extern struct Menu_Item_t userdel_item;
extern struct Menu_Item_t systemconfig_item;


uint16_t common_show_popup(const char *pop);
void common_show_rectangle(uint8_t i);
uint16_t common_int_process(void *param, uint16_t input);
uint16_t common_key_process(void *param, uint16_t input);
uint16_t common_setup(const char *title,
					  const char *option1,
					  const char *option2,
					  const char *option3,
					  const char *option4);

void user_show_username(uint8_t index, uint8_t force);
void user_show_background(uint8_t flag);					  


#endif
