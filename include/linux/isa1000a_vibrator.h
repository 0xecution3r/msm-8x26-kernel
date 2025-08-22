/*
* Copyright (C) 2014 Samsung Electronics Co. Ltd. All Rights Reserved.
*
* This software is licensed under the terms of the GNU General Public
* License version 2, as published by the Free Software Foundation, and
* may be copied, distributed, and modified under those terms.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
*/
#ifndef _LINUX_SEC_VIBRATOR_H
#define _LINUX_SEC_VIBRATOR_H

#include <mach/msm_iomap.h>
#include <linux/mfd/pm8xxx/pm8921.h>

struct isa1000a_vibrator_platform_data {
	int duty;
	int period;
	int max_timeout;
	char *regulator_name;
	unsigned int pwm_id;
};

#define MOTOR_VDD 3300000

/* Error and Return value codes */
#define VIBE_S_SUCCESS		0	/* Success */
#define VIBE_E_FAIL		 -4	/* Generic error */

int32_t g_nforce_32;

#define MOTOR_STRENGTH			98/*MOTOR_STRENGTH 98 %*/

#define GP_CLK_M_DEFAULT			2
#define GP_CLK_N_DEFAULT                        80
#define GP_CLK_D_DEFAULT			46  /* 50% duty cycle */
#define IMM_PWM_MULTIPLIER			91

#define MOTOR_MIN_STRENGTH			54/*IMMERSION VALUE*/
/*
 * ** Global variables for LRA PWM M,N and D values.
 * */
int32_t g_nlra_gp_clk_m = GP_CLK_M_DEFAULT;
int32_t g_nlra_gp_clk_n = GP_CLK_N_DEFAULT;
int32_t g_nlra_gp_clk_d = GP_CLK_D_DEFAULT;
int32_t g_nlra_gp_clk_pwm_mul = IMM_PWM_MULTIPLIER;
int32_t motor_strength = MOTOR_STRENGTH;
int32_t motor_min_strength = MOTOR_MIN_STRENGTH;

#define __inp(port) ioread8(port)
#define __inpw(port) ioread16(port)
#define __inpdw(port) ioread32(port)
#define __outp(port, val) iowrite8(val, port)
#define __outpw(port, val) iowrite16(val, port)
#define __outpdw(port, val) iowrite32(val, port)

#define in_dword(addr)              (__inpdw(addr))
#define in_dword_masked(addr, mask) (__inpdw(addr) & (mask))
#define out_dword(addr, val)        __outpdw(addr, val)
#define out_dword_masked(io, mask, val, shadow)  \
	(void) out_dword(io, \
	((shadow & (unsigned int)(~(mask))) | ((unsigned int)((val) & (mask)))))
#define out_dword_masked_ns(io, mask, val, current_reg_content) \
	(void) out_dword(io, \
	((current_reg_content & (unsigned int)(~(mask))) \
	| ((unsigned int)((val) & (mask)))))

static void __iomem *virt_mmss_gp1_base;

#define HWIO_GP1_CMD_RCGR_ADDR ((void __iomem *)(virt_mmss_gp1_base + 0))	//MMSS_CC_GP1_CMD_RCGR
#define HWIO_GP1_CFG_RCGR_ADDR ((void __iomem *)(virt_mmss_gp1_base + 4))	//MMSS_CC_GP1_CFG_RCGR
#define HWIO_GP_M_REG_ADDR ((void __iomem *)(virt_mmss_gp1_base + 8))	//MMSS_CC_GP1_M
#define HWIO_GP_NS_REG_ADDR ((void __iomem *)(virt_mmss_gp1_base + 0xc))	//MMSS_CC_GP1_N
#define HWIO_GP_D_REG_ADDR ((void __iomem *)(virt_mmss_gp1_base + 0x10))	//MMSS_CC_GP1_D

#define HWIO_CAMSS_GP1_CBCR_ADDR ((void __iomem *)(virt_mmss_gp1_base + 0x24))	//MMSS_CC_CAMSS_GP1_CBCR

