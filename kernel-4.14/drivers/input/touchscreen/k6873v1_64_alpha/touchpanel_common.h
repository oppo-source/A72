/**************************************************************
 * Copyright (c)  2008- 2030  CUSTOM_O Mobile communication Corp.ltd.￡?
 * VENDOR_EDIT
 * File       : touchpanel_common_driver.c
 * Description: Source file for Touch common driver
 * Version   : 1.0
 * Date        : 2016-09-02
 * Author    : Tong.han@Bsp.Group.Tp
 * TAG         : BSP.TP.Init
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
 * ---------------- Revision History: --------------------------
 *   <version>    <date>          < author >                            <desc>
 * Revision 1.1, 2016-09-09, Tong.han@Bsp.Group.Tp, modify based on gerrit
 *review result(http://gerrit.scm.adc.com:8080/#/c/223721/)
 ****************************************************************/
#ifndef _TOUCHPANEL_COMMON_H_
#define _TOUCHPANEL_COMMON_H_

/*********PART1:Head files**********************/
#include "device_info.h"
#include "tp_devices.h"
#include "util_interface/touch_interfaces.h"
#include <linux/delay.h>
#include <linux/firmware.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/input/mt.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/jiffies.h>
#include <linux/kthread.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#include <mt-plat/mtk_boot_common.h>

#define EFTM (250)
#define FW_UPDATE_COMPLETE_TIMEOUT msecs_to_jiffies(40 * 1000)

/*********PART2:Define Area**********************/
#define TPD_USE_EINT
#define TYPE_B_PROTOCOL

#define PAGESIZE 512
#define MAX_GESTURE_COORD 6

#define UnkownGesture 0
#define DouTap 1	 // double tap
#define UpVee 2		 // V
#define DownVee 3	// ^
#define LeftVee 4	// >
#define RightVee 5       // <
#define Circle 6	 // O
#define DouSwip 7	// ||
#define Left2RightSwip 8 // -->
#define Right2LeftSwip 9 // <--
#define Up2DownSwip 10   // |v
#define Down2UpSwip 11   // |^
#define Mgestrue 12      // M
#define Wgestrue 13      // W
#define FingerprintDown 14
#define FingerprintUp 15
#define SingleTap 16

#define FINGERPRINT_DOWN_DETECT 0X0f
#define FINGERPRINT_UP_DETECT 0X1f

/* bit operation */
#define SET_BIT(data, flag) ((data) |= (flag))
#define CLR_BIT(data, flag) ((data) &= ~(flag))
#define CHK_BIT(data, flag) ((data) & (flag))
#define VK_TAB                                                                 \
	{                                                                      \
		KEY_MENU, KEY_HOMEPAGE, KEY_BACK, KEY_SEARCH                   \
	}

#define TOUCH_BIT_CHECK                                                        \
	0x3FF // max support 10 point report.using for detect non-valid points
#define MAX_FW_NAME_LENGTH 60
#define MAX_EXTRA_NAME_LENGTH 60

#define MAX_DEVICE_VERSION_LENGTH 16
#define MAX_DEVICE_MANU_LENGTH 16

#define MESSAGE_SIZE (256)

#define SYNAPTICS_PREFIX "SY_"
#define GOODIX_PREFIX "GT_"
#define FOCAL_PREFIX "FT_"

#define SMART_GESTURE_THRESHOLD 0x0A
#define SMART_GESTURE_LOW_VALUE 0x05

#define GESTURE_RATE_MODE 0
#define GESTURE_COORD_GET 0
#define FW_UPDATE_DELAY msecs_to_jiffies(2 * 1000)
/*********PART3:Struct Area**********************/
enum delta_state {
	TYPE_DELTA_IDLE, /*means not in reading delta*/
	TYPE_DELTA_BUSY, /*reading delta data*/
};

enum vk_type {
	TYPE_PROPERTIES = 1, /*using board_properties*/
	/*using same IC (button zone &&  touch zone are seprate)*/
	TYPE_AREA_SEPRATE,
	/*using different IC (button zone &&  touch zone are seprate)*/
	TYPE_DIFF_IC,
	TYPE_NO_NEED, /*No need of virtual key process*/
};

