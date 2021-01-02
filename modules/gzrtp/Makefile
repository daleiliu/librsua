# Copyright (C) 2010 - 2017 Creytiv.com
# Copyright (C) 2021 Dalei Liu

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/../..

#
# To build libzrtpcppcore run the following commands:
#
#     git clone https://github.com/wernerd/ZRTPCPP.git
#     cd ZRTPCPP
#     mkdir build
#     cd build
#     cmake -DCMAKE_POSITION_INDEPENDENT_CODE=1 -DCORE_LIB=1 -DSDES=1 \
#         -DBUILD_STATIC=1 ..
#     make
#

# GNU ZRTP C++ library (ZRTPCPP) source directory
ZRTP_PATH ?= ../ZRTPCPP

ZRTP_LIB := $(shell find $(ZRTP_PATH) -name libzrtpcppcore.a)

MOD		:= gzrtp
$(MOD)_SRCS	+= gzrtp.cpp session.cpp stream.cpp messages.cpp srtp.cpp
$(MOD)_LFLAGS	+= $(ZRTP_LIB) -lstdc++
$(MOD)_CXXFLAGS   += \
	-I$(ZRTP_PATH) \
	-I$(ZRTP_PATH)/zrtp \
	-I$(ZRTP_PATH)/srtp

$(MOD)_CXXFLAGS   += -O2 -Wall -fPIC

# Uncomment this if you want to use libre SRTP facilities instead of the ones
# provided by ZRTPCPP. In this case only standard ciphers (AES) are supported.
#$(MOD)_CXXFLAGS   += -DGZRTP_USE_RE_SRTP=1

include $(RSUA_TOPDIR)/mk/mod.mk
