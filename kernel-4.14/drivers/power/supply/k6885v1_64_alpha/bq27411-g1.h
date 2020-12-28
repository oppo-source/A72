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

#ifndef _BQ27411_GAUGE_H
#define _BQ27411_GAUGE_H


struct bq27411_device_info {
	struct i2c_client *client;
	struct device *dev;
#ifdef CONFIG_PM_WAKELOCKS
	struct wakeup_source fg_int;
#else
	struct wake_lock fg_int;
#endif

};


struct bq_platform_power_supply{
#ifdef CONFIG_PM_WAKELOCKS
	struct wakeup_source fg_wakelock;
#else
	struct wake_lock fg_wakelock;
#endif
	/*wait_queue*/
	wait_queue_head_t  wait_que;
	bool thread_timeout;
	spinlock_t slock;
	struct hrtimer battery_thread_timer;
	struct mutex bat_i2c_access;
	int status;
	int health;
	int technology;
	int capacity;
	int avg_current;
	int voltage;
	int temp;
	int charge_full;
	int charge_counter;
	int present;
};

#define BQ_pwrsply_Interval 10

#define BQ27411_ADDRESS				(0x55)


/* core control R/W  2bytes*/
#define BQ27411_REG_Control							(0x00)
#define BQ27411_REG_Temperature						(0x02)

/* Read only 2bytes*/
#define BQ27411_REG_Voltage							(0x04)
#define BQ27411_REG_Flags							(0x06)
#define BQ27411_REG_NominalAvailableCapacity		(0x08)
#define BQ27411_REG_FullAvailableCapacity			(0x0A)
#define BQ27411_REG_RemainingCapacity				(0x0C)
#define BQ27411_REG_FullChargeCapacity				(0x0E)
#define BQ27411_REG_AverageCurrent					(0x10)
#define BQ27411_REG_StandbyCurrent					(0x12)
#define BQ27411_REG_MaxLoadCurrent					(0x14)
#define BQ27411_REG_AveragePower					(0x18)
#define BQ27411_REG_StateOfCharge					(0x1C)
#define BQ27411_REG_InternalTemperature				(0x1E)
#define BQ27411_REG_StateOfHealth					(0x20)
#define BQ27411_REG_RemainingCapacityUnfiltered		(0x28)
#define BQ27411_REG_RemainingCapacityFiltered		(0x2A)
#define BQ27411_REG_FullChargeCapacityUnfiltered	(0x2C)
#define BQ27411_REG_FullChargeCapacityFlitered		(0x2E)
#define BQ27411_REG_StateOfChargeUnfiltered			(0x30)


#define BQ27411_CHIP_ID			(0x30)




#define ATTR_SHOW_BQ(NAME, REGISTER) \
static ssize_t show_##NAME(struct device *dev,\
		struct device_attribute *attr, char *buf)\
{\
	int ver;\
	ver = bq_read_i2c_word(REGISTER);\
	return sprintf(buf, "%hd  %x\n", (short)ver, ver);\
} \
static DEVICE_ATTR(NAME, S_IRUGO, show_##NAME, NULL);

extern int bq_read_i2c_word(u32 addr);
extern struct bq27411_device_info *di_info;



#endif /* _BQ27411_GAUGE_H */
