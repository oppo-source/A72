obj-$(CONFIG_OPPO_FINGERPRINT) += oppo_fp_common/
obj-$(CONFIG_OPPO_FINGERPRINT) += fpc/

ifneq ($(filter MSM_17085 MSM_17081, $(OPPO_TARGET_DEVICE)),)
obj-$(CONFIG_OPPO_FINGERPRINT) += goodix_fp/
else
obj-$(CONFIG_OPPO_FINGERPRINT) += goodix_optical_fp/
endif


ifeq ($(CONFIG_OPPO_FINGERPRINT_MTK),y)
obj-$(CONFIG_OPPO_FINGERPRINT) += silead_optical_fp/
obj-$(CONFIG_OPPO_FINGERPRINT) += egis/
endif

ifeq ($(CONFIG_OPPO_FINGERPRINT_QCOM),y)
obj-$(CONFIG_OPPO_FINGERPRINT) += silead/
endif
