/***************************************************
 * File:touch.c
 * VENDOR_EDIT
 * Copyright (c)  2008- 2030  CUSTOM_O Mobile communication Corp.ltd.
 * Description:
 *			 tp dev
 * Version:1.0:
 * Date created:2016/09/02
 * Author: hao.wang@Bsp.Driver
 * TAG: BSP.TP.Init
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include "touch.h"
#include "device_info.h"
#include "touchpanel_common.h"
#include "tp_devices.h"
#include <linux/errno.h>
#include <linux/input.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/serio.h>
#include <linux/slab.h>

#define MAX_LIMIT_DATA_LENGTH 100
#define PROJECT 19131
#define NT36672C "NT36672C"
#define HX83112F "HX83112F"
#define CUSTOM_O_TP_NOFLASH "CUSTOM_O_TP_NOFLASH"

/*if can not compile success, please update vendor/custom_o_touchsreen*/
struct tp_dev_name tp_dev_names[] = {
	{ TP_OFILM, "OFILM" },
	{ TP_BIEL, "BIEL" },
	{ TP_TRULY, "TRULY" },
	{ TP_BOE, "BOE" },
	{ TP_G2Y, "G2Y" },
	{ TP_TPK, "TPK" },
	{ TP_JDI, "JDI" },
	{ TP_TIANMA, "TIANMA" },
	{ TP_SAMSUNG, "SAMSUNG" },
	{ TP_DSJM, "DSJM" },
	{ TP_BOE_B8, "BOEB8" },
	//{TP_TM, "TM"},
	{ TP_UNKNOWN, "UNKNOWN" },
};

int g_tp_dev_vendor = TP_UNKNOWN;
enum TP_USED_INDEX {
	TP_INDEX_NULL,
	himax_83112a,
	himax_83112f,
	ili9881_auo,
	ili9881_tm,
	nt36525b_boe,
	nt36525b_hlt,
	nt36672c,
	ili9881_inx
};
enum TP_USED_INDEX tp_used_index = TP_INDEX_NULL;

#define GET_TP_DEV_NAME(tp_type)                                               \
	((tp_dev_names[tp_type].type == (tp_type))                             \
	     ? tp_dev_names[tp_type].name                                      \
	     : "UNMATCH")

bool __init tp_judge_ic_match(char *tp_ic_name)
{
	pr_info("[TP] tp_ic_name = %s\n", tp_ic_name);
	// pr_info("[TP] get_project() = %d\n", get_project());
	pr_info("[TP] boot_command_line = %s\n", boot_command_line);

	if (strstr(tp_ic_name, "novatek,nf_nt36672c") &&
	    strstr(boot_command_line,
		   "nt36672c_fhdp_dsi_vdo_auo_cphy_90hz_tianma")) {
		pr_info("[TP] touch ic = nt36672c_tianma\n");
		tp_used_index = nt36672c;
		g_tp_dev_vendor = TP_TIANMA;
		return true;
	}
	if (strstr(tp_ic_name, "novatek,nf_nt36672c")
/*&& strstr(boot_command_line, "nt36672c_fhdp_dsi_vdo_auo_cphy_90hz_jdi")*/) {
		pr_info("[TP] touch ic = nt36672c_jdi\n");
		tp_used_index = nt36672c;
		g_tp_dev_vendor = TP_JDI;
		return true;
	}
	if (strstr(tp_ic_name, "himax,hx83112f_nf") &&
	    strstr(boot_command_line,
		   "hx83112f_fhdp_dsi_vdo_auo_cphy_90hz_jdi")) {
		pr_info("[TP] touch ic = hx83112f_jdi\n");
		tp_used_index = himax_83112f;
		g_tp_dev_vendor = TP_JDI;
		return true;
	}

	pr_info("[TP]: Lcd module not found\n");
	return false;
}

int tp_util_get_vendor(struct hw_resource *hw_res,
		       struct panel_info *panel_data)
{

	char *vendor;

	panel_data->test_limit_name =
	    kzalloc(MAX_LIMIT_DATA_LENGTH, GFP_KERNEL);
	if (panel_data->test_limit_name == NULL)
		pr_info("[TP]panel_data.test_limit_name kzalloc error\n");


	panel_data->extra = kzalloc(MAX_LIMIT_DATA_LENGTH, GFP_KERNEL);


	/*TP is first distingwished by gpio pins, and then by other ways*/
	panel_data->tp_type = g_tp_dev_vendor;
	memcpy(panel_data->manufacture_info.version, "0xDD3000000", 11);

