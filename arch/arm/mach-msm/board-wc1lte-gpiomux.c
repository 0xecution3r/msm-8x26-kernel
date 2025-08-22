/* Copyright (c) 2012-2013, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <mach/board.h>
#include <mach/gpio.h>
#include <mach/gpiomux.h>
#include <mach/socinfo.h>

#define WLAN_CLK	44
#define WLAN_SET	43
#define WLAN_DATA0	42
#define WLAN_DATA1	41
#define WLAN_DATA2	40

#define PONTE_HW_REV04		0x08
#define PONTE_HW_REV05		0x09

extern int system_rev;

enum WC1_HW_REV{
	HW_REV_00 = 0x0,
	HW_REV_01 = 0x1,
	HW_REV_02 = 0x2,
	HW_REV_03 = 0x3,
	HW_REV_04 = 0x4,
	HW_REV_MAX,
};

static struct gpiomux_setting hw_check_bit_cfg ={
	.func = GPIOMUX_FUNC_GPIO,
	.pull = GPIOMUX_PULL_NONE,
	.dir = GPIOMUX_IN,
	.drv = GPIOMUX_DRV_2MA,
};

static struct msm_gpiomux_config wc1_hw_check_bit_cfg[] = {
	{
		.gpio = 33,
		.settings = {
			[GPIOMUX_ACTIVE] = &hw_check_bit_cfg,
			[GPIOMUX_SUSPENDED] = &hw_check_bit_cfg,
		},
	},
	{
		.gpio = 118,
		.settings = {
			[GPIOMUX_ACTIVE] = &hw_check_bit_cfg,
			[GPIOMUX_SUSPENDED] = &hw_check_bit_cfg,
		},
	},
	{
		.gpio = 53,
		.settings = {
			[GPIOMUX_ACTIVE] = &hw_check_bit_cfg,
			[GPIOMUX_SUSPENDED] = &hw_check_bit_cfg,
		},
	},
};

static struct gpiomux_setting nc_cfg ={
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
	.dir = GPIOMUX_IN,
};

static struct msm_gpiomux_config wc1_nc_cfg[]={
	{
		.gpio = 38,
		.settings = {
			[GPIOMUX_ACTIVE] = &nc_cfg,
			[GPIOMUX_SUSPENDED] = &nc_cfg,
		},
	},
	{
		.gpio = 51,
		.settings = {
			[GPIOMUX_ACTIVE] = &nc_cfg,
			[GPIOMUX_SUSPENDED] = &nc_cfg,
		},
	},
#if !defined(CONFIG_MACH_WC1)
	/* gpio-55 CODEC_RESET_N */
	{
		.gpio = 55,
		.settings = {
			[GPIOMUX_ACTIVE] = &nc_cfg,
			[GPIOMUX_SUSPENDED] = &nc_cfg,
		},
	},
#endif
#if defined(CONFIG_MACH_B3_CDMA_SPR) || defined(CONFIG_MACH_B3_CDMA_VZW) || defined(CONFIG_MACH_B3_CDMA_USC)
	{
		.gpio = 57,
		.settings = {
			[GPIOMUX_ACTIVE] = &nc_cfg,
			[GPIOMUX_SUSPENDED] = &nc_cfg,
		},
	},
	{
		.gpio = 58,
		.settings = {
			[GPIOMUX_ACTIVE] = &nc_cfg,
			[GPIOMUX_SUSPENDED] = &nc_cfg,
		},
	},
	{
		.gpio = 59,
		.settings = {
			[GPIOMUX_ACTIVE] = &nc_cfg,
			[GPIOMUX_SUSPENDED] = &nc_cfg,
		},
	},
#endif
	{
		.gpio = 60,
		.settings = {
			[GPIOMUX_ACTIVE] = &nc_cfg,
			[GPIOMUX_SUSPENDED] = &nc_cfg,
		},
	},
	{
		.gpio = 76,
		.settings = {
			[GPIOMUX_ACTIVE] = &nc_cfg,
			[GPIOMUX_SUSPENDED] = &nc_cfg,
		},
	},
	{
		.gpio = 77,
		.settings = {
			[GPIOMUX_ACTIVE] = &nc_cfg,
			[GPIOMUX_SUSPENDED] = &nc_cfg,
		},
	},
	{
		.gpio = 80,
		.settings = {
			[GPIOMUX_ACTIVE] = &nc_cfg,
			[GPIOMUX_SUSPENDED] = &nc_cfg,
		},
	},
	{
		.gpio = 88,
		.settings = {
			[GPIOMUX_ACTIVE] = &nc_cfg,
			[GPIOMUX_SUSPENDED] = &nc_cfg,
		},
	},
	{
		.gpio = 89,
		.settings = {
			[GPIOMUX_ACTIVE] = &nc_cfg,
			[GPIOMUX_SUSPENDED] = &nc_cfg,
		},
	},
	{
		.gpio = 90,
		.settings = {
			[GPIOMUX_ACTIVE] = &nc_cfg,
			[GPIOMUX_SUSPENDED] = &nc_cfg,
		},
	},
	{
		.gpio = 91,
		.settings = {
			[GPIOMUX_ACTIVE] = &nc_cfg,
			[GPIOMUX_SUSPENDED] = &nc_cfg,
		},
	},
	{
		.gpio = 92,
		.settings = {
			[GPIOMUX_ACTIVE] = &nc_cfg,
			[GPIOMUX_SUSPENDED] = &nc_cfg,
		},
	},
	{
		.gpio = 93,
		.settings = {
			[GPIOMUX_ACTIVE] = &nc_cfg,
			[GPIOMUX_SUSPENDED] = &nc_cfg,
		},
	},
	{
		.gpio = 94,
		.settings = {
			[GPIOMUX_ACTIVE] = &nc_cfg,
			[GPIOMUX_SUSPENDED] = &nc_cfg,
		},
	},
	{
		.gpio = 103,
		.settings = {
			[GPIOMUX_ACTIVE] = &nc_cfg,
			[GPIOMUX_SUSPENDED] = &nc_cfg,
		},
	},
	{
		.gpio = 104,
		.settings = {
			[GPIOMUX_ACTIVE] = &nc_cfg,
			[GPIOMUX_SUSPENDED] = &nc_cfg,
		},
	},
	{
		.gpio = 110,
		.settings = {
			[GPIOMUX_ACTIVE] = &nc_cfg,
			[GPIOMUX_SUSPENDED] = &nc_cfg,
		},
	},
	{
		.gpio = 111,
		.settings = {
			[GPIOMUX_ACTIVE] = &nc_cfg,
			[GPIOMUX_SUSPENDED] = &nc_cfg,
		},
	},
	{
		.gpio = 112,
		.settings = {
			[GPIOMUX_ACTIVE] = &nc_cfg,
			[GPIOMUX_SUSPENDED] = &nc_cfg,
		},
	},
	{
		.gpio = 113,
		.settings = {
			[GPIOMUX_ACTIVE] = &nc_cfg,
			[GPIOMUX_SUSPENDED] = &nc_cfg,
		},
	},
};

static struct msm_gpiomux_config exception_nc_config[] = {
	{
		.gpio = 115,
		.settings = {
			[GPIOMUX_ACTIVE] = &nc_cfg,
			[GPIOMUX_SUSPENDED] = &nc_cfg,
		},
	},
	{
		.gpio = 116,
		.settings = {
			[GPIOMUX_ACTIVE] = &nc_cfg,
			[GPIOMUX_SUSPENDED] = &nc_cfg,
		},
	},
};

