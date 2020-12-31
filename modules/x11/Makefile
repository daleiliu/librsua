# Copyright (C) 2010 Creytiv.com
# Copyright (C) 2020 Dalei Liu

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/../..

MOD		:= x11
$(MOD)_SRCS	+= x11.c
$(MOD)_LFLAGS	+= -lX11 -lXext
$(MOD)_CFLAGS	+= -Wno-variadic-macros

include $(RSUA_TOPDIR)/mk/mod.mk
