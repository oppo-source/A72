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

#define pr_fmt(fmt) "<sensorlist> " fmt

#include <linux/module.h>

#include "sensor_list.h"
#include "hf_sensor_type.h"

//#ifdef VENDOR_EDIT
/* laq@PSW.BSP.sensor,2019/12/20, add for 19040 */

int sensorlist_sensor_to_handle(int sensor)
{
	int handle = -1;

	switch (sensor) {
	case SENSOR_TYPE_ACCELEROMETER:
		handle = accel_handle;
		break;
	case SENSOR_TYPE_GYROSCOPE:
		handle = gyro_handle;
		break;
	case SENSOR_TYPE_MAGNETIC_FIELD:
		handle = mag_handle;
		break;
	case SENSOR_TYPE_LIGHT:
		handle = als_handle;
		break;
	case SENSOR_TYPE_PROXIMITY:
		handle = ps_handle;
		break;
	case SENSOR_TYPE_PRESSURE:
		handle = baro_handle;
		break;
	case SENSOR_TYPE_SAR:
		handle = sar_handle;
		break;
//#ifdef VENDOR_EDIT
//Bo.Xiang@PSW.BSP.Sensor,2020.2.28 remove sensortype OIS added by MTK
// case SENSOR_TYPE_OIS:
//      handle = ois;
//      break;
//#endif
	}
	return handle;
}

int sensorlist_handle_to_sensor(int handle)
{
	int type = -1;

	switch (handle) {
	case accel_handle:
		type = SENSOR_TYPE_ACCELEROMETER;
		break;
	case gyro_handle:
		type = SENSOR_TYPE_GYROSCOPE;
		break;
	case mag_handle:
		type = SENSOR_TYPE_MAGNETIC_FIELD;
		break;
	case als_handle:
		type = SENSOR_TYPE_LIGHT;
		break;
	case ps_handle:
		type = SENSOR_TYPE_PROXIMITY;
		break;
	case baro_handle:
		type = SENSOR_TYPE_PRESSURE;
		break;
	case sar_handle:
		type = SENSOR_TYPE_SAR;
		break;
//#ifdef VENDOR_EDIT
//Bo.Xiang@PSW.BSP.Sensor,2020.2.28 remove sensortype OIS added by MTK
//   case ois:
//       type = SENSOR_TYPE_OIS;
//       break;
//#endif
	}
	return type;
}
//#endif
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("dynamic sensorlist driver");
MODULE_AUTHOR("hongxu.zhao@mediatek.com");
