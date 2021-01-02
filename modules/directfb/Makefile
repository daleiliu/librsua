# Copyright (C) 2010 Creytiv.com
# Copyright (C) 2013 Andreas Shimokawa <andi@fischlustig.de>.
# Copyright (C) 2021 Dalei Liu

# DirectFB video display module

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/../..

MOD                := directfb
$(MOD)_SRCS        += directfb.c
$(MOD)_LFLAGS      += $(shell pkg-config --libs directfb)
$(MOD)_CFLAGS      += $(shell pkg-config --cflags directfb \
			| sed -e 's/-I/-isystem/g')

include $(RSUA_TOPDIR)/mk/mod.mk
