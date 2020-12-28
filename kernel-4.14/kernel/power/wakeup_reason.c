/*
 * kernel/power/wakeup_reason.c
 *
 * Logs the reasons which caused the kernel to resume from
 * the suspend mode.
 *
 * Copyright (C) 2014 Google, Inc.
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/wakeup_reason.h>
#include <linux/kernel.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/notifier.h>
#include <linux/suspend.h>
#ifdef VENDOR_EDIT
//yunqing.zeng@bsp.power.basic 2019-11-30 Modify for statistics of deepsleep r13 blocker.
#ifdef MTK_6873_PLATFORM
#include "../../drivers/misc/mediatek/lpm/modules/include/mt6873/mt6873_pcm_def.h"
#elif defined MTK_6885_PLATFORM
#include "../../drivers/misc/mediatek/lpm/modules/include/mt6885/mt6885_pcm_def.h"
#endif
#endif


#define MAX_WAKEUP_REASON_IRQS 32
static int irq_list[MAX_WAKEUP_REASON_IRQS];
static int irqcount;
static bool suspend_abort;
static char abort_reason[MAX_SUSPEND_ABORT_LEN];
static struct kobject *wakeup_reason;
static DEFINE_SPINLOCK(resume_reason_lock);

static ktime_t last_monotime; /* monotonic time before last suspend */
static ktime_t curr_monotime; /* monotonic time after last suspend */
static ktime_t last_stime; /* monotonic boottime offset before last suspend */
static ktime_t curr_stime; /* monotonic boottime offset after last suspend */

#ifdef VENDOR_EDIT
//yunqing.zeng@bsp.power.basic 2019-11-27 Add for statistics of resume reason.
static char WLAN_DATA_IRQ_NAME[]=     "wlan";
static char ADSP_IRQ_NAME[]=          "ADSP";
static char PWRKEY_IRQ_NAME[]=        "pwrkey";
static char MODEM_IRQ_NAME[]=         "CCIF";
static char MODEM_IRQ_NAME2[]=        "DPMAIF_AP";
static char SENSOR_IRQ_NAME1[]=       "SCP";
static char SENSOR_IRQ_NAME2[]=       "MBOX";
u64 wakeup_source_count_wifi = 0;
u64 wakeup_source_count_adsp = 0;
u64 wakeup_source_count_modem = 0;
u64 wakeup_source_count_kpdpwr = 0;
u64 wakeup_source_count_sensor = 0;
extern u64 alarm_count;
extern u64 wakeup_source_count_rtc;

//Yongyao.Song@PSW.NW.PWR.1053636, 2017/08/01, add for modem wake up source
#define MODEM_WAKEUP_SRC_NUM 10
extern int data_wakeup_index;
extern int modem_wakeup_src_count[MODEM_WAKEUP_SRC_NUM];
extern char modem_wakeup_src_string[MODEM_WAKEUP_SRC_NUM][20];
extern void modem_clear_wakeupsrc_count(void);

static void modem_wakeup_reason_count_clear(void)
{
    int i;
    printk(KERN_INFO  "ENTER %s\n", __func__);
    for(i = 0; i < MODEM_WAKEUP_SRC_NUM; i++)
    {
        modem_wakeup_src_count[i] = 0;
    }
}

//Nanwei.Deng@BSP.Power.Basic, 2018/11/19, add for analysis power coumption.
static void wakeup_reason_count_clear(void)
{
    printk(KERN_INFO  "ENTER %s\n", __func__);
    alarm_count = 0;
	wakeup_source_count_rtc = 0;
	wakeup_source_count_wifi = 0;
	wakeup_source_count_modem = 0;
	wakeup_source_count_kpdpwr = 0;
	wakeup_source_count_adsp = 0;
	wakeup_source_count_sensor = 0;
}

