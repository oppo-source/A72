/**
 * Copyright 2008-2013 CUSTOM_O Mobile Comm Corp., Ltd, All rights reserved.
 * VENDOR_EDIT:
 * FileName:devinfo.h
 * ModuleName:devinfo
 * Author: wangjc
 * Create Date: 2013-10-23
 * Description:add interface to get device information.
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * INFORMATION CONTAINED IN THIS DOCUMENT IS PROVIDED "AS-IS," AND SYNAPTICS
 * EXPRESSLY DISCLAIMS ALL EXPRESS AND IMPLIED WARRANTIES, INCLUDING ANY
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE,
 * AND ANY WARRANTIES OF NON-INFRINGEMENT OF ANY INTELLECTUAL PROPERTY RIGHTS.
 * IN NO EVENT SHALL SYNAPTICS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, PUNITIVE, OR CONSEQUENTIAL DAMAGES ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OF THE INFORMATION CONTAINED IN THIS DOCUMENT, HOWEVER CAUSED
 * AND BASED ON ANY THEORY OF LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, AND EVEN IF SYNAPTICS WAS ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE. IF A TRIBUNAL OF COMPETENT JURISDICTION DOES
 * NOT PERMIT THE DISCLAIMER OF DIRECT DAMAGES OR ANY OTHER DAMAGES, SYNAPTICS'
 * TOTAL CUMULATIVE LIABILITY TO ANY PARTY SHALL NOT EXCEED ONE HUNDRED U.S.
 * DOLLARS.
 * History:
   <version >  <time>  <author>  <desc>
   1.0		2013-10-23	wangjc	init
 */

#ifndef _DEVICE_INFO_H
#define _DEVICE_INFO_H

// dram type
enum {
	DRAM_TYPE0 = 0,
	DRAM_TYPE1,
	DRAM_TYPE2,
	DRAM_TYPE3,
	DRAM_UNKNOWN,
};

struct manufacture_info {
	char *version;
	char *manufacture;
	char *fw_path;
};

struct o_ufsplus_status {
	int *hpb_status;
	int *tw_status;
};

#if 0
int register_device_proc(char *name, char *version, char *manufacture);
int register_device_proc_for_ufsplus(char *name, int *hpb_status,
	int *tw_status);
int register_devinfo(char *name, struct manufacture_info *info);
#else
// int register_device_proc(char *name, char *version, char *manufacture){return
// 0;}
// int register_device_proc_for_ufsplus(char *name, int *hpb_status,int
// *tw_status){return 0;}
// int register_devinfo(char *name, struct manufacture_info *info){return 0;}
#endif

#endif /*_DEVICE_INFO_H*/