static struct gpiomux_setting haptic_config ={
	.func = GPIOMUX_FUNC_3,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct msm_gpiomux_config b3_haptic_configs[]={
	{
		.gpio = 6,
		.settings = {
			[GPIOMUX_ACTIVE] = &haptic_config,
			[GPIOMUX_SUSPENDED] = &haptic_config,
		},
	},
	{
		.gpio = 7,
		.settings = {
			[GPIOMUX_ACTIVE] = &haptic_config,
			[GPIOMUX_SUSPENDED] = &haptic_config,
		},
	},
};


static struct gpiomux_setting haptic_temp_cofig ={
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
	.dir = GPIOMUX_IN,
};

static struct msm_gpiomux_config haptic_temp_cofigs[]={
	{
		.gpio = 6,
		.settings = {
			[GPIOMUX_ACTIVE] = &haptic_temp_cofig,
			[GPIOMUX_SUSPENDED] = &haptic_temp_cofig,
		},
	},
	{
		.gpio = 7,
		.settings = {
			[GPIOMUX_ACTIVE] = &haptic_temp_cofig,
			[GPIOMUX_SUSPENDED] = &haptic_temp_cofig,
		},
	},
};


void set_temp_gpio_set(bool is_temp_on)
{
	if( is_temp_on){
		msm_gpiomux_install(haptic_temp_cofigs, ARRAY_SIZE(haptic_temp_cofigs));
	}else{
		msm_gpiomux_install(b3_haptic_configs, ARRAY_SIZE(b3_haptic_configs));
	}
}



#ifdef CONFIG_USB_EHCI_MSM_HSIC
static struct gpiomux_setting hsic_sus_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
	.dir = GPIOMUX_OUT_LOW,
};

static struct gpiomux_setting hsic_act_cfg = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_16MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct msm_gpiomux_config msm_hsic_configs[] = {
	{
		.gpio = 119,               /* HSIC_STROBE */
		.settings = {
			[GPIOMUX_ACTIVE] = &hsic_act_cfg,
			[GPIOMUX_SUSPENDED] = &hsic_sus_cfg,
		},
	},
	{
		.gpio = 120,               /* HSIC_DATA */
		.settings = {
			[GPIOMUX_ACTIVE] = &hsic_act_cfg,
			[GPIOMUX_SUSPENDED] = &hsic_sus_cfg,
		},
	},
};
#endif

#define KS8851_IRQ_GPIO 115

#if defined(CONFIG_KS8851) || defined(CONFIG_KS8851_MODULE)
static struct gpiomux_setting gpio_eth_config = {
	.pull = GPIOMUX_PULL_UP,
	.drv = GPIOMUX_DRV_2MA,
	.func = GPIOMUX_FUNC_GPIO,
};

static struct msm_gpiomux_config msm_eth_configs[] = {
	{
		.gpio = KS8851_IRQ_GPIO,
		.settings = {
			[GPIOMUX_SUSPENDED] = &gpio_eth_config,
		}
	},
};
#endif

static struct gpiomux_setting slimbus = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_KEEPER,
};

static struct gpiomux_setting gpio_keys_active = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_UP,
};

static struct gpiomux_setting gpio_keys_suspend = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct gpiomux_setting gpio_spi_act_config = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct gpiomux_setting gpio_spi_cs_act_config = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_6MA,
	.pull = GPIOMUX_PULL_DOWN,
};

static struct gpiomux_setting gpio_spi_susp_config = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
};

#if defined(CONFIG_BT_BCM4334W)
static struct gpiomux_setting gpio_uart_config = {
	.func = GPIOMUX_FUNC_2,
	.drv = GPIOMUX_DRV_16MA,
	.pull = GPIOMUX_PULL_NONE,
	.dir = GPIOMUX_OUT_HIGH,
};
#endif

#ifndef CONFIG_SND_SOC_MAX98504
static struct gpiomux_setting gpio_spi_cs_eth_config = {
	.func = GPIOMUX_FUNC_4,
	.drv = GPIOMUX_DRV_6MA,
	.pull = GPIOMUX_PULL_DOWN,
};
#endif

static struct gpiomux_setting wcnss_5wire_suspend_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv  = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_UP,
};

static struct gpiomux_setting wcnss_5wire_active_cfg = {
	.func = GPIOMUX_FUNC_1,
	.drv  = GPIOMUX_DRV_6MA,
	.pull = GPIOMUX_PULL_DOWN,
};

static struct gpiomux_setting wcnss_5gpio_suspend_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv  = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_UP,
};

static struct gpiomux_setting wcnss_5gpio_active_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv  = GPIOMUX_DRV_6MA,
	.pull = GPIOMUX_PULL_DOWN,
};

static struct gpiomux_setting gpio_i2c_config = {
	.func = GPIOMUX_FUNC_3,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
	.dir = GPIOMUX_OUT_HIGH,
};

static struct msm_gpiomux_config msm8226_slimbus_config[] __initdata = {
	{
		.gpio	= 70,		/* slimbus clk */
		.settings = {
			[GPIOMUX_SUSPENDED] = &slimbus,
		},
	},
	{
		.gpio	= 71,		/* slimbus data */
		.settings = {
			[GPIOMUX_SUSPENDED] = &slimbus,
		},
	},
};

static struct msm_gpiomux_config msm_keypad_configs[] __initdata = {
	{
		.gpio = 106,
		.settings = {
			[GPIOMUX_ACTIVE]    = &gpio_keys_active,
			[GPIOMUX_SUSPENDED] = &gpio_keys_suspend,
		},
	},
	{
		.gpio = 107,
		.settings = {
			[GPIOMUX_ACTIVE]    = &gpio_keys_active,
			[GPIOMUX_SUSPENDED] = &gpio_keys_suspend,
		},
	},
	{
		.gpio = 108,
		.settings = {
			[GPIOMUX_ACTIVE]    = &gpio_keys_active,
			[GPIOMUX_SUSPENDED] = &gpio_keys_suspend,
		},
	},
};

static struct gpiomux_setting lcd_rst_act_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_NONE,
	.dir = GPIOMUX_OUT_HIGH,
};

static struct gpiomux_setting lcd_rst_sus_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
	.dir = GPIOMUX_OUT_LOW,
};

static struct gpiomux_setting lcd_esd_act_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_NONE,
	.dir = GPIOMUX_IN,
};

static struct gpiomux_setting lcd_esd_sus_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
};

static struct gpiomux_setting lcd_err_act_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_DOWN,
	.dir = GPIOMUX_IN,
};

static struct gpiomux_setting lcd_err_sus_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
};

static struct gpiomux_setting ofm_standby_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.dir = GPIOMUX_OUT_LOW,
	.pull = GPIOMUX_PULL_DOWN,
};

static struct gpiomux_setting ofm_powerdown_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.dir = GPIOMUX_OUT_HIGH,
	.pull = GPIOMUX_PULL_DOWN,
};

static struct gpiomux_setting ofm_sus_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct gpiomux_setting ofm_pd_sus_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
};

static struct gpiomux_setting ofm_int_act_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct gpiomux_setting ofm_int_sus_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct gpiomux_setting ofm_i2c_act_cfg = {
	.func = GPIOMUX_FUNC_3,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
};
static struct gpiomux_setting ofm_i2c_sus_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct gpiomux_setting detent_int_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct msm_gpiomux_config msm_detent_configs[] __initdata = {
	{
		.gpio = 39,		/* HALL INT 1 */
		.settings = {
			[GPIOMUX_ACTIVE] = &detent_int_cfg,
			[GPIOMUX_SUSPENDED] = &detent_int_cfg,
		},
	},
	{
		.gpio = 41,		/* HALL INT 2 */
		.settings = {
			[GPIOMUX_ACTIVE] = &detent_int_cfg,
			[GPIOMUX_SUSPENDED] = &detent_int_cfg,
		},
	},
	{
		.gpio = 37,		/* HALL INT 3 */
		.settings = {
			[GPIOMUX_ACTIVE] = &detent_int_cfg,
			[GPIOMUX_SUSPENDED] = &detent_int_cfg,
		},
	},
};

