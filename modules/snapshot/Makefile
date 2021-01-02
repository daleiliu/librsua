# Copyright (C) 2010 Creytiv.com
# Copyright (C) 2021 Dalei Liu

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/../..

MOD		:= snapshot
$(MOD)_SRCS	+= snapshot.c png_vf.c
$(MOD)_LFLAGS	+= -lpng

include $(RSUA_TOPDIR)/mk/mod.mk
