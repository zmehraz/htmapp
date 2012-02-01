
# Resource tool
RES_EXE := $(CFG_LOCALROOT)/cii
RES_DIR := $(CFG_PRJROOT)/_res

RES_CIIX = $(PRJ_CIIX)
ifeq (RES_CIIX,)
	RES_CIIX := *.cii
endif

# List of resource directories
RES_DEPS := $(foreach res,$(PRJ_RESD),$(RES_DIR)/$(res).d)

#include dependencies
include $(RES_DEPS)

# Make resource directory
$(RES_DIR): $(BLDOUT)
	-mkdir -p $@

$(RES_DIR)/%.d: % $(BLDOUT)
	$(RES_EXE) -i "$<" -o "$(RES_DIR)" -c "$(RES_CIIX)" -t "$(RES_DIR)/$<.d"
BLDOUT := $(RES_DEPS)

# Individual resource directories that need compiling
#$(RES_DIR)/%.d: $(RES_DIR)
#	-touch $@

# Build resources
#build_resources: $(RES_DEPS)
#	- rm -Rf "$(RES_DIR)/*"
#	$(RES_EXE) -i "$(PRJ_RESD)" -o "$(RES_DIR)" -c "$(RES_CIIX)"
#BLDTGT := build_resources

# Add this folder to our includes
CFG_INCS := $(CFG_INCS) $(RES_DIR)

PRJ_INCS := $(PRJ_INCS) $(CFG_LIBROOT)/inc
BLD_NAME := res
BLD_TYPE := cpp
BLD_EXTN := cpp
BLD_FLST :=
BLD_SRCDIR := $(RES_DIR)
include $(CFG_MAKROOT)/build.mk

