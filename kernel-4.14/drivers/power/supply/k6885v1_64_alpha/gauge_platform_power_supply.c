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

#include <linux/init.h>		/* For init/exit macros */
#include <linux/module.h>	/* For MODULE_ marcros  */
#include <linux/wait.h>		/* For wait queue*/
#include <linux/sched.h>	/* For wait queue*/
#include <linux/kthread.h>	/* For Kthread_run */
#include <linux/platform_device.h>	/* platform device */
#include <linux/time.h>		/*hr_timer*/
#include <linux/power_supply.h>	/*power_supply*/
#include <linux/wait.h>
#include <linux/workqueue.h>


#include <mt-plat/charger_type.h>
#include <mt-plat/mtk_charger.h>
#include "bq27411-g1.h"
#include "custome_external_battery.h"

struct bq_platform_power_supply *battery_info;
struct bq_gm bq_status;

static struct workqueue_struct *bq_wqueue;
static struct work_struct   work;

struct battery_chg_callback {
	struct charger_consumer *pbat_consumer;
	struct notifier_block bat_nb;
};
static struct battery_chg_callback gm;

struct bq_battery_pwsply_node {
	struct power_supply_desc battery_desc;
	struct power_supply_config battery_cfg;
	struct power_supply *battery_psy;
};

struct bq_battery_pwsply_node *battery_pwrsply_data1;


/*power supply node */
static enum power_supply_property battery_props[] = {
	POWER_SUPPLY_PROP_STATUS,
	POWER_SUPPLY_PROP_HEALTH,
	POWER_SUPPLY_PROP_PRESENT,
	POWER_SUPPLY_PROP_TECHNOLOGY,
	POWER_SUPPLY_PROP_CYCLE_COUNT,
	POWER_SUPPLY_PROP_CAPACITY,
	POWER_SUPPLY_PROP_CURRENT_NOW,
	POWER_SUPPLY_PROP_CURRENT_AVG,
	POWER_SUPPLY_PROP_VOLTAGE_NOW,
	POWER_SUPPLY_PROP_CHARGE_FULL,
	POWER_SUPPLY_PROP_CHARGE_COUNTER,
	POWER_SUPPLY_PROP_TEMP,
};

static int battery_get_property(struct power_supply *psy,
	enum power_supply_property psp,
	union power_supply_propval *val)
{
	int ret = 0;

	switch (psp) {
	case POWER_SUPPLY_PROP_STATUS:
		val->intval = battery_info->status;
		break;
	case POWER_SUPPLY_PROP_HEALTH:
		val->intval = battery_info->health;
		break;
	case POWER_SUPPLY_PROP_PRESENT:
		val->intval = battery_info->present;
		break;
	case POWER_SUPPLY_PROP_TECHNOLOGY:
		val->intval = battery_info->technology;
		break;
	case POWER_SUPPLY_PROP_CYCLE_COUNT:
		val->intval = 10;
		break;
	case POWER_SUPPLY_PROP_CAPACITY:
		if (bq_status.disablegauge)
			val->intval = 50;
		else
			val->intval = battery_info->capacity;
		break;
	case POWER_SUPPLY_PROP_CURRENT_NOW:
		if (bq_status.disablegauge)
			val->intval = 0;
		else
			val->intval = 1000 * (short)bq_read_i2c_word(BQ27411_REG_AverageCurrent);
		break;
	case POWER_SUPPLY_PROP_CURRENT_AVG:
		if (bq_status.disablegauge)
			val->intval = 0;
		else
			val->intval = battery_info->avg_current;
		break;
	case POWER_SUPPLY_PROP_CHARGE_FULL:
		val->intval = battery_info->charge_full;
		break;
	case POWER_SUPPLY_PROP_VOLTAGE_NOW:
#if defined(CONFIG_POWER_EXT) || defined(CONFIG_FPGA_EARLY_PORTING)
		return 4201;
#else
		val->intval = battery_info->voltage;
#endif
		break;
	case POWER_SUPPLY_PROP_CHARGE_COUNTER:
		if (bq_status.disablegauge)
			val->intval = 50*battery_info->charge_full/100;
		else
			val->intval = battery_info->charge_counter;
		break;
	case POWER_SUPPLY_PROP_TEMP:
		if (bq_status.disablegauge || bq_status.forcetemp25)
			val->intval = 250;
		else
			val->intval = battery_info->temp;
		break;


	default:
		ret = -EINVAL;
		break;
	}

	return ret;
}

void _wake_up_battery(struct bq_platform_power_supply *info)
{
	unsigned long flags;

	if (info == NULL)
		return;
	if (&info->slock != NULL) {
		spin_lock_irqsave(&info->slock, flags);
	#ifdef CONFIG_PM_WAKELOCKS
		if (!info->fg_wakelock.active)
			__pm_stay_awake(&info->fg_wakelock);
	#else
		if (wake_lock_active(&info->fg_wakelock) == 0)
			wake_lock(&info->fg_wakelock);
	#endif
		spin_unlock_irqrestore(&info->slock, flags);
	} else
		pr_info("%s:slock is null\n", __func__);

	info->thread_timeout = true;
	wake_up(&info->wait_que);
	pr_info("%s:wake up thread  \n", __func__);

}

