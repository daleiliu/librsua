# Copyright (C) 2020-2021 Dalei Liu

# Build app: baresip-replica (drop-in replacement of baresip)

RSUA_CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
RSUA_TOPDIR := $(RSUA_CURDIR)/../..

include $(RSUA_TOPDIR)/mk/common.mk
include $(RSUA_TOPDIR)/mk/modules.mk

HDRS := $(wildcard *.h)
SRCS := $(wildcard *.c)

LDFLAGS += -Wl,--unresolved-symbols=ignore-in-shared-libs

LIBRSUA_DIR := $(RSUA_TOPDIR)/src/build/$(ARCH)
LIBRSUA_TARGET := $(LIBRSUA_DIR)/librsua.so
CFLAGS += -I$(RSUA_TOPDIR)/include
LDFLAGS += -L$(LIBRSUA_DIR) -lrsua

LIBS := $(LIBRSUA_TARGET)

OBJS := $(addprefix $(BUILD)/, $(SRCS:.c=.o))
TARGET_BIN := baresip-replica
TARGET := $(BUILD)/$(TARGET_BIN)

.PHONY: modules
all: $(TARGET)

$(TARGET): $(OBJS) $(LIBS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(LIBRSUA_TARGET):
	make -C $(RSUA_TOPDIR)/src

$(BUILD)/%.o: %.c $(HDRS) $(LIBS) | $(BUILD)
	$(CC) $(CFLAGS) -c -o $@ $<

run:
	make -C $(RSUA_TOPDIR)/modules modbins
	cd $(BUILD); LD_LIBRARY_PATH=$(LIBRSUA_DIR) \
	  ./$(TARGET_BIN) -M $(RSUA_TOPDIR)/modules/build/$(ARCH)

