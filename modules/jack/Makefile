# Copyright (C) 2010 Creytiv.com
# Copyright (C) 2021 Dalei Liu

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/../..

MOD		:= jack
$(MOD)_SRCS	+= jack.c jack_play.c jack_src.c
$(MOD)_CFLAGS	+= $(shell pkg-config --cflags jack)
$(MOD)_LFLAGS	+= $(shell pkg-config --libs jack)

include $(RSUA_TOPDIR)/mk/mod.mk