static struct msm_gpiomux_config msm_lcd_configs[] __initdata = {
	{
		.gpio = 25,		/* LCD Reset*/
		.settings = {
			[GPIOMUX_ACTIVE] = &lcd_rst_act_cfg,
			[GPIOMUX_SUSPENDED] = &lcd_rst_sus_cfg,
		},
	},
	{
		.gpio = 73,		/* LCD_IO_EN */
		.settings = {
			[GPIOMUX_ACTIVE] = &lcd_rst_act_cfg,
			[GPIOMUX_SUSPENDED] = &lcd_rst_sus_cfg,
		},
	},
};

static struct msm_gpiomux_config msm_panel_configs[] __initdata = {
	{
		.gpio = 25,		/* LCD Reset*/
		.settings = {
			[GPIOMUX_ACTIVE] = &lcd_rst_act_cfg,
			[GPIOMUX_SUSPENDED] = &lcd_rst_sus_cfg,
		},
	},
	{
		.gpio = 115,		/* LCD_IO_EN */
		.settings = {
			[GPIOMUX_ACTIVE] = &lcd_rst_act_cfg,
			[GPIOMUX_SUSPENDED] = &lcd_rst_sus_cfg,
		},
	},
};


static struct msm_gpiomux_config msm_ofm_configs[] __initdata = {
	{
		.gpio = 35,		/* OFM_MOTION */
		.settings = {
			[GPIOMUX_ACTIVE] = &ofm_int_act_cfg,
			[GPIOMUX_SUSPENDED] = &ofm_int_sus_cfg,
		},
	},
	{
		.gpio = 36,		/*OFM_STBY */
		.settings = {
			[GPIOMUX_ACTIVE] = &ofm_standby_cfg,
			[GPIOMUX_SUSPENDED] = &ofm_sus_cfg,
		},
	},
	{
		.gpio = 37,		/* OFM_PWRDOWN */
		.settings = {
			[GPIOMUX_ACTIVE] = &ofm_powerdown_cfg,
			[GPIOMUX_SUSPENDED] = &ofm_pd_sus_cfg,
		},
	},
};

static struct msm_gpiomux_config msm_esd_configs[] __initdata = {
	{
		.gpio = 62,		/* OLED_DET */
		.settings = {
			[GPIOMUX_ACTIVE] = &lcd_esd_act_cfg,
			[GPIOMUX_SUSPENDED] = &lcd_esd_sus_cfg,
		},
	},
	{
		.gpio = 51,		/* ERR_FG */
		.settings = {
			[GPIOMUX_ACTIVE] = &lcd_err_act_cfg,
			[GPIOMUX_SUSPENDED] = &lcd_err_sus_cfg,
		},
	},
};

static struct gpiomux_setting gpio_i2c_tsp_active_config = {
	.func = GPIOMUX_FUNC_3,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
};
static struct gpiomux_setting gpio_i2c_tsp_suspend_config = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
};

static struct msm_gpiomux_config msm_blsp_configs[] __initdata = {
	{
		.gpio      = 0,		/* BLSP1 QUP1 SPI_DATA_MOSI */
		.settings = {
			[GPIOMUX_ACTIVE] = &gpio_spi_act_config,
			[GPIOMUX_SUSPENDED] = &gpio_spi_susp_config,
		},
	},
	{
		.gpio      = 1,		/* BLSP1 QUP1 SPI_DATA_MISO */
		.settings = {
			[GPIOMUX_ACTIVE] = &gpio_spi_act_config,
			[GPIOMUX_SUSPENDED] = &gpio_spi_susp_config,
		},
	},
	{
		.gpio      = 2,		/* BLSP1 QUP1 SPI_CS1 */
		.settings = {
			[GPIOMUX_ACTIVE] = &gpio_spi_cs_act_config,
			[GPIOMUX_SUSPENDED] = &gpio_spi_susp_config,
		},
	},
	{
		.gpio      = 3,		/* BLSP1 QUP1 SPI_CLK */
		.settings = {
			[GPIOMUX_ACTIVE] = &gpio_spi_act_config,
			[GPIOMUX_SUSPENDED] = &gpio_spi_susp_config,
		},
	},
#ifdef CONFIG_OT_PEARL_V3_1
	{
		.gpio     = 4,		/* BLSP2 NFC SPI MOSI */
		.settings = {
			[GPIOMUX_ACTIVE] = &gpio_spi_act_config,
		},
	},
	{
		.gpio     = 5,		/* BLSP2 NFC SPI MISO*/
		.settings = {
			[GPIOMUX_ACTIVE] = &gpio_spi_act_config,
		},
	},
	{
		.gpio      = 6,		/* BLSP1 QUP1 SPI SSN */
		.settings = {
			[GPIOMUX_ACTIVE] = &gpio_spi_cs_act_config,
		},
	},
	{
		.gpio      = 7,		/* BLSP1 QUP1 SPI_CLK */
		.settings = {
			[GPIOMUX_ACTIVE] = &gpio_spi_act_config,
		},
	},
#endif
	{
		.gpio      = 14,	/* BLSP1 QUP4 I2C_SDA */
		.settings = {
			[GPIOMUX_ACTIVE] = &ofm_i2c_act_cfg,
			[GPIOMUX_SUSPENDED] = &ofm_i2c_sus_cfg,
		},
	},
	{
		.gpio      = 15,	/* BLSP1 QUP4 I2C_SCL */
		.settings = {
			[GPIOMUX_ACTIVE] = &ofm_i2c_act_cfg,
			[GPIOMUX_SUSPENDED] = &ofm_i2c_sus_cfg,
		},
	},
	{
		.gpio      = 18,		/* BLSP1 QUP5 I2C_SDA */
		.settings = {
			[GPIOMUX_ACTIVE] = &gpio_i2c_tsp_active_config,
			[GPIOMUX_SUSPENDED] = &gpio_i2c_tsp_suspend_config,
		},
	},
	{
		.gpio      = 19,		/* BLSP1 QUP5 I2C_SCL */
		.settings = {
			[GPIOMUX_ACTIVE] = &gpio_i2c_tsp_active_config,
			[GPIOMUX_SUSPENDED] = &gpio_i2c_tsp_suspend_config,
		},
	},
#ifndef CONFIG_SND_SOC_MAX98504
	{
		.gpio      = 22,		/* BLSP1 QUP1 SPI_CS_ETH */
		.settings = {
			[GPIOMUX_SUSPENDED] = &gpio_spi_cs_eth_config,
		},
	},
#endif
};

#if 0
static struct gpiomux_setting goodix_ldo_en_act_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_6MA,
	.pull = GPIOMUX_PULL_UP,
};

static struct gpiomux_setting goodix_ldo_en_sus_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
};

static struct gpiomux_setting goodix_int_act_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_6MA,
	.pull = GPIOMUX_PULL_UP,
};

static struct gpiomux_setting goodix_int_sus_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
};

static struct gpiomux_setting goodix_reset_act_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_6MA,
	.pull = GPIOMUX_PULL_UP,
};

static struct gpiomux_setting goodix_reset_sus_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
};
#endif

#if 0
static struct msm_gpiomux_config msm_skuf_goodix_configs[] __initdata = {
	{
		.gpio = 15,		/* LDO EN */
		.settings = {
			[GPIOMUX_ACTIVE] = &goodix_ldo_en_act_cfg,
			[GPIOMUX_SUSPENDED] = &goodix_ldo_en_sus_cfg,
		},
	},
	{
		.gpio = 16,		/* RESET */
		.settings = {
			[GPIOMUX_ACTIVE] = &goodix_reset_act_cfg,
			[GPIOMUX_SUSPENDED] = &goodix_reset_sus_cfg,
		},
	},
	{
		.gpio = 17,		/* INT */
		.settings = {
			[GPIOMUX_ACTIVE] = &goodix_int_act_cfg,
			[GPIOMUX_SUSPENDED] = &goodix_int_sus_cfg,
		},
	},
	{
		.gpio      = 18,		/* BLSP1 QUP5 I2C_SDA */
		.settings = {
			[GPIOMUX_ACTIVE] = &gpio_i2c_config,
			[GPIOMUX_SUSPENDED] = &gpio_i2c_config,
		},
	},
	{
		.gpio      = 19,		/* BLSP1 QUP5 I2C_SCL */
		.settings = {
			[GPIOMUX_ACTIVE] = &gpio_i2c_config,
			[GPIOMUX_SUSPENDED] = &gpio_i2c_config,
		},
	},
};
#endif

