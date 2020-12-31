# Copyright (C) 2020 Dalei Liu

# Build app: baresip-replica (drop-in replacement of baresip)

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/../..

include $(RSUA_TOPDIR)/mk/common.mk
include $(RSUA_TOPDIR)/mk/modules.mk

HDRS := $(wildcard *.h)
SRCS := $(wildcard *.c)

LDFLAGS += -Wl,--unresolved-symbols=ignore-in-shared-libs

LIBRSUA_TARGET := $(RSUA_TOPDIR)/src/build/x86_64/librsua.so
LIBRSUA_DIR := $(RSUA_TOPDIR)/src/build/x86_64
CFLAGS += -I$(RSUA_TOPDIR)/include
LDFLAGS += -L$(LIBRSUA_DIR) -lrsua

LIBS := $(LIBRSUA_TARGET)

OBJS := $(addprefix $(BUILD)/, $(SRCS:.c=.o))
TARGET_BIN := baresip-replica
TARGET := $(BUILD)/$(TARGET_BIN)

.PHONY: modules
all: $(TARGET) $(BUILD)/modules.flag

$(TARGET): $(OBJS) $(LIBS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(LIBRSUA_TARGET):
	make -C $(RSUA_TOPDIR)/src

$(BUILD)/%.o: %.c $(HDRS) $(LIBS) | $(BUILD)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/modules.flag:
	make -C $(RSUA_TOPDIR)/modules
	for mod in $(MODULES); do \
	  cp $(RSUA_TOPDIR)/modules/$$mod/$(BUILD)/*.so \
	    $(RSUA_CURDIR)/$(BUILD) || :; \
	  done
	touch $@

run:
	cd $(BUILD); LD_LIBRARY_PATH=$(LIBRSUA_DIR) ./$(TARGET_BIN)

