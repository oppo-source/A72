/***************************************************
 * File:himax_common.h
 * VENDOR_EDIT
 * Copyright (c)  2008- 2030  CUSTOM_O Mobile communication Corp.ltd.
 * Description:
 *             himax common driver
 * Version:1.0:
 * Date created:2016/09/02
 * Author: Tong.han@Bsp.Driver
 * TAG: BSP.TP.Init
 * *
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
 * -------------- Revision History: -----------------
 *  <author >  <data>  <version>  <desc>
 ***************************************************/

#ifndef HIMAX_COMMON_H
#define HIMAX_COMMON_H

/*********PART1:Head files**********************/
#include <linux/firmware.h>
#include <linux/rtc.h>
#include <linux/syscalls.h>
#include <linux/time.h>
#include <linux/timer.h>

#include "../touchpanel_common.h"
#include "device_info.h"

/*********PART2:Define Area**********************/
#define SYNAPTICS_RMI4_PRODUCT_ID_SIZE 10
#define SYNAPTICS_RMI4_PRODUCT_INFO_SIZE 2

#define DiagonalUpperLimit 1100
#define DiagonalLowerLimit 900

/*********PART3:Struct Area**********************/
struct image_header {
	/* 0x00 - 0x0f */
	unsigned char checksum[4];
	unsigned char reserved_04;
	unsigned char reserved_05;
	unsigned char options_firmware_id : 1;
	unsigned char options_contain_bootloader : 1;
	/* only available in s4322 , reserved in other, begin*/
	unsigned char options_guest_code : 1;
	unsigned char options_tddi : 1;
	unsigned char options_reserved : 4;
	/* only available in s4322 , reserved in other ,  end*/
	unsigned char bootloader_version;
	unsigned char firmware_size[4];
	unsigned char config_size[4];
	/* 0x10 - 0x1f */
	unsigned char product_id[SYNAPTICS_RMI4_PRODUCT_ID_SIZE];
	unsigned char package_id[2];
	unsigned char package_id_revision[2];
	unsigned char product_info[SYNAPTICS_RMI4_PRODUCT_INFO_SIZE];
	/* 0x20 - 0x2f */
	/* only available in s4322 , reserved in other, begin*/
	unsigned char bootloader_addr[4];
	unsigned char bootloader_size[4];
	unsigned char ui_addr[4];
	unsigned char ui_size[4];
	/* only available in s4322 , reserved in other ,  end*/
	/* 0x30 - 0x3f */
	unsigned char ds_id[16];
	/* 0x40 - 0x4f */
	/* only available in s4322 , reserved in other, begin*/
	union {
		struct {
			unsigned char dsp_cfg_addr[4];
			unsigned char dsp_cfg_size[4];
			unsigned char reserved_48_4f[8];
		};
	};
	/* only available in s4322 , reserved in other ,  end*/
	/* 0x50 - 0x53 */
	unsigned char firmware_id[4];
};

struct image_header_data {
	bool contains_firmware_id;
	unsigned int firmware_id;
	unsigned int checksum;
	unsigned int firmware_size;
	unsigned int config_size;
	/* only available in s4322 , reserved in other, begin*/
	unsigned int disp_config_offset;
	unsigned int disp_config_size;
	unsigned int bootloader_offset;
	unsigned int bootloader_size;
	/* only available in s4322 , reserved in other ,  end*/
	unsigned char bootloader_version;
	unsigned char product_id[SYNAPTICS_RMI4_PRODUCT_ID_SIZE + 1];
	unsigned char product_info[SYNAPTICS_RMI4_PRODUCT_INFO_SIZE];
};

struct test_header {
	unsigned int magic1;
	unsigned int magic2;
	unsigned int withCBC;
	unsigned int array_limit_offset;
	unsigned int array_limit_size;
	unsigned int array_limitcbc_offset;
	unsigned int array_limitcbc_size;
};

struct syna_testdata {
	int TX_NUM;
	int RX_NUM;
	int fd;
	int irq_gpio;
	int key_TX;
	int key_RX;
	uint64_t TP_FW;
	const struct firmware *fw;
};

// import from
// "android/bootable/bootloader/lk/platform/msm_shared/include/msm_panel.h"
enum {
	CUSTOM_O_16037JDI_R63452_1080P_CMD_PANEL = 13,
	CUSTOM_O_16037SAMSUNG_S6E3FA5_1080P_CMD_PANEL = 14,
	UNKNOWN_PANEL
};

struct himax_proc_operations {
	void (*auto_test)(struct seq_file *s, void *chip_data,
			  struct syna_testdata *syna_testdata);
	size_t (*himax_proc_register_write)(struct file *file, const char *buff,
					    size_t len, loff_t *pos);
	size_t (*himax_proc_register_read)(struct file *file, char *buff,
					   size_t len, loff_t *pos);
	size_t (*himax_proc_diag_write)(struct file *file, const char *buff,
					size_t len, loff_t *pos);
	size_t (*himax_proc_diag_read)(struct file *file, char *buff,
				       size_t len, loff_t *pos);
	size_t (*himax_proc_DD_debug_read)(struct file *file, char *buf,
					   size_t len, loff_t *pos);
	size_t (*himax_proc_DD_debug_write)(struct file *file, const char *buff,
					    size_t len, loff_t *pos);
	size_t (*himax_proc_FW_debug_read)(struct file *file, char *buff,
					   size_t len, loff_t *pos);
	size_t (*himax_proc_reset_write)(struct file *file, const char *buff,
					 size_t len, loff_t *pos);
	size_t (*himax_proc_sense_on_off_write)(struct file *file,
						const char *buff, size_t len,
						loff_t *pos);
};

void himax_limit_read(struct seq_file *s, struct touchpanel_data *ts);
int himax_create_proc(struct touchpanel_data *ts,
		      struct himax_proc_operations *syna_ops);
void himax_parse_header(struct image_header_data *header,
			const unsigned char *fw_image);

#endif
