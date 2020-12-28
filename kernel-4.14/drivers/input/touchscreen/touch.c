/***************************************************
 * File:touch.c
 * VENDOR_EDIT
 * Copyright (c)  2008- 2030  Oppo Mobile communication Corp.ltd.
 * Description:
 *             tp dev
 * Version:1.0:
 * Date created:2016/09/02
 * Author: hao.wang@Bsp.Driver
 * TAG: BSP.TP.Init
*/

#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/serio.h>
#include "oppo_touchscreen/tp_devices.h"
#include "oppo_touchscreen/touchpanel_common.h"

#include <soc/oppo/oppo_project.h>
#include "touch.h"


#define MAX_LIMIT_DATA_LENGTH         100

/*if can not compile success, please update vendor/oppo_touchsreen*/
struct tp_dev_name tp_dev_names[] = {
    {TP_OFILM, "OFILM"},
    {TP_BIEL, "BIEL"},
    {TP_TRULY, "TRULY"},
    {TP_BOE, "BOE"},
    {TP_G2Y, "G2Y"},
    {TP_TPK, "TPK"},
    {TP_JDI, "JDI"},
    {TP_TIANMA, "TIANMA"},
    {TP_SAMSUNG, "SAMSUNG"},
    {TP_DSJM, "DSJM"},
    {TP_BOE_B8, "BOEB8"},
    {TP_INNOLUX, "INNOLUX"},
    {TP_HIMAX_DPT, "DPT"},
    {TP_AUO, "AUO"},
    {TP_DEPUTE, "DEPUTE"},
    {TP_HUAXING, "HUAXING"},
    {TP_HLT, "HLT"},
    {TP_DJN, "DJN"},
    {TP_UNKNOWN, "UNKNOWN"},
};

typedef enum {
    TP_INDEX_NULL,
    himax_83112a,
    himax_83112f,
    ili9881_auo,
    ili9881_tm,
    nt36525b_boe,
    nt36525b_hlt,
    nt36672c,
    ili9881_inx,
    goodix_gt9886,
    focal_ft3518,
    ili7807s_tm,
    nt36672c_boe
} TP_USED_INDEX;
TP_USED_INDEX tp_used_index  = TP_INDEX_NULL;

#define GET_TP_DEV_NAME(tp_type) ((tp_dev_names[tp_type].type == (tp_type))?tp_dev_names[tp_type].name:"UNMATCH")

int g_tp_dev_vendor = TP_UNKNOWN;
char *g_tp_chip_name;
struct hw_resource *g_hw_res;
static bool is_tp_type_got_in_match = false;    /*indicate whether the tp type is got in the process of ic match*/

void primary_display_esd_check_enable(int enable)
{
    return;
}
EXPORT_SYMBOL(primary_display_esd_check_enable);