static struct gpiomux_setting nfc_firm_act_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_6MA,
	.pull = GPIOMUX_PULL_NONE,
	.dir = GPIOMUX_OUT_LOW,
};

static struct gpiomux_setting nfc_firm_sus_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_6MA,
	.pull = GPIOMUX_PULL_NONE,
	.dir = GPIOMUX_OUT_LOW,
};

static struct gpiomux_setting nfc_enable_act_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_6MA,
	.pull = GPIOMUX_PULL_NONE,
	.dir = GPIOMUX_OUT_HIGH,
};

static struct gpiomux_setting nfc_enable_sus_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
	.dir = GPIOMUX_OUT_LOW,
};

static struct gpiomux_setting nfc_wake_act_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_6MA,
	.pull = GPIOMUX_PULL_DOWN,
	.dir = GPIOMUX_IN,
};

static struct gpiomux_setting nfc_wake_sus_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
	.dir = GPIOMUX_IN,
};

static struct msm_gpiomux_config msm_skuf_nfc_configs[] __initdata = {

	{					/*  NFC  FIRM */
		.gpio      = 42,
		.settings = {
			[GPIOMUX_ACTIVE] = &nfc_firm_act_cfg,
			[GPIOMUX_SUSPENDED] = &nfc_firm_sus_cfg,
		},
	},

	{					/*  NFC  REGC*/
		.gpio      = 45,
		.settings = {
			[GPIOMUX_ACTIVE] = &nfc_enable_act_cfg,
			[GPIOMUX_SUSPENDED] = &nfc_enable_sus_cfg,
		},
	},

	{					/*  NFC   WAKE */
		.gpio      = 72,
		.settings = {
			[GPIOMUX_ACTIVE] = &nfc_wake_act_cfg,
			[GPIOMUX_SUSPENDED] = &nfc_wake_sus_cfg,
		},
	},
	{					/*  NFC   */
		.gpio      = 10,		/* BLSP1 QUP3 I2C_DAT */
		.settings = {
			[GPIOMUX_ACTIVE] = &gpio_i2c_config,
			[GPIOMUX_SUSPENDED] = &gpio_i2c_config,
		},
	},
	{					/*  NFC   */
		.gpio      = 11,		/* BLSP1 QUP3 I2C_CLK */
		.settings = {
			[GPIOMUX_ACTIVE] = &gpio_i2c_config,
			[GPIOMUX_SUSPENDED] = &gpio_i2c_config,
		},
	},
};

static struct gpiomux_setting gpio_fuel_i2c_config = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
	.dir = GPIOMUX_IN,
};

static struct gpiomux_setting gpio_fuel_alert_config = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv  = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
	.dir = GPIOMUX_IN,
};

static struct msm_gpiomux_config fuel_i2c_config[] __initdata = {
        {       /* FUEL_SDA_1.8V */
                .gpio     = 43,
                .settings = {
                        [GPIOMUX_ACTIVE] = &gpio_fuel_i2c_config,
						[GPIOMUX_SUSPENDED] = &gpio_fuel_i2c_config,
                },
        },
        {       /* FUEL_SCL_1.8V */
                .gpio     = 44,
                .settings = {
                        [GPIOMUX_ACTIVE] = &gpio_fuel_i2c_config,
						[GPIOMUX_SUSPENDED] = &gpio_fuel_i2c_config,
                },
        },
        {       /* FUEL_ALERT */
                .gpio     = 69,
                .settings = {
			[GPIOMUX_ACTIVE] = &gpio_fuel_alert_config,
                        [GPIOMUX_SUSPENDED] = &gpio_fuel_alert_config,
                },
        },
};

static struct gpiomux_setting gpio_muic_i2c_config = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
	.dir = GPIOMUX_IN,
};

static struct msm_gpiomux_config muic_i2c_config[] __initdata = {
        {       /* MUIC_SCL_1.8V */
                .gpio     = 54,
                .settings = {
                        [GPIOMUX_ACTIVE] = &gpio_muic_i2c_config,
						[GPIOMUX_SUSPENDED] = &gpio_muic_i2c_config,
                },
        },
        {       /* MUIC_SDA_1.8V */
                .gpio     = 56,
                .settings = {
                        [GPIOMUX_ACTIVE] = &gpio_muic_i2c_config,
						[GPIOMUX_SUSPENDED] = &gpio_muic_i2c_config,
                },
        },
};

static struct gpiomux_setting gpio_pma_i2c_config = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
	.dir = GPIOMUX_IN,
};

static struct gpiomux_setting gpio_pma_int_config = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv  = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_UP,
	.dir = GPIOMUX_IN,
};

static struct msm_gpiomux_config pma_i2c_config[] __initdata = {
        {       /* PMA_SDA_1.8V */
                .gpio     = 119,
                .settings = {
                        [GPIOMUX_ACTIVE] = &gpio_pma_i2c_config,
						[GPIOMUX_SUSPENDED] = &gpio_pma_i2c_config,
                },
        },
        {       /* PMA_SCL_1.8V */
                .gpio     = 120,
                .settings = {
                        [GPIOMUX_ACTIVE] = &gpio_pma_i2c_config,
						[GPIOMUX_SUSPENDED] = &gpio_pma_i2c_config,
                },
        },
       	{		/* PMA_INT */
				.gpio = 67,
				.settings = {
						[GPIOMUX_ACTIVE] = &gpio_pma_int_config,
						[GPIOMUX_SUSPENDED] = &gpio_pma_int_config,
				},
		},
};

#if 0
static struct gpiomux_setting sd_card_det_active_config = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
	.dir = GPIOMUX_IN,
};

static struct gpiomux_setting sd_card_det_sleep_config = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_UP,
	.dir = GPIOMUX_IN,
};


static struct msm_gpiomux_config sd_card_det __initdata = {
	.gpio = 38,
	.settings = {
		[GPIOMUX_ACTIVE]    = &sd_card_det_active_config,
		[GPIOMUX_SUSPENDED] = &sd_card_det_sleep_config,
	},
};
#endif

static struct msm_gpiomux_config wcnss_5wire_interface[] = {
	{
		.gpio = 40,
		.settings = {
			[GPIOMUX_ACTIVE]    = &wcnss_5wire_active_cfg,
			[GPIOMUX_SUSPENDED] = &wcnss_5wire_suspend_cfg,
		},
	},
#if 0
	{
		.gpio = 41,
		.settings = {
			[GPIOMUX_ACTIVE]    = &wcnss_5wire_active_cfg,
			[GPIOMUX_SUSPENDED] = &wcnss_5wire_suspend_cfg,
		},
	},
#endif
	{
		.gpio = 42,
		.settings = {
			[GPIOMUX_ACTIVE]    = &wcnss_5wire_active_cfg,
			[GPIOMUX_SUSPENDED] = &wcnss_5wire_suspend_cfg,
		},
	},
	{
		.gpio = 43,
		.settings = {
			[GPIOMUX_ACTIVE]    = &wcnss_5wire_active_cfg,
			[GPIOMUX_SUSPENDED] = &wcnss_5wire_suspend_cfg,
		},
	},
	{
		.gpio = 44,
		.settings = {
			[GPIOMUX_ACTIVE]    = &wcnss_5wire_active_cfg,
			[GPIOMUX_SUSPENDED] = &wcnss_5wire_suspend_cfg,
		},
	},
};

