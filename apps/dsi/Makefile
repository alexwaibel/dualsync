BLOCKSDS ?= /opt/blocksds/core
BLOCKSDSEXT ?= /opt/blocksds/external
WONDERFUL_TOOLCHAIN ?= /opt/wonderful

APP_ROOT := ../../../apps/dsi
CORE_ROOT := ../../../core
CORE_BUILD_DIR := ../../../build/obj/core/blocksds
CORE_LIBRARY := $(CORE_BUILD_DIR)/libdualsync_core.a

MBEDTLS_FOUND := $(shell find $(BLOCKSDSEXT)/mbedtls -name libmbedtls.a 2> /dev/null)
LIBCURL_FOUND := $(shell find $(BLOCKSDSEXT)/libcurl -name libcurl.a 2> /dev/null)
ZLIB_FOUND := $(shell find $(WONDERFUL_TOOLCHAIN)/toolchain/gcc-arm-none-eabi/arm-none-eabi/lib \
	-name libz.a 2> /dev/null)

ifeq ($(strip $(MBEDTLS_FOUND)),)
$(error Required package blocksds-mbedtls is missing)
endif

ifeq ($(strip $(LIBCURL_FOUND)),)
$(error Required package blocksds-libcurl is missing)
endif

ifeq ($(strip $(ZLIB_FOUND)),)
$(error Required package toolchain-gcc-arm-none-eabi-zlib is missing)
endif

NAME := dualsync-dsi
GAME_TITLE := DualSync
GAME_SUBTITLE := RomM client
GAME_AUTHOR := DualSync contributors

SOURCEDIRS := $(APP_ROOT)/source
INCLUDEDIRS := $(CORE_ROOT)/include
ARM7ELF := $(BLOCKSDS)/sys/arm7/main_core/arm7_dswifi_maxmod.elf

LIBS := $(CORE_LIBRARY) \
	-ldswifi9 -lnds9 \
	-lmbedcrypto -lmbedtls -lmbedx509 -lcurl -lz

LIBDIRS := $(BLOCKSDS)/libs/dswifi \
	$(BLOCKSDS)/libs/libnds \
	$(BLOCKSDSEXT)/mbedtls \
	$(BLOCKSDSEXT)/libcurl

include $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile

$(CORE_LIBRARY):
	$(MAKE) -C $(CORE_ROOT) -f Makefile.blocksds \
		BUILD_DIR=../build/obj/core/blocksds

$(ELF): $(CORE_LIBRARY)