enum health_ctl {
	HEALTH_NONE,
	HEALTH_INTERACTIVE_CLEAR = 1, /*INTERACTIVE SW CLAR DATA*/
	HEALTH_FORMAL_CLEAR,	  /*FORMAL SW CLAR DATA*/
	HEALTH_UPDATARP,	      /*trigger chip to update those data*/
	HEALTH_INFO_GET,	      /*health_monitor node show*/
	HEALTH_LASTEXCP_GET,	  /*health_monitor node show*/
};

enum touch_area {
	AREA_NOTOUCH,
	AREA_EDGE,
	AREA_CRITICAL,
	AREA_NORMAL,
	AREA_CORNER,
};

enum corner_type {
	CORNER_TOPLEFT,    /*When Phone Face you in portrait top left corner*/
	CORNER_TOPRIGHT,   /*When Phone Face you in portrait top right corner*/
	CORNER_BOTTOMLEFT, /*When Phone Face you in portrait bottom left
			    *  corner
			    */
	/*When Phone Face you in portrait 7bottom right corner*/
	CORNER_BOTTOMRIGHT,
};

enum work_mode {
	MODE_NORMAL,
	MODE_SLEEP,
	MODE_EDGE,
	MODE_GESTURE,
	MODE_GLOVE,
	MODE_CHARGE,
	MODE_GAME,
	MODE_EARSENSE,
	MODE_PALM_REJECTION,
	MODE_FACE_DETECT,
	MODE_HEADSET,
};

enum fw_check_state {
	FW_NORMAL,   /*fw might update, depend on the fw id*/
	FW_ABNORMAL, /*fw abnormal, need update*/
};

enum fw_update_state {
	FW_UPDATE_SUCCESS,
	FW_NO_NEED_UPDATE,
	FW_UPDATE_ERROR,
	FW_UPDATE_FATAL,
};

enum suspend_resume_state {
	TP_SUSPEND_EARLY_EVENT,
	TP_SUSPEND_COMPLETE,
	TP_RESUME_EARLY_EVENT,
	TP_RESUME_COMPLETE,
	TP_SPEEDUP_RESUME_COMPLETE,
};

enum IRQ_TRIGGER_REASON {
	IRQ_IGNORE = 0x00,
	IRQ_TOUCH = 0x01,
	IRQ_GESTURE = 0x02,
	IRQ_BTN_KEY = 0x04,
	IRQ_EXCEPTION = 0x08,
	IRQ_FW_CONFIG = 0x10,
	IRQ_FW_HEALTH = 0x20,
	IRQ_FW_AUTO_RESET = 0x40,
	IRQ_FACE_STATE = 0x80,
	IRQ_FINGERPRINT = 0x0100,
};

enum vk_bitmap {
	BIT_reserve = 0x08,
	BIT_BACK = 0x04,
	BIT_HOME = 0x02,
	BIT_MENU = 0x01,
};

enum finger_protect_status {
	FINGER_PROTECT_TOUCH_UP,
	FINGER_PROTECT_TOUCH_DOWN,
	FINGER_PROTECT_NOTREADY,
};

enum debug_level {
	LEVEL_BASIC,  /*pr_info basic tp debug info*/
	LEVEL_DETAIL, /*pr_info tp detail log for stress test*/
	LEVEL_DEBUG,  /*pr_info all tp debug info*/
};

enum resume_order {
	TP_LCD_RESUME,
	LCD_TP_RESUME,
};

enum suspend_order {
	TP_LCD_SUSPEND,
	LCD_TP_SUSPEND,
};

enum lcd_power {
	LCD_POWER_OFF,
	LCD_POWER_ON,
};

enum oem_verified_boot_state {
	OEM_VERIFIED_BOOT_STATE_UNLOCKED,
	OEM_VERIFIED_BOOT_STATE_LOCKED,
};

struct Coordinate {
	int x;
	int y;
};

enum interrupt_mode {
	BANNABLE,
	UNBANNABLE,
	INTERRUPT_MODE_MAX,
};

enum switch_mode_type {
	SEQUENCE,
	SINGLE,
};

enum touch_direction {
	VERTICAL_SCREEN,
	LANDSCAPE_SCREEN_90,
	LANDSCAPE_SCREEN_270,
};