	if (panel_data->tp_type == TP_UNKNOWN) {
		pr_info("[TP]%s type is unknown\n", __func__);
		return 0;
	}

	vendor = GET_TP_DEV_NAME(panel_data->tp_type);
	strcpy(panel_data->manufacture_info.manufacture, vendor);

	if (tp_used_index == nt36672c) {
		snprintf(panel_data->fw_name, MAX_FW_NAME_LENGTH,
			 "tp/19131/FW_%s_%s.img", "NF_NT36672C", vendor);

		if (panel_data->test_limit_name) {
			snprintf(
			    panel_data->test_limit_name, MAX_LIMIT_DATA_LENGTH,
			    "tp/19131/LIMIT_%s_%s.img", "NF_NT36672C", vendor);
		}

		if (panel_data->extra) {
			snprintf(panel_data->extra, MAX_LIMIT_DATA_LENGTH,
				 "tp/19131/BOOT_FW_%s_%s.ihex", "NF_NT36672C",
				 vendor);
		}
	}

	if (tp_used_index == himax_83112f) {
		snprintf(panel_data->fw_name, MAX_FW_NAME_LENGTH,
			 "tp/19131/FW_%s_%s.img", "NF_HX83112F", vendor);

		if (panel_data->test_limit_name) {
			snprintf(
			    panel_data->test_limit_name, MAX_LIMIT_DATA_LENGTH,
			    "tp/19131/LIMIT_%s_%s.img", "NF_HX83112F", vendor);
		}

		if (panel_data->extra) {
			snprintf(panel_data->extra, MAX_LIMIT_DATA_LENGTH,
				 "tp/19131/BOOT_FW_%s_%s.ihex", "NF_HX83112F",
				 vendor);
		}
	}

	panel_data->manufacture_info.fw_path = panel_data->fw_name;
	// if (strstr(panel_data->chip_name, CUSTOM_O_TP_NOFLASH)) {
	if (tp_used_index == nt36672c) {
		pr_info("[TP]: firmware_headfile = NT36672C\n");
		panel_data->firmware_headfile.firmware_data =
		    FW_19131_NT36672C_JDI;
		panel_data->firmware_headfile.firmware_size =
		    sizeof(FW_19131_NT36672C_JDI);
	}
	// if (strstr(panel_data->chip_name, CUSTOM_O_TP_NOFLASH)) {
	if (tp_used_index == himax_83112f) {
		pr_info("[TP]: firmware_headfile = HX83112F\n");
		panel_data->firmware_headfile.firmware_data =
		    FW_19131_HX83112F_JDI;
		panel_data->firmware_headfile.firmware_size =
		    sizeof(FW_19131_HX83112F_JDI);
	}

	pr_info("[TP]: Vendor:%s\n", vendor);
	pr_info("[TP]: Fw:%s\n", panel_data->fw_name);
	pr_info("[TP]: Limit:%s\n", panel_data->test_limit_name == NULL
					? "NO Limit"
					: panel_data->test_limit_name);
	pr_info("[TP]: Extra:%s\n",
		panel_data->extra == NULL ? "NO Extra" : panel_data->extra);
	// pr_info("[TP]: is matched %d, type %d\n", is_tp_type_got_in_match,
	// panel_data->tp_type);
	return 0;

#if 0
	char *vendor;

	panel_data->test_limit_name = kzalloc(MAX_LIMIT_DATA_LENGTH,
		GFP_KERNEL);

	snprintf(panel_data->fw_name, MAX_FW_NAME_LENGTH,
	"tp/19131/FW_NV36672C_JDI.img");

	if (panel_data->test_limit_name) {
		snprintf(panel_data->test_limit_name, MAX_LIMIT_DATA_LENGTH,
		"tp/19131/LIMIT_NV36672C_JDI.img");
	}

	memcpy(panel_data->manufacture_info.version, "NVT_JDI", 7);
	panel_data->firmware_headfile.firmware_data = FW_19131_NT36672C_JDI;
	panel_data->firmware_headfile.firmware_size =
		sizeof(FW_19131_NT36672C_JDI);
	pr_info("[TP] firmware_size = %d\n",
		panel_data->firmware_headfile.firmware_size);

//	panel_data->version_len = strlen(panel_data->manufacture_info.version);
	panel_data->manufacture_info.fw_path = panel_data->fw_name;

	pr_info("[TP]vendor:%s fw:%s limit:%s\n",
		vendor,
		panel_data->fw_name,
		panel_data->test_limit_name);

	return 0;
#endif
}
