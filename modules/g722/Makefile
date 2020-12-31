# Copyright (C) 2010 Creytiv.com
# Copyright (C) 2020 Dalei Liu

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/../..

MOD		:= g722
$(MOD)_SRCS	+= g722.c
$(MOD)_LFLAGS	+= -lspandsp

include $(RSUA_TOPDIR)/mk/mod.mk
