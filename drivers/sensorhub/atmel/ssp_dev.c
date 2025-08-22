/*
 *  Copyright (C) 2012, Samsung Electronics Co. Ltd. All Rights Reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */
#include "ssp.h"
#ifdef CONFIG_OF
#include <linux/of_gpio.h>
#endif
#ifdef CONFIG_KNOX_GEARPAY
#include <linux/knox_gearlock.h>
#endif
#if SSP_SEC_DEBUG
#include <mach/sec_debug.h>

#undef SSP_IRQ_EDGE_PROTECT
#if defined SSP_IRQ_EDGE_PROTECT
static int prevent_irq;
#endif
#endif

#ifdef CONFIG_HAS_EARLYSUSPEND
static void ssp_early_suspend(struct early_suspend *handler);
static void ssp_late_resume(struct early_suspend *handler);
#endif
#define NORMAL_SENSOR_STATE_K	0x3FEFF
struct ssp_data *sensorhub_data;

static int ssp_check_lpmode(void)
{
#ifdef CONFIG_SAMSUNG_LPM_MODE
	if (poweroff_charging == 0x01)
		return true;
	else
		return false;
#else
	return false;
#endif
}

void ssp_enable(struct ssp_data *data, bool enable)
{
	pr_info("[SSP] %s, enable = %d, old enable = %d\n",
		__func__, enable, data->bSspShutdown);

	if (enable && data->bSspShutdown) {
		data->bSspShutdown = false;
		enable_irq(data->iIrq);
		enable_irq_wake(data->iIrq);
	} else if (!enable && !data->bSspShutdown) {
		data->bSspShutdown = true;
		disable_irq(data->iIrq);
		disable_irq_wake(data->iIrq);
	} else
		pr_err("[SSP] %s, enable error\n", __func__);
}
/************************************************************************/
/* interrupt happened due to transition/change of SSP MCU		*/
/************************************************************************/

static irqreturn_t sensordata_irq_thread_fn(int iIrq, void *dev_id)
{
	struct ssp_data *data = dev_id;

	wake_lock(&data->ssp_wake_lock);
#if defined SSP_IRQ_EDGE_PROTECT
	if (prevent_irq != 0) {
		select_irq_msg(data);
		data->uIrqCnt++;
#if SSP_STATUS_MONITOR
		data->uSubIrqCnt++;
#endif
	} else {
		pr_err("[SSP] %s, irq Occured while Boot.\n", __func__);
	}
#else
	if (gpio_get_value(data->mcu_int1))
		pr_info("[SSP] MCU int HIGH");

	select_irq_msg(data);
	data->uIrqCnt++;
#if SSP_STATUS_MONITOR
	data->uSubIrqCnt++;
#endif
#endif
	wake_unlock(&data->ssp_wake_lock);

	return IRQ_HANDLED;
}

/*************************************************************************/
/* initialize sensor hub						 */
/*************************************************************************/

