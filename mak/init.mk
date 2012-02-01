
#-------------------------------------------------------------------
# default target
#-------------------------------------------------------------------
.PHONY: default_target
default_target: all

#-------------------------------------------------------------------
# Configure lib paths
#-------------------------------------------------------------------
CFG_LIBROOT := $(PRJ_LIBROOT)
CFG_MAKROOT := $(CFG_LIBROOT)/mak

#-------------------------------------------------------------------
# Parse target
#-------------------------------------------------------------------
include $(CFG_MAKROOT)/init/tgt.mk

#-------------------------------------------------------------------
# Configure output paths
#-------------------------------------------------------------------
ifneq ($(BINROOT),)
	CFG_BINROOT := $(BINROOT)
else
	CFG_BINROOT := $(CFG_LIBROOT)/../bin
endif
CFG_OUTROOT := $(CFG_BINROOT)/$(TTYPE)
CFG_OBJROOT := $(CFG_OUTROOT)/obj
CFG_PRJROOT := $(CFG_OBJROOT)/$(PRJ_NAME)

#-------------------------------------------------------------------
# Project info
#-------------------------------------------------------------------
CFG_EXTR := $(CFG_EXTR) -DCII_PROJECT_NAME="\"$(PRJ_NAME)\"" \
						-DCII_PROJECT_DESC="\"$(PRJ_DESC)\""

#-------------------------------------------------------------------
# Configure local built tools
#-------------------------------------------------------------------
ifneq ($(LOCAL),)
	CFG_LOCALROOT := $(CFG_BINROOT)/$(LOCAL)
else
	CFG_LOCALROOT := $(CFG_OUTROOT)
endif

#-------------------------------------------------------------------
# Initialize
#-------------------------------------------------------------------
.PHONY: init
init: $(BLDOUT)
	- @echo .=======================================================
	- @echo .= $(PRJ_NAME)
	- @echo .=======================================================
BLDOUT := init

# bin
$(CFG_BINROOT): $(BLDOUT)
	-mkdir -p $@
BLDOUT := $(CFG_BINROOT)

# bin/<type>
$(CFG_OUTROOT): $(BLDOUT)
	-mkdir -p $@
BLDOUT := $(CFG_OUTROOT)

# bin/<type>/obj
$(CFG_OBJROOT): $(BLDOUT)
	-mkdir -p $@
BLDOUT := $(CFG_OBJROOT)

# bin/<type>/obj/<project>
$(CFG_PRJROOT): $(BLDOUT)
	-mkdir -p $@
BLDOUT := $(CFG_PRJROOT)

#-------------------------------------------------------------------
# Configure gcc
#-------------------------------------------------------------------
include $(CFG_MAKROOT)/init/gcc.mk

# Target filename
CFG_OUTFILE := $(CFG_OUTROOT)/$(CFG_TGT_PRE)$(PRJ_NAME)$(CFG_TGT_EXT)

# Resources?
ifneq ($(PRJ_RESD),)
	include $(CFG_MAKROOT)/init/res.mk
endif