struct gesture_info {
	uint32_t gesture_type;
	uint32_t clockwise;
	struct Coordinate Point_start;
	struct Coordinate Point_end;
	struct Coordinate Point_1st;
	struct Coordinate Point_2nd;
	struct Coordinate Point_3rd;
	struct Coordinate Point_4th;
};

struct point_info {
	uint16_t x;
	uint16_t y;
	uint16_t z;
	uint8_t width_major;
	uint8_t touch_major;
	uint8_t status;
	uint8_t tx_press;
	uint8_t rx_press;
	enum touch_area type;
};

struct corner_info {
	uint8_t id;
	bool flag;
	struct point_info point;
};

struct firmware_headfile {
	const uint8_t *firmware_data;
	size_t firmware_size;
};

struct panel_info {
	char *fw_name;	 /*FW name*/
	char *test_limit_name; /*test limit name*/
			       /*for some ic, may need other information*/
	char *extra;
	/*chip name the panel is controlled by*/
	const char *chip_name;
	uint32_t TP_FW; /*FW Version Read from IC*/
	enum tp_dev tp_type;
	/*Length of tp name show in  test apk*/
	int vid_len;
	int version_len;
	u32 project_id;
	/*firmware headfile for noflash ic*/
	struct firmware_headfile firmware_headfile;
	struct manufacture_info manufacture_info; /*touchpanel device info*/
};

struct hw_resource {
	// gpio
	int id1_gpio;
	int id2_gpio;
	int id3_gpio;

	int irq_gpio;   /*irq GPIO num*/
	int reset_gpio; /*Reset GPIO*/

	int enable2v8_gpio; /*vdd_2v8 enable GPIO*/
	int enable1v8_gpio; /*vcc_1v8 enable GPIO*/

	// TX&&RX Num
	int TX_NUM;
	int RX_NUM;
	/*the tx num occupied by touchkey*/
	int key_TX;
	/*the rx num occupied by touchkey*/
	int key_RX;
	/*for earsense function data reading*/
	int EARSENSE_TX_NUM;
	/*for earsense function data reading*/
	int EARSENSE_RX_NUM;

	// power
	struct regulator *vdd_2v8; /*power 2v8*/
	struct regulator *vcc_1v8; /*power 1v8*/
	uint32_t vdd_volt;	 /*avdd specific volt*/

	// pinctrl
	struct pinctrl *pinctrl;
	struct pinctrl_state *pin_set_high;
	struct pinctrl_state *pin_set_low;
	struct pinctrl_state *pin_set_nopull;
};

struct edge_limit {
	int limit_area;
	int left_x1;
	int right_x1;
	int left_x2;
	int right_x2;
	int left_x3;
	int right_x3;
	int left_y1;
	int right_y1;
	int left_y2;
	int right_y2;
	int left_y3;
	int right_y3;
	enum touch_area in_which_area;
};

struct touch_major_limit {
	int width_range;
	int height_range;
};

struct button_map {
	/*width of each key area */
	int width_x;
	/*height of each key area*/
	int height_y;
	/*Menu centre coordinates*/
	struct Coordinate coord_menu;
	/*Home centre coordinates*/
	struct Coordinate coord_home;
	/*Back centre coordinates*/
	struct Coordinate coord_back;
};

struct resolution_info {
	uint32_t max_x;      /*touchpanel width */
	uint32_t max_y;      /*touchpanel height*/
	uint32_t LCD_WIDTH;  /*LCD WIDTH        */
	uint32_t LCD_HEIGHT; /*LCD HEIGHT       */
};

struct esd_information {
	bool esd_running_flag;
	int esd_work_time;
	struct mutex esd_lock;
	struct workqueue_struct *esd_workqueue;
	struct delayed_work esd_check_work;
};

struct freq_hop_info {
	struct workqueue_struct *freq_hop_workqueue;
	struct delayed_work freq_hop_work;
	bool freq_hop_simulating;
	/*save frequency-hopping frequency, trigger frequency-hopping every
	 * freq_hop_freq seconds
	 */
	int freq_hop_freq;
};

struct spurious_fp_touch {
	/*thread only turn into running state by fingerprint kick
	 * proc/touchpanel/finger_protect_trigger
	 */
	bool fp_trigger;
	bool lcd_resume_ok;
	bool lcd_trigger_fp_check;
	/*provide result to fingerprint of touch status data*/
	enum finger_protect_status fp_touch_st;
	/*tread use for fingerprint susprious touch check*/
	struct task_struct *thread;
};