static void initialize_variable(struct ssp_data *data)
{
	int iSensorIndex;

	for (iSensorIndex = 0; iSensorIndex < SENSOR_MAX; iSensorIndex++) {
		data->adDelayBuf[iSensorIndex] = DEFUALT_POLLING_DELAY;
		data->batchLatencyBuf[iSensorIndex] = 0;
		data->batchOptBuf[iSensorIndex] = 0;
		data->aiCheckStatus[iSensorIndex] = INITIALIZATION_STATE;
	}
	data->adDelayBuf[BIO_HRM_LIB] = (100 * NSEC_PER_MSEC);

	atomic_set(&data->aSensorEnable, 0);
	data->iLibraryLength = 0;
	data->uSensorState = NORMAL_SENSOR_STATE_K;
	data->uMagCntlRegData = 1;

	data->uResetCnt = 0;
	data->uTimeOutCnt = 0;
	data->uComFailCnt = 0;
	data->uIrqCnt = 0;
#if SSP_STATUS_MONITOR
	data->uSubIrqCnt = 0;
#endif

	data->bSspShutdown = true;
	data->bGeomagneticRawEnabled = false;
	data->bAccelAlert = false;
	data->bLpModeEnabled = false;
	data->bTimeSyncing = true;
#if SSP_STATUS_MONITOR
	data->bRefreshing = false;
#endif

	data->accelcal.x = 0;
	data->accelcal.y = 0;
	data->accelcal.z = 0;

	data->gyrocal.x = 0;
	data->gyrocal.y = 0;
	data->gyrocal.z = 0;

	data->magoffset.x = 0;
	data->magoffset.y = 0;
	data->magoffset.z = 0;

	data->iPressureCal = 0;
	data->sealevelpressure = 0;

	data->uGyroDps = GYROSCOPE_DPS2000;

	memset(data->hrmcal, 0x00, sizeof(data->hrmcal));

	data->mcu_device = NULL;
	data->acc_device = NULL;
	data->gyro_device = NULL;
	data->mag_device = NULL;
	data->prs_device = NULL;
	data->light_device = NULL;
	data->hrm_device = NULL;
#ifdef CONFIG_KNOX_GEARPAY
	data->strap_status = -1;
#endif

	data->voice_device = NULL;
#if SSP_SEC_DEBUG
	data->bMcuDumpMode = sec_debug_is_enabled();
#endif
	INIT_LIST_HEAD(&data->pending_list);

	initialize_function_pointer(data);
}

int initialize_mcu(struct ssp_data *data)
{
	int iRet = 0;

	clean_pending_list(data);

	iRet = get_chipid(data);
	pr_info("[SSP] MCU device ID = %d, reading ID = %d\n", DEVICE_ID, iRet);
	if (iRet != DEVICE_ID) {
		if (iRet < 0) {
			pr_err("[SSP]: %s - MCU is not working : 0x%x\n",
				__func__, iRet);
		} else {
			pr_err("[SSP]: %s - MCU identification failed\n",
				__func__);
			iRet = -ENODEV;
		}
		goto out;
	}

	iRet = set_sensor_position(data);
	if (iRet < 0) {
		pr_err("[SSP]: %s - set_sensor_position failed\n", __func__);
		goto out;
	}

	data->uSensorState = get_sensor_scanning_info(data);
	if (data->uSensorState == 0) {
		pr_err("[SSP]: %s - get_sensor_scanning_info failed\n",
			__func__);
		iRet = ERROR;
		goto out;
	}

#ifdef CONFIG_SENSORS_SSP_MAGNETIC_SENSOR
	iRet = initialize_magnetic_sensor(data);
	if (iRet < 0)
		pr_err("[SSP]: %s - initialize magnetic sensor failed\n",
			__func__);
#endif

	data->uCurFirmRev = get_firmware_rev(data);
	pr_info("[SSP] MCU Firm Rev : New = %8u\n",
		data->uCurFirmRev);

out:
	return iRet;
}

static int initialize_irq(struct ssp_data *data)
{
	int iRet, iIrq;
	iIrq = gpio_to_irq(data->mcu_int1);

	pr_info("[SSP] requesting IRQ %d\n", iIrq);
	iRet = request_threaded_irq(iIrq, NULL, sensordata_irq_thread_fn,
				    IRQF_TRIGGER_FALLING|IRQF_ONESHOT|
				    IRQF_NO_SUSPEND, "SSP_Int", data);
	if (iRet < 0) {
		pr_err("[SSP] %s, request_irq(%d) failed for gpio %d (%d)\n",
		       __func__, iIrq, iIrq, iRet);
		goto err_request_irq;
	}

	/* start with interrupts disabled */
	data->iIrq = iIrq;
	disable_irq(data->iIrq);
	return 0;

err_request_irq:
	gpio_free(data->mcu_int1);
	return iRet;
}

#ifdef CONFIG_KNOX_GEARPAY
static void ssp_payment_work_strap(struct work_struct *work)
{
	struct ssp_data *ssp_data = container_of(work,struct ssp_data, payment_work);
	int ret=0;
	pr_info("[SSP]%s, STRAP = %d \n", __func__, ssp_data->strap_status);

	if (ssp_data->strap_status == 0x01) {
		ret = qcom_smc_gearpay(STRAP, 1);
	}
	else if (ssp_data->strap_status == 0x02) {
		ret = qcom_smc_gearpay(STRAP, 0);
	}

	if(ret) {
		pr_err("[SSP]%s, STRAP Event SMC failure : %d\n", __func__, ret);
	}
}
#endif


