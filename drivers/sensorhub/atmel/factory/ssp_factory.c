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
#include "../ssp.h"
#include "ssp_factory.h"

/*************************************************************************/
/* factory Sysfs                                                         */
/*************************************************************************/

int accel_open_calibration(struct ssp_data *data)
{
	int ret = 0;

#ifdef CONFIG_MACH_WC1
	if (data->ap_rev < 5)
		ret = bmi168_accel_open_calibration(data);
	else
		ret = k6ds_accel_open_calibration(data);
#else
	ret = bmi168_accel_open_calibration(data);
#endif

	return ret;
}

int set_accel_cal(struct ssp_data *data)
{
	int ret = 0;

#ifdef CONFIG_MACH_WC1
	if (data->ap_rev < 5)
		ret = bmi168_set_accel_cal(data);
	else
		ret = k6ds_set_accel_cal(data);
#else
	ret = bmi168_set_accel_cal(data);
#endif

	return ret;
}

int gyro_open_calibration(struct ssp_data *data)
{
	int ret = 0;

#ifdef CONFIG_MACH_WC1
	if (data->ap_rev < 5)
		ret = bmi168_gyro_open_calibration(data);
	else
		ret = k6ds_gyro_open_calibration(data);
#else
	ret = bmi168_gyro_open_calibration(data);
#endif

	return ret;
}

int set_gyro_cal(struct ssp_data *data)
{
	int ret = 0;

#ifdef CONFIG_MACH_WC1
	if (data->ap_rev < 5)
		ret = bmi168_set_gyro_cal(data);
	else
		ret = k6ds_set_gyro_cal(data);
#else
	ret = bmi168_set_gyro_cal(data);
#endif

	return ret;
}

int pressure_open_calibration(struct ssp_data *data)
{
	int ret = 0;

	ret = lps25h_pressure_open_calibration(data);

	return ret;
}

int set_pressure_cal(struct ssp_data *data)
{
	int ret = 0;

	ret = lps25h_set_pressure_cal(data);

	return ret;
}

int hrm_open_calibration(struct ssp_data *data)
{
	int ret = 0;

#ifdef CONFIG_MACH_WC1
	if (data->ap_rev < 3 || data->ap_rev >= 7)
		ret = ad45251_hrm_open_calibration(data);
	else 
		ret = pps960_hrm_open_calibration(data);
#else
	if (data->ap_rev < 2)
		ret = ad45251_hrm_open_calibration(data);
	else 
		ret = pps960_hrm_open_calibration(data);
#endif

	return ret;
}

int set_hrm_calibration(struct ssp_data *data)
{
	int ret = 0;

#ifdef CONFIG_MACH_WC1
	if (data->ap_rev < 3 || data->ap_rev >= 7)
		ret = ad45251_set_hrm_calibration(data);
	else 
		ret = pps960_set_hrm_calibration(data);
#else
	if (data->ap_rev < 2)
		ret = ad45251_set_hrm_calibration(data);
	else 
		ret = pps960_set_hrm_calibration(data);
#endif

	return ret;
}

int initialize_magnetic_sensor(struct ssp_data *data)
{
	int ret = SUCCESS;

#ifdef CONFIG_MACH_WC1
	if (data->ap_rev < 3)
		ret = initialize_yas537_magnetic_sensor(data);
#else
	if (data->ap_rev < 2)
		ret = initialize_yas537_magnetic_sensor(data);
#endif

	return ret;
}

void initialize_factorytest(struct ssp_data *data)
{
#ifdef CONFIG_MACH_WC1
#ifdef CONFIG_SENSORS_SSP_ACCELEROMETER_SENSOR
	if (data->ap_rev < 5)
		initialize_bmi168_accel_factorytest(data);
	else
		initialize_k6ds_accel_factorytest(data);
#endif
#ifdef CONFIG_SENSORS_SSP_GYRO_SENSOR
	if (data->ap_rev < 5)
		initialize_bmi168_gyro_factorytest(data);
	else
		initialize_k6ds_gyro_factorytest(data);
#endif

#ifdef CONFIG_SENSORS_SSP_PRESSURE_SENSOR
	initialize_lps25h_pressure_factorytest(data);
#endif
#ifdef CONFIG_SENSORS_SSP_LIGHT_SENSOR
	if (data->ap_rev > 4)
		initialize_tsl2584_light_factorytest(data);
#endif
#endif
#ifdef CONFIG_MACH_WC1
	if (data->ap_rev < 3)
		initialize_yas537_magnetic_factorytest(data);
	if (data->ap_rev < 3 || data->ap_rev >= 7)
		initialize_ad45251_hrm_factorytest(data);
	else
		initialize_pps960_hrm_factorytest(data);
#else
	if (data->ap_rev < 2) {
		initialize_yas537_magnetic_factorytest(data);
		initialize_ad45251_hrm_factorytest(data);
	} else
		initialize_pps960_hrm_factorytest(data);
#endif

}

void remove_factorytest(struct ssp_data *data)
{
#ifdef CONFIG_MACH_WC1
#ifdef CONFIG_SENSORS_SSP_ACCELEROMETER_SENSOR
	if (data->ap_rev < 5)
		remove_bmi168_accel_factorytest(data);
	else
		remove_k6ds_accel_factorytest(data);
#endif
#ifdef CONFIG_SENSORS_SSP_GYRO_SENSOR
	if (data->ap_rev < 5)
		remove_bmi168_gyro_factorytest(data);
	else
		remove_k6ds_gyro_factorytest(data);
#endif
#ifdef CONFIG_SENSORS_SSP_PRESSURE_SENSOR
	remove_lps25h_pressure_factorytest(data);
#endif
#ifdef CONFIG_SENSORS_SSP_LIGHT_SENSOR
	if (data->ap_rev > 4)
		remove_tsl2584_light_factorytest(data);
#endif
#endif
#ifdef CONFIG_MACH_WC1
	if (data->ap_rev < 3)
		remove_yas537_magnetic_factorytest(data);
	if (data->ap_rev < 3 || data->ap_rev >= 7)
		remove_ad45251_hrm_factorytest(data);
	else
		remove_pps960_hrm_factorytest(data);
#else
	if (data->ap_rev < 2) {
		remove_yas537_magnetic_factorytest(data);
		remove_ad45251_hrm_factorytest(data);
	} else
		remove_pps960_hrm_factorytest(data);
#endif 
}


