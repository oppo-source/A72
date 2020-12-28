#include <linux/kernel.h>
#include <stdbool.h>
#include <soc/oppo/oppo_project.h>
#include "nfc_common.h"

static unsigned int SN100T_PROJECTS[] = {19165, 19166, 19040};
static unsigned int ST21H_PROJECTS[] = {19420, 20200};
static unsigned int ST54H_PROJECTS[] = {19133, 20003};

static unsigned int SN100T_COUNT = sizeof(SN100T_PROJECTS)/sizeof(unsigned int);
static unsigned int ST21H_COUNT = sizeof(ST21H_PROJECTS)/sizeof(unsigned int);
static unsigned int ST54H_COUNT = sizeof(ST54H_PROJECTS)/sizeof(unsigned int);

static chip_type get_chip_type() {
    pr_err("%s : enter\n", __func__);
    unsigned int current_project = get_project();
    unsigned int i = 0;
    pr_err("%s : current_project is %d\n", __func__, current_project);
    for (i = 0; i < SN100T_COUNT; i++) {
        if(current_project == SN100T_PROJECTS[i]) {
            return SN100T;
        }
    }
    for (i = 0; i < ST21H_COUNT; i++) {
        if(current_project == ST21H_PROJECTS[i]) {
            return ST21H;
        }
    }
    for (i = 0; i < ST54H_COUNT; i++) {
        if(current_project == ST54H_PROJECTS[i]) {
            return ST54H;
        }
    }
    return UNKNOWN;
}

bool is_support_chip(chip_type chip){
    pr_err("%s : enter\n", __func__);
    chip_type current_chip = get_chip_type();
    pr_err("%s : is_support_chip is %d\n", __func__, current_chip);
    if(current_chip == ST54H && chip == ST21H) {
        return true;
    }
    return chip == current_chip;
}