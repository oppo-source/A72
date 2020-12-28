#ifdef VENDOR_EDIT
# Bin.Li@EXP.BSP.bootloader.bootflow, 2017/05/15, Add for oppo device info and oppo project
obj-$(CONFIG_OPPO_COMMON_SOFT)  +=  oppo_project/
obj-$(CONFIG_OPPO_DEVICE_IFNO)  +=  oppo_devinfo/
obj-$(CONFIG_OPPO_ALS_CALI)   +=  oppo_als_cali_value/
obj-y += oppo_custom/
obj-y += oppo_criticallog/
#ifdef VENDOR_EDIT
#//wenbin.liu@PSW.Kernel.MM, 2018/05/23, Add for Healthinfo monitor
obj-y +=  oppo_healthinfo/
#endif /* VENDOR_EDIT */
#Liang.Zhang@TECH.Storage.Stability.OPPO_SHUTDOWN_DETECT, 2019/04/28, Add for shutdown detect
obj-$(CONFIG_OPPO_SHUTDOWN_DETECT)  +=  oppo_shutdown_detect/
#endif /* VENDOR_EDIT */
obj-$(CONFIG_OPPO_HYPNUS)     +=  oppo_hypnus/

#Kun.Hu@TECH.BSP.Stability.PHOENIX_PROJECT 2019/06/15, Add for PHOENIX_PROJECT
obj-$(CONFIG_OPPO_PHOENIX)     +=  oppo_phoenix/

#/* Hank.liu@TECH.PLAT.Storage, 2020-02-18, add oppo iomonitor driver*/
obj-$(CONFIG_OPPO_IOMONITOR)    +=  oppo_iomonitor/