struct register_info {
	uint8_t reg_length;
	uint16_t reg_addr;
	uint8_t *reg_result;
};

struct black_gesture_test {
	/*store the gesture enable flag */
	bool gesture_backup;
	/* indicate do black gesture test or not*/
	bool flag;
	/* failure information if gesture test failed */
	char *message;
};

struct monitor_data {
	unsigned long monitor_down;
	unsigned long monitor_up;
	enum health_ctl ctl_type;

	int bootup_test;
	int repeat_finger;
	int miss_irq;
	int grip_report;
	int baseline_err;
	int noise_count;
	int shield_palm;
	int shield_edge;
	int shield_metal;
	int shield_water;
	int shield_esd;
	int hard_rst;
	int inst_rst;
	int parity_rst;
	int wd_rst;
	int other_rst;
	int reserve1;
	int reserve2;
	int reserve3;
	int reserve4;
	int reserve5;

	int fw_download_retry;
	int fw_download_fail;

	int eli_size;
	int eli_ver_range;
	int eli_hor_range;
	int *eli_ver_pos;
	int *eli_hor_pos;
};

struct fp_underscreen_info {
	uint8_t touch_state;
	uint8_t area_rate;
	uint16_t x;
	uint16_t y;
};

enum TP_USED_IC { UNKNOWN_IC, HIMAX83112A, ILI9881H, NT36525B };

//#define CONFIG_CUSTOM_O_TP_APK please define this in arch/arm64/configs
#ifdef CONFIG_CUSTOM_O_TP_APK

enum APK_SWITCH_TYPE {
	APK_NULL = 0,
	APK_CHARGER = 'C',
	APK_DATA = 'D',
	APK_EARPHONE = 'E',
	APK_GESTURE = 'G',
	APK_INFO = 'I',
	APK_NOISE = 'N',
	APK_PROXIMITY = 'P',
	APK_WATER = 'W',
	APK_DEBUG_MODE = 'd',
	APK_GAME_MODE = 'g'
};

enum APK_DATA_TYPE {
	DATA_NULL = 0,
	BASE_DATA = 'B',
	DIFF_DATA = 'D',
	DEBUG_INFO = 'I',
	RAW_DATA = 'R',
	BACK_DATA = 'T'
};

struct apk_proc_operations {
	void (*apk_game_set)(void *chip_data, bool on_off);
	bool (*apk_game_get)(void *chip_data);
	void (*apk_debug_set)(void *chip_data, bool on_off);
	bool (*apk_debug_get)(void *chip_data);
	void (*apk_noise_set)(void *chip_data, bool on_off);
	bool (*apk_noise_get)(void *chip_data);
	void (*apk_water_set)(void *chip_data, int type);
	int (*apk_water_get)(void *chip_data);
	void (*apk_proximity_set)(void *chip_data, bool on_off);
	int (*apk_proximity_dis)(void *chip_data);
	void (*apk_gesture_debug)(void *chip_data, bool on_off);
	bool (*apk_gesture_get)(void *chip_data);
	int (*apk_gesture_info)(void *chip_data, char *buf, int len);
	void (*apk_earphone_set)(void *chip_data, bool on_off);
	bool (*apk_earphone_get)(void *chip_data);
	void (*apk_charger_set)(void *chip_data, bool on_off);
	bool (*apk_charger_get)(void *chip_data);
	int (*apk_tp_info_get)(void *chip_data, char *buf, int len);
	void (*apk_data_type_set)(void *chip_data, int type);
	int (*apk_rawdata_get)(void *chip_data, char *buf, int len);
	int (*apk_diffdata_get)(void *chip_data, char *buf, int len);
	int (*apk_basedata_get)(void *chip_data, char *buf, int len);
	int (*apk_backdata_get)(void *chip_data, char *buf, int len);
};

#endif // end of CONFIG_CUSTOM_O_TP_APK

