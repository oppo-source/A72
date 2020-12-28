#
#Makefile for the touchscreen drivers.
#
ifeq ($(CONFIG_TRUSTONIC_TEE_SUPPORT), y)
subdir-ccflags-y += -I$(srctree)/drivers/spi/trustzone/spi/Tlspi/inc
endif

ifeq ($(CONFIG_TRUSTONIC_TEE_SUPPORT), y)
	include $(srctree)/drivers/misc/mediatek/gud/Makefile.include
endif
subdir-ccflags-y += -I$(srctree)/drivers/spi/mediatek/mt6763/

ifeq ($(CONFIG_DRM_MEDIATEK), y)
ccflags-$(CONFIG_DRM_MEDIATEK) += -I$(srctree)/drivers/gpu/drm/mediatek/
endif

#Each configuration option enables a list of files.
obj-y += nvt_drivers_nt36672c_noflash.o
#endif
