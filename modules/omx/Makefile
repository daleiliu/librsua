# Copyright (C) 2010 - 2015 Creytiv.com
# Copyright (C) 2021 Dalei Liu

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/../..

MOD		:= omx
$(MOD)_SRCS	+= omx.c module.c

ifneq ($(USE_OMX_RPI),)
$(MOD)_CFLAGS	:= -DRASPBERRY_PI -DOMX_SKIP64BIT \
	-isystem /usr/local/include/interface/vmcs_host/linux/ \
	-isystem /usr/local/include/interface/vcos/pthreads/ \
	-isystem /opt/vc/include \
	-isystem /opt/vc/include/interface/vmcs_host/linux \
	-isystem /opt/vc/include/interface/vcos/pthreads
$(MOD)_LFLAGS	+= -lvcos -lbcm_host -lopenmaxil -L /opt/vc/lib
endif

ifneq ($(USE_OMX_BELLAGIO),)
$(MOD)_LFLAGS  += -lomxil-bellagio
endif

include $(RSUA_TOPDIR)/mk/mod.mk