struct debug_info_proc_operations;
struct earsense_proc_operations;
struct touchpanel_data {
	bool register_is_16bit; /*register is 16bit*/
				/*glove_mode support feature*/
	bool glove_mode_support;
	/*black_gesture support feature*/
	bool black_gesture_support;
	/*black_gesture support feature*/
	bool single_tap_support;
	/*charger_pump support feature*/
	bool charger_pump_support;
	/*headset_pump support feature*/
	bool headset_pump_support;
	/*edge_limit support feature*/
	bool edge_limit_support;
	/*edge_limit by FW support feature*/
	bool fw_edge_limit_support;
	/*remove driver side edge limit control*/
	bool drlimit_remove_support;
	/*esd handle support feature*/
	bool esd_handle_support;
	/*avoid fingerprint spurious trrigger feature*/
	bool spurious_fp_support;
	/*indicate test black gesture or not*/
	bool gesture_test_support;
	/*indicate game switch support or not*/
	bool game_switch_support;
	/*touch porximity function*/
	bool ear_sense_support;
	/*feature used to controltouch_major report*/
	bool smart_gesture_support;
	/*feature use to control ABS_MT_PRESSURE report*/
	bool pressure_report_support;
	/*touch porximity function*/
	bool face_detect_support;
	/*fingerprint underscreen support*/
	bool fingerprint_underscreen_support;
	/*samsung s6d7ate ic int feature*/
	bool sec_long_low_trigger;
	bool suspend_gesture_cfg;
	/*auto test force pass in early project*/
	bool auto_test_force_pass_support;
	/*frequency hopping simulate feature*/
	bool freq_hop_simulate_support;
	/*external key used for touch point report*/
	bool external_touch_support;
	/*using grip function in kernel touch driver*/
	bool kernel_grip_support;
	bool kernel_grip_support_special; /*only for findX Q*/
	/*if call enable_irq_wake, can not call disable_irq_nosync*/
	bool new_set_irq_wake_support;
	/*screen off fingerprint info coordinates need*/
	bool screenoff_fingerprint_info_support;

	/*shows external key status*/
	bool external_touch_status;
	bool i2c_ready; /*i2c resume status*/
			/*state of headset or usb*/
	bool is_headset_checked;
	/*state of charger or usb*/
	bool is_usb_checked;
	/*touchpanel FW updating*/
	bool loading_fw;
	bool is_incell_panel; /*touchpanel is incell*/
	bool is_noflash_ic;   /*noflash ic*/
	/*trigger load tp fw by lcd driver after lcd reset*/
	bool lcd_trigger_load_tp_fw_support;
	/*whether have callback method to invoke common*/
	bool has_callback;
	/*notify speed resume process*/
	bool use_resume_notify;
	bool fw_update_app_support;  /*bspFwUpdate is used*/
	bool health_monitor_support; /*bspFwUpdate is used*/
	/*some no-flash ic (such as TD4330) need irq to trigger hdl*/
	bool irq_trigger_hdl_support;
	/*flag whether in test process*/
	bool in_test_process;
	/*noise mode test is used*/
	bool noise_modetest_support;
	/*lcd will wait tp resume finished*/
	bool lcd_wait_tp_resume_finished_support;
	/*report flow is unlock, need to lock when all touch release*/
	bool report_flow_unlock_support;
	bool fw_update_in_probe_with_headfile;
	struct firmware *firmware_in_dts;
	/*every bit declear one state of key
	 * "reserve(keycode)|home(keycode)|menu(keycode)|back(keycode)"
	 */
	u8 vk_bitmap;
	enum vk_type vk_type; /*virtual_key type*/
	enum delta_state delta_state;

	uint32_t irq_flags; /*irq setting flag*/
	int irq;	    /*irq num*/

	/*cover irq setting flag*/
	uint32_t irq_flags_cover;

	/*control state of black gesture*/
	int gesture_enable;
#if GESTURE_RATE_MODE
	int geature_ignore;
#endif
	int palm_enable;
	int es_enable;
	int fd_enable;
	int fp_enable;
	int touch_count;
	/*control state of glove gesture*/
	int glove_enable;
	/*control state of limit ebale */
	int limit_enable;
	/*control state of limit edge*/
	int limit_edge;
	/*control state of limit corner*/
	int limit_corner;
	/*parse the horizontal area configed in dts*/
	int default_hor_area;
	/*show current app in whitlist or not*/
	int limit_valid;
	/*suspend/resume flow exec flag*/
	int is_suspended;
	/*detail suspend/resume state*/
	enum suspend_resume_state suspend_state;

