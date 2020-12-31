# Copyright (C) 2010 Creytiv.com
# Copyright (C) 2020 Dalei Liu

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/../..

MOD		:= v4l2
$(MOD)_SRCS	+= v4l2.c
ifneq ($(HAVE_LIBV4L2),)
$(MOD)_LFLAGS	+= -lv4l2
$(MOD)_CFLAGS	+= -DHAVE_LIBV4L2
endif

include $(RSUA_TOPDIR)/mk/mod.mk
