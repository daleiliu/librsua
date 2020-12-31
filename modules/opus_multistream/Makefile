# Copyright (C) 2019 Creytiv.com
# Copyright (C) 2020 Dalei Liu

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/../..

MOD		:= opus_multistream
$(MOD)_SRCS	+= decode.c
$(MOD)_SRCS	+= encode.c
$(MOD)_SRCS	+= opus_multistream.c
$(MOD)_SRCS	+= sdp.c
$(MOD)_LFLAGS	+= -lopus -lm

include $(RSUA_TOPDIR)/mk/mod.mk
