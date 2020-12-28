#
# Copyright (C) 2013 The Android-x86 Open Source Project
#
# Licensed under the GNU General Public License Version 2 or later.
# You may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.gnu.org/licenses/gpl.html
#

ifndef FFMPEG_DIR
FFMPEG_DIR := $(dir $(call my-dir))
endif

FFMPEG_ARCH_VARIANT := $(TARGET_ARCH_VARIANT)
ifeq ($(FFMPEG_2ND_ARCH), true)
#ifndef VENDOR_EDIT
#Wuhan@PSW.MM.MediaServer.Player., 2018/09/17,
#Modify for compile error
#  FFMPEG_ARCH_VARIANT := $(TARGET_2ND_ARCH_VARIANT)
#else /* VENDOR_EDIT */
   FFMPEG_ARCH_VARIANT := armv7-a-neon
#endif /* VENDOR_EDIT */
endif

# bionic include must be the first
LOCAL_C_INCLUDES := \
	bionic/libc/include \
	$(FFMPEG_DIR)android/include \
	$(FFMPEG_DIR) \

LOCAL_CFLAGS := \
	-DANDROID_SDK_VERSION=$(PLATFORM_SDK_VERSION) \
	-DTARGET_CONFIG=\"config-$(FFMPEG_ARCH_VARIANT).h\" \
	-DHAVE_AV_CONFIG_H -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DPIC \

LOCAL_ASFLAGS := $(LOCAL_CFLAGS)
