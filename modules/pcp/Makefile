# Copyright (C) 2010 Creytiv.com
# Copyright (C) 2021 Dalei Liu

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/../..

MOD		:= pcp
$(MOD)_SRCS	+= pcp.c listener.c
$(MOD)_CFLAGS	+= -I$(SYSROOT)/local/include/rew
$(MOD)_LFLAGS	+= -lrew

include $(RSUA_TOPDIR)/mk/mod.mk