void wakeup_src_clean(void)
{
	wakeup_reason_count_clear();
	modem_wakeup_reason_count_clear();
	record_suspend_r13_count_clear();
}
EXPORT_SYMBOL(wakeup_src_clean);

//yunqing.zeng@bsp.power.basic 2019-11-27 Modify for statistics of resume reason.
#define RTC_CLOCK_FREQ  (32768)
static ssize_t ap_resume_reason_stastics_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	int buf_offset = 0, i=0;
	struct mt6885_sleep_static_info *mt6885_sleep_oppo_info = NULL;
	struct r13_blocker_detail_info *base = NULL;
	int *size = NULL;

	buf_offset += sprintf(buf + buf_offset, "wcnss_wlan");
	buf_offset += sprintf(buf + buf_offset,  "%s",":");
	buf_offset += sprintf(buf + buf_offset,  "%lld \n",wakeup_source_count_wifi);
	printk(KERN_WARNING "%s wakeup %lld times\n","wcnss_wlan",wakeup_source_count_wifi);

	buf_offset += sprintf(buf + buf_offset, "modem");
	buf_offset += sprintf(buf + buf_offset,  "%s",":");
	buf_offset += sprintf(buf + buf_offset,  "%lld \n",wakeup_source_count_modem);
	printk(KERN_WARNING "%s wakeup %lld times\n","qcom,smd-modem",wakeup_source_count_modem);

	buf_offset += sprintf(buf + buf_offset, "qpnp_rtc_alarm");
	buf_offset += sprintf(buf + buf_offset,  "%s",":");
	buf_offset += sprintf(buf + buf_offset,  "%lld \n",wakeup_source_count_rtc);
	printk(KERN_WARNING "%s wakeup %lld times\n","qpnp_rtc_alarm",wakeup_source_count_rtc);

	buf_offset += sprintf(buf + buf_offset, "power_key");
	buf_offset += sprintf(buf + buf_offset,  "%s",":");
	buf_offset += sprintf(buf + buf_offset,  "%lld \n",wakeup_source_count_kpdpwr);
	printk(KERN_WARNING "%s wakeup %lld times\n","power_key",wakeup_source_count_kpdpwr);

	buf_offset += sprintf(buf + buf_offset, "adsp");
	buf_offset += sprintf(buf + buf_offset,  "%s",":");
	buf_offset += sprintf(buf + buf_offset,  "%lld \n",wakeup_source_count_adsp);
	printk(KERN_WARNING "%s wakeup %lld times\n","adsp",wakeup_source_count_adsp);

	buf_offset += sprintf(buf + buf_offset, "sensor");
	buf_offset += sprintf(buf + buf_offset,  "%s",":");
	buf_offset += sprintf(buf + buf_offset,  "%lld \n",wakeup_source_count_sensor);
	printk(KERN_WARNING "%s wakeup %lld times\n","sensor",wakeup_source_count_sensor);

	//more info for deep sleep blocker
	get_mt6885_wakeup_r13_table(&mt6885_sleep_oppo_info, &base, &size);
	buf_offset += sprintf(buf + buf_offset, "ks_time");//kernel sleep time sum
	buf_offset += sprintf(buf + buf_offset,  "%s","-");
	buf_offset += sprintf(buf + buf_offset,  "%lld \n",mt6885_sleep_oppo_info->kernel_sleep_duration/RTC_CLOCK_FREQ);
	buf_offset += sprintf(buf + buf_offset, "ds_time");//deep sleep time sum
	buf_offset += sprintf(buf + buf_offset,  "%s","-");
	buf_offset += sprintf(buf + buf_offset,  "%lld \n",mt6885_sleep_oppo_info->deep_sleep_duration/RTC_CLOCK_FREQ);
	buf_offset += sprintf(buf + buf_offset, "ds_count");//deep sleep count
	buf_offset += sprintf(buf + buf_offset,  "%s","-");
	buf_offset += sprintf(buf + buf_offset,  "%lld \n",mt6885_sleep_oppo_info->deep_sleep_count);
	buf_offset += sprintf(buf + buf_offset, "nds_time");//not deep sleep time sum
	buf_offset += sprintf(buf + buf_offset,  "%s","-");
	buf_offset += sprintf(buf + buf_offset,  "%lld \n",mt6885_sleep_oppo_info->ndeep_sleep_duration/RTC_CLOCK_FREQ);
	buf_offset += sprintf(buf + buf_offset, "nds_count");//not deep sleep count
	buf_offset += sprintf(buf + buf_offset,  "%s","-");
	buf_offset += sprintf(buf + buf_offset,  "%lld \n",mt6885_sleep_oppo_info->ndeep_sleep_count);
	for(i = 0; i < *size; i++) {
		 buf_offset += sprintf(buf + buf_offset, "%s_%s",base[i].name, "count");
		 buf_offset += sprintf(buf + buf_offset,  "%s","-");
		 buf_offset += sprintf(buf + buf_offset,  "%lld \n",base[i].count);
	}
	for(i = 0; i < *size; i++) {
		 buf_offset += sprintf(buf + buf_offset, "%s_%s",base[i].name, "time");
		 buf_offset += sprintf(buf + buf_offset,  "%s","-");
		 buf_offset += sprintf(buf + buf_offset,  "%lld \n",base[i].duration/RTC_CLOCK_FREQ);
	}

	return buf_offset;
}