static void bq_battery_start_timer(struct bq_platform_power_supply *info)
{
		ktime_t ktime = ktime_set(BQ_pwrsply_Interval, 0);

		pr_info("%s:hrtimer  start  \n", __func__);
		hrtimer_start(&info->battery_thread_timer, ktime
			, HRTIMER_MODE_REL);
}

enum hrtimer_restart battery_kthread_hrtimer_func(struct hrtimer *timer)
{
	struct bq_platform_power_supply *info =
	container_of(timer, struct bq_platform_power_supply,
			battery_thread_timer);
	_wake_up_battery(info);
	return HRTIMER_NORESTART;
}


static void bq_battery_init_timer(struct bq_platform_power_supply *info)
{
		ktime_t ktime = ktime_set(BQ_pwrsply_Interval, 0);

		hrtimer_init(&info->battery_thread_timer,
			CLOCK_MONOTONIC, HRTIMER_MODE_REL);
		info->battery_thread_timer.function =
				battery_kthread_hrtimer_func;
		hrtimer_start(&info->battery_thread_timer, ktime,
				HRTIMER_MODE_REL);
}

static int battery_update_thread(void *arg)
{
	unsigned long flags;
	struct bq_platform_power_supply *info = arg;

	while (1) {
		wait_event(info->wait_que, (info->thread_timeout == true));
		pr_info("%s:start \n", __func__);
		mutex_lock(&info->bat_i2c_access);

		if (&info->slock != NULL) {
			spin_lock_irqsave(&info->slock, flags);
		#ifdef CONFIG_PM_WAKELOCKS
			if (!info->fg_wakelock.active)
				__pm_stay_awake(&info->fg_wakelock);
		#else
			if (wake_lock_active(&info->fg_wakelock) == 0)
				wake_lock(&info->fg_wakelock);
		#endif
			spin_unlock_irqrestore(&info->slock, flags);
		} else {
		#ifdef CONFIG_PM_WAKELOCKS
			if (!info->fg_wakelock.active)
				__pm_stay_awake(&info->fg_wakelock);
		#else
			if (wake_lock_active(&info->fg_wakelock) == 0)
				wake_lock(&info->fg_wakelock);
		#endif
			pr_info("%s:slock is null\n", __func__);
		}
		info->thread_timeout = false;

		info->capacity = bq_read_i2c_word(BQ27411_REG_StateOfCharge);
		info->avg_current = (short)bq_read_i2c_word(BQ27411_REG_AverageCurrent)*1000;
		info->voltage = bq_read_i2c_word(BQ27411_REG_Voltage)*1000;
		info->temp = (bq_read_i2c_word(BQ27411_REG_Temperature)-2731);
		info->charge_full = bq_read_i2c_word(BQ27411_REG_FullChargeCapacity)*1000;
		info->charge_counter = (info->charge_full)*(info->capacity)/100;
		if (!bq_status.disablethread)
			bq_battery_start_timer(info);

		power_supply_changed(battery_pwrsply_data1->battery_psy);

		pr_info("%s:disable:%d current:%d voltage:%d temp:%d , charger_full:%d ,charger_counter:%d \n",
			__func__, info->capacity, info->avg_current, info->voltage,
			info->temp, info->charge_full, info->charge_counter);
#ifdef CONFIG_PM_WAKELOCKS
		if (info->fg_wakelock.active)
			__pm_relax(&info->fg_wakelock);
#else
		if (wake_lock_active(&info->fg_wakelock) != 0)
			wake_unlock(&info->fg_wakelock);
#endif
		mutex_unlock(&info->bat_i2c_access);

	}

	return 0;
}

void work_func(struct work_struct *work)
{
	pr_info("%s:start \n", __func__);
	if (battery_info != NULL)
	_wake_up_battery(battery_info);

}

/*************************************/
static int battery_callback(
	struct notifier_block *nb, unsigned long event, void *v)
{
	pr_info("%s:%ld\n",
		__func__, event);
	switch (event) {
	case CHARGER_NOTIFY_START_CHARGING:
		{
/* START CHARGING */
			battery_info->status = POWER_SUPPLY_STATUS_CHARGING;
			_wake_up_battery(battery_info);
		}
		break;
	case CHARGER_NOTIFY_STOP_CHARGING:
		{
/* STOP CHARGING */
			battery_info->status =
			POWER_SUPPLY_STATUS_DISCHARGING;
			_wake_up_battery(battery_info);
		}
		break;
	case CHARGER_NOTIFY_ERROR:
		{
/* charging enter error state */
		battery_info->status = POWER_SUPPLY_STATUS_NOT_CHARGING;
		_wake_up_battery(battery_info);
		}
		break;
	case CHARGER_NOTIFY_NORMAL:
		{
/* charging leave error state */
		battery_info->status = POWER_SUPPLY_STATUS_CHARGING;
		_wake_up_battery(battery_info);

		}
		break;

	default:
		{
		}
		break;
	}

