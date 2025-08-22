/* drivers/motor/isa1000a_vibrator.c
* Copyright (C) 2016 Samsung Electronics Co. Ltd. All Rights Reserved.
*
* Author: Sanghyeon Lee <sirano06.lee@samsung.com>\
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

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/input.h>
#include <linux/pwm.h>
#include <linux/platform_device.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/clk.h>
#include <linux/regulator/consumer.h>
#include <linux/isa1000a_vibrator.h>
#include <linux/gpio.h>
#include <linux/of_platform.h>
#include <linux/of_gpio.h>
#include <linux/of_device.h>
#include <linux/of_irq.h>
#include <linux/of.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/pm_qos.h>

#define MAX_MAGNITUDE		0xffff

extern struct class *sec_class;
struct device *motor_device;
struct isa1000a_vibrator_data *g_hap_data;
static struct pm_qos_request motor_dma_qos;

static int pwm_gpio;
static int motor_en_gpio;

enum ISA1000A_VIBRATOR_CONTROL {
	ISA1000A_VIBRATOR_DISABLE = 0,
	ISA1000A_VIBRATOR_ENABLE = 1,
};

struct isa1000a_vibrator_data {
	struct device *dev;
	struct input_dev *input_dev;
	struct isa1000a_vibrator_platform_data *pdata;
	struct pwm_device *pwm;
	struct regulator *regulator;
	struct work_struct work;
	int max_mV;
	int min_mV;
	int intensity; /* mV */
	int level;
	spinlock_t lock;
	bool resumed;
	bool enabled;
};

int32_t vibe_pwm_onoff(u8 onoff)
{
	if (onoff) {
		HWIO_OUTM(GP1_CMD_RCGR,HWIO_UPDATE_VAL_BMSK,
					1 << HWIO_UPDATE_VAL_SHFT);//UPDATE ACTIVE
		HWIO_OUTM(GP1_CMD_RCGR,HWIO_ROOT_EN_VAL_BMSK,
					1 << HWIO_ROOT_EN_VAL_SHFT);//ROOT_EN
		HWIO_OUTM(CAMSS_GP1_CBCR, HWIO_CLK_ENABLE_VAL_BMSK,
					1 << HWIO_CLK_ENABLE_VAL_SHFT); //CLK_ENABLE
	} else {
		HWIO_OUTM(GP1_CMD_RCGR,HWIO_UPDATE_VAL_BMSK,
					0 << HWIO_UPDATE_VAL_SHFT);
		HWIO_OUTM(GP1_CMD_RCGR,HWIO_ROOT_EN_VAL_BMSK,
					0 << HWIO_ROOT_EN_VAL_SHFT);
		HWIO_OUTM(CAMSS_GP1_CBCR, HWIO_CLK_ENABLE_VAL_BMSK,
					0 << HWIO_CLK_ENABLE_VAL_SHFT);
	}
	return VIBE_S_SUCCESS;
}

static void vib_pwm(int nForce)
{
	/* Put the MND counter in reset mode for programming */
	HWIO_OUTM(GP1_CFG_RCGR, HWIO_GP_SRC_SEL_VAL_BMSK,
				0 << HWIO_GP_SRC_SEL_VAL_SHFT); //SRC_SEL = 000(cxo)

	HWIO_OUTM(GP1_CFG_RCGR, HWIO_GP_SRC_DIV_VAL_BMSK,
				23 << HWIO_GP_SRC_DIV_VAL_SHFT); //SRC_DIV = 10111 (Div 12)

	HWIO_OUTM(GP1_CFG_RCGR, HWIO_GP_MODE_VAL_BMSK,
				2 << HWIO_GP_MODE_VAL_SHFT); //Mode Select 10
	//M value
	HWIO_OUTM(GP_M_REG, HWIO_GP_MD_REG_M_VAL_BMSK,
		g_nlra_gp_clk_m << HWIO_GP_MD_REG_M_VAL_SHFT);

	if (nForce > 0) {
		g_nforce_32 = g_nlra_gp_clk_n - (((nForce * g_nlra_gp_clk_pwm_mul) >> 8));
		g_nforce_32 = g_nforce_32 * motor_strength /100;
		if(g_nforce_32 < motor_min_strength)
			g_nforce_32 = motor_min_strength;
	} else {
		g_nforce_32 = ((nForce * g_nlra_gp_clk_pwm_mul) >> 8) + g_nlra_gp_clk_d;
		if(g_nlra_gp_clk_n - g_nforce_32 > g_nlra_gp_clk_n * motor_strength /100)
			g_nforce_32 = g_nlra_gp_clk_n - g_nlra_gp_clk_n * motor_strength /100;
	}

	//D value
	HWIO_OUTM(GP_D_REG, HWIO_GP_MD_REG_D_VAL_BMSK,
	 (~((int16_t)g_nforce_32 << 1)) << HWIO_GP_MD_REG_D_VAL_SHFT);

	//N value
	HWIO_OUTM(GP_NS_REG, HWIO_GP_NS_REG_GP_N_VAL_BMSK,
	 ~(g_nlra_gp_clk_n - g_nlra_gp_clk_m) << 0);
}