	int boot_mode; /*boot up mode */
	/*view area touched flag*/
	int view_area_touched;
	int force_update; /*force update flag*/
			  /*max muti-touch num supportted*/
	int max_num;
	/*debug use, for print all finger's first touch log*/
	int irq_slot;
	/*firmware_update_type: 0=check firmware version 1=force update; 2=for
	 * FAE
	 * debug
	 */
	int firmware_update_type;

	enum resume_order tp_resume_order;
	enum suspend_order tp_suspend_order;
	/*whether interrupt and be disabled*/
	enum interrupt_mode int_mode;
	enum switch_mode_type mode_switch_type; /*used for switch mode*/
	/*some incell ic is reset by lcd reset*/
	bool skip_reset_in_resume;
	bool skip_suspend_operate;
			/*LCD and TP is in one chip,lcd power off in
			 * suspend at first,
			 * can not operate i2c when tp suspend
			 */
	/*save ps status, ps near = 1, ps far = 0*/
	bool ps_status;
	/* whether tp resume finished */
	bool resume_finished;
	/*save ps status, ps near = 1, ps far = 0*/
	int noise_level;

#if defined(TPD_USE_EINT)
	/*using polling instead of IRQ*/
	struct hrtimer timer;
#endif
#if defined(CONFIG_FB)
	/*register to control suspend/resume*/
	struct notifier_block fb_notif;
#endif
	struct monitor_data monitor_data;
	/*mutex for lock i2c related flow*/
	struct mutex mutex;
	/*mutex for lock input report flow*/
	struct mutex report_mutex;
	struct mutex mutex_earsense;
	/*completion for control suspend and resume flow*/
	struct completion pm_complete;
	/*completion for control fw update*/
	struct completion fw_complete;
	/*completion for control fw update*/
	struct completion resume_complete;
	/*GPIO control(id && pinctrl && tp_type)*/
	struct panel_info panel_data;
	/*hw resourc information*/
	struct hw_resource hw_res;
	struct edge_limit edge_limit; /*edge limit*/
				      /*virtual_key button area*/
	struct button_map button_map;
	/*resolution of touchpanel && LCD*/
	struct resolution_info resolution_info;
	struct gesture_info gesture; /*gesture related info*/
	/*used for control touch major reporting area*/
	struct touch_major_limit touch_major_limit;
	/*tp info used for underscreen fingerprint*/
	struct fp_underscreen_info fp_info;

	/*using for speedup resume*/
	struct work_struct speed_up_work;
	/*using for touchpanel speedup resume wq*/
	struct workqueue_struct *speedup_resume_wq;
	/*trigger load tp fw by lcd driver after lcd reset*/
	struct work_struct lcd_trigger_load_tp_fw_work;
	/*trigger laod tp fw by lcd driver after lcd reset*/
	struct workqueue_struct *lcd_trigger_load_tp_fw_wq;

	/*using for read delta*/
	struct work_struct read_delta_work;
	struct workqueue_struct *delta_read_wq;

	struct work_struct async_work;
	struct workqueue_struct *async_workqueue;
	struct work_struct fw_update_work; /*using for fw update*/

	struct esd_information esd_info;
	struct freq_hop_info freq_hop_info;
	/*spurious_finger_support*/
	struct spurious_fp_touch spuri_fp_touch;

	struct device *dev; /*used for i2c->dev*/
	struct i2c_client *client;
	struct spi_device *s_client;
	struct input_dev *input_dev;
	struct input_dev *kpd_input_dev;
	struct input_dev *ps_input_dev;

	/*call_back function*/
	struct custom_o_touchpanel_operations *ts_ops;
	/*struct proc_dir_entry of "/proc/touchpanel"*/
	struct proc_dir_entry *prEntry_tp;
	/*struct proc_dir_entry of "/proc/touchpanel/debug_info"*/
	struct proc_dir_entry *prEntry_debug_tp;
	/*debug info data*/
	struct debug_info_proc_operations *debug_info_ops;
	struct earsense_proc_operations *earsense_ops;
	/*debug node for register length*/
	struct register_info reg_info;
	struct black_gesture_test gesture_test; /*gesture test struct*/

