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

#include "bq27411-g1.h"

#define I2C_RETRY_CNT	3

struct bq27411_device_info *di_info;


int bq_read_i2c_word(u32 addr)
{
	if (di_info == NULL)
		return 50;
	return i2c_smbus_read_word_data(di_info->client, addr);
}


static inline int bq27411_chip_id_check(struct i2c_client *i2c)
{
	int vendor_id = 0;

	return vendor_id;
}


ATTR_SHOW_BQ(Voltage, BQ27411_REG_Voltage)
ATTR_SHOW_BQ(AverageCurrent, BQ27411_REG_AverageCurrent)
ATTR_SHOW_BQ(StandbyCurrent, BQ27411_REG_StandbyCurrent)
ATTR_SHOW_BQ(StateOfCharge, BQ27411_REG_StateOfCharge)
ATTR_SHOW_BQ(RemainingCapacity, BQ27411_REG_RemainingCapacity)
ATTR_SHOW_BQ(FullChargeCapacity, BQ27411_REG_FullChargeCapacity)
ATTR_SHOW_BQ(Temperature, BQ27411_REG_Temperature)
ATTR_SHOW_BQ(InternalTemperature, BQ27411_REG_InternalTemperature)
ATTR_SHOW_BQ(StateOfHealth, BQ27411_REG_StateOfHealth)
ATTR_SHOW_BQ(Flags, BQ27411_REG_Flags)
ATTR_SHOW_BQ(NominalAvailableCapacity, BQ27411_REG_NominalAvailableCapacity)
ATTR_SHOW_BQ(FullAvailableCapacity, BQ27411_REG_FullAvailableCapacity)
ATTR_SHOW_BQ(RemainingCapacityUnfiltered, BQ27411_REG_RemainingCapacityUnfiltered)
ATTR_SHOW_BQ(RemainingCapacityFiltered, BQ27411_REG_RemainingCapacityFiltered)
ATTR_SHOW_BQ(FullChargeCapacityUnfiltered, BQ27411_REG_FullChargeCapacityUnfiltered)
ATTR_SHOW_BQ(FullChargeCapacityFlitered, BQ27411_REG_FullChargeCapacityFlitered)
ATTR_SHOW_BQ(StateOfChargeUnfiltered, BQ27411_REG_StateOfChargeUnfiltered)

static struct attribute *bq27411_attributes[] = {
	&dev_attr_Voltage.attr,
	&dev_attr_AverageCurrent.attr,
	&dev_attr_StandbyCurrent.attr,
	&dev_attr_StateOfCharge.attr,
	&dev_attr_RemainingCapacity.attr,
	&dev_attr_FullChargeCapacity.attr,
	&dev_attr_Temperature.attr,
	&dev_attr_InternalTemperature.attr,
	&dev_attr_StateOfHealth.attr,
	&dev_attr_Flags.attr,
	&dev_attr_NominalAvailableCapacity.attr,
	&dev_attr_FullAvailableCapacity.attr,
	&dev_attr_RemainingCapacityUnfiltered.attr,
	&dev_attr_RemainingCapacityFiltered.attr,
	&dev_attr_FullChargeCapacityUnfiltered.attr,
	&dev_attr_FullChargeCapacityFlitered.attr,
	&dev_attr_StateOfChargeUnfiltered.attr,
	NULL
};


static const struct attribute_group bq27411_attr_group = {
	.attrs = bq27411_attributes,
};



static int bq27411_battery_probe(struct i2c_client *client,
				 const struct i2c_device_id *id)
{
	struct bq27411_device_info *di;
	int retval = 0;
	int ret = 0;
	signed int fullcapacity;

	pr_info("%s:start \n", __func__);
	ret = bq27411_chip_id_check(client);
	if (ret < 0)
		return ret;
	di = kzalloc(sizeof(*di), GFP_KERNEL);
	if (!di) {
		return -ENOMEM;
	}

	di->dev = &client->dev;
	di->client = client;
	i2c_set_clientdata(client, di);

	di_info = di;

#if 1


	pr_info("%s:bq_read_i2c_word start\n", __func__);
	fullcapacity = bq_read_i2c_word(BQ27411_REG_FullChargeCapacity);
	pr_info("%s:bq_read_i2c_word  fullcapacity %d\n", __func__, fullcapacity);

	pr_info("%s:fullcapacity %d\n", __func__, fullcapacity);


	fullcapacity = bq_read_i2c_word(BQ27411_REG_StateOfCharge);
	pr_info("%s:bq_read_i2c_word  soc %d\n", __func__, fullcapacity);


	fullcapacity = bq_read_i2c_word(BQ27411_REG_Voltage);
	pr_info("%s:bq_read_i2c_word  vbat %d\n", __func__, fullcapacity);


#endif
/*debug node*/
	retval = sysfs_create_group(&client->dev.kobj, &bq27411_attr_group);
	if (retval)
		pr_info("%s:group fail %d\n", __func__, retval);


#ifdef CONFIG_PM_WAKELOCKS
		wakeup_source_init(&di->fg_int, "FG_INT bq27411 suspend wakelock");
#else
		wake_lock_init(&di->fg_int, WAKE_LOCK_SUSPEND, "FG_INT bq27411 suspend wakelock");
#endif


	return 0;
}

static int bq27411_battery_remove(struct i2c_client *client)
{
	return 0;
}

static const struct i2c_device_id bq27411_id[] = {
	{ "bq27411", 0},
	{}
};

MODULE_DEVICE_TABLE(i2c, bq27411_id);
#ifdef CONFIG_OF
static const struct of_device_id bq27411_of_ids[] = {
		{.compatible = "mediatek,bq27411_gauge"},
			{},
};
#endif



static struct i2c_driver bq27411_battery_driver = {
	.driver = {
		.name = "bq27411",
		.owner = THIS_MODULE,
#ifdef CONFIG_OF
			  .of_match_table = bq27411_of_ids,
#endif
	},
	.probe = bq27411_battery_probe,
	.remove = bq27411_battery_remove,
	.id_table = bq27411_id,
};

module_i2c_driver(bq27411_battery_driver);

MODULE_AUTHOR("PS.Z");
MODULE_DESCRIPTION("sn27411/bq27411-g1 battery monitor driver");
MODULE_LICENSE("GPL");
