# Copyright (C) 2011 Creytiv.com
# Copyright (C) 2021 Dalei Liu

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/../..

MOD		:= rst
$(MOD)_SRCS	+= audio.c
$(MOD)_SRCS	+= rst.c
$(MOD)_SRCS	+= video.c
$(MOD)_LFLAGS	+= $(shell pkg-config --libs cairo libmpg123)
$(MOD)_CFLAGS	+= $(shell pkg-config --cflags cairo libmpg123)

include $(RSUA_TOPDIR)/mk/mod.mk
