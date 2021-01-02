# Copyright (C) 2010 Creytiv.com
# Copyright (C) 2021 Dalei Liu

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/../..

MOD		:= codec2
$(MOD)_SRCS	+= codec2.c
$(MOD)_LFLAGS	+= -lcodec2

include $(RSUA_TOPDIR)/mk/mod.mk
