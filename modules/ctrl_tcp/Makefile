# Copyright (C) 2018 46 Labs LLC
# Copyright (C) 2020 Dalei Liu

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/../..

MOD		:= ctrl_tcp
$(MOD)_SRCS	+= ctrl_tcp.c tcp_netstring.c ./netstring/netstring.c

include $(RSUA_TOPDIR)/mk/mod.mk