static void work_function_firmware_update(struct work_struct *work)
{
	struct ssp_data *data = container_of((struct delayed_work *)work,
				struct ssp_data, work_firmware);
	int iRet;

	pr_info("[SSP] : %s\n", __func__);

	iRet = forced_to_download_binary(data, KERNEL_BINARY);
	if (iRet < 0) {
		ssp_dbg("[SSP]: %s - forced_to_download_binary failed!\n",
			__func__);
		data->uSensorState = 0;
		return;
	}

	queue_refresh_task(data, SSP_SW_RESET_TIME);

#ifdef CONFIG_SENSORS_SSP_LPM_MOTION
	if (data->check_lpmode() == true) {
		data->bLpModeEnabled = true;
		pr_info("[SSP]: LPM Charging...\n");
	} else {
		data->bLpModeEnabled = false;
		pr_info("[SSP]: Normal Booting OK\n");
	}
#endif

	pr_info("[SSP] : %s done\n!", __func__);
}

#ifdef CONFIG_OF
static int ssp_parse_dt(struct device *dev,
	struct ssp_data *data)
{
	struct device_node *np = dev->of_node;
	enum of_gpio_flags flags;
	int errorno = 0;

	data->mcu_int1 = of_get_named_gpio_flags(np, "ssp,mcu_int1-gpio",
		0, &flags);
	if (data->mcu_int1 < 0) {
		errorno = data->mcu_int1;
		goto dt_exit;
	}

	data->mcu_int2 = of_get_named_gpio_flags(np, "ssp,mcu_int2-gpio",
		0, &flags);
	if (data->mcu_int2 < 0) {
		errorno = data->mcu_int2;
		goto dt_exit;
	}

	data->ap_int = of_get_named_gpio_flags(np, "ssp,ap_int-gpio",
		0, &flags);
	if (data->ap_int < 0) {
		errorno = data->ap_int;
		goto dt_exit;
	}

	data->rst = of_get_named_gpio_flags(np, "ssp,rst-gpio",
		0, &flags);
	if (data->rst < 0) {
		errorno = data->rst ;
		goto dt_exit;
	}

	if (of_property_read_u32(np, "ssp,acc-position", &data->accel_position))
		data->accel_position = 0;

	if (of_property_read_u32(np, "ssp,mag-position", &data->mag_position))
		data->mag_position = 0;

	pr_info("[SSP] acc-posi[%d] mag-posi[%d]\n",
			data->accel_position, data->mag_position);

	if (of_property_read_u32(np, "ssp,ap-rev", &data->ap_rev))
		data->ap_rev = 0;
	pr_info("[SSP] ap_rev = [%d]\n", data->ap_rev);

	if (of_property_read_string(np, "ssp,fw_name", &data->fw_name))
		pr_err("[SSP] %s, fail to get fw name\n", __func__);

	pr_err("[SSP] %s, fw name = %s\n", __func__, data->fw_name);

	if (of_property_read_string(np, "ssp,ums_fw_name", &data->ums_fw_name))
		pr_err("[SSP] %s, fail to get ums_fw name\n", __func__);

#if SSP_STATUS_MONITOR
	data->reg_hub = devm_regulator_get(dev, "hub_vreg");
	if (IS_ERR(data->reg_hub)) {
		pr_err("[SSP] could not get hub_vreg, %ld\n",
			PTR_ERR(data->reg_hub));
		data->reg_hub = 0;
	} else {
		errorno = regulator_enable(data->reg_hub);
		if (errorno) {
			dev_err(dev, "Failed to enable reg_hub regulators: %d\n",
				errorno);
			goto dt_exit;
		}
	}
#endif