static void vib_en(bool en)
{
	pr_info("[VIB] %s %s\n", __func__, en ? "on" : "off");
	if (en) {
		if (!pm_qos_request_active(&motor_dma_qos)) {
                        pm_qos_add_request(&motor_dma_qos,
                                PM_QOS_CPU_DMA_LATENCY, 1);
                }

		if (!regulator_is_enabled(g_hap_data->regulator))
			regulator_enable(g_hap_data->regulator);

		gpio_tlmm_config(GPIO_CFG(
			motor_en_gpio, 0,
		       GPIO_CFG_OUTPUT,
		       GPIO_CFG_PULL_UP,
		       GPIO_CFG_4MA),
		       GPIO_CFG_ENABLE);

		gpio_set_value(motor_en_gpio, 1);

		gpio_tlmm_config(GPIO_CFG(
		       pwm_gpio, 3,
		       GPIO_CFG_OUTPUT,
		       GPIO_CFG_PULL_UP,
		       GPIO_CFG_4MA),
		       GPIO_CFG_ENABLE);
	} else {
		if (regulator_is_enabled(g_hap_data->regulator))
			regulator_disable(g_hap_data->regulator);

		gpio_tlmm_config(GPIO_CFG(
		       motor_en_gpio, 0,
		       GPIO_CFG_INPUT,
		       GPIO_CFG_PULL_DOWN,
		       GPIO_CFG_4MA),
		       GPIO_CFG_ENABLE);

		/*msleep(1);*/
		gpio_set_value(motor_en_gpio, 0);

		gpio_tlmm_config(GPIO_CFG(
		       pwm_gpio, 0,
		       GPIO_CFG_INPUT,
		       GPIO_CFG_PULL_DOWN,
		       GPIO_CFG_4MA),
		       GPIO_CFG_ENABLE);

		if (pm_qos_request_active(&motor_dma_qos))
                        pm_qos_remove_request(&motor_dma_qos);
	}
	return;
}

static int vibe_clk_on(struct device *dev, bool en)
{
	struct clk *vibe_clk = NULL;

#if defined(CONFIG_OF)
	struct device_node *np;
	np = of_find_node_by_name(NULL, "pwm");
	if (np == NULL) {
		printk("%s : pwm error to get dt node\n", __func__);
		return -EINVAL;
	}
#else
	vibe_clk = clk_get(dev, "timers");
#endif
	pr_info("[VIB] DEV NAME %s %lu\n",
		 dev_name(dev), clk_get_rate(vibe_clk));

	if (IS_ERR(vibe_clk)) {
		pr_err("[VIB] failed to get clock for the motor\n");
		goto err_clk_get;
	}

	if (en)
		clk_enable(vibe_clk);
	else
		clk_disable(vibe_clk);

	clk_put(vibe_clk);
	return 0;

err_clk_get:
	clk_put(vibe_clk);
	return -EINVAL;
}


static void haptic_work(struct work_struct *work)
{
	struct isa1000a_vibrator_data *haptic = container_of(work,
						       struct isa1000a_vibrator_data,
						       work);
	if (haptic->level)
		vib_en((bool)ISA1000A_VIBRATOR_ENABLE);
	else
		vib_en((bool)ISA1000A_VIBRATOR_DISABLE);
}


static int isa1000a_haptic_play(struct input_dev *input, void *data,
				struct ff_effect *effect)
{
	struct isa1000a_vibrator_data *haptic = input_get_drvdata(input);

	haptic->level = effect->u.rumble.strong_magnitude;
	if (!haptic->level)
		haptic->level = effect->u.rumble.weak_magnitude;

	haptic->intensity =
		(haptic->max_mV - haptic->min_mV) * haptic->level /
		MAX_MAGNITUDE;
	haptic->intensity = haptic->intensity + haptic->min_mV;

	if (haptic->intensity > haptic->max_mV)
		haptic->intensity = haptic->max_mV;
	if (haptic->intensity < haptic->min_mV)
		haptic->intensity = haptic->min_mV;

	if (haptic->level != 0) {
                if (haptic->level > 0)
                        haptic->level = 128 - (haptic->level >> 8) / 2;
                else
                        haptic->level = 0;

                vib_pwm(haptic->level);
        }

	vibe_pwm_onoff((haptic->level > 0) ?
					ISA1000A_VIBRATOR_ENABLE :
					ISA1000A_VIBRATOR_DISABLE);

	schedule_work(&haptic->work);

	return 0;
}



EXPORT_SYMBOL(vibe_pwm_onoff);


