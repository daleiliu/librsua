# Copyright (C) 2010 Creytiv.com
# Copyright (C) 2021 Dalei Liu

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/../..

MOD		:= cairo
$(MOD)_SRCS	+= cairo.c
$(MOD)_LFLAGS	+= $(shell pkg-config --libs cairo)
$(MOD)_CFLAGS	+= $(shell pkg-config --cflags cairo)

include $(RSUA_TOPDIR)/mk/mod.mk
