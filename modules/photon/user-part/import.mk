USER_PART_MODULE_VERSION ?= 2
USER_PART_MODULE_PATH ?= $(PROJECT_ROOT)/modules/photon/user-part
include $(call rwildcard,$(USER_PART_MODULE_PATH)/,include.mk)