static struct msm_gpiomux_config wcnss_5gpio_interface[] = {
	{
		.gpio = 40,
		.settings = {
			[GPIOMUX_ACTIVE]    = &wcnss_5gpio_active_cfg,
			[GPIOMUX_SUSPENDED] = &wcnss_5gpio_suspend_cfg,
		},
	},
#if 0
	{
		.gpio = 41,
		.settings = {
			[GPIOMUX_ACTIVE]    = &wcnss_5gpio_active_cfg,
			[GPIOMUX_SUSPENDED] = &wcnss_5gpio_suspend_cfg,
		},
	},
#endif
	{
		.gpio = 42,
		.settings = {
			[GPIOMUX_ACTIVE]    = &wcnss_5gpio_active_cfg,
			[GPIOMUX_SUSPENDED] = &wcnss_5gpio_suspend_cfg,
		},
	},
	{
		.gpio = 43,
		.settings = {
			[GPIOMUX_ACTIVE]    = &wcnss_5gpio_active_cfg,
			[GPIOMUX_SUSPENDED] = &wcnss_5gpio_suspend_cfg,
		},
	},
	{
		.gpio = 44,
		.settings = {
			[GPIOMUX_ACTIVE]    = &wcnss_5gpio_active_cfg,
			[GPIOMUX_SUSPENDED] = &wcnss_5gpio_suspend_cfg,
		},
	},
};

#if 0 /* B3 doesn't have cam */
static struct gpiomux_setting gpio_suspend_config[] = {
	{
		.func = GPIOMUX_FUNC_GPIO,  /* IN-NP */
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_NONE,
	},
	{
		.func = GPIOMUX_FUNC_GPIO,  /* O-LOW */
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_NONE,
		.dir = GPIOMUX_OUT_LOW,
	},
};

static struct gpiomux_setting cam_settings[] = {
	{
		.func = GPIOMUX_FUNC_1, /*active 1*/ /* 0 */
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_NONE,
	},

	{
		.func = GPIOMUX_FUNC_1, /*suspend*/ /* 1 */
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_DOWN,
	},

	{
		.func = GPIOMUX_FUNC_1, /*i2c suspend*/ /* 2 */
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_KEEPER,
	},

	{
		.func = GPIOMUX_FUNC_GPIO, /*active 0*/ /* 3 */
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_NONE,
	},

	{
		.func = GPIOMUX_FUNC_GPIO, /*suspend 0*/ /* 4 */
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_DOWN,
	},
};

static struct msm_gpiomux_config msm_sensor_configs[] __initdata = {
	{
		.gpio = 26, /* CAM_MCLK0 */
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[0],
			[GPIOMUX_SUSPENDED] = &cam_settings[1],
		},
	},
	{
		.gpio = 27, /* CAM_MCLK1 */
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[0],
			[GPIOMUX_SUSPENDED] = &cam_settings[1],
		},

	},
	{
		.gpio = 29, /* CCI_I2C_SDA0 */
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[0],
			[GPIOMUX_SUSPENDED] = &gpio_suspend_config[0],
		},
	},
	{
		.gpio = 30, /* CCI_I2C_SCL0 */
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[0],
			[GPIOMUX_SUSPENDED] = &gpio_suspend_config[0],
		},
	},
	{
		.gpio = 36, /* CAM1_STANDBY_N */
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[3],
			[GPIOMUX_SUSPENDED] = &cam_settings[4],
		},
	},
	{
		.gpio = 37, /* CAM1_RST_N */
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[3],
			[GPIOMUX_SUSPENDED] = &cam_settings[4],
		},
	},
	{
		.gpio = 35, /* CAM2_STANDBY_N */
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[3],
			[GPIOMUX_SUSPENDED] = &cam_settings[4],
		},
	},
	{
		.gpio = 28, /* CAM2_RST_N */
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[3],
			[GPIOMUX_SUSPENDED] = &cam_settings[4],
		},
	},

};

static struct msm_gpiomux_config msm_sensor_configs_skuf_plus[] __initdata = {
	{
		.gpio = 22, /* CAM1_VDD */
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[3],
			[GPIOMUX_SUSPENDED] = &cam_settings[4],
		},
	},
	{
		.gpio = 34, /* CAM1 VCM_PWDN */
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[3],
			[GPIOMUX_SUSPENDED] = &cam_settings[4],
		},
	},
};
#endif

static struct gpiomux_setting auxpcm_act_cfg = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct gpiomux_setting auxpcm_sus_cfg = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
};

static struct msm_gpiomux_config msm_auxpcm_configs[] __initdata = {
	{
		.gpio = 63,
		.settings = {
			[GPIOMUX_SUSPENDED] = &auxpcm_sus_cfg,
			[GPIOMUX_ACTIVE] = &auxpcm_act_cfg,
		},
	},
	{
		.gpio = 64,
		.settings = {
			[GPIOMUX_SUSPENDED] = &auxpcm_sus_cfg,
			[GPIOMUX_ACTIVE] = &auxpcm_act_cfg,
		},
	},
	{
		.gpio = 65,
		.settings = {
			[GPIOMUX_SUSPENDED] = &auxpcm_sus_cfg,
			[GPIOMUX_ACTIVE] = &auxpcm_act_cfg,
		},
	},
	{
		.gpio = 66,
		.settings = {
			[GPIOMUX_SUSPENDED] = &auxpcm_sus_cfg,
			[GPIOMUX_ACTIVE] = &auxpcm_act_cfg,
		},
	},
};

static struct gpiomux_setting tapan_int = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct gpiomux_setting tapan_reset = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_6MA,
	.pull = GPIOMUX_PULL_NONE,
	.dir = GPIOMUX_OUT_LOW,
};

static struct msm_gpiomux_config msm_tapan_config[] __initdata = {
	{
		.gpio	= 68,		/* CDC_INT */
		.settings = {
			[GPIOMUX_SUSPENDED] = &tapan_int,
		},
	},
	{
#if defined(CONFIG_MACH_WC1)
		.gpio	= 55,		/* SYS_RST_N */
#else
		.gpio	= 72,		/* SYS_RST_N */
#endif
		.settings = {
			[GPIOMUX_SUSPENDED] = &tapan_reset,
		},
	},
};

#ifdef CONFIG_SND_SOC_MAX98504
static struct gpiomux_setting gpio_i2c_codec_active_config = {
	.func = GPIOMUX_FUNC_3,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_UP,
};
static struct gpiomux_setting gpio_i2c_codec_suspend_config = {
	.func = GPIOMUX_FUNC_3,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_UP,
};

static struct gpiomux_setting  pri_mi2s_act_cfg = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct gpiomux_setting  pri_mi2s_sus_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
};

static struct msm_gpiomux_config msm8226_blsp_codec_configs[] __initdata = {
	{					/*	MAX_SDA   */
		.gpio	   = 22,		/* BLSP1 QUP3 I2C_DAT */
		.settings = {
			[GPIOMUX_ACTIVE] = &gpio_i2c_codec_active_config,
			[GPIOMUX_SUSPENDED] = &gpio_i2c_codec_suspend_config,
		},
	},
	{					/*	MAX_SCL   */
		.gpio	   = 23,		/* BLSP1 QUP3 I2C_CLK */
		.settings = {
			[GPIOMUX_ACTIVE] = &gpio_i2c_codec_active_config,
			[GPIOMUX_SUSPENDED] = &gpio_i2c_codec_suspend_config,
		},
	},
};

static struct msm_gpiomux_config msm8226_tertiary_mi2s_configs[] __initdata = {
	{
		.gpio	= 50,
		.settings = {
			[GPIOMUX_SUSPENDED] = &pri_mi2s_sus_cfg,
			[GPIOMUX_ACTIVE] = &pri_mi2s_act_cfg,
		},
	},
	{
		.gpio = 51,
		.settings = {
			[GPIOMUX_SUSPENDED] = &pri_mi2s_sus_cfg,
			[GPIOMUX_ACTIVE] = &pri_mi2s_act_cfg,
		},
	},
	{
		.gpio = 52,
		.settings = {
			[GPIOMUX_SUSPENDED] = &pri_mi2s_sus_cfg,
			[GPIOMUX_ACTIVE] = &pri_mi2s_act_cfg,
		},
	},

};
#endif /* CONFIG_SND_SOC_MAX98504 */

