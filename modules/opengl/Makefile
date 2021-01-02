# Copyright (C) 2010 Creytiv.com
# Copyright (C) 2021 Dalei Liu

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/../..

MOD		:= opengl
$(MOD)_SRCS	+= opengl.m
$(MOD)_LFLAGS	+= -framework OpenGL -framework Cocoa -lobjc

include $(RSUA_TOPDIR)/mk/mod.mk