/*
* this function is used to judge whether the ic driver should be loaded
* For incell module, tp is defined by lcd module, so if we judge the tp ic
* by the boot command line of containing lcd string, we can also get tp type.
*/
bool __init tp_judge_ic_match(char *tp_ic_name)
{
    pr_err("[TP] tp_ic_name = %s \n", tp_ic_name);
    pr_err("[TP] get_project() = %d \n", get_project());
    pr_err("[TP] boot_command_line = %s \n", boot_command_line);

    switch(get_project()) {
    case 19165:
    case 19166:
        is_tp_type_got_in_match = true;
        if (strstr(tp_ic_name, "Goodix-gt9886") && strstr(boot_command_line, "s68fc01")) {
            g_tp_dev_vendor = TP_SAMSUNG;
            return true;
        }
        if (strstr(tp_ic_name, "synaptics-s3706") && strstr(boot_command_line, "ams641rw01")) {
            g_tp_dev_vendor = TP_SAMSUNG;
            return true;
        }
        break;
    case 19040:
        is_tp_type_got_in_match = true;
        if (strstr(tp_ic_name, "sec-s6sy771")) {
            g_tp_dev_vendor = TP_SAMSUNG;
            return true;
        }
        break;
    case 19131:
    case 19132:
    case 19133:
    case 19420:
    case 19421:
#ifdef CONFIG_MACH_MT6873
        pr_info("[TP] case 19131\n");
        is_tp_type_got_in_match = true;
        if (strstr(tp_ic_name, "novatek,nf_nt36672c") && strstr(boot_command_line, "nt36672c_fhdp_dsi_vdo_auo_cphy_90hz_tianma")) {
            pr_err("[TP] touch ic = nt36672c_tianma \n");
            tp_used_index = nt36672c;
            g_tp_dev_vendor = TP_TIANMA;
            return true;
        }
        if (strstr(tp_ic_name, "novatek,nf_nt36672c") && strstr(boot_command_line, "nt36672c_fhdp_dsi_vdo_auo_cphy_120hz_tianma")) {
            pr_err("[TP] touch ic = nt36672c_tianma \n");
            tp_used_index = nt36672c;
            g_tp_dev_vendor = TP_TIANMA;
            return true;
        }
        if (strstr(tp_ic_name, "novatek,nf_nt36672c") && strstr(boot_command_line, "nt36672c_fhdp_dsi_vdo_auo_cphy_90hz_jdi")) {
            pr_err("[TP] touch ic = nt36672c_jdi \n");
            tp_used_index = nt36672c;
            g_tp_dev_vendor = TP_JDI;
            return true;
        }
        if (strstr(tp_ic_name, "himax,hx83112f_nf") && strstr(boot_command_line, "hx83112f_fhdp_dsi_vdo_auo_cphy_90hz_jdi")) {
            pr_err("[TP] touch ic = hx83112f_jdi \n");
            tp_used_index = himax_83112f;
            g_tp_dev_vendor = TP_JDI;
            return true;
        }
#endif
        break;
    case 20051:
        pr_info("[TP] case 20051\n");
        is_tp_type_got_in_match = true;
        pr_err("[TP] touch ic = nt36672c_jdi \n");
        tp_used_index = nt36672c;
        g_tp_dev_vendor = TP_JDI;
        return true;
        break;
    case 20001:
    case 20002:
    case 20003:
    case 20200:
        pr_info("[TP] case 20001\n");
        is_tp_type_got_in_match = true;
        if (strstr(tp_ic_name, "novatek,nf_nt36672c") && strstr(boot_command_line, "nt36672c")) {
            pr_err("[TP] touch ic = nt36672c_jdi \n");
            tp_used_index = nt36672c;
            g_tp_dev_vendor = TP_JDI;
            return true;
        }
        if (strstr(tp_ic_name, "himax,hx83112f_nf") && strstr(boot_command_line, "hx83112f")) {
            pr_err("[TP] touch ic = hx83112f_tianma \n");
            tp_used_index = himax_83112f;
            g_tp_dev_vendor = TP_TIANMA;
            return true;
        }
        if (strstr(tp_ic_name, "ilitek,ili7807s") && strstr(boot_command_line, "ili7807s")) {
            pr_err("[TP] touch ic = ili7807s_tianma \n");
            tp_used_index = ili7807s_tm;
            g_tp_dev_vendor = TP_TIANMA;
            return true;
        }
        break;
    case 20075:
    case 20076:
        pr_info("[TP] case 20075\n");
        is_tp_type_got_in_match = true;
        /* Goodix GT9886 */
        //if (strstr(tp_ic_name, "Goodix-gt9886") && strstr(boot_command_line, "oppo20075_samsung_dsi")) {
        //    tp_used_index = goodix_gt9886;
        //    g_tp_dev_vendor = TP_SAMSUNG;
        //    pr_info("[TP] 20075: GT9886 got matched \n");
        //    return true;
        //}
        /* Focal FT3518 */
        if (strstr(tp_ic_name, "focaltech,fts")){
            g_tp_dev_vendor = TP_SAMSUNG;
            tp_used_index = focal_ft3518;
            pr_info("[TP] 20075: FT3518 got matched \n");
            return true;
        }
        break;
    case 20041:
    case 20042:
        pr_info("[TP] case 20041\n");
        is_tp_type_got_in_match = true;
        if (strstr(tp_ic_name, "novatek,nf_nt36672c") && strstr(boot_command_line, "nt36672c_fhdp_dsi_vdo_dphy_jdi_lcm_drv")) {
            pr_err("[TP] touch ic = nt36672c_jdi \n");
            tp_used_index = nt36672c;
            g_tp_dev_vendor = TP_JDI;
            return true;
        }
        if (strstr(tp_ic_name, "novatek,nf_nt36672c") && strstr(boot_command_line, "nt36672c_fhdp_dsi_vdo_dphy_boe_lcm_drv")) {
            pr_err("[TP] touch ic = nt36672c_boe \n");
            tp_used_index = nt36672c_boe;
            g_tp_dev_vendor = TP_BOE;
            return true;
        }
        if (strstr(tp_ic_name, "himax,hx83112f_nf") && strstr(boot_command_line, "hx83112f")) {
            pr_err("[TP] touch ic = hx83112f_tianma \n");
            tp_used_index = himax_83112f;
            g_tp_dev_vendor = TP_TIANMA;
            return true;
        }
        if (strstr(tp_ic_name, "ilitek,ili7807s") && strstr(boot_command_line, "ili7807s")) {
            pr_err("[TP] touch ic = ili7807s_tianma \n");
            tp_used_index = ili7807s_tm;
            g_tp_dev_vendor = TP_TIANMA;
            return true;
        }
        break;
    default:
        pr_err("Invalid project\n");
        break;
    }
    pr_err("Lcd module not found\n");
    return false;
}

