# Copyright (C) 2021 Dalei Liu

# Build modules

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/..

include $(RSUA_TOPDIR)/mk/common.mk
include $(RSUA_TOPDIR)/mk/modules.mk

MOD_BINS:= $(foreach mod,$(MODULES),$(mod)/$(BUILD)/$(mod)$(MOD_SUFFIX))

all: $(MOD_BINS)

$(MOD_BINS):
	@make -C $(basename $(notdir $@))

clean:
	@for mod in $(MODULES); \
	  do make -C $$mod clean; \
	done
