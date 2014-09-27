

ifndef SPARK_PRODUCT_ID
SPARK_PRODUCT_ID=0
$(info SPARK_PRODUCT_ID not set, assuming 0)
endif

# Determine which is the target device

STM32_DEVICE=STM32F10X_MD
ARCH=arm

ifeq ("$(SPARK_PRODUCT_ID)","0")
STM32_DEVICE=STM32F10X_MD
PLATFORM_MCU=STM32F1xx
PLATFORM_NET=CC3000
endif

ifeq ("$(SPARK_PRODUCT_ID)","1")
STM32_DEVICE=STM32F10X_MD
PLATFORM_MCU=STM32F1xx
PLATFORM_NET=CC3000
endif

ifeq ("$(SPARK_PRODUCT_ID)","2")
STM32_DEVICE=STM32F10X_HD
PLATFORM_MCU=STM32F1xx
PLATFORM_NET=CC3000
endif

ifeq ("$(SPARK_PRODUCT_ID)","3")
PLATFORM_MCU=gcc
PLATFORM_NET=gcc
ARCH=gcc
endif


ifeq ("$(PLATFORM_MCU)","")
$(error PLATFORM_MCU not defined. Check product id $(SPARK_PRODUCT_ID))
endif

ifeq ("$(PLATFORM_NET)","")
$(error PLATFORM_NET not defined. Check product id $(SPARK_PRODUCT_ID))
endif

# lower case version of the STM32_DEVICE string for use in filenames
STM32_DEVICE_LC  = $(shell echo $(STM32_DEVICE) | tr A-Z a-z)

CFLAGS += -D$(STM32_DEVICE)
CFLAGS += -DSPARK_PRODUCT_ID=$(SPARK_PRODUCT_ID)

MAKE_ARGS += SPARK_PRODUCT_ID=$(SPARK_PRODUCT_ID)
