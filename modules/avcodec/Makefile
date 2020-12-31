# Copyright (C) 2010 Creytiv.com
# Copyright (C) 2020 Dalei Liu

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/../..

MOD		:= avcodec
$(MOD)_SRCS	+= avcodec.c
$(MOD)_SRCS	+= decode.c
$(MOD)_SRCS	+= encode.c
$(MOD)_SRCS	+= h263.c
$(MOD)_SRCS	+= h265.c
$(MOD)_SRCS	+= sdp.c
$(MOD)_CFLAGS	+= -isystem /usr/local/include
$(MOD)_LFLAGS	+= `pkg-config --libs libavcodec libavutil`

include $(RSUA_TOPDIR)/mk/mod.mk