	errorno = gpio_request(data->mcu_int1, "mcu_ap_int1");
	if (errorno) {
		pr_err("[SSP] failed to request MCU_INT1 for SSP\n");
		goto dt_exit;
	}
	errorno = gpio_direction_input(data->mcu_int1);
	if (errorno) {
		pr_err("[SSP] failed to set mcu_int1 as input\n");
		goto dt_exit;
	}
	errorno = gpio_request(data->mcu_int2, "MCU_INT2");
	if (errorno) {
		pr_err("[SSP] failed to request MCU_INT2 for SSP\n");
		goto dt_exit;
	}
	gpio_direction_input(data->mcu_int2);

	errorno = gpio_request(data->ap_int, "AP_MCU_INT");
	if (errorno) {
		pr_err("[SSP] failed to request AP_INT for SSP\n");
		goto dt_exit;
	}
	gpio_direction_output(data->ap_int, 1);

	errorno = gpio_request(data->rst, "MCU_RST");
	if (errorno) {
		pr_err("[SSP] failed to request MCU_RST for SSP\n");
		goto dt_exit;
	}
	gpio_direction_output(data->rst, 1);

dt_exit:
	return errorno;
}
#else
static int ssp_parse_dt(struct device *dev,
	struct ssp_data *data)
{
	return -1;
}
#endif

#ifdef CONFIG_SLEEP_MONITOR
#define SHUB_LIB_DATA_APDR	1
#define SHUB_LIB_DATA_CALL_POSE	2
#define SHUB_LIB_DATA_PEDOMETER	3
#define SHUB_LIB_DATA_MOTION	4
#define SHUB_LIB_DATA_APPROACH	5
#define SHUB_LIB_DATA_STEP_COUNT_ALERT	6
#define SHUB_LIB_DATA_AUTO_ROTATION	7
#define SHUB_LIB_DATA_MOVEMENT	8
#define SHUB_LIB_DATA_MOVEMENT_FOR_POSITIONING	9
#define SHUB_LIB_DATA_DIRECT_CALL	10
#define SHUB_LIB_DATA_STOP_ALERT	11
#define SHUB_LIB_DATA_ENVIRONMENT_SENSOR	12
#define SHUB_LIB_DATA_SHAKE_MOTION	13
#define SHUB_LIB_DATA_FLIP_COVER_ACTION	14
#define SHUB_LIB_DATA_GYRO_TEMPERATURE	15
#define SHUB_LIB_DATA_PUT_DOWN_MOTION	16
#define SHUB_LIB_DATA_BOUNCE_SHORT_MOTION	18
#define SHUB_LIB_DATA_WRIST_UP	19
#define SHUB_LIB_DATA_BOUNCE_LONG_MOTION	20
#define SHUB_LIB_DATA_FLAT_MOTION	21
#define SHUB_LIB_DATA_MOVEMENT_ALERT	22
#define SHUB_LIB_DATA_TEST_FLAT_MOTION	23
#define SHUB_LIB_DATA_SPECIFIC_POSE_ALERT	25
#define SHUB_LIB_DATA_ACTIVITY_TRACKER	26
#define SHUB_LIB_DATA_SLEEP_MONITOR	37
#define SHUB_LIB_DATA_CAPTURE_MOTION	39
#define SHUB_LIB_DATA_HRM_EX_COACH	40
#define SHUB_LIB_DATA_CALL_MOTION	41
#define SHUB_LIB_DATA_DOUBLE_TAP	42
#define SHUB_LIB_DATA_SIDE_PRESS	43
#define SHUB_LIB_DATA_SLMONITOR		44
#define SHUB_LIB_DATA_EXERCISE		45
#define SHUB_LIB_DATA_HEARTRATE		46
#define SHUB_LIB_DATA_LOCATION	    47
#define SHUB_LIB_DATA_AUTO_BRIGHTNESS    50
#define SHUB_LIB_DATA_WEAR_STATUS	52
#define SHUB_LIB_DATA_RESTING_HR	53
#define SHUB_LIB_DATA_WEARONOFF_MONITOR	54


#define SENSOR_ACTIVE2_MASK	(1ULL <<  SHUB_LIB_DATA_HRM_EX_COACH) |\
	(1ULL << SHUB_LIB_DATA_EXERCISE) |(1ULL <<  SHUB_LIB_DATA_HEARTRATE) |\
	(1ULL <<  SHUB_LIB_DATA_LOCATION)


