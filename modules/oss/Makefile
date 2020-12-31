# Copyright (C) 2010 Creytiv.com
# Copyright (C) 2020 Dalei Liu

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/../..

MOD		:= oss
$(MOD)_SRCS	+= oss.c
$(MOD)_LFLAGS	+=

ifeq ($(OS), openbsd)
$(MOD)_LFLAGS	+= -lossaudio
endif
ifeq ($(OS), netbsd)
$(MOD)_LFLAGS	+= -lossaudio
endif

include $(RSUA_TOPDIR)/mk/mod.mk
