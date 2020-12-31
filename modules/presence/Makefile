# Copyright (C) 2010 Creytiv.com
# Copyright (C) 2020 Dalei Liu

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/../..

MOD		:= presence
$(MOD)_SRCS	+= presence.c subscriber.c notifier.c publisher.c
$(MOD)_LFLAGS	+=

include $(RSUA_TOPDIR)/mk/mod.mk
