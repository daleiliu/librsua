# Copyright (C) 2016 Symonics GmbH
# Copyright (C) 2021 Dalei Liu

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/../..

MOD		:= mpa
$(MOD)_SRCS	+= mpa.c
$(MOD)_SRCS	+= decode.c
$(MOD)_SRCS	+= sdp.c
$(MOD)_SRCS	+= encode.c
$(MOD)_LFLAGS	+= -ltwolame -lmp3lame -lmpg123 -lspeexdsp -lm

include $(RSUA_TOPDIR)/mk/mod.mk
