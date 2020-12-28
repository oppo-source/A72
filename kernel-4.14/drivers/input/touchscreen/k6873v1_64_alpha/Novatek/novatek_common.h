/***************************************************
 * File:nova_common.h
 * VENDOR_EDIT
 * Copyright (c)  2008- 2030  CUSTOM_O Mobile communication Corp.ltd.
 * Description:
 *             nova common driver
 * Version:1.0:
 * Date created:2017/09/18
 * Author: Cong.Dai@Bsp.Driver
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

#ifndef NOVA_H
#define NOVA_H

/*********PART1:Head files**********************/
#include <linux/firmware.h>
#include <linux/rtc.h>
#include <linux/syscalls.h>
#include <linux/time.h>
#include <linux/timer.h>

#include "../touchpanel_common.h"
#include "device_info.h"

/*********PART2:Define Area**********************/
struct nvt_testdata {
	int TX_NUM;
	int RX_NUM;
	int fd;
	int irq_gpio;
	int key_TX;
	int key_RX;
	uint64_t TP_FW;
	const struct firmware *fw;
};

struct nvt_test_header {
	unsigned int magic1;
	unsigned int magic2;
	// normal mode test
	unsigned int array_fw_rawdata_P_offset;
	unsigned int array_fw_rawdata_N_offset;
	unsigned int array_open_rawdata_P_offset;
	unsigned int array_open_rawdata_N_offset;
	signed int config_Lmt_Short_Rawdata_P;
	signed int config_Lmt_Short_Rawdata_N;
	unsigned int config_Diff_Test_Frame;
	signed int config_Lmt_FW_Diff_P;
	signed int config_Lmt_FW_Diff_N;
	signed int config_Lmt_FW_CC_P;
	signed int config_Lmt_FW_CC_N;
	// doze mode test
	unsigned int doze_X_Channel;
	signed int config_Lmt_Doze_Rawdata_P;
	signed int config_Lmt_Doze_Rawdata_N;
	unsigned int config_Doze_Noise_Test_Frame;
	signed int config_Lmt_Doze_Diff_P;
	signed int config_Lmt_Doze_Diff_N;
	// lpwg mode test
	signed int config_Lmt_LPWG_Rawdata_P;
	signed int config_Lmt_LPWG_Rawdata_N;
	signed int config_Lmt_LPWG_Diff_P;
	signed int config_Lmt_LPWG_Diff_N;
	// fdm mode test
	unsigned int fdm_X_Channel;
	signed int config_Lmt_FDM_Rawdata_P;
	signed int config_Lmt_FDM_Rawdata_N;
	unsigned int config_FDM_Noise_Test_Frame;
	signed int config_Lmt_FDM_Diff_P;
	signed int config_Lmt_FDM_Diff_N;
	// offset
	unsigned int array_Short_Rawdata_P_offset;
	unsigned int array_Short_Rawdata_N_offset;
	unsigned int array_FW_CC_P_offset;
	unsigned int array_FW_CC_N_offset;
	unsigned int array_FW_Diff_P_offset;
	unsigned int array_FW_Diff_N_offset;
	unsigned int array_Doze_Diff_P_offset;
	unsigned int array_Doze_Diff_N_offset;
	unsigned int array_Doze_Rawdata_P_offset;
	unsigned int array_Doze_Rawdata_N_offset;
	unsigned int array_LPWG_Rawdata_P_offset;
	unsigned int array_LPWG_Rawdata_N_offset;
	unsigned int array_LPWG_Diff_P_offset;
	unsigned int array_LPWG_Diff_N_offset;
	unsigned int array_FDM_Diff_P_offset;
	unsigned int array_FDM_Diff_N_offset;
	unsigned int array_FDM_Rawdata_P_offset;
	unsigned int array_FDM_Rawdata_N_offset;
	// reserve space
	signed int reserve[16];
};

/*********PART3:Struct Area**********************/
struct nvt_proc_operations {
	void (*auto_test)(struct seq_file *s, void *chip_data,
			  struct nvt_testdata *nvt_testdata);
};

/*********PART4:function declare*****************/
int nvt_create_proc(struct touchpanel_data *ts,
		    struct nvt_proc_operations *nvt_ops);
void nvt_flash_proc_init(struct touchpanel_data *ts, const char *name);
void nvt_limit_read(struct seq_file *s, struct touchpanel_data *ts);

#endif