//Yongyao.Song@PSW.NW.PWR.1053636, 2017/08/01, add for modem wake up source
static ssize_t modem_resume_reason_stastics_show(struct kobject *kobj, struct kobj_attribute *attr,
        char *buf)
{
        int max_wakeup_src_count = 0;
        int max_wakeup_src_index = 0;
        int i, total = 0;
        int temp_d = 0;

        for(i = 0; i < MODEM_WAKEUP_SRC_NUM; i++)
        {
            total += modem_wakeup_src_count[i];
            printk(KERN_WARNING "%s wakeup %d times, total %d times\n",
                    modem_wakeup_src_string[i],modem_wakeup_src_count[i],total);
            if (i == data_wakeup_index)
            {
                temp_d = modem_wakeup_src_count[i] + (modem_wakeup_src_count[i]>>1);
                printk(KERN_WARNING "%s wakeup real %d times, count %d times\n",
                             modem_wakeup_src_string[i],modem_wakeup_src_count[i],temp_d);
                if(temp_d > max_wakeup_src_count){
                    max_wakeup_src_index = i;
                    max_wakeup_src_count = temp_d;
                }
            }
            else if (modem_wakeup_src_count[i] > max_wakeup_src_count)
            {
                max_wakeup_src_index = i;
                max_wakeup_src_count = modem_wakeup_src_count[i];
            }
        }
        return sprintf(buf, "%s:%d:%d\n", modem_wakeup_src_string[max_wakeup_src_index], max_wakeup_src_count, total);
}


//Nanwei.Deng@BSP.Power.Basic, 2018/11/19,  Add for clean wake up source  according to
//echo reset >   /sys/kernel/wakeup_reasons/wakeup_stastisc_reset
static ssize_t  wakeup_stastisc_reset_store(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count)
{
	char reset_string[]="reset";
	if(!((count == strlen(reset_string)) || ((count == strlen(reset_string) + 1) && (buf[count-1] == '\n')))) {
		return count;
       }

	if (strncmp(buf, reset_string, strlen(reset_string)) != 0) {
		return count;
       }

	wakeup_src_clean();
	return count;
}


static struct kobj_attribute ap_resume_reason_stastics = __ATTR_RO(ap_resume_reason_stastics);
static struct kobj_attribute modem_resume_reason_stastics = __ATTR_RO(modem_resume_reason_stastics);
static struct kobj_attribute wakeup_stastisc_reset_sys = __ATTR(wakeup_stastisc_reset, S_IWUSR|S_IRUGO, NULL, wakeup_stastisc_reset_store);
#endif /* VENDOR_EDIT */