static int ssp_get_sleep_monitor_cb(void *priv, long long *raw_val, int check_level, int caller_type)
{
	struct ssp_data *data = priv;
	int ret = DEVICE_ON_ACTIVE1;
	* raw_val = data->service_mask;

	switch (check_level) {
		/* To Do*/
		case SLEEP_MONITOR_CHECK_SOFT:
		case SLEEP_MONITOR_CHECK_HARD:
		default:
			if (*raw_val &( SENSOR_ACTIVE2_MASK))
				ret = DEVICE_ON_ACTIVE2;
			else
				ret = DEVICE_ON_ACTIVE1;
		break;
	}

	return ret;
}

static struct sleep_monitor_ops ssp_sleep_monitor_ops = {
	.read64_cb_func = ssp_get_sleep_monitor_cb,
};
#endif

static int ssp_probe(struct spi_device *spi)
{
	int iRet = 0;
	struct ssp_data *data;
#ifndef CONFIG_OF
	struct ssp_platform_data *pdata;
#endif

	pr_info("[SSP] %s\n", __func__);

	data = kzalloc(sizeof(*data), GFP_KERNEL);
	if (data == NULL) {
		pr_err("[SSP]: %s - failed to allocate memory for data\n",
			__func__);
		iRet = -ENOMEM;
		goto exit;
	}

	if (spi->dev.of_node) {
		iRet = ssp_parse_dt(&spi->dev, data);
		if (iRet) {
			pr_err("[SSP]: %s - Failed to parse DT\n", __func__);
			goto err_setup;
		}
	} else {
#ifndef CONFIG_OF
		pdata = spi->dev.platform_data;
		if (pdata == NULL) {
			pr_err("[SSP] %s, platform_data is null\n", __func__);
			iRet = -ENOMEM;
			goto exit;
		}

		data->set_mcu_reset = pdata->set_mcu_reset;
		data->rst = pdata->rst;
		data->ap_int = pdata->ap_int;
		data->mcu_int1 = pdata->mcu_int1;
		data->mcu_int2 = pdata->mcu_int2;
		data->iIrq = pdata->irq;
#ifdef CONFIG_SENSORS_SSP_LPM_MOTION
		data->check_lpmode = pdata->check_lpmode;
#endif
		pr_info("%s : set_mcu_reset = %d, rst = %d, ap_int = %d,"\
			" mcu_int1 = %d, mcu_int2 = %d\n", __func__,
			(int)data->set_mcu_reset, (int)data->rst,
			(int)data->ap_int, (int)data->mcu_int1,
			(int)data->mcu_int2);

		/* AP system_rev */
		if (pdata->check_ap_rev)
			data->ap_rev = pdata->check_ap_rev();
		else
			data->ap_rev = 0;
		pr_info("[SSP] %s, system Rev = 0x%x\n", __func__,
			data->ap_rev);

		/* Get sensor positions */
		if (pdata->get_positions) {
			pdata->get_positions(&data->accel_position,
				&data->mag_position);
		} else {
			data->accel_position = 0;
			data->mag_position = 0;
		}
		if (pdata->mag_matrix) {
			data->mag_matrix_size = pdata->mag_matrix_size;
			data->mag_matrix = pdata->mag_matrix;
		}
#endif
	}
#ifdef CONFIG_SENSORS_SSP_LPM_MOTION
	data->check_lpmode = ssp_check_lpmode;
#endif

	spi->mode = SPI_MODE_1;
	if (spi_setup(spi)) {
		pr_err("[SSP] %s, failed to setup spi\n", __func__);
		iRet = -ENODEV;
		goto err_setup;
	}

	data->bProbeIsDone = false;
	data->fw_dl_state = FW_DL_STATE_NONE;
	data->spi = spi;
	sensorhub_data = data;
	spi_set_drvdata(spi, data);

	mutex_init(&data->comm_mutex);
	mutex_init(&data->pending_mutex);

	pr_info("\n#####################################################\n");

	INIT_DELAYED_WORK(&data->work_firmware, work_function_firmware_update);
#ifdef CONFIG_KNOX_GEARPAY
	INIT_WORK(&data->payment_work, ssp_payment_work_strap);
#endif

	wake_lock_init(&data->ssp_wake_lock,
		WAKE_LOCK_SUSPEND, "ssp_wake_lock");

	iRet = initialize_input_dev(data);
	if (iRet < 0) {
		pr_err("[SSP]: %s - could not create input device\n", __func__);
		goto err_input_register_device;
	}

	iRet = initialize_debug_timer(data);
	if (iRet < 0) {
		pr_err("[SSP]: %s - could not create workqueue\n", __func__);
		goto err_create_workqueue;
	}
	data->dump_wq = create_singlethread_workqueue("ssp_dump_wq");
	if (!data->dump_wq)
	goto err_create_workqueue;

#if SSP_STATUS_MONITOR
	iRet = initialize_polling_work(data);
	if (iRet < 0) {
		pr_err("[SSP]: %s - could not create delayed_work\n", __func__);
		goto err_create_workqueue;
	}
#endif

#ifdef CONFIG_SENSORS_SSP_LPM_MOTION
	iRet = intialize_lpm_motion(data);
	if (iRet < 0) {
		pr_err("[SSP]: %s - could not create workqueue\n", __func__);
		goto err_create_lpm_motion;
	}
#endif

	iRet = initialize_irq(data);
	if (iRet < 0) {
		pr_err("[SSP]: %s - could not create irq\n", __func__);
		goto err_setup_irq;
	}

	iRet = initialize_sysfs(data);
	if (iRet < 0) {
		pr_err("[SSP]: %s - could not create sysfs\n", __func__);
		goto err_sysfs_create;
	}

	iRet = initialize_event_symlink(data);
	if (iRet < 0) {
		pr_err("[SSP]: %s - could not create symlink\n", __func__);
		goto err_symlink_create;
	}

	initialize_variable(data);

#ifdef CONFIG_SENSORS_SSP_SENSORHUB
	/* init sensorhub device */
	iRet = ssp_sensorhub_initialize(data);
	if (iRet < 0) {
		pr_err("%s: ssp_sensorhub_initialize err(%d)", __func__, iRet);
		ssp_sensorhub_remove(data);
	}
#endif

	ssp_enable(data, true);
#if defined(SSP_IRQ_EDGE_PROTECT)
	prevent_irq = 1;
#endif
	/* check boot loader binary */
	data->fw_dl_state = check_fwbl(data);

	if (data->fw_dl_state == FW_DL_STATE_NONE) {
		iRet = initialize_mcu(data);
		if (iRet == ERROR) {
			toggle_mcu_reset(data);
		} else if (iRet < ERROR) {
			pr_err("[SSP]: %s - initialize_mcu failed\n", __func__);
			goto err_read_reg;
		}
	}

#ifdef CONFIG_HAS_EARLYSUSPEND
	data->early_suspend.suspend = ssp_early_suspend;
	data->early_suspend.resume = ssp_late_resume;
	register_early_suspend(&data->early_suspend);
#endif
#ifdef CONFIG_DISPLAY_EARLY_DPMS
	device_set_early_complete(&spi->dev, EARLY_COMP_SLAVE);
#endif

#ifdef CONFIG_SLEEP_MONITOR
	data->service_mask = 0;
	sleep_monitor_register_ops(data, &ssp_sleep_monitor_ops, SLEEP_MONITOR_SENSOR);
#endif

	pr_info("[SSP]: %s - probe success!\n", __func__);

	enable_debug_timer(data);

	if (data->fw_dl_state == FW_DL_STATE_NEED_TO_SCHEDULE) {
		pr_info("[SSP] Firmware update is scheduled\n");
		schedule_delayed_work(&data->work_firmware,
				msecs_to_jiffies(1000));
		data->fw_dl_state = FW_DL_STATE_SCHEDULED;
	} else if (data->fw_dl_state == FW_DL_STATE_FAIL) {
		data->bSspShutdown = true;
	}
#if SSP_STATUS_MONITOR
	else
		schedule_delayed_work(&data->polling_work,
		msecs_to_jiffies(10000));
#endif

	data->bProbeIsDone = true;
	iRet = 0;

#ifdef CONFIG_SENSORS_SSP_LPM_MOTION
	if (data->check_lpmode() == true) {
		ssp_charging_motion(data, 1);
		ssp_charging_rotation(data, 1);
		data->bLpModeEnabled = true;
		pr_info("[SSP]: LPM Charging...\n");
	} else {
		data->bLpModeEnabled = false;
		pr_info("[SSP]: Normal Booting OK\n");
	}
#endif

	goto exit;

err_read_reg:
err_symlink_create:
	remove_sysfs(data);
err_sysfs_create:
	free_irq(data->iIrq, data);
	gpio_free(data->mcu_int1);
err_setup_irq:
#ifdef CONFIG_SENSORS_SSP_LPM_MOTION
	destroy_workqueue(data->lpm_motion_wq);
err_create_lpm_motion:
#endif
	destroy_workqueue(data->debug_wq);
	destroy_workqueue(data->dump_wq);
err_create_workqueue:
	remove_input_dev(data);
err_input_register_device:
	wake_lock_destroy(&data->ssp_wake_lock);
	mutex_destroy(&data->comm_mutex);
	mutex_destroy(&data->pending_mutex);
err_setup:
	kfree(data);
	pr_err("[SSP] %s, probe failed!\n", __func__);
exit:
	pr_info("#####################################################\n\n");
	return iRet;
}

