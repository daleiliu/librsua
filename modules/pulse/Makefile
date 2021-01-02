# Copyright (C) 2010 - 2016 Creytiv.com
# Copyright (C) 2021 Dalei Liu

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/../..

MOD		:= pulse
$(MOD)_SRCS	+= pulse.c
$(MOD)_SRCS	+= player.c
$(MOD)_SRCS	+= recorder.c
$(MOD)_LFLAGS	+= $(shell pkg-config --libs libpulse-simple)
$(MOD)_CFLAGS	+= $(shell pkg-config --cflags libpulse-simple)

include $(RSUA_TOPDIR)/mk/mod.mk
