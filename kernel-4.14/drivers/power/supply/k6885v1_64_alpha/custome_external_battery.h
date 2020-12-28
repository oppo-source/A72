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

#ifndef _CUSTOM_BATTERY
#define _CUSTOM_BATTERY

struct bq_gm {
	bool disablegauge;
	bool disablethread;
	bool forcetemp25;
};

extern signed int gauge_get_bat_current(void);
extern signed int gauge_get_bat_uisoc(void);
extern signed int gauge_get_bat_soc(void);
extern signed int gauge_get_bat_voltage(void);
extern signed int gauge_get_bat_temperature(void);
extern struct bq_gm bq_status;


#define _CODE_DEFINEDE \
signed int battery_get_bat_voltage(void)\
{\
	return gauge_get_bat_voltage();\
} \
\
signed int battery_get_bat_current(void)\
{\
	return gauge_get_bat_current()*10;\
} \
\
signed int battery_get_bat_current_mA(void)\
{\
	return gauge_get_bat_current();\
} \
\
signed int battery_get_soc(void)\
{\
	return gauge_get_bat_soc();\
} \
\
signed int battery_get_uisoc(void)\
{\
	return gauge_get_bat_uisoc();\
} \
\
signed int battery_get_bat_temperature(void)\
{\
		return gauge_get_bat_temperature();\
} \
\
signed int battery_get_ibus(void)\
{\
	return pmic_get_ibus();\
} \
\
signed int battery_get_vbus(void)\
{\
	return charger_get_vbus();\
} \
\
signed int battery_get_bat_avg_current(void)\
{\
	return gauge_get_bat_current();\
} \
\
signed int battery_get_bat_uisoc(void)\
{\
	return battery_get_uisoc();\
} \
\
signed int battery_get_bat_soc(void)\
{\
	return battery_get_soc();\
} \
\
bool battery_get_bat_current_sign(void)\
{\
	if (gauge_get_bat_current() < 0)\
		return 1;\
	else\
		return 0;\
} \
\
int get_ui_soc(void)\
{\
	return battery_get_uisoc();\
} \
\
signed int battery_get_bat_avg_voltage(void)\
{\
	return gauge_get_bat_voltage();\
} \
\
signed int battery_meter_get_battery_current(void)\
{\
	return battery_get_bat_current();\
} \
\
bool battery_meter_get_battery_current_sign(void)\
{\
	return battery_get_bat_current_sign();\
} \
\
signed int battery_meter_get_battery_temperature(void)\
{\
	return battery_get_bat_temperature();\
} \
\
signed int battery_meter_get_charger_voltage(void)\
{\
	return battery_get_vbus();\
} \
\
unsigned long BAT_Get_Battery_Current(int polling_mode)\
{\
	return (long)battery_get_bat_avg_current();\
} \
\
unsigned long BAT_Get_Battery_Voltage(int polling_mode)\
{\
	long int ret;\
\
	ret = (long)battery_get_bat_voltage();\
	return ret;\
} \
\
unsigned int bat_get_ui_percentage(void)\
{\
	return battery_get_uisoc();\
} \
\
unsigned int battery_get_is_kpoc(void)\
{\
	return is_kernel_power_off_charging();\
} \
\
bool battery_is_battery_exist(void)\
{\
	return 1;\
} \
\
\
void wake_up_bat(void)\
{\
} \
EXPORT_SYMBOL(wake_up_bat);\
\
int en_intr_VBATON_UNDET(int en)\
{\
	return 0;\
} \
\
int reg_VBATON_UNDET(void (*callback)(void))\
{\
	return 0;\
}



#define FG_DEV_NODE_ATTR(SYSFSNAME, VALUE) \
static ssize_t show_FG_##SYSFSNAME##_disable(\
	struct device *dev, struct device_attribute *attr, char *buf)\
{\
	pr_info("[FG] show FG ##SYSFSNAME : %d\n", VALUE);\
	return sprintf(buf, "%d\n", VALUE);\
} \
\
static ssize_t store_FG_##SYSFSNAME##_disable(\
	struct device *dev, struct device_attribute *attr,\
					const char *buf, size_t size)\
{\
	VALUE = true;\
	_wake_up_battery(battery_info);\
	return size;\
} \
static DEVICE_ATTR(\
	FG_##SYSFSNAME##_disable, 0664,\
	show_FG_##SYSFSNAME##_disable, store_FG_##SYSFSNAME##_disable);\
\
static ssize_t show_FG_##SYSFSNAME##_enable(\
	struct device *dev, struct device_attribute *attr, char *buf)\
{\
	pr_info("[FG] show FG disable##SYSFSNAME : %d\n", VALUE);\
	return sprintf(buf, "%d\n", VALUE);\
} \
\
static ssize_t store_FG_##SYSFSNAME##_enable(\
	struct device *dev, struct device_attribute *attr,\
					const char *buf, size_t size)\
{\
	VALUE = false;\
	_wake_up_battery(battery_info);\
	return size;\
} \
static DEVICE_ATTR(\
	FG_##SYSFSNAME##_enable, 0664,\
	show_FG_##SYSFSNAME##_enable, store_FG_##SYSFSNAME##_enable);\


#endif
