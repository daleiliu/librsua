# Copyright (C) 2010 Creytiv.com
# Copyright (C) 2021 Dalei Liu

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/../..


#
# To build zrtp libraries and include files, run the following commands:
#
#     git clone git@github.com:juha-h/libzrtp.git
#     cd libzrtp
#     ./bootstrap.sh
#     ./configure CFLAGS="-O0 -g3 -W -Wall -DBUILD_WITH_CFUNC \
#         -DBUILD_DEFAULT_CACHE -DBUILD_DEFAULT_TIMER"
#     make
#     sudo make install
#

MOD		:= zrtp
$(MOD)_SRCS	+= zrtp.c
$(MOD)_LFLAGS	+= -lzrtp -lbn
$(MOD)_CFLAGS   += -isystem /usr/local/include/libzrtp
$(MOD)_CFLAGS   += -Wno-strict-prototypes
include $(RSUA_TOPDIR)/mk/mod.mk
