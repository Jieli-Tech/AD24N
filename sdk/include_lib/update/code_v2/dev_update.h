#ifndef __DEV_UPDATE_H__
#define __DEV_UPDATE_H__

#include "typedef.h"

void *dev_update_get_parm(int type);
// u16  dev_update_check(char *logo);
u16  dev_update_check(char *logo, bool check_flag);
u16 dev_update_by_change_stack(update_mode_info_t *p_info);

#endif//__DEV_UPDATE_H__