static void ssp_shutdown(struct spi_device *spi)
{
	struct ssp_data *data = spi_get_drvdata(spi);

	func_dbg();
	atomic_set(&data->apShutdownProgress, 1);
	if (data->bProbeIsDone == false)
		goto exit;

	disable_debug_timer(data);

	if (data->fw_dl_state >= FW_DL_STATE_SCHEDULED &&
		data->fw_dl_state < FW_DL_STATE_DONE) {
		pr_err("[SSP] %s, cancel_delayed_work_sync state = %d\n",
			__func__, data->fw_dl_state);
		cancel_delayed_work_sync(&data->work_firmware);
	}

	if (SUCCESS != ssp_send_cmd(data, MSG2SSP_AP_STATUS_SHUTDOWN, 0))
		pr_err("[SSP]: %s MSG2SSP_AP_STATUS_SHUTDOWN failed\n",
			__func__);

	ssp_enable(data, false);
	clean_pending_list(data);
#ifdef CONFIG_SLEEP_MONITOR
	sleep_monitor_unregister_ops(SLEEP_MONITOR_SENSOR);
#endif


#ifdef CONFIG_HAS_EARLYSUSPEND
	unregister_early_suspend(&data->early_suspend);
#endif

	free_irq(data->iIrq, data);
	gpio_free(data->mcu_int1);

	remove_event_symlink(data);
	remove_sysfs(data);
	remove_input_dev(data);

#ifdef CONFIG_SENSORS_SSP_SENSORHUB
	ssp_sensorhub_remove(data);
#endif

#if SSP_STATUS_MONITOR
	cancel_delayed_work_sync(&data->polling_work);
#endif
	del_timer_sync(&data->debug_timer);
	cancel_work_sync(&data->work_debug);
	cancel_delayed_work_sync(&data->work_refresh);
	destroy_workqueue(data->dump_wq);
#ifdef CONFIG_SENSORS_SSP_LPM_MOTION
	cancel_work_sync(&data->work_lpm_motion);
	destroy_workqueue(data->lpm_motion_wq);
#endif
	destroy_workqueue(data->debug_wq);
	wake_lock_destroy(&data->ssp_wake_lock);
	mutex_destroy(&data->comm_mutex);
	mutex_destroy(&data->pending_mutex);
	toggle_mcu_reset(data);
	pr_info("[SSP]: %s done\n", __func__);
exit:
	kfree(data);
}