static ssize_t last_resume_reason_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	int irq_no, buf_offset = 0;
	struct irq_desc *desc;
	spin_lock(&resume_reason_lock);
	if (suspend_abort) {
		buf_offset = sprintf(buf, "Abort: %s", abort_reason);
	} else {
		for (irq_no = 0; irq_no < irqcount; irq_no++) {
			desc = irq_to_desc(irq_list[irq_no]);
			if (desc && desc->action && desc->action->name)
				buf_offset += sprintf(buf + buf_offset, "%d %s\n",
						irq_list[irq_no], desc->action->name);
			else
				buf_offset += sprintf(buf + buf_offset, "%d\n",
						irq_list[irq_no]);
		}
	}
	spin_unlock(&resume_reason_lock);
	return buf_offset;
}

static ssize_t last_suspend_time_show(struct kobject *kobj,
			struct kobj_attribute *attr, char *buf)
{
	struct timespec sleep_time;
	struct timespec total_time;
	struct timespec suspend_resume_time;

	/*
	 * total_time is calculated from monotonic bootoffsets because
	 * unlike CLOCK_MONOTONIC it include the time spent in suspend state.
	 */
	total_time = ktime_to_timespec(ktime_sub(curr_stime, last_stime));

	/*
	 * suspend_resume_time is calculated as monotonic (CLOCK_MONOTONIC)
	 * time interval before entering suspend and post suspend.
	 */
	suspend_resume_time = ktime_to_timespec(ktime_sub(curr_monotime, last_monotime));

	/* sleep_time = total_time - suspend_resume_time */
	sleep_time = timespec_sub(total_time, suspend_resume_time);

	/* Export suspend_resume_time and sleep_time in pair here. */
	return sprintf(buf, "%lu.%09lu %lu.%09lu\n",
				suspend_resume_time.tv_sec, suspend_resume_time.tv_nsec,
				sleep_time.tv_sec, sleep_time.tv_nsec);
}

static struct kobj_attribute resume_reason = __ATTR_RO(last_resume_reason);
static struct kobj_attribute suspend_time = __ATTR_RO(last_suspend_time);

static struct attribute *attrs[] = {
	&resume_reason.attr,
	&suspend_time.attr,
	#ifdef VENDOR_EDIT
	&modem_resume_reason_stastics.attr, //Yongyao.Song@PSW.NW.PWR.1053636, 2017/08/01, add for modem wake up source
	&ap_resume_reason_stastics.attr,    //Nanwei.Deng@BSP.Power.Basic, 2018/11/19, add for analysis power coumption.
	&wakeup_stastisc_reset_sys.attr,    //Nanwei.Deng@BSP.Power.Basic, 2018/11/19, add for analysis power coumption.
	#endif /* VENDOR_EDIT */
	NULL,
};
static struct attribute_group attr_group = {
	.attrs = attrs,
};

/*
 * logs all the wake up reasons to the kernel
 * stores the irqs to expose them to the userspace via sysfs
 */
