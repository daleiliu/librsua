# Copyright (C) 2010 Creytiv.com
# Copyright (C) 2021 Dalei Liu

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/../..

MOD		:= vp9
$(MOD)_SRCS	+= decode.c
$(MOD)_SRCS	+= encode.c
$(MOD)_SRCS	+= vp9.c
$(MOD)_SRCS	+= sdp.c
$(MOD)_LFLAGS	+= -lvpx

include $(RSUA_TOPDIR)/mk/mod.mk