#define HWIO_GP_MD_REG_RMSK		0xffffffff
#define HWIO_GP_NS_REG_RMSK		0xffffffff

#define HWIO_GP_MD_REG_M_VAL_BMSK		0xff
#define HWIO_GP_MD_REG_M_VAL_SHFT		0
#define HWIO_GP_MD_REG_D_VAL_BMSK		0xff
#define HWIO_GP_MD_REG_D_VAL_SHFT		0
#define HWIO_GP_NS_REG_GP_N_VAL_BMSK	0xff
#define HWIO_GP_SRC_SEL_VAL_BMSK		0x700
#define HWIO_GP_SRC_SEL_VAL_SHFT		8
#define HWIO_GP_SRC_DIV_VAL_BMSK		0x1f
#define HWIO_GP_SRC_DIV_VAL_SHFT		0
#define HWIO_GP_MODE_VAL_BMSK			0x3000
#define HWIO_GP_MODE_VAL_SHFT			12

#define HWIO_CLK_ENABLE_VAL_BMSK	0x1
#define HWIO_CLK_ENABLE_VAL_SHFT	0
#define HWIO_UPDATE_VAL_BMSK	0x1
#define HWIO_UPDATE_VAL_SHFT	0
#define HWIO_ROOT_EN_VAL_BMSK	0x2
#define HWIO_ROOT_EN_VAL_SHFT	1

#define HWIO_GP1_CMD_RCGR_IN		\
		in_dword_masked(HWIO_GP1_CMD_RCGR_ADDR, HWIO_GP_NS_REG_RMSK)
#define HWIO_GP1_CMD_RCGR_OUTM(m, v)	\
	out_dword_masked_ns(HWIO_GP1_CMD_RCGR_ADDR, m, v, HWIO_GP1_CMD_RCGR_IN)

#define HWIO_GP1_CFG_RCGR_IN		\
		in_dword_masked(HWIO_GP1_CFG_RCGR_ADDR, HWIO_GP_NS_REG_RMSK)
#define HWIO_GP1_CFG_RCGR_OUTM(m, v)	\
	out_dword_masked_ns(HWIO_GP1_CFG_RCGR_ADDR, m, v, HWIO_GP1_CFG_RCGR_IN)

#define HWIO_CAMSS_GP1_CBCR_IN		\
		in_dword_masked(HWIO_CAMSS_GP1_CBCR_ADDR, HWIO_GP_NS_REG_RMSK)
#define HWIO_CAMSS_GP1_CBCR_OUTM(m, v)	\
	out_dword_masked_ns(HWIO_CAMSS_GP1_CBCR_ADDR, m, v, HWIO_CAMSS_GP1_CBCR_IN)

#define HWIO_GP_D_REG_IN		\
		in_dword_masked(HWIO_GP_D_REG_ADDR, HWIO_GP_MD_REG_RMSK)

#define HWIO_GP_D_REG_OUTM(m, v)\
	out_dword_masked_ns(HWIO_GP_D_REG_ADDR, m, v, HWIO_GP_D_REG_IN)

#define HWIO_GP_M_REG_IN		\
		in_dword_masked(HWIO_GP_M_REG_ADDR, HWIO_GP_MD_REG_RMSK)
#define HWIO_GP_M_REG_OUTM(m, v)\
	out_dword_masked_ns(HWIO_GP_M_REG_ADDR, m, v, HWIO_GP_M_REG_IN)

#define HWIO_GP_NS_REG_IN		\
		in_dword_masked(HWIO_GP_NS_REG_ADDR, HWIO_GP_NS_REG_RMSK)
#define HWIO_GP_NS_REG_OUTM(m, v)	\
	out_dword_masked_ns(HWIO_GP_NS_REG_ADDR, m, v, HWIO_GP_NS_REG_IN)

#define __msmhwio_outm(hwiosym, mask, val)  HWIO_##hwiosym##_OUTM(mask, val)
#define HWIO_OUTM(hwiosym, mask, val)	__msmhwio_outm(hwiosym, mask, val)
#endif
