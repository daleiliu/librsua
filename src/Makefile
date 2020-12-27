# Copyright (C) 2020 Dalei Liu

# Build librsua

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/..

include $(RSUA_TOPDIR)/mk/common.mk

COMPS := acct aucodec audio \
	aufilt auframe aulevel auplay ausrc \
	call cmd conf contact custom_hdrs \
	data ept ev h264 log \
	mctrl mediadev menc message metric mnat module \
	net play reg rtpext rtpstat \
	sdp sipreq stream stunuri timestamp ui \
	vidcodec video vidfilt vidisp vidsrc vidutil \

HDRS := ../include/rsua.h magic.h $(addsuffix .h, $(COMPS))

SRCS := rsua_cfg.c rsua_rt.c $(addsuffix .c, $(COMPS))

MODAPI_COMPS := acct aucodec audio \
	aufilt auframe aulevel auplay ausrc \
	call cmd conf contact \
	data ept ev h264 log \
	mediadev menc message mnat \
	net play \
	sdp sipreq stream stunuri ui \
	vidcodec video vidfilt vidisp vidsrc vidutil \

MODAPI_HDRS := modapi.h $(addsuffix .h, $(MODAPI_COMPS))

LIBRE_DIR := $(RSUA_TOPDIR)/re
LIBREM_DIR := $(RSUA_TOPDIR)/rem

BUILD_INCLUDE := build/include
LIBRE_INCLUDE := $(BUILD_INCLUDE)/rsua-re
LIBREM_INCLUDE := $(BUILD_INCLUDE)/rsua-rem
MODAPI_INCLUDE := $(BUILD_INCLUDE)/rsua-mod

CFLAGS += -DSHARE_PATH=\"\" \
	-I../include -I$(BUILD_INCLUDE) \

LDFLAGS += -ldl -lssl -lcrypto -lz -lpthread -lm

LIBRE_A := $(LIBRE_DIR)/libre.a
LIBREM_A := $(LIBREM_DIR)/librem.a
LIBS := $(LIBRE_A) $(LIBREM_A)

LDFLAGS += -Wl,--whole-archive $(LIBRE_A) $(LIBREM_A) \
	-Wl,--no-whole-archive \

OBJS := $(addprefix $(BUILD)/, $(SRCS:.c=.o))
TARGET_SONAME := librsua.so
TARGET := $(BUILD)/$(TARGET_SONAME)

all: $(TARGET) $(MODAPI_INCLUDE)

$(TARGET): $(OBJS) $(LIBS)
	$(CC) -shared -Wl,-soname,$(TARGET_SONAME) -o $@ \
	  $(LIBRE_A) $(LIBREM_A) $(OBJS) $(LDFLAGS)

$(LIBRE_A):
	make -C $(LIBRE_DIR)

$(LIBRE_INCLUDE):
	mkdir -p $(dir $@)
	ln -sf $(LIBRE_DIR)/include $@

$(LIBREM_A):
	make -C $(LIBREM_DIR)

$(LIBREM_INCLUDE):
	mkdir -p $(dir $@)
	ln -sf $(LIBREM_DIR)/include $@

$(BUILD)/%.o: %.c $(HDRS) | \
  $(BUILD) $(LIBRE_INCLUDE) $(LIBREM_INCLUDE) $(MODAPI_INCLUDE)
	$(CC) $(CFLAGS) -c -o $@ $<

$(MODAPI_INCLUDE):
	mkdir -p $(MODAPI_INCLUDE)
	cd $(MODAPI_INCLUDE); \
	  for modh in $(MODAPI_HDRS); do \
	    ln -sf ../../../$$modh .; \
	  done
