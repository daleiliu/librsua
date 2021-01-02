# Copyright (C) 2010 Creytiv.com
# Copyright (C) 2021 Dalei Liu

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/../..

MOD		:= g7221
$(MOD)_SRCS	+= decode.c
$(MOD)_SRCS	+= encode.c
$(MOD)_SRCS	+= g7221.c
$(MOD)_SRCS	+= sdp.c
$(MOD)_LFLAGS	+= -lg722_1

include $(RSUA_TOPDIR)/mk/mod.mk
