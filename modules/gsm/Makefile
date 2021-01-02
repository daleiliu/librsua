# Copyright (C) 2010 Creytiv.com
# Copyright (C) 2021 Dalei Liu

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/../..

MOD		:= gsm
$(MOD)_SRCS	+= gsm.c
$(MOD)_LFLAGS	+= -L$(SYSROOT)/lib -lgsm
$(MOD)_CFLAGS	+= -I$(SYSROOT)/include/gsm -I$(SYSROOT)/local/include

include $(RSUA_TOPDIR)/mk/mod.mk