#if defined(CONFIG_IR_REMOCON_FPGA)
/* active */
static struct gpiomux_setting gpio_fpga_spi_si_active = {
	.func = GPIOMUX_FUNC_GPIO,
	.pull = GPIOMUX_PULL_NONE,
	.dir = GPIOMUX_OUT_HIGH
};
static struct gpiomux_setting gpio_fpga_main_clk_active = {
	.func = GPIOMUX_FUNC_3,
	.pull = GPIOMUX_PULL_DOWN,
};
static struct gpiomux_setting gpio_fpga_creset_b_active = {
	.func = GPIOMUX_FUNC_GPIO,
	.pull = GPIOMUX_PULL_NONE,
	.dir = GPIOMUX_OUT_HIGH
};
static struct gpiomux_setting gpio_fpga_spi_clk_active = {
	.func = GPIOMUX_FUNC_GPIO,
	.pull = GPIOMUX_PULL_NONE,
	.dir = GPIOMUX_OUT_HIGH
};
static struct gpiomux_setting gpio_fpga_irda_irq_active = {
	.func = GPIOMUX_FUNC_GPIO,
	.pull = GPIOMUX_PULL_NONE,
	.dir = GPIOMUX_IN
};
static struct gpiomux_setting gpio_fpga_cdone_active = {
	.func = GPIOMUX_FUNC_GPIO,
	.pull = GPIOMUX_PULL_NONE,
	.dir = GPIOMUX_IN
};
static struct gpiomux_setting gpio_fpga_spi_en_active = {
	.func = GPIOMUX_FUNC_GPIO,
	.pull = GPIOMUX_PULL_NONE,
	.dir = GPIOMUX_OUT_LOW
};

/* suspend */
static struct gpiomux_setting gpio_fpga_spi_si_suspend = {
	.func = GPIOMUX_FUNC_GPIO,
	.pull = GPIOMUX_PULL_NONE,
	.dir = GPIOMUX_OUT_HIGH
};
static struct gpiomux_setting gpio_fpga_main_clk_suspend = {
	.func = GPIOMUX_FUNC_3,
	.pull = GPIOMUX_PULL_DOWN,
};
static struct gpiomux_setting gpio_fpga_creset_b_suspend = {
	.func = GPIOMUX_FUNC_GPIO,
	.pull = GPIOMUX_PULL_NONE,
	.dir = GPIOMUX_OUT_HIGH
};
static struct gpiomux_setting gpio_fpga_spi_clk_suspend = {
	.func = GPIOMUX_FUNC_GPIO,
	.pull = GPIOMUX_PULL_NONE,
	.dir = GPIOMUX_OUT_HIGH
};
static struct gpiomux_setting gpio_fpga_irda_irq_suspend = {
	.func = GPIOMUX_FUNC_GPIO,
	.pull = GPIOMUX_PULL_NONE,
	.dir = GPIOMUX_IN
};
static struct gpiomux_setting gpio_fpga_cdone_suspend = {
	.func = GPIOMUX_FUNC_GPIO,
	.pull = GPIOMUX_PULL_NONE,
	.dir = GPIOMUX_IN
};
static struct gpiomux_setting gpio_fpga_spi_en_suspend = {
	.func = GPIOMUX_FUNC_GPIO,
	.pull = GPIOMUX_PULL_NONE,
	.dir = GPIOMUX_OUT_LOW
};

static struct msm_gpiomux_config fpga_configs[] __initdata = {
	{
		.gpio = 33,
		.settings = {
			[GPIOMUX_ACTIVE]    = &gpio_fpga_spi_si_active,
			[GPIOMUX_SUSPENDED] = &gpio_fpga_spi_si_suspend,
		},
	},
	{
		.gpio = 34,
		.settings = {
			[GPIOMUX_ACTIVE]    = &gpio_fpga_main_clk_active,
			[GPIOMUX_SUSPENDED] = &gpio_fpga_main_clk_suspend,
		},
	},
	{
		.gpio = 38,
		.settings = {
			[GPIOMUX_ACTIVE]	= &gpio_fpga_spi_clk_active,
			[GPIOMUX_SUSPENDED] = &gpio_fpga_spi_clk_suspend,
		},
	},
	{
		.gpio = 67,
		.settings = {
			[GPIOMUX_ACTIVE]	= &gpio_fpga_spi_en_active,
			[GPIOMUX_SUSPENDED] = &gpio_fpga_spi_en_suspend,
		},
	},
};
#endif

#ifdef CONFIG_MMC_MSM_SDC3_SUPPORT
static struct gpiomux_setting sdc3_clk_actv_cfg = {
	.func = GPIOMUX_FUNC_2,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct gpiomux_setting sdc3_clk_suspend_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
	.dir = GPIOMUX_OUT_LOW,
};

static struct gpiomux_setting sdc3_cmd_data_0_3_actv_cfg = {
	.func = GPIOMUX_FUNC_2,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct gpiomux_setting sdc3_suspend_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
};

#else
static struct gpiomux_setting sdc3_clk_nc_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
};

static struct gpiomux_setting sdc3_cmd_data_nc_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
};
#endif /* CONFIG_MMC_MSM_SDC3_SUPPORT */

static struct msm_gpiomux_config msm8226_sdc3_configs[] __initdata = {
#if 0
	{
		/* DAT3 */
		.gpio      = 39,
		.settings = {
#ifdef CONFIG_MMC_MSM_SDC3_SUPPORT
			[GPIOMUX_ACTIVE]    = &sdc3_cmd_data_0_3_actv_cfg,
			[GPIOMUX_SUSPENDED] = &sdc3_suspend_cfg,
#else
			[GPIOMUX_ACTIVE]    = &sdc3_cmd_data_nc_cfg,
			[GPIOMUX_SUSPENDED] = &sdc3_cmd_data_nc_cfg,
#endif /* CONFIG_MMC_MSM_SDC3_SUPPORT */
		},
	},
#endif
	{
		/* DAT2 */
		.gpio      = 40,
		.settings = {
#ifdef CONFIG_MMC_MSM_SDC3_SUPPORT
			[GPIOMUX_ACTIVE]    = &sdc3_cmd_data_0_3_actv_cfg,
			[GPIOMUX_SUSPENDED] = &sdc3_suspend_cfg,
#else
			[GPIOMUX_ACTIVE]    = &sdc3_cmd_data_nc_cfg,
			[GPIOMUX_SUSPENDED] = &sdc3_cmd_data_nc_cfg,
#endif /* CONFIG_MMC_MSM_SDC3_SUPPORT */
		},
	},
#if 0
	{
		/* DAT1 */
		.gpio      = 41,
		.settings = {
#ifdef CONFIG_MMC_MSM_SDC3_SUPPORT
			[GPIOMUX_ACTIVE]    = &sdc3_cmd_data_0_3_actv_cfg,
			[GPIOMUX_SUSPENDED] = &sdc3_suspend_cfg,
#else
			[GPIOMUX_ACTIVE]    = &sdc3_cmd_data_nc_cfg,
			[GPIOMUX_SUSPENDED] = &sdc3_cmd_data_nc_cfg,
#endif /* CONFIG_MMC_MSM_SDC3_SUPPORT */
		},
	},
#endif
	{
		/* DAT0 */
		.gpio      = 42,
		.settings = {
#ifdef CONFIG_MMC_MSM_SDC3_SUPPORT
			[GPIOMUX_ACTIVE]    = &sdc3_cmd_data_0_3_actv_cfg,
			[GPIOMUX_SUSPENDED] = &sdc3_suspend_cfg,
#else
			[GPIOMUX_ACTIVE]    = &sdc3_cmd_data_nc_cfg,
			[GPIOMUX_SUSPENDED] = &sdc3_cmd_data_nc_cfg,
#endif /* CONFIG_MMC_MSM_SDC3_SUPPORT */
		},
	},
	{
		/* CMD */
		.gpio      = 43,
		.settings = {
#ifdef CONFIG_MMC_MSM_SDC3_SUPPORT
			[GPIOMUX_ACTIVE]    = &sdc3_cmd_data_0_3_actv_cfg,
			[GPIOMUX_SUSPENDED] = &sdc3_suspend_cfg,
#else
			[GPIOMUX_ACTIVE]    = &sdc3_cmd_data_nc_cfg,
			[GPIOMUX_SUSPENDED] = &sdc3_cmd_data_nc_cfg,
#endif /* CONFIG_MMC_MSM_SDC3_SUPPORT */
		},
	},
	{
		/* CLK */
		.gpio      = 44,
		.settings = {
#ifdef CONFIG_MMC_MSM_SDC3_SUPPORT
			[GPIOMUX_ACTIVE]    = &sdc3_clk_actv_cfg,
			[GPIOMUX_SUSPENDED] = &sdc3_clk_suspend_cfg,
#else
			[GPIOMUX_ACTIVE]    = &sdc3_clk_nc_cfg,
			[GPIOMUX_SUSPENDED] = &sdc3_clk_nc_cfg,
#endif /* CONFIG_MMC_MSM_SDC3_SUPPORT */
		},
	},
};