	void *chip_data; /*Chip Related data*/
			 /*Reserved Private data*/
	void *private_data;
	char *earsense_delta;
	/*when lcd_trigger_load_tp_fw start no need to control gesture*/
	bool disable_gesture_ctrl;
#ifdef CONFIG_CUSTOM_O_TP_APK
	struct apk_proc_operations *apk_op;
	enum APK_SWITCH_TYPE type_now;
	enum APK_DATA_TYPE data_now;
	u8 *log_buf;
	u8 *gesture_buf;
	bool gesture_debug_sta;
#endif // end of CONFIG_CUSTOM_O_TP_APK
};

#ifdef CONFIG_CUSTOM_O_TP_APK
void log_buf_write(struct touchpanel_data *ts, u8 value);
#endif // end of CONFIG_CUSTOM_O_TP_APK

struct custom_o_touchpanel_operations {
	int (*get_chip_info)(void *chip_data); /*return 0:success;other:failed*/
	int (*mode_switch)(void *chip_data, enum work_mode mode,
			   bool flag); /*return 0:success;other:failed*/
	int (*get_touch_points)(void *chip_data, struct point_info *points,
				int max_num); /*return point bit-map*/
	int (*get_gesture_info)(
	    void *chip_data,
	    struct gesture_info *gesture);   /*return 0:success;other:failed*/
	int (*ftm_process)(void *chip_data); /*ftm boot mode process*/
	void (*ftm_process_extra)(void);
	int (*get_vendor)(
	    void *chip_data,
	    struct panel_info *panel_data); /*distingush which panel we use,
					     * (TRULY/OFLIM/BIEL/TPK)
					     */
	int (*reset)(void *chip_data);      /*Reset Touchpanel*/
	int (*reinit_device)(void *chip_data);
	enum fw_check_state (*fw_check)(void *chip_data,
				   struct resolution_info *resolution_info,
				   /*return < 0 :failed; 0 success*/
				   struct panel_info *panel_data);
	enum fw_update_state (*fw_update)(
	    void *chip_data, const struct firmware *fw,
	    bool force); /*return 0 normal; return -1:update failed;*/
	int (*power_control)(
	    void *chip_data,
	    bool enable); /*return 0:success;other:abnormal, need to jump out*/
	int (*reset_gpio_control)(void *chip_data,
				  bool enable); /*used for reset gpio*/
	u8 (*trigger_reason)(
	    void *chip_data, int gesture_enable,
	    int is_suspended); /*clear innterrupt reg && detect irq trigger
				*reason
				*/
	u32 (*u32_trigger_reason)(void *chip_data, int gesture_enable,
				  int is_suspended);
	u8 (*get_keycode)(void *chip_data); /*get touch-key code*/
	int (*esd_handle)(void *chip_data);
	int (*fw_handle)(
	    void *chip_data); /*return 0 normal; return -1:update failed;*/
	void (*resume_prepare)(
	    void *chip_data);
			/*using for operation before resume
			 * flow,
			 * eg:incell 3320 need to disable
			 * gesture to release inter pins for
			 * lcd resume
			 */
	enum finger_protect_status (*spurious_fp_check)(
	    void *chip_data); /*spurious fingerprint check*/
	void (*finger_proctect_data_get)(
	    void *chip_data);			/*finger protect data get*/
	void (*exit_esd_mode)(void *chip_data); /*add for s4322 exit esd mode*/
	void (*register_info_read)(void *chip_data, uint16_t register_addr,
				   uint8_t *result,
				   uint8_t length); /*add for read registers*/
	void (*write_ps_status)(
	    void *chip_data,
	    int ps_status); /*when detect iron plate, if ps is
			     * near ,enter iron plate mode;if ps
			     * is far, can not enter; exit esd
			     * mode when ps is far
			     */
	void (*specific_resume_operate)(
	    void *chip_data); /*some ic need specific opearation in resuming*/
	void (*resume_timedout_operate)(
	    void *chip_data); /*some ic need opearation if resume timed out*/
	int (*get_usb_state)(void); /*get current usb state*/
	void (*black_screen_test)(void *chip_data,
				  char *msg); /*message of black gesture test*/
	int (*irq_handle_unlock)(void *chip_info); /*irq handler without mutex*/
	int (*async_work)(void *chip_info);	/*async work*/
	int (*get_face_state)(void *chip_info);    /*get face detect state*/
	void (*health_report)(
	    void *chip_data, struct monitor_data *mon_data); /*data logger get*/
	void (*bootup_test)(void *chip_data, const struct firmware *fw,
			    struct monitor_data *mon_data,
			    struct hw_resource *hw_res); /*boot_up test*/
	void (*get_gesture_coord)(void *chip_data, uint32_t gesture_type);
	void (*enable_fingerprint)(void *chip_data, uint32_t enable);
	void (*enable_gesture_mask)(void *chip_data, uint32_t enable);
	void (*set_touch_direction)(void *chip_data, uint8_t dir);
	uint8_t (*get_touch_direction)(void *chip_data);
	void (*screenon_fingerprint_info)(
	    void *chip_data,
	    struct fp_underscreen_info *fp_tpinfo); /*get gesture
						     * info of
						     * fingerprint
						     * underscreen
						     * when screen
						     * on
						     */
	/*trigger frequency-hopping*/
	void (*freq_hop_trigger)(void *chip_data);
	void (*set_noise_modetest)(void *chip_data, bool enable);
	uint8_t (*get_noise_modetest)(void *chip_data);
	/*If the tp ic need do something, use this!*/
	void (*tp_queue_work_prepare)(void);
	bool (*tp_irq_throw_away)(void *chip_data);
};

