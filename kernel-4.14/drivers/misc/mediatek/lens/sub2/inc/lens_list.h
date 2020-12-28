/*
 * Copyright (C) 2015 MediaTek Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#ifndef _LENS_LIST_H

#define _LENS_LIST_H

#ifdef VENDOR_EDIT
/* Feiping@Cam.Drv, 20200102, add for 19040 AF*/
#define BU64253AF_SetI2Cclient BU64253AF_SetI2Cclient_Sub2
#define BU64253AF_Ioctl BU64253AF_Ioctl_Sub2
#define BU64253AF_Release BU64253AF_Release_Sub2
#define BU64253AF_GetFileName BU64253AF_GetFileName_Sub2
extern int BU64253AF_SetI2Cclient(struct i2c_client *pstAF_I2Cclient,
				 spinlock_t *pAF_SpinLock, int *pAF_Opened);
extern long BU64253AF_Ioctl(struct file *a_pstFile, unsigned int a_u4Command,
			   unsigned long a_u4Param);
extern int BU64253AF_Release(struct inode *a_pstInode, struct file *a_pstFile);
extern int BU64253AF_GetFileName(unsigned char *pFileName);
#endif

#if 0
#define AK7371AF_SetI2Cclient AK7371AF_SetI2Cclient_Sub2
#define AK7371AF_Ioctl AK7371AF_Ioctl_Sub2
#define AK7371AF_Release AK7371AF_Release_Sub2
#define AK7371AF_PowerDown AK7371AF_PowerDown_Sub2
#define AK7371AF_GetFileName AK7371AF_GetFileName_Sub2
extern int AK7371AF_SetI2Cclient(struct i2c_client *pstAF_I2Cclient,
				 spinlock_t *pAF_SpinLock, int *pAF_Opened);
extern long AK7371AF_Ioctl(struct file *a_pstFile, unsigned int a_u4Command,
			   unsigned long a_u4Param);
extern int AK7371AF_Release(struct inode *a_pstInode, struct file *a_pstFile);
extern int AK7371AF_PowerDown(struct i2c_client *pstAF_I2Cclient,
				int *pAF_Opened);
extern int AK7371AF_GetFileName(unsigned char *pFileName);
#endif

#endif
