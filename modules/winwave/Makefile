# Copyright (C) 2010 Creytiv.com
# Copyright (C) 2021 Dalei Liu

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/../..

MOD		:= winwave
$(MOD)_SRCS	+= winwave.c src.c play.c
$(MOD)_LFLAGS	+= -lwinmm

include $(RSUA_TOPDIR)/mk/mod.mk