/*
* For separate lcd and tp, tp can be distingwished by gpio pins
* different project may have different combination, if needed,
* add your combination with project distingwished by is_project function.
*/
static void tp_get_vendor_via_pin(struct hw_resource *hw_res, struct panel_info *panel_data)
{
    panel_data->tp_type = TP_UNKNOWN;
    return;
}

/*
* If no gpio pins used to distingwish tp module, maybe have other ways(like command line)
* add your way of getting vendor info with project distingwished by is_project function.
*/
static void tp_get_vendor_separate(struct hw_resource *hw_res, struct panel_info *panel_data)
{
    panel_data->tp_type = TP_UNKNOWN;
    return;
}

int tp_util_get_vendor(struct hw_resource *hw_res, struct panel_info *panel_data)
{
    char *vendor;
    g_hw_res = hw_res;

    panel_data->test_limit_name = kzalloc(MAX_LIMIT_DATA_LENGTH, GFP_KERNEL);
    if (panel_data->test_limit_name == NULL) {
        pr_err("[TP]panel_data.test_limit_name kzalloc error\n");
    }

    panel_data->extra = kzalloc(MAX_LIMIT_DATA_LENGTH, GFP_KERNEL);
    if (panel_data->extra == NULL) {
        pr_err("[TP]panel_data.extra kzalloc error\n");
    }

    /*TP is first distingwished by gpio pins, and then by other ways*/
    if (is_tp_type_got_in_match) {
        panel_data->tp_type = g_tp_dev_vendor;
        if (is_project(OPPO_19165) || is_project(OPPO_19166)) {
            panel_data->tp_type = TP_SAMSUNG;
            memcpy(panel_data->manufacture_info.version, "0xBD3100000", 11);
        } else if (is_project(OPPO_19040)) {
            panel_data->tp_type = TP_SAMSUNG;
        } else if (is_project(OPPO_19131) || is_project(OPPO_19132) || is_project(OPPO_19133) || is_project(OPPO_19420)
		|| is_project(OPPO_19421)) {
//#ifdef CONFIG_MACH_MT6873
//            pr_info("[TP] is_project: OPPO_19131\n");
//            memcpy(panel_data->manufacture_info.version, "0xDD3000000", 11);
//#endif
        } else if (is_project(OPPO_20051)) {

        } else if (is_project(OPPO_20001) || is_project(OPPO_20002) || is_project(OPPO_20003) || is_project(OPPO_20200)) {

        } else if ( is_project(OPPO_20075) || is_project(OPPO_20076) ) {
            panel_data->tp_type = TP_SAMSUNG;
            memcpy(panel_data->manufacture_info.version, "0xRA5230000", 11);
        } else if (gpio_is_valid(hw_res->id1_gpio) || gpio_is_valid(hw_res->id2_gpio) || gpio_is_valid(hw_res->id3_gpio)) {
            tp_get_vendor_via_pin(hw_res, panel_data);
        } else if (is_project(OPPO_20041) || is_project(OPPO_20042) ) {

        } else {
            tp_get_vendor_separate(hw_res, panel_data);
        }

        if (panel_data->tp_type == TP_UNKNOWN) {
            pr_err("[TP]%s type is unknown\n", __func__);
            return 0;
        }
    }

    vendor = GET_TP_DEV_NAME(panel_data->tp_type);
    strcpy(panel_data->manufacture_info.manufacture, vendor);

    switch(get_project()) {
    case OPPO_19165:
    case OPPO_19166:
        snprintf(panel_data->fw_name, MAX_FW_NAME_LENGTH,
                 "tp/19165/FW_%s_%s.img",
                 panel_data->chip_name, vendor);

        if (panel_data->test_limit_name) {
            snprintf(panel_data->test_limit_name, MAX_LIMIT_DATA_LENGTH,
                     "tp/19165/LIMIT_%s_%s.img",
                     panel_data->chip_name, vendor);
        }

        if (panel_data->extra) {
            snprintf(panel_data->extra, MAX_LIMIT_DATA_LENGTH,
                     "tp/19165/BOOT_FW_%s_%s.ihex",
                     panel_data->chip_name, vendor);
        }
        panel_data->manufacture_info.fw_path = panel_data->fw_name;
        break;
    case OPPO_19040:
        snprintf(panel_data->fw_name, MAX_FW_NAME_LENGTH,
                 "tp/19040/FW_%s_%s.img",
                 panel_data->chip_name, vendor);

        if (panel_data->test_limit_name) {
            snprintf(panel_data->test_limit_name, MAX_LIMIT_DATA_LENGTH,
                     "tp/19040/LIMIT_%s_%s.img",
                     panel_data->chip_name, vendor);
        }

        if (panel_data->extra) {
            snprintf(panel_data->extra, MAX_LIMIT_DATA_LENGTH,
                     "tp/19040/BOOT_FW_%s_%s.ihex",
                     panel_data->chip_name, vendor);
        }
        break;
    case OPPO_19131:
    case OPPO_19132:
    case OPPO_19133:
    case OPPO_19420:
    case OPPO_19421:
#ifdef CONFIG_MACH_MT6873
        pr_info("[TP] enter case OPPO_19131\n");
        if (tp_used_index == nt36672c) {
            snprintf(panel_data->fw_name, MAX_FW_NAME_LENGTH,
                     "tp/19131/FW_%s_%s.img",
                     "NF_NT36672C", vendor);

            if (panel_data->test_limit_name) {
                snprintf(panel_data->test_limit_name, MAX_LIMIT_DATA_LENGTH,
                         "tp/19131/LIMIT_%s_%s.img",
                         "NF_NT36672C", vendor);
            }

            if (panel_data->extra) {
                snprintf(panel_data->extra, MAX_LIMIT_DATA_LENGTH,
                         "tp/19131/BOOT_FW_%s_%s.ihex",
                         "NF_NT36672C", vendor);
            }
            panel_data->manufacture_info.fw_path = panel_data->fw_name;
            if ((tp_used_index == nt36672c) && (g_tp_dev_vendor == TP_JDI)) {
                pr_info("[TP]: firmware_headfile = FW_19131_NF_NT36672C_JDI_fae_jdi\n");
                memcpy(panel_data->manufacture_info.version, "0xDD300JN200", 12);
                //panel_data->firmware_headfile.firmware_data = FW_19131_NF_NT36672C_JDI;
                //panel_data->firmware_headfile.firmware_size = sizeof(FW_19131_NF_NT36672C_JDI);
                panel_data->firmware_headfile.firmware_data = FW_19131_NF_NT36672C_JDI_fae_jdi;
                panel_data->firmware_headfile.firmware_size = sizeof(FW_19131_NF_NT36672C_JDI_fae_jdi);
            }
            if ((tp_used_index == nt36672c) && (g_tp_dev_vendor == TP_TIANMA)) {
                pr_info("[TP]: firmware_headfile = FW_19131_NF_NT36672C_TIANMA_fae_tianma\n");
                memcpy(panel_data->manufacture_info.version, "0xDD300TN000", 12);
                //panel_data->firmware_headfile.firmware_data = FW_19131_NF_NT36672C_TIANMA_realme;
                //panel_data->firmware_headfile.firmware_size = sizeof(FW_19131_NF_NT36672C_TIANMA_realme);
                panel_data->firmware_headfile.firmware_data = FW_19131_NF_NT36672C_TIANMA_fae_tianma;
                panel_data->firmware_headfile.firmware_size = sizeof(FW_19131_NF_NT36672C_TIANMA_fae_tianma);
            }
        }

        if (tp_used_index == himax_83112f) {
            snprintf(panel_data->fw_name, MAX_FW_NAME_LENGTH,
                     "tp/19131/FW_%s_%s.img",
                     "NF_HX83112F", vendor);

            if (panel_data->test_limit_name) {
                snprintf(panel_data->test_limit_name, MAX_LIMIT_DATA_LENGTH,
                         "tp/19131/LIMIT_%s_%s.img",
                         "NF_HX83112F", vendor);
            }

            if (panel_data->extra) {
                snprintf(panel_data->extra, MAX_LIMIT_DATA_LENGTH,
                         "tp/19131/BOOT_FW_%s_%s.ihex",
                         "NF_HX83112F", vendor);
            }
            panel_data->manufacture_info.fw_path = panel_data->fw_name;
            if (tp_used_index == himax_83112f) {
                pr_info("[TP]: firmware_headfile = FW_19131_NF_HX83112F_JDI\n");
                memcpy(panel_data->manufacture_info.version, "0xDD300JH000", 12);
                panel_data->firmware_headfile.firmware_data = FW_19131_NF_HX83112F_JDI;
                panel_data->firmware_headfile.firmware_size = sizeof(FW_19131_NF_HX83112F_JDI);
            }
        }
#endif
        break;
    case OPPO_20051:
        pr_info("[TP] enter case OPPO_20051\n");
        if (tp_used_index == nt36672c) {
            snprintf(panel_data->fw_name, MAX_FW_NAME_LENGTH,
                     "tp/20051/FW_%s_%s.img",
                     "NF_NT36672C", "JDI");

            if (panel_data->test_limit_name) {
                snprintf(panel_data->test_limit_name, MAX_LIMIT_DATA_LENGTH,
                         "tp/20051/LIMIT_%s_%s.img",
                         "NF_NT36672C", "JDI");
            }
            if (panel_data->extra) {
                snprintf(panel_data->extra, MAX_LIMIT_DATA_LENGTH,
                         "tp/20051/BOOT_FW_%s_%s.ihex",
                         "NF_NT36672C", "JDI");
            }
            panel_data->manufacture_info.fw_path = panel_data->fw_name;
            if ((tp_used_index == nt36672c) && (g_tp_dev_vendor == TP_JDI)) {
                pr_info("[TP]: firmware_headfile = FW_20051_NF_NT36672C_JDI_fae_jdi\n");
                memcpy(panel_data->manufacture_info.version, "0xBD358JN200", 12);
                panel_data->firmware_headfile.firmware_data = FW_20051_NF_NT36672C_JDI_fae_jdi;
                panel_data->firmware_headfile.firmware_size = sizeof(FW_20051_NF_NT36672C_JDI_fae_jdi);
            }
        }
        break;
    case OPPO_20001:
    case OPPO_20002:
    case OPPO_20003:
    case OPPO_20200:
        pr_info("[TP] enter case OPPO_20001\n");
        if (tp_used_index == nt36672c) {
            snprintf(panel_data->fw_name, MAX_FW_NAME_LENGTH,
                     "tp/20001/FW_%s_%s.img",
                     "NF_NT36672C", vendor);

            if (panel_data->test_limit_name) {
                snprintf(panel_data->test_limit_name, MAX_LIMIT_DATA_LENGTH,
                         "tp/20001/LIMIT_%s_%s.img",
                         "NF_NT36672C", vendor);
            }
            if (panel_data->extra) {
                snprintf(panel_data->extra, MAX_LIMIT_DATA_LENGTH,
                         "tp/20001/BOOT_FW_%s_%s.ihex",
                         "NF_NT36672C", vendor);
            }
            panel_data->manufacture_info.fw_path = panel_data->fw_name;
            if (tp_used_index == nt36672c) {
                pr_info("[TP]: firmware_headfile = FW_20001_NF_NT36672C_JDI_fae_jdi\n");
                memcpy(panel_data->manufacture_info.version, "0xFA219DN", 9);
                panel_data->firmware_headfile.firmware_data = FW_20001_NF_NT36672C_JDI;
                panel_data->firmware_headfile.firmware_size = sizeof(FW_20001_NF_NT36672C_JDI);
            }
        }

        if (tp_used_index == himax_83112f) {
            snprintf(panel_data->fw_name, MAX_FW_NAME_LENGTH,
                     "tp/20001/FW_%s_%s.img",
                     "NF_HX83112F", vendor);

            if (panel_data->test_limit_name) {
                snprintf(panel_data->test_limit_name, MAX_LIMIT_DATA_LENGTH,
                         "tp/20001/LIMIT_%s_%s.img",
                         "NF_HX83112F", vendor);
            }

            if (panel_data->extra) {
                snprintf(panel_data->extra, MAX_LIMIT_DATA_LENGTH,
                         "tp/20001/BOOT_FW_%s_%s.ihex",
                         "NF_HX83112F", vendor);
            }
            panel_data->manufacture_info.fw_path = panel_data->fw_name;
            if (tp_used_index == himax_83112f) {
                pr_info("[TP]: firmware_headfile = FW_20001_NF_HX83112F_TIANMA\n");
                memcpy(panel_data->manufacture_info.version, "0xFA219TH", 9);
                panel_data->firmware_headfile.firmware_data = FW_20001_NF_HX83112F_TIANMA;
                panel_data->firmware_headfile.firmware_size = sizeof(FW_20001_NF_HX83112F_TIANMA);
            }
        }

        if (tp_used_index == ili7807s_tm) {
            snprintf(panel_data->fw_name, MAX_FW_NAME_LENGTH,
                     "tp/20001/FW_%s_%s.img",
                     "NF_ILI7807S", vendor);

            if (panel_data->test_limit_name) {
                snprintf(panel_data->test_limit_name, MAX_LIMIT_DATA_LENGTH,
                         "tp/20001/LIMIT_%s_%s.img",
                         "NF_ILI7807S", vendor);
            }

            if (panel_data->extra) {
                snprintf(panel_data->extra, MAX_LIMIT_DATA_LENGTH,
                         "tp/20001/BOOT_FW_%s_%s.ihex",
                         "NF_ILI7807S", vendor);
            }

            panel_data->manufacture_info.fw_path = panel_data->fw_name;

            if (tp_used_index == ili7807s_tm) {
                pr_info("[TP]: firmware_headfile = FW_20001_NF_ILI7807S_TIANMA\n");
                memcpy(panel_data->manufacture_info.version, "0xFA219TI", 9);
                panel_data->firmware_headfile.firmware_data = FW_20001_NF_ILI7807S_TIANMA;
                panel_data->firmware_headfile.firmware_size = sizeof(FW_20001_NF_ILI7807S_TIANMA);
            }
        }
        break;
    case OPPO_20075:
    case OPPO_20076:
        pr_info("[TP] enter case OPPO_20075\n");
        if (tp_used_index == focal_ft3518) {
            snprintf(panel_data->fw_name, MAX_FW_NAME_LENGTH,
                     "tp/20075/FW_%s_%s.img",
                     "FT3518", vendor);

            if (panel_data->test_limit_name) {
                snprintf(panel_data->test_limit_name, MAX_LIMIT_DATA_LENGTH,
                         "tp/20075/LIMIT_%s_%s.img",
                         "FT3518", vendor);
            }
            if (panel_data->extra) {
                snprintf(panel_data->extra, MAX_LIMIT_DATA_LENGTH,
                         "tp/20075/BOOT_FW_%s_%s.ihex",
                         "FT3518", vendor);
            }
            panel_data->manufacture_info.fw_path = panel_data->fw_name;
            pr_info("[TP] ft3518 fw_name = [%s] \n", panel_data->fw_name);
            pr_info("[TP] ft3518 limit_name = [%s] \n", panel_data->test_limit_name);
        }
        break;
    case OPPO_20041:
    case OPPO_20042:
        pr_info("[TP] enter case OPPO_20041\n");
        if (tp_used_index == nt36672c) {
            snprintf(panel_data->fw_name, MAX_FW_NAME_LENGTH,
                     "tp/20041/FW_%s_%s.img",
                     "NF_NT36672C", vendor);

            if (panel_data->test_limit_name) {
                snprintf(panel_data->test_limit_name, MAX_LIMIT_DATA_LENGTH,
                         "tp/20041/LIMIT_%s_%s.img",
                         "NF_NT36672C", vendor);
            }
            if (panel_data->extra) {
                snprintf(panel_data->extra, MAX_LIMIT_DATA_LENGTH,
                         "tp/20041/BOOT_FW_%s_%s.ihex",
                         "NF_NT36672C", vendor);
            }
            panel_data->manufacture_info.fw_path = panel_data->fw_name;
            if (tp_used_index == nt36672c) {
                pr_info("[TP]: firmware_headfile = FW_20001_NF_NT36672C_JDI\n");
                memcpy(panel_data->manufacture_info.version, "0xFA278DN", 9);
                panel_data->firmware_headfile.firmware_data = FW_20041_NF_NT36672C_JDI;
                panel_data->firmware_headfile.firmware_size = sizeof(FW_20041_NF_NT36672C_JDI);
            }
        }

        if (tp_used_index == nt36672c_boe) {
            snprintf(panel_data->fw_name, MAX_FW_NAME_LENGTH,
                     "tp/20041/FW_%s_%s.img",
                     "NF_NT36672C", vendor);

            if (panel_data->test_limit_name) {
                snprintf(panel_data->test_limit_name, MAX_LIMIT_DATA_LENGTH,
                         "tp/20041/LIMIT_%s_%s.img",
                         "NF_NT36672C", vendor);
            }
            if (panel_data->extra) {
                snprintf(panel_data->extra, MAX_LIMIT_DATA_LENGTH,
                         "tp/20041/BOOT_FW_%s_%s.ihex",
                         "NF_NT36672C", vendor);
            }
            panel_data->manufacture_info.fw_path = panel_data->fw_name;
            if (tp_used_index == nt36672c_boe) {
                pr_info("[TP]: firmware_headfile = FW_20001_NF_NT36672C_BOE\n");
                memcpy(panel_data->manufacture_info.version, "0xFA278BN", 9);
                panel_data->firmware_headfile.firmware_data = FW_20041_NF_NT36672C_BOE;
                panel_data->firmware_headfile.firmware_size = sizeof(FW_20041_NF_NT36672C_BOE);
            }

            hw_res->TX_NUM = 18;
            hw_res->RX_NUM = 36;
            pr_info("[TP] BOE panel TX_NUM = %d, RX_NUM = %d \n", hw_res->TX_NUM, hw_res->RX_NUM);
        }

        if (tp_used_index == himax_83112f) {
            snprintf(panel_data->fw_name, MAX_FW_NAME_LENGTH,
                     "tp/20041/FW_%s_%s.img",
                     "NF_HX83112F", vendor);

            if (panel_data->test_limit_name) {
                snprintf(panel_data->test_limit_name, MAX_LIMIT_DATA_LENGTH,
                         "tp/20041/LIMIT_%s_%s.img",
                         "NF_HX83112F", vendor);
            }

            if (panel_data->extra) {
                snprintf(panel_data->extra, MAX_LIMIT_DATA_LENGTH,
                         "tp/20041/BOOT_FW_%s_%s.ihex",
                         "NF_HX83112F", vendor);
            }
            panel_data->manufacture_info.fw_path = panel_data->fw_name;
            if (tp_used_index == himax_83112f) {
                pr_info("[TP]: firmware_headfile = FW_20041_NF_HX83112F_TIANMA\n");
                memcpy(panel_data->manufacture_info.version, "0xFA278TH", 9);
                panel_data->firmware_headfile.firmware_data = FW_20041_NF_HX83112F_TIANMA;
                panel_data->firmware_headfile.firmware_size = sizeof(FW_20041_NF_HX83112F_TIANMA);
                }
            }

            if (tp_used_index == ili7807s_tm) {
                snprintf(panel_data->fw_name, MAX_FW_NAME_LENGTH,
                         "tp/20041/FW_%s_%s.img",
                         "NF_ILI7807S", vendor);

                if (panel_data->test_limit_name) {
                    snprintf(panel_data->test_limit_name, MAX_LIMIT_DATA_LENGTH,
                             "tp/20041/LIMIT_%s_%s.img",
                             "NF_ILI7807S", vendor);
                }

                if (panel_data->extra) {
                    snprintf(panel_data->extra, MAX_LIMIT_DATA_LENGTH,
                             "tp/20041/BOOT_FW_%s_%s.ihex",
                             "NF_ILI7807S", vendor);
                }

                panel_data->manufacture_info.fw_path = panel_data->fw_name;

                if (tp_used_index == ili7807s_tm) {
                    pr_info("[TP]: firmware_headfile = FW_20041_NF_ILI7807S_TIANMA\n");
                    memcpy(panel_data->manufacture_info.version, "0xFA278TI", 9);
                    panel_data->firmware_headfile.firmware_data = FW_20041_NF_ILI7807S_TIANMA;
                    panel_data->firmware_headfile.firmware_size = sizeof(FW_20041_NF_ILI7807S_TIANMA);
                }
            }
        break;
    default:
        snprintf(panel_data->fw_name, MAX_FW_NAME_LENGTH,
                 "tp/%d/FW_%s_%s.img",
                 get_project(), panel_data->chip_name, vendor);

        if (panel_data->test_limit_name) {
            snprintf(panel_data->test_limit_name, MAX_LIMIT_DATA_LENGTH,
                     "tp/%d/LIMIT_%s_%s.img",
                     get_project(), panel_data->chip_name, vendor);
        }

        if (panel_data->extra) {
            snprintf(panel_data->extra, MAX_LIMIT_DATA_LENGTH,
                     "tp/%d/BOOT_FW_%s_%s.ihex",
                     get_project(), panel_data->chip_name, vendor);
        }
        break;
    }

    pr_info("Vendor:%s\n", vendor);
    pr_info("Fw:%s\n", panel_data->fw_name);
    pr_info("Limit:%s\n", panel_data->test_limit_name == NULL ? "NO Limit" : panel_data->test_limit_name);
    pr_info("Extra:%s\n", panel_data->extra == NULL ? "NO Extra" : panel_data->extra);
    pr_info("is matched %d, type %d\n", is_tp_type_got_in_match, panel_data->tp_type);
    return 0;
}


/**
 * Description:
 * pulldown spi7 cs to avoid current leakage
 * because of current sourcing from cs (pullup state) flowing into display module
 **/
void switch_spi7cs_state(bool normal)
{
#if 0
    if(normal){
        if( !IS_ERR_OR_NULL(g_hw_res->pin_set_high) ) {
            pr_info("%s: going to set spi7 cs to spi mode .\n", __func__);
            pinctrl_select_state(g_hw_res->pinctrl, g_hw_res->pin_set_high);
        }else{
            pr_info("%s: cannot to set spi7 cs to spi mode .\n", __func__);
        }
    } else {
        if( !IS_ERR_OR_NULL(g_hw_res->pin_set_low) ) {
            pr_info("%s: going to set spi7 cs to pulldown .\n", __func__);
            pinctrl_select_state(g_hw_res->pinctrl, g_hw_res->pin_set_low);
        }else{
            pr_info("%s: cannot to set spi7 cs to pulldown .\n", __func__);
        }
    }
#endif
}
EXPORT_SYMBOL(switch_spi7cs_state);