#ifdef CONFIG_HAS_EARLYSUSPEND
static void ssp_early_suspend(struct early_suspend *handler)
{
	struct ssp_data *data;
	data = container_of(handler, struct ssp_data, early_suspend);

	func_dbg();
	disable_debug_timer(data);

#ifdef CONFIG_SENSORS_SSP_SENSORHUB
	/* give notice to user that AP goes to sleep */
	ssp_sensorhub_report_notice(data, MSG2SSP_AP_STATUS_SLEEP);
	ssp_sleep_mode(data);
	data->uLastAPState = MSG2SSP_AP_STATUS_SLEEP;
#else
	if (atomic_read(&data->aSensorEnable) > 0)
		ssp_sleep_mode(data);
#endif
}

static void ssp_late_resume(struct early_suspend *handler)
{
	struct ssp_data *data;
	data = container_of(handler, struct ssp_data, early_suspend);

	func_dbg();
	enable_debug_timer(data);

#ifdef CONFIG_SENSORS_SSP_SENSORHUB
	/* give notice to user that AP goes to sleep */
	ssp_sensorhub_report_notice(data, MSG2SSP_AP_STATUS_WAKEUP);
	ssp_resume_mode(data);
	data->uLastAPState = MSG2SSP_AP_STATUS_WAKEUP;
#else
	if (atomic_read(&data->aSensorEnable) > 0)
		ssp_resume_mode(data);
#endif
}