	return NOTIFY_DONE;
}

/*sysfs attr*/
/*disablegauge*/
FG_DEV_NODE_ATTR(daemon, bq_status.disablegauge)
/*disablethread*/
FG_DEV_NODE_ATTR(thread, bq_status.disablethread)
/*DEV NODE force 25*/
FG_DEV_NODE_ATTR(tempfunction, bq_status.forcetemp25)


/*****************/
static int bq_battery_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct bq_battery_pwsply_node *mt_battery = NULL;
	struct bq_platform_power_supply *di = NULL;

	pr_info("[%s] Initialization : START\n",
		__func__);
	mt_battery = devm_kzalloc(&pdev->dev, sizeof(*mt_battery), GFP_KERNEL);
	mt_battery->battery_desc.name = "battery";
	mt_battery->battery_desc.type = POWER_SUPPLY_TYPE_BATTERY;
	mt_battery->battery_desc.properties = battery_props;
	mt_battery->battery_desc.num_properties =  ARRAY_SIZE(battery_props);
	mt_battery->battery_desc.get_property = battery_get_property;
	mt_battery->battery_cfg.drv_data = mt_battery;

	mt_battery->battery_psy = power_supply_register(&pdev->dev,
			&mt_battery->battery_desc, &mt_battery->battery_cfg);
	battery_pwrsply_data1 = mt_battery;

	di = devm_kzalloc(&pdev->dev, sizeof(*di), GFP_KERNEL);

#ifdef CONFIG_PM_WAKELOCKS
	wakeup_source_init(&di->fg_wakelock, "FG_INT bq27411 suspend wakelock");
	pr_info("%s:wakeup_source \n", __func__);
#else
	wake_lock_init(&di->fg_wakelock, WAKE_LOCK_SUSPEND, "FG_INT bq27411 suspend wakelock");
	pr_info("%s:wakelock \n", __func__);
#endif
	mutex_init(&di->bat_i2c_access);
	spin_lock_init(&di->slock);
	bq_battery_init_timer(di);
	init_waitqueue_head(&di->wait_que);
	di->thread_timeout = false;
	kthread_run(battery_update_thread, di, "battery_update");

	di->present = 1;
	di->status = POWER_SUPPLY_STATUS_DISCHARGING;
	di->health = POWER_SUPPLY_HEALTH_GOOD;
	di->technology = POWER_SUPPLY_TECHNOLOGY_LION;
	battery_info = di;

	gm.pbat_consumer = charger_manager_get_by_name(&(pdev->dev), "charger");
	if (gm.pbat_consumer != NULL) {
		gm.bat_nb.notifier_call = battery_callback;
		register_charger_manager_notifier(gm.pbat_consumer, &gm.bat_nb);
		pr_info("%s:get charger_callback \n", __func__);
	}

	/*sysfs node*/
	ret = device_create_file(&(pdev->dev),
		&dev_attr_FG_daemon_disable);
	ret = device_create_file(&(pdev->dev),
		&dev_attr_FG_daemon_enable);
	ret = device_create_file(&(pdev->dev),
		&dev_attr_FG_thread_disable);
	ret = device_create_file(&(pdev->dev),
		&dev_attr_FG_thread_enable);
	ret = device_create_file(&(pdev->dev),
		&dev_attr_FG_tempfunction_disable);
	ret = device_create_file(&(pdev->dev),
		&dev_attr_FG_tempfunction_enable);

	bq_wqueue = create_singlethread_workqueue("bq_battery");
	INIT_WORK(&work, work_func);

	di->thread_timeout = true;
	wake_up(&di->wait_que);

	pr_info("%s:wake up thread  \n", __func__);

	return ret;
}



static int bq_battery_resume(struct platform_device *dev)
{
	if (!bq_status.disablegauge)
		queue_work(bq_wqueue, &work);

	return 0;
}


struct platform_device battery_device1 = {
	.name = "battery_bq",
	.id = -1,
};


static struct platform_driver battery_driver_probe1 = {
	.probe = bq_battery_probe,
	.remove = NULL,
	.shutdown = NULL,
	.suspend = NULL,
	.resume = bq_battery_resume,
	.driver = {
		.name = "battery_bq",
#if 0
		.of_match_table = mtk_bat_of_match,
#endif
	},
};






static int __init bq_battery_init(void)
{
	int ret = 0;
	pr_info("[%s] Initialization : START\n",
		__func__);

	ret = platform_device_register(&battery_device1);
	pr_info("[%s] Initialization : deviece register\n",
		__func__);

	ret = platform_driver_register(&battery_driver_probe1);

	pr_info("[%s] Initialization : DONE\n",
		__func__);

	return 0;
}

static void __exit battery_exit(void)
{

}
module_init(bq_battery_init);
module_exit(battery_exit);

MODULE_AUTHOR("PS.Z");
MODULE_DESCRIPTION("Battery Device Driver");
MODULE_LICENSE("GPL");
