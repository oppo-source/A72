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

#ifndef _SENSOR_LIST_H_
#define _SENSOR_LIST_H_

//#ifdef VENDOR_EDIT
/* laq@PSW.BSP.sensor,2019/12/20, add for 19040 */
enum sensorlist {
	accel_handle,
	gyro_handle,
	mag_handle,
	als_handle,
	ps_handle,
	baro_handle,
	sar_handle,
	ois,
	maxhandle,
};
//#endif

int sensorlist_sensor_to_handle(int sensor);
int sensorlist_handle_to_sensor(int handle);

#endif
