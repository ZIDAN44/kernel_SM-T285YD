/*
 * Copyright (C) 2012 Spreadtrum Communications Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/bitops.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <soc/sprd/hardware.h>
#include <soc/sprd/board.h>
#include "../sensor_drv_sprd.h"

static int sensor_s5k4h5yc_poweron(uint32_t *fd_handle, struct sensor_power *dev0, struct sensor_power *dev1, struct sensor_power *dev2)
{
	int ret = 0;

	sensor_k_sensor_sel(fd_handle, SENSOR_DEV_1);//select sub sensor
	sensor_k_set_rst_level(fd_handle, 0);//reset
	sensor_k_sensor_sel(fd_handle, SENSOR_DEV_0);//select main sensor
	sensor_k_set_rst_level(fd_handle, 0);//reset
	mdelay(3);
	
	sensor_k_set_voltage_avdd(fd_handle, SENSOR_VDD_2800MV);
	udelay(100);
	sensor_k_set_voltage_iovdd(fd_handle, SENSOR_VDD_1200MV);
	udelay(10);
	sensor_k_set_voltage_cammot(fd_handle, SENSOR_VDD_2800MV);
	mdelay(10);
	sensor_k_set_voltage_dvdd(fd_handle, SENSOR_VDD_1800MV);
	mdelay(3);
	sensor_k_set_mclk(fd_handle, 24);
	udelay(2000);
	sensor_k_set_rst_level(fd_handle, 1);
	udelay(2000);

	printk("s5k4h5yc_poweron OK \n");

	return ret;
}

static int sensor_s5k4h5yc_poweroff(uint32_t *fd_handle, struct sensor_power *dev0, struct sensor_power *dev1, struct sensor_power *dev2)
{
	int ret = 0;

	sensor_k_sensor_sel(fd_handle, SENSOR_DEV_1);//select sub sensor
	sensor_k_set_rst_level(fd_handle, 0);//reset
	sensor_k_sensor_sel(fd_handle, SENSOR_DEV_0);//select main sensor
	
	sensor_k_set_voltage_cammot(fd_handle, SENSOR_VDD_CLOSED);
	udelay(150);
	sensor_k_set_mclk(fd_handle, 0);// disable mclk
	udelay(100);
	sensor_k_set_rst_level(fd_handle, 0);//reset
	udelay(100);
	sensor_k_set_voltage_dvdd(fd_handle, SENSOR_VDD_CLOSED);
	udelay(200);
	sensor_k_set_voltage_iovdd(fd_handle, SENSOR_VDD_CLOSED);
	udelay(200);
	sensor_k_set_voltage_avdd(fd_handle, SENSOR_VDD_CLOSED);
	udelay(200);

	printk("s5k4h5yc_poweroff OK \n");

	return ret;
}


static int sensor_sr259_poweron(uint32_t *fd_handle, struct sensor_power *dev0, struct sensor_power *dev1, struct sensor_power *dev2)
{
	int ret = 0;

	sensor_k_sensor_sel(fd_handle, SENSOR_DEV_0);//select main sensor(sensor sr259);
	sensor_k_set_pd_level(fd_handle,0);//power down valid for sr259
	sensor_k_set_rst_level(fd_handle,0);//reset valid for hisr259
	sensor_k_sensor_sel(fd_handle, SENSOR_DEV_1);//select main sensor(sensor sr259);
	sensor_k_set_pd_level(fd_handle,0);//power down valid for sr259
	sensor_k_set_rst_level(fd_handle,0);//reset valid for sr259
	udelay(1);
	sensor_k_set_voltage_dvdd(fd_handle,SENSOR_VDD_1800MV);//IO vdd
	udelay(10);//delay < 10ms
	sensor_k_set_voltage_avdd(fd_handle,SENSOR_VDD_2800MV);// Analog vdd
	udelay(10);//delay < 10ms 

	mdelay(15);//delay > 10ms 
	sensor_k_set_mclk(fd_handle,26);
	mdelay(15);//delay > 10ms 
	sensor_k_set_pd_level(fd_handle,1);
	mdelay(35);//delay > 30ms
	sensor_k_set_rst_level(fd_handle,1);
	udelay(2);//delay 2us > 16MCLK = 16/24 us
	printk("sr259_poweron OK \n");

	return ret;
}

static int sensor_sr259_poweroff(uint32_t *fd_handle, struct sensor_power *dev0, struct sensor_power *dev1, struct sensor_power *dev2)
{
	int ret = 0;

	sensor_k_sensor_sel(fd_handle, SENSOR_DEV_1);//select main sensor(sensor sr259);
	udelay(2);//delay 2us > 16MCLK = 16/24 us
	sensor_k_set_rst_level(fd_handle,0);//reset valid for sr259
	udelay(50);//delay 2us > 16MCLK = 16/24 us
	sensor_k_set_mclk(fd_handle,0);// disable mclk
	udelay(1);//delay 1us > 0ns
	sensor_k_set_pd_level(fd_handle,0);//power down valid for sr259
	mdelay(6);//delay 6ms < 10ms
	sensor_k_set_voltage_avdd(fd_handle,SENSOR_VDD_CLOSED);
	sensor_k_set_voltage_dvdd(fd_handle,SENSOR_VDD_CLOSED);//close core vdd
	udelay(1);
	printk("sr259_poweroff OK \n");

	return ret;
}

int sensor_power_on(uint32_t *fd_handle, uint32_t sensor_id, struct sensor_power *dev0, struct sensor_power *dev1, struct sensor_power *dev2)
{
	int ret = 0;

	if (SENSOR_DEV_0 == sensor_id) {
		ret = sensor_s5k4h5yc_poweron(fd_handle, dev0, dev1, dev2);
	} else if (SENSOR_DEV_1 == sensor_id) {
		ret = sensor_sr259_poweron(fd_handle, dev0, dev1, dev2);
	}
	return ret;
}

int sensor_power_off(uint32_t *fd_handle, uint32_t sensor_id, struct sensor_power *dev0, struct sensor_power *dev1, struct sensor_power *dev2)
{
	int ret = 0;

	if (SENSOR_DEV_0 == sensor_id) {
		ret = sensor_s5k4h5yc_poweroff(fd_handle, dev0, dev1, dev2);
	} else if (SENSOR_DEV_1 == sensor_id) {
		ret = sensor_sr259_poweroff(fd_handle, dev0, dev1, dev2);
	}

	return ret;
}
