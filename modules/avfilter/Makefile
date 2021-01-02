# Copyright (C) 2021 Mikhail Kurkov
# Copyright (C) 2021 Dalei Liu

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/../..

MOD		:= avfilter
$(MOD)_SRCS	+= avfilter.c
$(MOD)_SRCS	+= filter.c
$(MOD)_SRCS	+= util.c
$(MOD)_LFLAGS	+= -lavfilter -lavutil

include $(RSUA_TOPDIR)/mk/mod.mk
