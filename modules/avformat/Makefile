# Copyright (C) 2010 Creytiv.com
# Copyright (C) 2020 Dalei Liu

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/../..

MOD		:= avformat
$(MOD)_SRCS	+= avformat.c
$(MOD)_SRCS	+= audio.c
$(MOD)_SRCS	+= video.c
$(MOD)_LFLAGS	+= \
	`pkg-config --libs libavformat libavcodec libswresample \
		libavutil libavdevice libavfilter libswscale libpostproc`

include $(RSUA_TOPDIR)/mk/mod.mk
