#ifndef __ACTIVITY_TASK_H__
#define __ACTIVITY_TASK_H__

#include "sys.h"
#include "menu.h"


uint8_t activity_debug_parse(uint8_t _input);
void acitivy_send_msg_isr(const void *msg);
int32_t acitivy_send_msg(const void *msg, uint32_t timeout);
void activity_task_create(struct Menu_Item_t *menu, void *activity_hook);
void activity_task_delete(void);

#endif
