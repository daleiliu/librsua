# Copyright (C) 2010 Creytiv.com
# Copyright (C) 2021 Dalei Liu

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/../..

MOD		:= isac
$(MOD)_SRCS	+= isac.c
$(MOD)_LFLAGS	+= -lisac

include $(RSUA_TOPDIR)/mk/mod.mk
