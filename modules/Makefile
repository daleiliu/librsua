# Copyright (C) 2021 Dalei Liu

# Build modules

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/..

include $(RSUA_TOPDIR)/mk/common.mk
# Use modules.mk to detect buildable modules
#include $(RSUA_TOPDIR)/mk/modules.mk

MODULES := account alsa aubridge aufile auloop ausine avcodec avformat \
	b2bua cons contact ctrl_dbus ctrl_tcp debug_cmd dtls_srtp \
	ebuacip echo evdev fakevideo g711 g722 g726 \
	httpd httpreq ice l16 menu mwi natpmp \
	opus opus_multistream oss \
	plc presence selfview serreg srtp stdio stun syslog \
	turn uuid v4l2 v4l2_codec vidbridge vidinfo vidloop vumeter \
	x11 x11grab

MOD_BINS:= $(foreach mod,$(MODULES),$(mod)/$(BUILD)/$(mod)$(MOD_SUFFIX))

all: $(MOD_BINS)

$(MOD_BINS):
	@make -C $(basename $(notdir $@))

clean:
	@for mod in $(MODULES); \
	  do make -C $$mod clean; \
	done

.PHONY: modbins debreqs
modbins: $(MOD_BINS) | $(BUILD)
	@for modbin in $(MOD_BINS); \
	  do rsync -a $$modbin $(BUILD); \
	done

debreqs:
	sudo apt update && sudo apt install \
	  libavcodec-dev \
	  libavdevice-dev \
	  libavformat-dev \
	  libopus-dev \
	  libspandsp-dev \
	  libssl-dev \
	  libglib2.0-dev