struct debug_info_proc_operations {
	void (*limit_read)(struct seq_file *s, struct touchpanel_data *ts);
	void (*delta_read)(struct seq_file *s, void *chip_data);
	void (*self_delta_read)(struct seq_file *s, void *chip_data);
	void (*self_raw_read)(struct seq_file *s, void *chip_data);
	void (*baseline_read)(struct seq_file *s, void *chip_data);
	void (*baseline_blackscreen_read)(struct seq_file *s, void *chip_data);
	void (*main_register_read)(struct seq_file *s, void *chip_data);
	void (*reserve_read)(struct seq_file *s, void *chip_data);
	void (*abs_doze_read)(struct seq_file *s, void *chip_data);
	void (*RT251)(struct seq_file *s, void *chip_data);
	void (*RT76)(struct seq_file *s, void *chip_data);
	void (*RT254)(struct seq_file *s, void *chip_data);
	void (*DRT)(struct seq_file *s, void *chip_data);
	void (*gesture_rate)(struct seq_file *s, u16 *coord_arg,
			     void *chip_data);
};

struct invoke_method {
	void (*invoke_common)(void);
	void (*async_work)(void);
};

struct earsense_proc_operations {
	void (*rawdata_read)(void *chip_data, char *earsense_baseline,
			     int read_length);
	void (*delta_read)(void *chip_data, char *earsense_delta,
			   int read_length);
	void (*self_data_read)(void *chip_data, char *earsense_self_data,
			       int read_length);
};

/*********PART3:function or variables for other files**********************/
extern unsigned int tp_debug; /*using for print debug log*/

struct touchpanel_data *common_touch_data_alloc(void);

int common_touch_data_free(struct touchpanel_data *pdata);
int register_common_touch_device(struct touchpanel_data *pdata);

void tp_i2c_suspend(struct touchpanel_data *ts);
void tp_i2c_resume(struct touchpanel_data *ts);

int tp_powercontrol_1v8(struct hw_resource *hw_res, bool on);
int tp_powercontrol_2v8(struct hw_resource *hw_res, bool on);

void operate_mode_switch(struct touchpanel_data *ts);
void input_report_key_custom_o(struct input_dev *dev, unsigned int code,
			       int value);
void esd_handle_switch(struct esd_information *esd_info, bool on);
void clear_view_touchdown_flag(void);
void tp_touch_btnkey_release(void);
extern int tp_util_get_vendor(struct hw_resource *hw_res,
			      struct panel_info *panel_data);
// TP_USED_IC tp_judge_ic_match(void);
bool __init tp_judge_ic_match(char *tp_ic_name);

__attribute__((weak)) int
request_firmware_select(const struct firmware **firmware_p, const char *name,
			struct device *device)
{
	return 1;
}
__attribute__((weak)) int
opticalfp_irq_handler(struct fp_underscreen_info *fp_tpinfo)
{
	return 0;
}
__attribute__((weak)) int get_lcd_status(void) { return 0; }

bool is_oem_unlocked(void);
int __init get_oem_verified_boot_state(void);

#endif
