/*
 * Copyright (C) 2016 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/i2c.h>
#include <linux/pm.h>
#include <linux/of_gpio.h>
#include <linux/pm_runtime.h>
#include <asm-generic/delay.h>


#ifdef CONFIG_PM_WAKELOCKS
#include <linux/pm_wakeup.h>
#else
#include <linux/wakelock.h>
#endif

#include "custome_external_battery.h"
#include "bq27411-g1.h"

signed int gauge_get_bat_voltage(void)
{
#if defined(CONFIG_POWER_EXT) || defined(CONFIG_FPGA_EARLY_PORTING)
		return 4201;
#else
		return bq_read_i2c_word(BQ27411_REG_Voltage);
#endif
}

signed int gauge_get_bat_current(void)
{
	if (bq_status.disablegauge)
		return 0;
	else
		return (short)bq_read_i2c_word(BQ27411_REG_AverageCurrent);
}

signed int gauge_get_bat_soc(void)
{
	return (short)bq_read_i2c_word(BQ27411_REG_StateOfChargeUnfiltered);
}

signed int gauge_get_bat_uisoc(void)
{
	if (bq_status.disablegauge)
		return 50;
	else
		return (short)bq_read_i2c_word(BQ27411_REG_StateOfCharge);
}

signed int gauge_get_bat_temperature(void)
{
	if (bq_status.disablegauge || bq_status.forcetemp25)
		return 25;
	else
		return (bq_read_i2c_word(BQ27411_REG_Temperature)-2731)/10;
}