#else /* no early suspend */

static int ssp_suspend(struct device *dev)
{
	struct spi_device *spi = to_spi_device(dev);
	struct ssp_data *data = spi_get_drvdata(spi);

	if (unlikely(data->bSspShutdown)) {
		pr_err("stop sending ssp suspend cmd(shutdown)");
		return 0;
	}

	func_dbg();
	data->uLastResumeState = MSG2SSP_AP_STATUS_SUSPEND;
	disable_debug_timer(data);
#if SSP_STATUS_MONITOR
	cancel_delayed_work_sync(&data->polling_work);
#endif

	if (SUCCESS != ssp_send_cmd(data, MSG2SSP_AP_STATUS_SUSPEND, 0))
		pr_err("[SSP]: %s MSG2SSP_AP_STATUS_SUSPEND failed\n",
			__func__);
	data->bTimeSyncing = false;

	return 0;
}

static int ssp_resume(struct device *dev)
{
	struct spi_device *spi = to_spi_device(dev);
	struct ssp_data *data = spi_get_drvdata(spi);

	func_dbg();
	enable_debug_timer(data);

	if (unlikely(data->bSspShutdown)) {
		pr_err("stop sending ssp resume cmd(shutdown)");
		return 0;
	}

	if (SUCCESS != ssp_send_cmd(data, MSG2SSP_AP_STATUS_RESUME, 0))
		pr_err("[SSP]: %s MSG2SSP_AP_STATUS_RESUME failed\n",
			__func__);
	data->uLastResumeState = MSG2SSP_AP_STATUS_RESUME;

#if SSP_STATUS_MONITOR
	schedule_delayed_work(&data->polling_work, msecs_to_jiffies(3000));
#endif
	return 0;
}

static const struct dev_pm_ops ssp_pm_ops = {
	.suspend = ssp_suspend,
	.resume = ssp_resume
};
#endif /* CONFIG_HAS_EARLYSUSPEND */

static const struct spi_device_id ssp_id[] = {
	{"ssp", 0},
	{}
};

MODULE_DEVICE_TABLE(spi, ssp_id);
#ifdef CONFIG_OF
static struct of_device_id ssp_match_table[] = {
	{ .compatible = "ssp,ATSAMG55",},
	{},
};
#endif
static struct spi_driver ssp_driver = {
	.probe = ssp_probe,
	.shutdown = ssp_shutdown,
	.id_table = ssp_id,
	.driver = {
#ifndef CONFIG_HAS_EARLYSUSPEND
		   .pm = &ssp_pm_ops,
#endif
		   .owner = THIS_MODULE,
		   .name = "ssp",
#ifdef CONFIG_OF
		   .of_match_table = ssp_match_table
#endif
		},
};

module_spi_driver(ssp_driver);
MODULE_DESCRIPTION("ssp spi driver");
MODULE_AUTHOR("Samsung Electronics");
MODULE_LICENSE("GPL");
