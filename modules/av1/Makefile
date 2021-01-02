# Copyright (C) 2010 - 2016 Creytiv.com
# Copyright (C) 2021 Dalei Liu

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/../..

MOD		:= av1
$(MOD)_SRCS	+= av1.c
$(MOD)_SRCS	+= decode.c
$(MOD)_SRCS	+= encode.c
$(MOD)_LFLAGS	+= -laom

include $(RSUA_TOPDIR)/mk/mod.mk
