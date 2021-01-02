# Copyright (C) 2021 Dalei Liu

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/../..

MOD		:= mqtt
$(MOD)_SRCS	+= mqtt.c
$(MOD)_SRCS	+= publish.c
$(MOD)_SRCS	+= subscribe.c
$(MOD)_LFLAGS   += -lmosquitto
$(MOD)_CFLAGS   +=

include $(RSUA_TOPDIR)/mk/mod.mk