#if defined(CONFIG_MACH_WC1) || defined(CONFIG_MACH_NC1)
static ssize_t motor_control_show_motor_on(struct device *dev, struct device_attribute *attr, char *buf)
{
	/* printk("######## %s\n", __func__); */
	vib_en(1);
	return 0;
}

static ssize_t motor_control_show_motor_off(struct device *dev, struct device_attribute *attr, char *buf)
{
	/* printk("######## %s\n", __func__); */
	vib_en(0);
	return 0;
}
#endif

#if defined(CONFIG_MACH_WC1) || defined(CONFIG_MACH_NC1)
static DEVICE_ATTR(motor_on, S_IRUGO, motor_control_show_motor_on, NULL);
static DEVICE_ATTR(motor_off, S_IRUGO, motor_control_show_motor_off, NULL);

static struct attribute *motor_control_attributes[] = {
	&dev_attr_motor_on.attr,
	&dev_attr_motor_off.attr,
	NULL
};
static const struct attribute_group motor_control_group = {
	.attrs = motor_control_attributes,
};
#endif

static void isa1000a_haptic_close(struct input_dev *input)
{
	struct isa1000a_vibrator_data *haptic = input_get_drvdata(input);

	cancel_work_sync(&haptic->work);
	vib_en(ISA1000A_VIBRATOR_DISABLE);
}

#if defined(CONFIG_OF)
static int of_isa1000a_vibrator_dt(struct isa1000a_vibrator_platform_data *pdata)
{
	struct device_node *np_haptic;
	int temp;
	const char *temp_str;

	pr_info("[VIB] ++ %s\n", __func__);

	np_haptic = of_find_node_by_path("/soc/isa1000a-vibrator");
	if (np_haptic == NULL) {
		printk("%s : error to get dt node\n", __func__);
		return -EINVAL;
	}

	of_property_read_u32(np_haptic, "haptic,max_timeout", &temp);
	pdata->max_timeout = temp;

	of_property_read_u32(np_haptic, "haptic,duty", &temp);
	pdata->duty = temp;

	of_property_read_u32(np_haptic, "haptic,period", &temp);
	pdata->period = temp;

	of_property_read_u32(np_haptic, "haptic,pwm_id", &temp);
	pdata->pwm_id = temp;

	motor_en_gpio = of_get_named_gpio(np_haptic, "motor_en", 0);
	pwm_gpio = of_get_named_gpio(np_haptic, "pmic_vib_pwm", 0);

	of_property_read_string(np_haptic, "haptic,regulator_name", &temp_str);
	pdata->regulator_name = (char *)temp_str;

/* debugging */
	printk("%s : max_timeout = %d\n", __func__, pdata->max_timeout);
	printk("%s : duty = %d\n", __func__, pdata->duty);
	printk("%s : period = %d\n", __func__, pdata->period);
	printk("%s : pwm_id = %d\n", __func__, pdata->pwm_id);
	printk("%s : regulator_name = %s\n", __func__, pdata->regulator_name);

	return 0;
}
#endif /* CONFIG_OF */

