#
# Copyright (C) 2013 The Android-x86 Open Source Project
#
# Licensed under the GNU General Public License Version 2 or later.
# You may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.gnu.org/licenses/gpl.html
#
LOCAL_PATH := $(call my-dir)
ifeq ($(NOT_PREBUILT), yes)
include $(call all-subdir-makefiles)
else
include $(LOCAL_PATH)/prebuilt/Android_prebuilt.mk
endif
