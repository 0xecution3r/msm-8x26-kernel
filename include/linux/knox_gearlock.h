/*
 * include/linux/knox_gearlock.h
 *
 * Include file for the KNOX GEAR Pay.
 */

#ifndef _LINUX_KNOX_GEARLOCK_H
#define _LINUX_KNOX_GEARLOCK_H

#include <linux/types.h>
#include <mach/scm.h>

extern const struct file_operations knox_gearlock_fops;
extern int g_lock_layout_active;

#define TZBSP_SVC_OEM_GEARPAY	246
#define OEM_GEARPAY_CMD_ID	0x01



typedef struct gearpay_req_s {
	u32 flag;
	u32 set;
} gearpay_req_t;

typedef enum gearpay_bool_s {
	TOUCH = (u32)0,
	PHYSBUTTON,
	STRAP
} gearpay_bool_t;

u32 qcom_smc_gearpay(gearpay_bool_t flag_id, u32 set);
#endif /* _LINUX_KNOX_GEARLOCK_H */