void log_wakeup_reason(int irq)
{
	struct irq_desc *desc;
	desc = irq_to_desc(irq);
	if (desc && desc->action && desc->action->name)
		printk(KERN_INFO "Resume caused by IRQ %d, %s\n", irq,
				desc->action->name);
	else
		printk(KERN_INFO "Resume caused by IRQ %d\n", irq);

	#ifdef VENDOR_EDIT
	//yunqing.zeng@bsp.power.basic 2019-11-27 Modify for statistics of resume reason.
	if (desc && desc->action && desc->action->name) {
		if(strstr(desc->action->name, WLAN_DATA_IRQ_NAME) != NULL) {
			wakeup_source_count_wifi++;
		} else if(strstr(desc->action->name, ADSP_IRQ_NAME) != NULL) {
			wakeup_source_count_adsp++;
		} else if((strstr(desc->action->name, MODEM_IRQ_NAME) != NULL) || (strstr(desc->action->name, MODEM_IRQ_NAME2) != NULL)) {
			wakeup_source_count_modem++;
		} else if(strstr(desc->action->name, PWRKEY_IRQ_NAME) != NULL) {
			wakeup_source_count_kpdpwr++;
		} else if((strstr(desc->action->name, SENSOR_IRQ_NAME1) != NULL) || (strstr(desc->action->name, SENSOR_IRQ_NAME2) != NULL)) {
			wakeup_source_count_sensor++;
		}
	}
	#endif

	spin_lock(&resume_reason_lock);
	if (irqcount == MAX_WAKEUP_REASON_IRQS) {
		spin_unlock(&resume_reason_lock);
		printk(KERN_WARNING "Resume caused by more than %d IRQs\n",
				MAX_WAKEUP_REASON_IRQS);
		return;
	}

	irq_list[irqcount++] = irq;
	spin_unlock(&resume_reason_lock);
}

int check_wakeup_reason(int irq)
{
	int irq_no;
	int ret = false;

	spin_lock(&resume_reason_lock);
	for (irq_no = 0; irq_no < irqcount; irq_no++)
		if (irq_list[irq_no] == irq) {
			ret = true;
			break;
	}
	spin_unlock(&resume_reason_lock);
	return ret;
}

void log_suspend_abort_reason(const char *fmt, ...)
{
	va_list args;

	spin_lock(&resume_reason_lock);

	//Suspend abort reason has already been logged.
	if (suspend_abort) {
		spin_unlock(&resume_reason_lock);
		return;
	}

	suspend_abort = true;
	va_start(args, fmt);
	vsnprintf(abort_reason, MAX_SUSPEND_ABORT_LEN, fmt, args);
	va_end(args);
	spin_unlock(&resume_reason_lock);
}

/* Detects a suspend and clears all the previous wake up reasons*/
static int wakeup_reason_pm_event(struct notifier_block *notifier,
		unsigned long pm_event, void *unused)
{
	switch (pm_event) {
	case PM_SUSPEND_PREPARE:
		spin_lock(&resume_reason_lock);
		irqcount = 0;
		suspend_abort = false;
		spin_unlock(&resume_reason_lock);
		/* monotonic time since boot */
		last_monotime = ktime_get();
		/* monotonic time since boot including the time spent in suspend */
		last_stime = ktime_get_boottime();
		break;
	case PM_POST_SUSPEND:
		/* monotonic time since boot */
		curr_monotime = ktime_get();
		/* monotonic time since boot including the time spent in suspend */
		curr_stime = ktime_get_boottime();
		break;
	default:
		break;
	}
	return NOTIFY_DONE;
}

static struct notifier_block wakeup_reason_pm_notifier_block = {
	.notifier_call = wakeup_reason_pm_event,
};

/* Initializes the sysfs parameter
 * registers the pm_event notifier
 */
int __init wakeup_reason_init(void)
{
	int retval;

	retval = register_pm_notifier(&wakeup_reason_pm_notifier_block);
	if (retval)
		printk(KERN_WARNING "[%s] failed to register PM notifier %d\n",
				__func__, retval);

	wakeup_reason = kobject_create_and_add("wakeup_reasons", kernel_kobj);
	if (!wakeup_reason) {
		printk(KERN_WARNING "[%s] failed to create a sysfs kobject\n",
				__func__);
		return 1;
	}
	retval = sysfs_create_group(wakeup_reason, &attr_group);
	if (retval) {
		kobject_put(wakeup_reason);
		printk(KERN_WARNING "[%s] failed to create a sysfs group %d\n",
				__func__, retval);
	}
	return 0;
}

late_initcall(wakeup_reason_init);
