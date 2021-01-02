# Copyright (C) 2010 Creytiv.com
# Copyright (C) 2021 Dalei Liu

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/../..

MOD		:= gst_video
$(MOD)_SRCS	+= gst_video.c encode.c sdp.c
$(MOD)_LFLAGS	+= $(shell pkg-config --libs gstreamer-1.0 gstreamer-app-1.0)
$(MOD)_CFLAGS   += $(shell pkg-config --cflags gstreamer-1.0 gstreamer-app-1.0)
$(MOD)_CFLAGS	+= -Wno-cast-align

include $(RSUA_TOPDIR)/mk/mod.mk