static void msm_gpiomux_sdc3_install(void)
{
	msm_gpiomux_install(msm8226_sdc3_configs,
			    ARRAY_SIZE(msm8226_sdc3_configs));
}

#if defined(CONFIG_BT_BCM4334W)
static struct msm_gpiomux_config msm_btuart_configs[] __initdata = {
	{
		/* TXD */
		.gpio      = 20,
		.settings = {
			[GPIOMUX_SUSPENDED] = &gpio_uart_config,
		},
	},
	{
		/* RXD */
		.gpio      = 21,
		.settings = {
			[GPIOMUX_SUSPENDED] = &gpio_uart_config,
		},
	},
	{
		/* CTS */
		.gpio      = 22,
		.settings = {
			[GPIOMUX_SUSPENDED] = &gpio_uart_config,
		},
	},
	{
		/* RTS */
		.gpio      = 23,
		.settings = {
			[GPIOMUX_SUSPENDED] = &gpio_uart_config,
		},
	},
};
#endif

#ifdef CONFIG_SENSORS_SSP
static struct gpiomux_setting ssp_gpio_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct gpiomux_setting ssp_gpio_suspend_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct gpiomux_setting ssp_gpio_pu_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_UP,
};

static struct msm_gpiomux_config ssp_configs[] __initdata = {
	{
		.gpio = 28,	/*MCU_nRST_1.8V*/
		.settings = {
			[GPIOMUX_ACTIVE]    = &ssp_gpio_cfg,
			[GPIOMUX_SUSPENDED] = &ssp_gpio_suspend_cfg,
		},
	},
	{
		.gpio = 106,	/*MCU_AP_INT_1_1.8V*/
		.settings = {
			[GPIOMUX_ACTIVE]    = &ssp_gpio_pu_cfg,
			[GPIOMUX_SUSPENDED] = &ssp_gpio_pu_cfg,
		},
	},
	{
		.gpio = 107,	/*MCU_AP_INT_2_1.8V*/
		.settings = {
			[GPIOMUX_ACTIVE]    = &ssp_gpio_cfg,
			[GPIOMUX_SUSPENDED] = &ssp_gpio_suspend_cfg,
		},
	},
	{
		.gpio = 108,	/*AP_MCU_INT_1.8V*/
		.settings = {
			[GPIOMUX_ACTIVE]    = &ssp_gpio_cfg,
			[GPIOMUX_SUSPENDED] = &ssp_gpio_suspend_cfg,
		},
	},
};
#endif

#if defined(CONFIG_SND_SOC_DBMD2) || defined(CONFIG_SND_SOC_DBMD2P3)
static struct gpiomux_setting dbmd2_reset = {
	.pull = GPIOMUX_PULL_DOWN,
	.drv = GPIOMUX_DRV_2MA,
	.func = GPIOMUX_FUNC_GPIO,
	.dir = GPIOMUX_OUT_HIGH,
};

static struct gpiomux_setting dbmd2_wake_config = {
	.pull = GPIOMUX_PULL_NONE,
	.drv = GPIOMUX_DRV_2MA,
	.func = GPIOMUX_FUNC_GPIO,
	.dir = GPIOMUX_OUT_LOW,
};

static struct gpiomux_setting dbmd2_intrevent_config = {
	.pull = GPIOMUX_PULL_DOWN,
	.drv = GPIOMUX_DRV_2MA,
	.func = GPIOMUX_FUNC_GPIO,
	.dir = GPIOMUX_IN,
};

static struct msm_gpiomux_config dbmd2_config[] __initdata = {
	{
		.gpio   = 26,           /* dbmd2 reset */
		.settings = {
			[GPIOMUX_SUSPENDED] = &dbmd2_reset,
		},
	},
	{
		.gpio	= 27,		/* dbmd2 sensory int */
		.settings = {
			[GPIOMUX_SUSPENDED] = &dbmd2_intrevent_config,
		},
	},
	{
		.gpio	= 46,		/* dbmd2 wake */
		.settings = {
			[GPIOMUX_SUSPENDED] = &dbmd2_wake_config,
		},
	},
};
#endif

#ifdef CONFIG_MSM8226_UART4
static struct gpiomux_setting gpio_uart4_config = {
	.func = GPIOMUX_FUNC_2,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
	/*.dir = GPIOMUX_OUT_HIGH, */
};

static struct msm_gpiomux_config msm_uart4_config[] __initdata = {
	{
		.gpio	= 16,		/* dbmd2 uart tx */
		.settings = {
			[GPIOMUX_ACTIVE] = &gpio_uart4_config,
			[GPIOMUX_SUSPENDED] = &gpio_uart4_config,
		},
	},
	{
		.gpio	= 17,		/* dbmd2 uart rx */
		.settings = {
			[GPIOMUX_ACTIVE] = &gpio_uart4_config,
			[GPIOMUX_SUSPENDED] = &gpio_uart4_config,
		},
	},
};
#endif

static struct gpiomux_setting tsp_int_act_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct gpiomux_setting tsp_int_sus_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct msm_gpiomux_config msm_tsp_int_config[] __initdata = {
	{
		.gpio      = 49,
		.settings = {
			[GPIOMUX_ACTIVE] = &tsp_int_act_cfg,
			[GPIOMUX_SUSPENDED] = &tsp_int_sus_cfg,
		},
	},
};

extern int system_rev;

