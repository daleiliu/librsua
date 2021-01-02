# Copyright (C) 2019 Hessischer Rundfunk
# Copyright (C) 2021 Dalei Liu

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/../..

MOD		:= aptx
$(MOD)_SRCS	+= decode.c
$(MOD)_SRCS	+= encode.c
$(MOD)_SRCS	+= aptx.c
$(MOD)_SRCS	+= sdp.c
$(MOD)_LFLAGS += -lopenaptx

include $(RSUA_TOPDIR)/mk/mod.mk
