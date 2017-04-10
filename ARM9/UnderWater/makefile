#/*
# *********************************************************
# * cgt_arm.mk
# *
# * 
# ********************************************************
# */
EXBASE = ..
include ../../BurchFw/makerules/rules.mk
include ../../BurchFw/makerules/cgt_arm.mk

EXEFILE := UdWater

srcs = main.c DecFsk.c EncFsk.c \
		EncFskDemo.c \
		DecFskDemo.c \
		TerminalIntr.c \
		crc32.c u_boot_env.c

objs = $(addprefix bin/$(PROFILE)/obj/,$(patsubst %.c,%.ov5T,$(srcs)))
	   
libs = $(ROOT_DIR)/BurchFw/BurchFw/lib/BurchFw.a_$(PROFILE) \
	   $(ROOT_DIR)/BurchFw/osa/lib/BurchOSA.a_$(PROFILE) \
	   $(SYSLINK_INSTALL_DIR)/packages/ti/syslink/lib/syslink.a_release \
	   $(LINUXUTILS_INSTALL_DIR)/packages/ti/sdo/linuxutils/cmem/lib/cmem.a470MV \
	   -lm

LDLIBS += 

BURCHFW_PATH += -I"$(ROOT_DIR)/BurchFw/BurchFw/include" \
				-I"$(ROOT_DIR)/BurchFw/osa/include" \
				-I"$(ROOT_DIR)/Codecs" \
				-I"$(LINUXUTILS_INSTALL_DIR)/packages" \
				-I"$(XDAIS_INSTALL_DIR)/packages"
				
		
CFLAGS += $(BURCHFW_PATH)
		
.PHONY: clean debug all install default

default: all

help:
	@$(ECHO) "make all              # build executables"
	@$(ECHO) "make clean            # clean everything"

all: debug release

debug:
	@$(ECHO) "#"
	@$(ECHO) "# Making $@ ..."
	$(MAKE) PROFILE=debug $(EXEFILE)
	
release:
	@$(ECHO) "#"
	@$(ECHO) "# Making $@ ..."	
	$(MAKE) PROFILE=release $(EXEFILE)
	
install:
	@$(ECHO) "#"
	@$(ECHO) "# Making $@ ..."
	@$(MKDIR) $(INSTALL_DIR)/debug
	$(CP) bin/debug/$(EXEFILE) $(INSTALL_DIR)/debug
	@$(MKDIR) $(INSTALL_DIR)/release
	$(CP) bin/release/$(EXEFILE) $(INSTALL_DIR)/release
	
proj_install:
	@$(ECHO) "#"
	@$(CP) bin/release/$(EXEFILE) $(RELEASE_DIR)
	
clean::
	$(RMDIR) bin

CODECS_ITF_LIBS:=
include $(ROOT_DIR)/Codecs/liva/dsp/DecFsk/itf.mk
include $(ROOT_DIR)/Codecs/liva/dsp/EncFsk/itf.mk
include $(ROOT_DIR)/Codecs/liva/common/sout/itf.mk

#
#  ======== rules ========
#
$(EXEFILE): prebuild bin/$(PROFILE)/$(EXEFILE)
bin/$(PROFILE)/$(EXEFILE): $(objs) 
	@$(ECHO) "#"
	@$(ECHO) "# Making $@ ..."
	$(LD) $(LDFLAGS) -o $@ $^  $(CODECS_ITF_LIBS) $(LDLIBS) $(libs)
	
bin/$(PROFILE)/obj/%.ov5T: %.h
bin/$(PROFILE)/obj/%.ov5T: %.c
	@$(ECHO) "#"
	@$(ECHO) "# Making $@ ..."
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $<

prebuild:
	@$(MKDIR) bin
	@$(MKDIR) bin/$(PROFILE)
	@$(MKDIR) bin/$(PROFILE)/obj
	
#  ======== install validation ========
ifeq (install,$(MAKECMDGOALS))
ifeq (,$(INSTALL_DIR))
$(error must specify INSTALL_DIR)
endif
endif
#  ======== create output directories ========
ifneq (clean,$(MAKECMDGOALS))
ifneq (,$(PROFILE))
ifeq (,$(wildcard bin/$(PROFILE)/obj))
    $(shell $(MKDIR) -p bin/$(PROFILE)/obj)
endif
endif
endif