void __init msm8226_init_gpiomux(void)
{
	int rc;

	rc = msm_gpiomux_init_dt();
	if (rc) {
		pr_err("%s failed %d\n", __func__, rc);
		return;
	}

#if defined(CONFIG_KS8851) || defined(CONFIG_KS8851_MODULE)
	msm_gpiomux_install(msm_eth_configs, ARRAY_SIZE(msm_eth_configs));
#endif
	msm_gpiomux_install(msm8226_slimbus_config,
		ARRAY_SIZE(msm8226_slimbus_config));

	msm_gpiomux_install(msm_keypad_configs,
			ARRAY_SIZE(msm_keypad_configs));

	msm_gpiomux_install(msm_blsp_configs,
			ARRAY_SIZE(msm_blsp_configs));

#if defined(CONFIG_BT_BCM4334W)
	msm_gpiomux_install(msm_btuart_configs,
			    ARRAY_SIZE(msm_btuart_configs));
#endif

	msm_gpiomux_install(wcnss_5wire_interface,
				ARRAY_SIZE(wcnss_5wire_interface));

	msm_gpiomux_install(msm_tapan_config,
		ARRAY_SIZE(msm_tapan_config));
#if 0
	msm_gpiomux_install(&sd_card_det, 1);
#endif
#if 0
	if (of_board_is_skuf())
		msm_gpiomux_install(msm_skuf_goodix_configs,
				ARRAY_SIZE(msm_skuf_goodix_configs));
#endif

	msm_gpiomux_install(msm_skuf_nfc_configs,
			ARRAY_SIZE(msm_skuf_nfc_configs));

	if (system_rev > 0x00)
		msm_gpiomux_install_nowrite(msm_panel_configs,
				ARRAY_SIZE(msm_panel_configs));
	else
		msm_gpiomux_install_nowrite(msm_lcd_configs,
				ARRAY_SIZE(msm_lcd_configs));

	msm_gpiomux_install(msm_esd_configs,
			ARRAY_SIZE(msm_esd_configs));

#if 0 /* B3 doesn't have camera */
	msm_gpiomux_install(msm_sensor_configs, ARRAY_SIZE(msm_sensor_configs));

	if (of_board_is_skuf())
		msm_gpiomux_install(msm_sensor_configs_skuf_plus,
			ARRAY_SIZE(msm_sensor_configs_skuf_plus));
#endif
	msm_gpiomux_install(msm_auxpcm_configs,
			ARRAY_SIZE(msm_auxpcm_configs));

	msm_gpiomux_sdc3_install();

#if defined(CONFIG_IR_REMOCON_FPGA)
	msm_gpiomux_install(fpga_configs,
				ARRAY_SIZE(fpga_configs));
#endif

	/*
	 * HSIC STROBE gpio is also used by the ethernet. Install HSIC
	 * gpio mux config only when HSIC is enabled. HSIC config will
	 * be disabled when ethernet config is enabled.
	 */
#ifdef CONFIG_USB_EHCI_MSM_HSIC
	if (machine_is_msm8926()) {
		msm_hsic_configs[0].gpio = 119; /* STROBE */
		msm_hsic_configs[1].gpio = 120; /* DATA */
	}
	msm_gpiomux_install(msm_hsic_configs, ARRAY_SIZE(msm_hsic_configs));
#endif
#ifdef CONFIG_SND_SOC_MAX98504
		if ( system_rev >= 2 )	{
			msm_gpiomux_install(msm8226_tertiary_mi2s_configs,ARRAY_SIZE(msm8226_tertiary_mi2s_configs));
			msm_gpiomux_install(msm8226_blsp_codec_configs,ARRAY_SIZE(msm8226_blsp_codec_configs));
		}
#endif
	msm_gpiomux_install(fuel_i2c_config, ARRAY_SIZE(fuel_i2c_config));
	msm_gpiomux_install(muic_i2c_config, ARRAY_SIZE(muic_i2c_config));
	msm_gpiomux_install(pma_i2c_config, ARRAY_SIZE(pma_i2c_config));


#ifdef CONFIG_SENSORS_SSP
	msm_gpiomux_install(ssp_configs, ARRAY_SIZE(ssp_configs));
#endif
#ifndef CONFIG_MACH_WC1
	msm_gpiomux_install(b3_haptic_configs, ARRAY_SIZE(b3_haptic_configs));
#endif
	msm_gpiomux_install(wc1_nc_cfg, ARRAY_SIZE(wc1_nc_cfg));
	if(system_rev <= HW_REV_00)
		msm_gpiomux_install(exception_nc_config, ARRAY_SIZE(exception_nc_config));

	msm_gpiomux_install(wc1_hw_check_bit_cfg, ARRAY_SIZE(wc1_hw_check_bit_cfg));
#if defined(CONFIG_SND_SOC_DBMD2) || defined(CONFIG_SND_SOC_DBMD2P3)
	msm_gpiomux_install(dbmd2_config, ARRAY_SIZE(dbmd2_config));
#endif
#ifdef CONFIG_MSM8226_UART4
	msm_gpiomux_install(msm_uart4_config, ARRAY_SIZE(msm_uart4_config));
#endif
	msm_gpiomux_install(msm_detent_configs, ARRAY_SIZE(msm_detent_configs));

	if (system_rev < 3)
		msm_gpiomux_install(msm_ofm_configs, ARRAY_SIZE(msm_ofm_configs));

	msm_gpiomux_install(msm_tsp_int_config, ARRAY_SIZE(msm_tsp_int_config));
}

static void wcnss_switch_to_gpio(void)
{
	/* Switch MUX to GPIO */
	msm_gpiomux_install(wcnss_5gpio_interface,
			ARRAY_SIZE(wcnss_5gpio_interface));

	/* Ensure GPIO config */
	gpio_direction_input(WLAN_DATA2);
	gpio_direction_input(WLAN_DATA1);
	gpio_direction_input(WLAN_DATA0);
	gpio_direction_output(WLAN_SET, 0);
	gpio_direction_output(WLAN_CLK, 0);
}

static void wcnss_switch_to_5wire(void)
{
	msm_gpiomux_install(wcnss_5wire_interface,
			ARRAY_SIZE(wcnss_5wire_interface));
}

u32 wcnss_rf_read_reg(u32 rf_reg_addr)
{
	int count = 0;
	u32 rf_cmd_and_addr = 0;
	u32 rf_data_received = 0;
	u32 rf_bit = 0;

	wcnss_switch_to_gpio();

	/* Reset the signal if it is already being used. */
	gpio_set_value(WLAN_SET, 0);
	gpio_set_value(WLAN_CLK, 0);

	/* We start with cmd_set high WLAN_SET = 1. */
	gpio_set_value(WLAN_SET, 1);

	gpio_direction_output(WLAN_DATA0, 1);
	gpio_direction_output(WLAN_DATA1, 1);
	gpio_direction_output(WLAN_DATA2, 1);

	gpio_set_value(WLAN_DATA0, 0);
	gpio_set_value(WLAN_DATA1, 0);
	gpio_set_value(WLAN_DATA2, 0);

	/* Prepare command and RF register address that need to sent out.
	 * Make sure that we send only 14 bits from LSB.
	 */
	rf_cmd_and_addr  = (((WLAN_RF_READ_REG_CMD) |
		(rf_reg_addr << WLAN_RF_REG_ADDR_START_OFFSET)) &
		WLAN_RF_READ_CMD_MASK);

	for (count = 0; count < 5; count++) {
		gpio_set_value(WLAN_CLK, 0);

		rf_bit = (rf_cmd_and_addr & 0x1);
		gpio_set_value(WLAN_DATA0, rf_bit ? 1 : 0);
		rf_cmd_and_addr = (rf_cmd_and_addr >> 1);

		rf_bit = (rf_cmd_and_addr & 0x1);
		gpio_set_value(WLAN_DATA1, rf_bit ? 1 : 0);
		rf_cmd_and_addr = (rf_cmd_and_addr >> 1);

		rf_bit = (rf_cmd_and_addr & 0x1);
		gpio_set_value(WLAN_DATA2, rf_bit ? 1 : 0);
		rf_cmd_and_addr = (rf_cmd_and_addr >> 1);

		/* Send the data out WLAN_CLK = 1 */
		gpio_set_value(WLAN_CLK, 1);
	}

	/* Pull down the clock signal */
	gpio_set_value(WLAN_CLK, 0);

	/* Configure data pins to input IO pins */
	gpio_direction_input(WLAN_DATA0);
	gpio_direction_input(WLAN_DATA1);
	gpio_direction_input(WLAN_DATA2);

	for (count = 0; count < 2; count++) {
		gpio_set_value(WLAN_CLK, 1);
		gpio_set_value(WLAN_CLK, 0);
	}

	rf_bit = 0;
	for (count = 0; count < 6; count++) {
		gpio_set_value(WLAN_CLK, 1);
		gpio_set_value(WLAN_CLK, 0);

		rf_bit = gpio_get_value(WLAN_DATA0);
		rf_data_received |= (rf_bit << (count * 3 + 0));

		if (count != 5) {
			rf_bit = gpio_get_value(WLAN_DATA1);
			rf_data_received |= (rf_bit << (count * 3 + 1));

			rf_bit = gpio_get_value(WLAN_DATA2);
			rf_data_received |= (rf_bit << (count * 3 + 2));
		}
	}

	gpio_set_value(WLAN_SET, 0);
	wcnss_switch_to_5wire();

	return rf_data_received;
}
