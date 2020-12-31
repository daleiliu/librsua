# Copyright (C) 2010 Creytiv.com
# Copyright (C) 2020 Dalei Liu

# Module build helpers

ifeq ($(RSUA_MK_MKDIR),)
  ifeq ($(RSUA_TOPDIR),)
    $(error need RSUA_MK_MKDIR or RSUA_TOPDIR)
  else
    RSUA_MK_MKDIR := $(RSUA_TOPDIR)/mk
  endif
else
  ifeq ($(RSUA_TOPDIR),)
    RSUA_TOPDIR := $(RSUA_MK_MKDIR)/..
  endif
endif

include $(RSUA_MK_MKDIR)/common.mk

$(MOD)_OBJS     := $(patsubst %.c,$(BUILD)/%.o,\
	$(filter %.c,$($(MOD)_SRCS)))
$(MOD)_OBJS     += $(patsubst %.cpp,$(BUILD)/%.o,\
	$(filter %.cpp,$($(MOD)_SRCS)))
$(MOD)_OBJS     += $(patsubst %.m,$(BUILD)/%.o,\
	$(filter %.m,$($(MOD)_SRCS)))
$(MOD)_OBJS     += $(patsubst %.S,$(BUILD)/%.o,\
	$(filter %.S,$($(MOD)_SRCS)))

-include $($(MOD)_OBJS:.o=.d)

CFLAGS += -I$(RSUA_TOPDIR)/src/build/include
LFLAGS +=

ifeq ($(RSUA_MK_LIBDIR),)
RSUA_MK_LIBDIR := $(RSUA_TOPDIR)/src/$(BUILD)
endif

$(MOD)_NAME := $(MOD)

$(MOD)_CCHECK_OPT     := $(patsubst %.c,%.c,\
	$($(MOD)_CCHECK_OPT))
$(MOD)_CCHECK_OPT     := $(patsubst %.h,%.h,\
	$($(MOD)_CCHECK_OPT))
MOD_CCHECK_OPT        := $(MOD_CCHECK_OPT) $($(MOD)_CCHECK_OPT)

#
# function to extract the name of the module from the file/dir path
#
modulename = $(lastword $(subst /, ,$(dir $1)))


ifeq ($(STATIC),)

all: $(BUILD)/$(MOD)$(MOD_SUFFIX)

#
# Dynamically loaded modules
#

$(BUILD)/$(MOD)$(MOD_SUFFIX): $($(MOD)_OBJS) $(RSUA_MK_LIBDIR)/librsua.so
	@echo "  LD [M]  $@"
	$(HIDE)$(LD) $(LFLAGS) $(SH_LFLAGS) $(MOD_LFLAGS) \
		$($(MOD)_OBJS) \
		$($(MOD)_LFLAGS) -L$(RSUA_MK_LIBDIR) -lrsua -o $@

$(BUILD)/%.o: %.c | $(BUILD)
	@echo "  CC [M]  $@"
	@mkdir -p $(dir $@)
	$(HIDE)$(CC) $(CFLAGS) $($(MOD)_CFLAGS) \
		-c $< -o $@ $(DFLAGS)

$(BUILD)/%.o: %.m | $(BUILD)
	@echo "  OC [M]  $@"
	@mkdir -p $(dir $@)
	$(HIDE)$(CC) $(CFLAGS) $($(MOD)_CFLAGS) $(OBJCFLAGS) \
		-c $< -o $@ $(DFLAGS)

$(BUILD)/%.o: %.cpp | $(BUILD)
	@echo "  CXX [M] $@"
	@mkdir -p $(dir $@)
	$(HIDE)$(CXX) $(CPPFLAGS) $(CXXFLAGS) \
		$($(MOD)_CXXFLAGS) \
		-c $< -o $@ $(DFLAGS)

$(BUILD)/%.o: %.S | $(BUILD)
	@echo "  AS [M]  $@"
	@mkdir -p $(dir $@)
	$(HIDE)$(CC) $(CFLAGS) -DMOD_NAME=\"$(MOD)\" -c $< -o $@ $(DFLAGS)

else

#
# Static linking of modules
#

# needed to deref variable now, append to list
MOD_OBJS        := $(MOD_OBJS) $($(MOD)_OBJS)
MOD_LFLAGS      := $(MOD_LFLAGS) $($(MOD)_LFLAGS)

$(BUILD)/%.o: %.c $(BUILD) Makefile mk/mod.mk \
				module.mk mk/modules.mk
	@echo "  CC [m]  $@"
	@mkdir -p $(dir $@)
	$(HIDE)$(CC) $(CFLAGS) $($(call modulename,$@)_CFLAGS) \
		-DMOD_NAME=\"$(MOD)\" -c $< -o $@ $(DFLAGS)

$(BUILD)/%.o: %.m $(BUILD) Makefile mk/mod.mk \
				module.mk mk/modules.mk
	@echo "  OC [m]  $@"
	@mkdir -p $(dir $@)
	$(HIDE)$(CC) $(CFLAGS) $($(call modulename,$@)_CFLAGS) $(OBJCFLAGS) \
		-DMOD_NAME=\"$(MOD)\" -c $< -o $@ $(DFLAGS)


$(BUILD)/%.o: %.cpp $(BUILD) Makefile mk/mod.mk \
				module.mk mk/modules.mk
	@echo "  CXX [m] $@"
	@mkdir -p $(dir $@)
	$(HIDE)$(CXX) $(CPPFLAGS) $(CXXFLAGS) \
		$($(call modulename,$@)_CXXFLAGS) \
		-DMOD_NAME=\"$(MOD)\" -c $< -o $@ $(DFLAGS)

$(BUILD)/%.o: %.S $(BUILD) Makefile mk/mod.mk \
				module.mk mk/modules.mk
	@echo "  AS [m]  $@"
	@mkdir -p $(dir $@)
	$(HIDE)$(CC) $(CFLAGS) -DMOD_NAME=\"$(MOD)\" -c $< -o $@ $(DFLAGS)

endif
