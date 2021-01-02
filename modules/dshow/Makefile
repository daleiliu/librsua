# Copyright (C) 2010 Creytiv.com
# Copyright (C) 2021 Dalei Liu

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/../..

MOD		:= dshow
$(MOD)_SRCS	+= dshow.cpp
$(MOD)_LFLAGS	+= -lstrmiids -lole32 -loleaut32 -lstdc++

include $(RSUA_TOPDIR)/mk/mod.mk
