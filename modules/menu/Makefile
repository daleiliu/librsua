# Copyright (C) 2010 Creytiv.com
# Copyright (C) 2020 Dalei Liu

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/../..

MOD		:= menu
$(MOD)_SRCS	+= menu.c static_menu.c dynamic_menu.c

include $(RSUA_TOPDIR)/mk/mod.mk