static int isa1000a_vibrator_probe(struct platform_device *pdev)
{
	int ret;
	int error = 0;
	struct input_dev *input_dev;

#if !defined(CONFIG_OF)
	struct isa1000a_vibrator_platform_data *isa1000a_pdata
		= dev_get_platdata(&pdev->dev);
#endif
	struct isa1000a_vibrator_data *hap_data;

	pr_info("[VIB] ++ %s\n", __func__);

	hap_data = kzalloc(sizeof(struct isa1000a_vibrator_data), GFP_KERNEL);
	if (!hap_data)
		return -ENOMEM;

	input_dev = input_allocate_device();
	if (!input_dev) {
		dev_err(&pdev->dev, "unable to allocate memory\n");
		error =  -ENOMEM;
		goto err_kfree_mem;
	}

#if defined(CONFIG_OF)
	hap_data->pdata = kzalloc(sizeof(struct isa1000a_vibrator_data), GFP_KERNEL);
	if (!hap_data->pdata)
		goto err_kfree_mem;

	ret = of_isa1000a_vibrator_dt(hap_data->pdata);
	if (ret < 0) {
		pr_info("isa1000a_vibrator : %s not found haptic dt! ret[%d]\n",
				 __func__, ret);
		goto err_free_input;
	}

	virt_mmss_gp1_base = ioremap(MSM_MMSS_GP1_BASE,0x28);
	if (!virt_mmss_gp1_base)
		panic("tspdrv : Unable to ioremap MSM_MMSS_GP1 memory!");
#else
	hap_data->pdata = isa1000a_pdata;

	if (pdata == NULL) {
		pr_info("%s: no pdata\n", __func__);
		goto err_free_input;
	}
#endif /* CONFIG_OF */

	platform_set_drvdata(pdev, hap_data);
	g_hap_data = hap_data;
	hap_data->dev = &pdev->dev;
	hap_data->input_dev = input_dev;
	INIT_WORK(&hap_data->work, haptic_work);
	spin_lock_init(&(hap_data->lock));
	hap_data->input_dev->name = "isa1000a_haptic";
	hap_data->input_dev->dev.parent = &pdev->dev;
	hap_data->input_dev->close = isa1000a_haptic_close;
	hap_data->enabled = false;

	input_set_drvdata(hap_data->input_dev, hap_data);
	input_set_capability(hap_data->input_dev, EV_FF, FF_RUMBLE);
	error = input_ff_create_memless(input_dev, NULL,
		isa1000a_haptic_play);

	if (error) {
		dev_err(&pdev->dev,
			"input_ff_create_memless() failed: %d\n",
			error);
		goto err_kfree_pdata;
	}

	error = input_register_device(hap_data->input_dev);
	if (error) {
		dev_err(&pdev->dev,
			"couldn't register input device: %d\n",
			error);
		goto err_destroy_ff;
	}

	hap_data->regulator
			= regulator_get(NULL, hap_data->pdata->regulator_name);
	if (IS_ERR(hap_data->regulator)) {
		pr_info("[VIB] Failed to get vmoter regulator.\n");
		error = -EFAULT;
		goto err_unregister_input;
	}
	regulator_set_voltage(g_hap_data->regulator, MOTOR_VDD, MOTOR_VDD);
	
	ret = sysfs_create_group(&motor_device->kobj, &motor_control_group);
	if (ret) {
		pr_info("%s: failed to create motor control attribute group\n", __func__);
		goto
			err_regulator_put;
	}
	vibe_clk_on(&pdev->dev, true);
	platform_set_drvdata(pdev, hap_data);
	pr_info("[VIB] -- %s\n", __func__);

	return error;


err_regulator_put:
	regulator_put(hap_data->regulator);
err_unregister_input:
	input_unregister_device(hap_data->input_dev);
err_destroy_ff:
	input_ff_destroy(hap_data->input_dev);
err_kfree_pdata:
	kfree(hap_data->pdata);
err_free_input:
	input_free_device(hap_data->input_dev);
err_kfree_mem:
	kfree(hap_data);

	return error;
}

static int __devexit isa1000a_vibrator_remove(struct platform_device *pdev)
{
	struct isa1000a_vibrator_data *data = platform_get_drvdata(pdev);

	iounmap(virt_mmss_gp1_base);
	regulator_put(data->regulator);
	pwm_free(data->pwm);
	kfree(data->pdata);
	kfree(data);
	g_hap_data = NULL;
	return 0;
}

#if defined(CONFIG_OF)
static struct of_device_id haptic_dt_ids[] = {
	{ .compatible = "isa1000a-vibrator" },
	{ },
};
MODULE_DEVICE_TABLE(of, haptic_dt_ids);
#endif /* CONFIG_OF */

static int isa1000a_vibrator_suspend(struct platform_device *pdev,
			pm_message_t state)
{
	pr_info("[VIB] %s\n", __func__);
	if (g_hap_data != NULL) {
		cancel_work_sync(&g_hap_data->work);
	}
	vibe_clk_on(&pdev->dev, false);
	return 0;
}

static int isa1000a_vibrator_resume(struct platform_device *pdev)
{
	pr_info("[VIB] %s\n", __func__);
	vibe_clk_on(&pdev->dev, true);
	g_hap_data->resumed = true;
	return 0;
}

static struct platform_driver isa1000a_vibrator_driver = {
	.probe		= isa1000a_vibrator_probe,
	.remove		= isa1000a_vibrator_remove,
	.suspend	= isa1000a_vibrator_suspend,
	.resume		= isa1000a_vibrator_resume,
	.driver = {
		.name	= "isa1000a-vibrator",
		.owner	= THIS_MODULE,
#if defined(CONFIG_OF)
		.of_match_table = haptic_dt_ids,
#endif /* CONFIG_OF */
	},
};

static int __init isa1000a_vibrator_init(void)
{
	motor_device = device_create(sec_class, NULL, 0, NULL, "motor");
	if (IS_ERR(motor_device)) {
		pr_err("%s Failed to create device(motor)!\n", __func__);
		return -ENODEV;
	}
	
	pr_debug("[VIB] %s\n", __func__);
	pr_info("[VIB] %s\n", __func__);

	return platform_driver_register(&isa1000a_vibrator_driver);
}
module_init(isa1000a_vibrator_init);

static void __exit isa1000a_vibrator_exit(void)
{
	platform_driver_unregister(&isa1000a_vibrator_driver);
}
module_exit(isa1000a_vibrator_exit);

MODULE_AUTHOR("Samsung Electronics");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("ISA1000A motor driver");
