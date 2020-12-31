# Copyright (C) 2010 Creytiv.com
# Copyright (C) 2020 Dalei Liu

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/../..

MOD		:= vidinfo
$(MOD)_SRCS	+= draw.c
$(MOD)_SRCS	+= vidinfo.c
$(MOD)_SRCS	+= xga_font_data.c

include $(RSUA_TOPDIR)/mk/mod.mk
