# Copyright (C) 2010 Creytiv.com
# Copyright (C) 2021 Dalei Liu

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/../..

MOD		:= sdl
$(MOD)_SRCS	+= sdl.c
$(MOD)_LFLAGS	+= -lSDL2

include $(RSUA_TOPDIR)/mk/mod.mk
